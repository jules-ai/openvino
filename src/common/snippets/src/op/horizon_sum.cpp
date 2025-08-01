// Copyright (C) 2018-2025 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "snippets/op/horizon_sum.hpp"

#include <memory>

#include "openvino/core/node.hpp"
#include "openvino/core/node_output.hpp"
#include "openvino/core/shape.hpp"
#include "openvino/op/op.hpp"
#include "snippets/itt.hpp"

namespace ov::snippets::op {

HorizonSum::HorizonSum(const Output<Node>& x) : Op({x}) {
    constructor_validate_and_infer_types();
}

std::shared_ptr<Node> HorizonSum::clone_with_new_inputs(const OutputVector& new_args) const {
    INTERNAL_OP_SCOPE(HorizonSum_clone_with_new_inputs);
    check_new_args_count(this, new_args);
    return std::make_shared<HorizonSum>(new_args.at(0));
}

void HorizonSum::validate_and_infer_types() {
    INTERNAL_OP_SCOPE(HorizonSum_validate_and_infer_types);
    auto new_shape = get_input_partial_shape(0);
    if (!ov::is_scalar(new_shape)) {
        new_shape[new_shape.size() - 1] = 1LU;
    }
    set_output_type(0, get_input_element_type(0), new_shape);
}

}  // namespace ov::snippets::op
