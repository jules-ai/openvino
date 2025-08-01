// Copyright (C) 2024 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "jit_snippets_emitters.hpp"

#include <xbyak_aarch64/xbyak_aarch64/xbyak_aarch64_adr.h>

#include <common/utils.hpp>
#include <cpu/aarch64/cpu_isa_traits.hpp>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "cpu/aarch64/jit_generator.hpp"
#include "emitters/plugin/aarch64/jit_emitter.hpp"
#include "emitters/utils.hpp"
#include "openvino/core/type.hpp"
#include "openvino/core/type/element_type.hpp"
#include "openvino/op/constant.hpp"
#include "snippets/lowered/expression.hpp"
#include "utils/general_utils.h"

using namespace Xbyak_aarch64;

namespace ov::intel_cpu::aarch64 {

using jit_generator = dnnl::impl::cpu::aarch64::jit_generator;
using cpu_isa_t = dnnl::impl::cpu::aarch64::cpu_isa_t;
using ExpressionPtr = ov::snippets::lowered::ExpressionPtr;

jit_nop_emitter::jit_nop_emitter(jit_generator* h, cpu_isa_t isa, [[maybe_unused]] const ExpressionPtr& expr)
    : aarch64::jit_emitter(h, isa) {
    in_out_type_ = emitter_in_out_map::gpr_to_gpr;
}

jit_broadcast_move_emitter::jit_broadcast_move_emitter(jit_generator* h, cpu_isa_t isa, const ExpressionPtr& expr)
    : jit_emitter(h, isa) {
    const auto n = expr->get_node();
    OV_CPU_JIT_EMITTER_ASSERT(n->get_input_element_type(0) == n->get_output_element_type(0),
                              "Only supports equal input and output types but gets ",
                              n->get_input_element_type(0),
                              " and ",
                              n->get_output_element_type(0));
    const auto element_type = n->get_input_element_type(0);
    OV_CPU_JIT_EMITTER_ASSERT(any_of(element_type.size(), 1U, 2U, 4U), "Unsupported element type: ", element_type);

    byte_size = n->get_input_element_type(0).size();
}

void jit_broadcast_move_emitter::emit_impl(const std::vector<size_t>& in, const std::vector<size_t>& out) const {
    if (host_isa_ == dnnl::impl::cpu::aarch64::asimd) {
        emit_isa<dnnl::impl::cpu::aarch64::asimd>(in, out);
    } else {
        OV_CPU_JIT_EMITTER_THROW("Doesn't support isa ", host_isa_);
    }
}

template <cpu_isa_t isa>
void jit_broadcast_move_emitter::emit_isa(const std::vector<size_t>& in, const std::vector<size_t>& out) const {
    using TReg = typename dnnl::impl::cpu::aarch64::cpu_isa_traits<isa>::TReg;
    auto src = TReg(in[0]);
    auto dst = TReg(out[0]);

    switch (byte_size) {
    case 1:
        h->dup(dst.b, src.b[0]);
        break;
    case 2:
        h->dup(dst.h, src.h[0]);
        break;
    case 4:
        h->dup(dst.s, src.s[0]);
        break;
    default:
        OV_CPU_JIT_EMITTER_THROW("Unsupported data size ", byte_size);
    }
}

jit_scalar_emitter::jit_scalar_emitter(jit_generator* h, cpu_isa_t isa, const ExpressionPtr& expr)
    : jit_emitter(h, isa) {
    const auto n = expr->get_node();
    const auto& precision = n->get_output_element_type(0);
    switch (precision) {
    case element::i32: {
        value = ov::as_type_ptr<ov::op::v0::Constant>(n)->cast_vector<int32_t>()[0];
        break;
    }
    case element::f32: {
        value = dnnl::impl::float2int(ov::as_type_ptr<ov::op::v0::Constant>(n)->cast_vector<float>()[0]);
        break;
    }
    default: {
        OV_CPU_JIT_EMITTER_THROW("Doesn't support precision ", precision);
    }
    }
    push_arg_entry_of("scalar", value, true);
    prepare_table();
}

void jit_scalar_emitter::emit_impl(const std::vector<size_t>& in, const std::vector<size_t>& out) const {
    if (host_isa_ == dnnl::impl::cpu::aarch64::asimd) {
        emit_isa<dnnl::impl::cpu::aarch64::asimd>(in, out);
    } else {
        OV_CPU_JIT_EMITTER_THROW("Doesn't support isa ", host_isa_);
    }
}

template <cpu_isa_t isa>
void jit_scalar_emitter::emit_isa([[maybe_unused]] const std::vector<size_t>& in,
                                  const std::vector<size_t>& out) const {
    using TReg = typename dnnl::impl::cpu::aarch64::cpu_isa_traits<isa>::TReg;
    auto dst = TReg(out[0]);
    AdrImm src = table_val("scalar");

    h->uni_ld1rw(dst.s, src.getXn(), src.getImm());
}

}  // namespace ov::intel_cpu::aarch64
