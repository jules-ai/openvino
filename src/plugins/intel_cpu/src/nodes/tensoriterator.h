// Copyright (C) 2018-2025 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <graph.h>
#include <node.h>

#include <common/utils.hpp>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <oneapi/dnnl/dnnl.hpp>
#include <oneapi/dnnl/dnnl_common.hpp>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "allocation_context.hpp"
#include "cpu_memory.h"
#include "graph_context.h"
#include "openvino/core/node.hpp"

namespace ov::intel_cpu::node {

struct PortMap {
    // Data map rule
    int from; /**< Index of external data from ins/outs fields of node */
    int to;   /**< Index of internal data in iterator body */

    // Iteration rule
    int axis;      /**< Axis to iterate throught */
    int stride;    /**< Stride to iterate throught */
    int start;     /**< Start index of iteration range */
    int end;       /**< Last index of iteration range  */
    int part_size; /**< Part size which will be transfered to body subnetwork */
};

/**
 * Functor interface to perform some action with pointed tensors (captured in constructor)
 * Generally it's read, write or move data from specified tensors.
 * Action may depends on iteration index.
 */
class PortMapHelper {
public:
    virtual ~PortMapHelper() = default;
    virtual void execute(const dnnl::stream& strm, int n_iter) = 0;

protected:
    dnnl::primitive reorder;
    dnnl::memory mem_holder_src;
    dnnl::memory mem_holder_dst;
};

/**
 * Functor interface to perform check of data tensor (captured in constructor)
 * Information extracted as int. Meaning of returned value is specific for
 * particular type of checker.
 */
class PortChecker {
public:
    virtual ~PortChecker() = default;
    virtual int getStatus() = 0;

protected:
    dnnl::memory mem_holder;
};

/**
 * Class for storing intermediate output buffer state for dynamism when we don't know
 * final output shape but we should concatenate output after each iteration
 */
class DynamicBuffer {
public:
    DynamicBuffer(MemoryPtr from_, std::vector<MemoryPtr> to_, const PortMap& map_rule_);

    void execute(const dnnl::engine& eng, int iter);
    void transfer(const Node* node);

    void reset(int max_iter_count_);  // reset local

private:
    void init(const dnnl::engine& eng);

    /* methods for resize and refill buffer */
    [[nodiscard]] bool check_buffer() const;
    MemoryPtr create_buffer(const dnnl::engine& eng);
    void move_buffer(const MemoryPtr& new_buffer);
    void move_data();

    static void copy(const uint8_t* src, uint8_t* dst, size_t src_stride, size_t dst_stride, size_t count, size_t len);

    /* variable states */
    size_t len = 1LU;
    size_t count = 1LU;

    ptrdiff_t chunk_stride_in_byte = 0;
    ptrdiff_t chunk_offset_in_byte = 0;
    size_t chunk_unit_in_byte = 0LU;  // the amount of bytes copied per each count per each execution (iteration)
    int num_execs = 0LU;              // number of executions happened
    int max_iter_count = -1;          // estimated maximum iter count

    /* invariable states */
    MemoryPtr from;
    std::vector<MemoryPtr> to;
    PortMap map_rule;
    size_t elem_size = 0LU;

    MemoryPtr mem_holder_buffer;
};

class TensorIterator : public Node {
public:
    TensorIterator(const std::shared_ptr<ov::Node>& op, const GraphContext::CPtr& context);

    static bool isSupportedOperation(const std::shared_ptr<const ov::Node>& op, std::string& errorMessage) noexcept;
    void initSupportedPrimitiveDescriptors() override;
    void getSupportedDescriptors() override {};
    void createPrimitive() override;
    int registerToAllocationContext(int offset, AllocationContext& context) override;
    bool created() const override;
    void execute(const dnnl::stream& strm) override;
    bool neverExecute() const override {
        return false;
    }
    bool isExecutable() const override {
        return true;
    }
    // @todo limit to particular in / out ports
    static bool usesInOutMemoryMultipleTimes() {
        return true;
    }

protected:
    //  needShapeInfer() should return false
    //  because we cannot resolve the output dimensions before the inference is completed
    bool needShapeInfer() const override {
        return false;
    };

    bool needPrepareParams() const override;
    void prepareParams() override;
    void executeDynamicImpl(const dnnl::stream& strm) override;

private:
    void prepareInputPorts();
    void prepareOutputPorts();
    void prepareBackEdges();
    void prepareDynamicBackEdges();
    void prepareDynamicBuffers();
    void prepareLoopBodyCurrentIteration();
    void prepareContinueCond();
    void prepareInitialCond(bool compileStage);
    void prepareTripCount(bool compileStage);

    /* Dynamic support */
    void reshapeSubgraphInput();
    void reshapeAndFillOutput(const dnnl::stream& strm);
    bool checkForInputAndBodyShapesInequality() const;
    int getNumIteration(const std::vector<PortMap>& inputPortMap, const std::vector<PortMap>& outputPortMap) const;
    void prepareParamsImpl(bool compileStage);

    /* run dynamic subgraph inside a static node */
    bool runAsDynamic() const;
    void restoreSubgraphInputByBackEdges();

    Graph sub_graph;
    std::vector<std::vector<MemoryPtr>> input_mems;
    std::vector<MemoryPtr> output_mem;

    struct PortMapHasher {
        std::size_t operator()(const std::pair<int, int>& p) const {
            std::size_t seed = 0;
            dnnl::impl::hash_combine(seed, p.first);
            dnnl::impl::hash_combine(seed, p.second);
            return seed;
        }
    };
    std::unordered_map<std::pair<int, int>, std::shared_ptr<PortMapHelper>, PortMapHasher>
        first_mappers;  /// < Applied once before loop

    std::vector<std::shared_ptr<PortMapHelper>> last_mappers,  /// < Applied once after loop
        before_mappers,                                        /// < Applied before each iteration
        after_mappers,                                         /// < Applied after each iteration
        back_mappers;                                          /// < Applied before each iteration for dynamic shapes

    std::shared_ptr<PortChecker> trip_count_check,  /// < Perform check of trip count value. value >= -1
        initial_cond_check,   /// < Perform check of initial continue condition value. value [0, 1]
        continue_cond_check;  /// < Perform check of continue condition value of body. value [0, 1]

    std::vector<std::shared_ptr<DynamicBuffer>> buffers;

    std::vector<PortMap> inputPortMap;   //!< Input ports map
    std::vector<PortMap> outputPortMap;  //!< Output ports map
    std::vector<PortMap> backEdges;      //!< Back edges map

    std::vector<int> loopBodyCurrentIterationIdx;
    int loopBodyConditionOutputIdx = -1;
    int loopTripCountIdx = -1;
    int loopExecutionConditionIdx = -1;

    int lastUsedTripCount = -1;
    bool lastUsedCond = false;

    const std::shared_ptr<ov::Node> ngraphOp;
};

}  // namespace ov::intel_cpu::node
