// Copyright (C) 2018-2025 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "openvino/core/visibility.hpp"
#include "functional_test_utils/skip_tests_config.hpp"
#include "openvino/runtime/system_conf.hpp"
#include "utils/precision_support.h"
#include "snippets/utils.hpp"
#if defined(OPENVINO_ARCH_RISCV64)
#   include "nodes/kernels/riscv64/cpu_isa_traits.hpp"
#endif
#include <string>
#include <vector>

#include "utils/cpu_test_utils.hpp"

std::vector<std::string> disabledTestPatterns() {
    std::vector<std::string> retVector{
        // TODO: Issue 31841
        R"(.*(QuantGroupConvBackpropData3D).*)",
        // TODO: Issue 31843
        R"(.*(QuantConvBackpropData3D).*)",
        R"(.*(QuantConvBackpropData2D).*(QG=Perchannel).*)",
        R"(.*(QuantGroupConvBackpropData2D).*(QG=Perchannel).*)",
        // TODO: Issue 33886
        R"(.*(QuantGroupConv2D).*)",
        R"(.*(QuantGroupConv3D).*)",
        R"(.*(RangeAddSubgraphTest).*Start=1.2.*Stop=(5.2|-5.2).*Step=(0.1|-0.1).*ET=f16.*)",
        R"(.*(RangeNumpyAddSubgraphTest).*ET=f16.*)",
        // TODO: Issue: 43793
        R"(.*InferRequestPreprocessDynamicallyInSetBlobTest.*iPRC=0.*_iLT=1.*)",
        R"(.*InferRequestPreprocessDynamicallyInSetBlobTest.*oPRC=0.*_oLT=1.*)",
        // TODO: Issue: 63469
        R"(.*ConversionLayerTest.*ConvertLike.*)",
        // TODO: Issue: 34055
        R"(.*ReluShapeOfSubgraphTest.*)",
        // TODO: Issue: 43314
        R"(.*Broadcast.*mode=BIDIRECTIONAL.*inNPrec=BOOL.*)",
        // TODO: Issue 43417 sporadic issue, looks like an issue in test, reproducible only on Windows platform
        R"(.*decomposition1_batch=5_hidden_size=10_input_size=30_.*tanh.relu.*_clip=0_linear_before_reset=1.*_targetDevice=CPU_.*)",
        // Skip platforms that do not support BF16 (i.e. sse, avx, avx2)
        R"(.*(BF|bf)16.*(jit_avx(?!5)|jit_sse).*)",
        // TODO: Incorrect blob sizes for node BinaryConvolution_X
        R"(.*BinaryConvolutionLayerTest.*)",
        // TODO: 53618. BF16 gemm ncsp convolution crash
        R"(.*_GroupConv.*_inFmts=nc.*_primitive=jit_gemm.*ENFORCE_BF16=YES.*)",
        // TODO: 157596 convolution bf16 leftover test case
        R"(smoke_JIT_AVX512_DW_GroupConv/GroupConvolutionLayerCPUTest.*ndhwc.*jit_avx512_dw.*INFERENCE_PRECISION_HINT=bf16.*)",
        R"(smoke_Conv_1D_1x1_BF16/ConvolutionLayerCPUTest\.CompareWithRefs/IS=\[\]_TS=\(\((1|2)\.6(4|7)\.7\)_\)_K\(1\)_S\(1\)_PB\(0\)_PE\(0\)_D=\(1\)_O=63_AP=explicit_netPRC=f32_inPRC=dynamic_outPRC=dynamic_trgDev=CPU_inFmts=nhwc_outFmts=nhwc_primitive=jit_avx512_1x1_.*PluginConf_INFERENCE_PRECISION_HINT=bf16)",
        R"(smoke_Conv_1D_1x1_BF16/ConvolutionLayerCPUTest\.CompareWithRefs/IS=\[1\.\.200\.64\.\?\]_TS=\(\(2\.64\.7\)_\(1\.64\.5\)_\)_K\(1\)_S\(1\)_PB\(0\)_PE\(0\)_D=\(1\)_O=63_AP=explicit_netPRC=f32_inPRC=dynamic_outPRC=dynamic_trgDev=CPU_inFmts=nhwc_outFmts=nhwc_primitive=jit_avx512_1x1_.*PluginConf_INFERENCE_PRECISION_HINT=bf16)",
        R"(smoke_Conv_1D_1x1_BF16/ConvolutionLayerCPUTest\.CompareWithRefs/IS=\[\?\.6(4|7)\.1\.\.200\]_TS=\(\(2\.6(4|7)\.7\)_\(1\.6(4|7)\.9\)_\)_K\(1\)_S\(1\)_PB\(0\)_PE\(0\)_D=\(1\)_O=63_AP=explicit_netPRC=f32_inPRC=dynamic_outPRC=dynamic_trgDev=CPU_inFmts=nhwc_outFmts=nhwc_primitive=jit_avx512_1x1_.*PluginConf_INFERENCE_PRECISION_HINT=bf16)",
        R"(smoke_GroupConv_brgemm_2D_BF16/GroupConvolutionLayerCPUTest\.CompareWithRefs/IS=\[\]_TS=\(\(1\.64\.7\.7\)_\)_K\(3\.3\)_S\(2\.2\)_PB\((0|1)\.(0|1)\)_PE\(0\.0\)_D=\(2\.2\)_O=64_G=2_AP=explicit_netPRC=f32_inPRC=dynamic_outPRC=dynamic_trgDev=CPU_inFmts=nhwc_outFmts=nhwc_primitive=brgconv_avx512_amx_.*PluginConf_INFERENCE_PRECISION_HINT=bf16)",
        R"(smoke_JIT_AVX512_DW_GroupConv/GroupConvolutionLayerCPUTest.*inFmts=nCdhw16c.*INFERENCE_PRECISION_HINT=bf16.*)",
        R"(smoke_Conv_1D_BF16/ConvolutionLayerCPUTest.*IS=\[\].*K\(3\).*S\(2\).*PE\(0\).*D=\(1\).*O=6(3|4).*brgconv_avx512_amx.*)",
        // TODO: 56827. Sporadic test failures
        R"(.*smoke_Conv.+_FP32.ConvolutionLayerCPUTest\.CompareWithRefs.*TS=\(\(.\.67.+\).*inFmts=n.+c.*_primitive=jit_avx2.*)",
        // incorrect jit_uni_planar_convolution with dilation = {1, 2, 1} and output channel 1
        R"(.*smoke_Convolution3D.*D=\(1.2.1\)_O=1.*)",

        // TODO: Issue: 35627. CPU Normalize supports from 2D to 4D blobs
        R"(.*NormalizeL2_1D.*)",
        R"(.*NormalizeL2_5D.*)",
        // Issue: 59788. dnnl_normalize_nchw applies eps after sqrt for across_spatial
        R"(.*NormalizeL2_.*axes=\(1.2.*_eps=100.*)",
        R"(.*NormalizeL2_.*axes=\(2.1.*_eps=100.*)",
        R"(.*NormalizeL2_.*axes=\(3.1.2.*_eps=100.*)",

        // Unsupported operation of type: NormalizeL2 name : Doesn't support reduction axes: (2.2)
        R"(.*BF16NetworkRestore1.*)",
        R"(.*MobileNet_ssd_with_branching.*)",

        // Not expected behavior
        R"(.*Behavior.*CorrectConfigCheck.*(canSetConfigAndCheckGetConfig|canSetConfigTwiceAndCheckGetConfig).*CPU_BIND_THREAD=YES.*)",
        // Issue: 72021 Unreasonable abs_threshold for comparing bf16 results
        R"(.*smoke_Reduce.*type=(Prod|Min).*INFERENCE_PRECISION_HINT=(BF|bf)16.*)",

        // CPU does not support dynamic rank
        // Issue: 66778
        R"(.*smoke_BehaviorTests.*InferFullyDynamicNetworkWith(S|G)etTensor.*)",
        R"(.*smoke_BehaviorTests.*DynamicOutputToDynamicInput.*)",
        R"(.*smoke_BehaviorTests.*DynamicInputToDynamicOutput.*)",
        // unsupported metrics
        R"(.*OVGetMetricPropsTest.*OVGetMetricPropsTest.*(MAX_BATCH_SIZE).*)",
        // supports only '' as device id
        R"(.*OVClassQueryModelTest.*QueryModelWithDeviceID.*)",
        // Issue 67214
        R"(smoke_PrePostProcess.*resize_and_convert_layout_i8.*)",
        // TODO: 67255
        R"(smoke_If.*SimpleIf2OutTest.*)",
        // Issue: 69086
        // need to add support convert BIN -> FP32
        // if we set output precision as BIN, when we create output blob precision looks like UNSPECIFIED
        R"(.*smoke_FakeQuantizeLayerCPUTest.*bin.*)",
        // Issue: 69222
        R"(.*smoke_PriorBoxClustered.*PriorBoxClusteredLayerCPUTest.*_netPRC=f16_.*)",
        // Issue: 72005
        // there are some inconsistency between cpu plugin and ng ref
        // for ctcMergeRepeated is true when legal randomized inputs value.
        // Failure happened on win and macos for current seeds.
        R"(.*CTCLossLayerTest.*CMR=1.*)",
        R"(.*CTCLossLayerCPUTest.*ctcMergeRepeated=1.*)",
        // Issue: 71756
        R"(.*GroupDeconv_2D_DW_BF16/GroupDeconvolutionLayerCPUTest.CompareWithRefs.*PRC=f32.*inFmts=nChw16c_outFmts=nChw16c_primitive=jit_avx512_dw_Fused=Multiply\(PerChannel\).Add\(PerChannel\)_PluginConf_INFERENCE_PRECISION_HINT=bf16*)",
        R"(.*smoke_GroupDeconv_(2|3)D_Blocked_BF16.*S=(\(2\.2\)|\(2\.2\.2\))_PB=(\(0\.0\)|\(0\.0\.0\))_PE=(\(0\.0\)|\(0\.0\.0\))_D=(\(1\.1\)|\(1\.1\.1\))_.*_O=64_G=4.*)",
        // Issue: 59594
        R"(smoke_ConversionLayerTest/ConversionLayerTest.CompareWithRefs.*BOOL.*)",
        R"(smoke_ConversionLayerTest/ConversionLayerTest.CompareWithRefs.*MIXED.*)",
        R"(smoke_ConversionLayerTest/ConversionLayerTest.CompareWithRefs.*Q78.*)",
        R"(smoke_ConversionLayerTest/ConversionLayerTest.CompareWithRefs.*U4.*)",
        R"(smoke_ConversionLayerTest/ConversionLayerTest.CompareWithRefs.*I4.*)",
        R"(smoke_ConversionLayerTest/ConversionLayerTest.CompareWithRefs.*BIN.*)",
        R"(smoke_ConversionLayerTest/ConversionLayerTest.CompareWithRefs.*CUSTOM.*)",
        R"(smoke_ConversionLayerTest/ConversionLayerTest.CompareWithRefs.*UNSPECIFIED.*)",
        // Issue:
        // New API tensor tests
        R"(.*OVInferRequestCheckTensorPrecision.*type=u1.*)",
        // Issue: 77390
        R"(.*LoopLayerCPUTest.*exec_cond=0.*)",
        R"(.*LoopLayerCPUTest.*trip_count=0.*)",
        R"(.*LoopForDiffShapesLayerCPUTest.*exec_cond=0.*)",
        R"(.*LoopForDiffShapesLayerCPUTest.*trip_count=0.*)",
        R"(.*LoopForConcatLayerCPUTest.*exec_cond=0.*)",
        R"(.*LoopForConcatLayerCPUTest.*trip_count=0.*)",
        // [ INFO ] Can't compile network without cache for ..  with precision ..
        R"(.*CompileModelCacheTestBase.*CompareWithRefImpl.*KSOFunction.*)",
        R"(.*CompileModelCacheTestBase.*CompareWithRefImpl.*NonMaxSuppression.*)",
        R"(.*CompileModelCacheTestBase.*CompareWithRefImpl.*Nms.*)",
        // 94982. FP32->I32 conversion issue in the reference implementation. There can be some garbage in the rest of
        // float values like 0.333333745.
        // The kernel does not have such garbage. The diff 0.000000745 is taken into account in calculations and affects
        // further type conversion.
        // Reorder->GridSample->Reorder also does not work here. Potential fix is to use nearest conversion instead of
        // truncation.
        R"(.*GridSampleLayerTestCPU.*(BILINEAR|BICUBIC).*(i32|i8).*)",
        R"(.*smoke_static/GridSampleLayerTestCPU.CompareWithRefs/.*_TS=.*(1.7.5.3|2.6.3.10).*_interpMode=NEAREST_padMode=REFLECTION_alignCorners=False_dataPrc=(f32|i32)_gridPrc=f32_.*)",
        R"(.*smoke_static/GridSampleLayerTestCPU.CompareWithRefs/.*_TS=.*5.3.2.13.*_interpMode=BICUBIC_padMode=REFLECTION_alignCorners=True_dataPrc=f32_gridPrc=f32_.*)",
        R"(.*smoke_static/GridSampleLayerTestCPU.CompareWithRefs/.*_TS=.*2.1.6.16.*_interpMode=NEAREST_padMode=(BORDER|REFLECTION)_alignCorners=(True|False)_dataPrc=(f32|i32)_gridPrc=f32_.*)",
        R"(.*smoke_dynamic/GridSampleLayerTestCPU.CompareWithRefs/IS=\(\[2..15.\?.\?.\?\]_\[\?.3.7.2\]\)_.*_interpMode=NEAREST_padMode=REFLECTION_alignCorners=False_dataPrc=f32_gridPrc=f32_.*)",
        R"(.*smoke_dynamic/GridSampleLayerTestCPU.CompareWithRefs/IS=\(\[\?.\?.\?.\?\]_\[\?.\?.\?.\?\]\).*interpMode=NEAREST_padMode=REFLECTION_alignCorners=False_dataPrc=f32_gridPrc=f32_.*)",
        R"(.*smoke_dynamic/GridSampleLayerTestCPU.CompareWithRefs/IS=\(\[\?.3.\?.\?\]_\[\?.\?.\?.2\]\).*interpMode=BICUBIC_padMode=REFLECTION_alignCorners=True_dataPrc=f32_gridPrc=f32_.*)",
        // AdaptiveAvgPool is converted into Reduce op for suitable parameters. CPU Reduce impl doesn't support non
        // planar layout for 3D case
        R"(.*StaticAdaPoolAvg3DLayoutTest.*OS=\(1\).*_inFmts=(nwc|nCw16c|nCw8c).*)",
        // Issue: 111404
        R"(.*smoke_set1/GatherElementsCPUTest.*)",
        // Issue: 111406
        R"(.*smoke_InterpolateLinearOnnx_Layout_Test/InterpolateLayerCPUTest.*)",
        R"(.*smoke_InterpolateLinear_Layout_Test/InterpolateLayerCPUTest.*)",
        R"(.*smoke_InterpolateCubic_Layout_Test/InterpolateLayerCPUTest.*)",
        // Issue: 111412
        R"(.*smoke_Proposal_(Static|Dynamic)_Test_Case1/ProposalLayerCPUTest.*)",
        // Issue: 111418
        R"(.*smoke_Snippets_ConvertStub/ConvertStub\.CompareWithRefImpl/IS.*_OT=\(bf16\)_#N=2_#S=2_targetDevice=CPU.*)",
        R"(.*smoke_Snippets_Convert/Convert\.CompareWithRefImpl/IS.*_IT=\((f32|f16)\)_OT=\(u8\)_#N=1_#S=1_targetDevice=CPU.*)",
        R"(.*smoke_Snippets_ConvertManyOnInputs/ConvertManyOnInputs\.CompareWithRefImpl/IS.*_IT=\(f32\.u8\)_OT=\(\)_#N=1_#S=1_targetDevice=CPU.*)",
        // New plugin API doesn't support changes of pre-processing
        R"(.*InferRequestPreprocessTest.*SetPreProcessToInputInfo.*)",
        R"(.*InferRequestPreprocessTest.*SetPreProcessToInferRequest.*)",
        // Plugin version was changed to ov::Version
        R"(.*VersionTest.pluginCurrentVersionIsCorrect.*)",
        // Issue: 114765
        R"(.*smoke_PSROIPoolingAverageLayoutTest/PSROIPoolingLayerCPUTest.*bf16.*)",
        R"(.*smoke_PSROIPoolingBilinearLayoutTest/PSROIPoolingLayerCPUTest.*bf16.*)",
        // Issue: 120222
        R"(.*smoke_TopK/TopKLayerTest.Inference.*_k=1_axis=3_.*_modelType=f16_trgDev=CPU.*)",
        R"(.*smoke_TopK/TopKLayerTest.Inference.*_k=7_axis=3_.*_modelType=f16_trgDev=CPU.*)",
        R"(.*smoke_TopK/TopKLayerTest.Inference.*_k=1_axis=1_.*_modelType=f16_trgDev=CPU.*)",
        R"(.*smoke_TopK/TopKLayerTest.Inference.*_k=7_axis=1_.*_modelType=f16_trgDev=CPU.*)",
        R"(.*smoke_TopK/TopKLayerTest.Inference.*_k=18_.*_modelType=f16_trgDev=CPU.*)",
        R"(.*smoke_TopK/TopKLayerTest.Inference.*_k=21_.*_sort=value_modelType=f16_trgDev=CPU.*)",
        // Issue: 121812
        R"(.*ConvertCPULayerTest.*outFmts=(nhwc|nChw8c|nChw16c).*)",
        // Issue: 123320
        // Input precision bf16 is converted to fp32 by logic in core_config.cpp during ngraph reference test.
        R"(.*FakeConvertLayerTest.*dataPrecision=bf16.*)",
        // Need to generate sequence exactly in the i64 data type. Enable in scope of i64 enabling.
        R"(.*RandomUniformLayerTestCPU.*OutPrc=i64.*)",
        // Issue: 123815 (Tests are sensintive to available thread count on testing machines)
        R"(.*smoke_Snippets_MHA_.?D_SplitDimensionM_static.*)",
        // Issue: 126095
        R"(^smoke_Multinomial(?:Static|Dynamic)+(?:Log)*.*seed_g=0_seed_o=0.*device=CPU.*)",
        // Issue: 129931
        R"(smoke_LPT/ConvolutionTransformation.CompareWithRefImpl/f32_\[.*,3,16,16\]_CPU_f32_rank=4D_fq_on_data=\{level=256_shape=\[1\]_input_low=\{ 0 \}_input_high=\{ 255 \}_output_low=\{ .*18.7 \}_output_high\{ 18.8 \}_precision=\}_fq_on_weights=\{_255_\[6,1,1,1\]_\{ .*1.52806e.*39, .*0.2, .*0.3, .*0.3, .*0.2, .*0.1 \}_\{ 1.52806e.*39, 0.2, 0.3, 0.3, 0.2, 0.1 \}\})",
        // TODO: 141068
        R"(smoke_Snippets_FQDecomposition.*netPRC=f16_D=CPU.*)",
        // Issue: 160734
        R"(.*smoke_LPT/ConvolutionTransformation.CompareWithRefImpl/f32_\[(1|4),3,16,16\]_CPU_f32_rank=4D_fq_on_data=\{level=256_shape=\[1\]_input_low=\{ 0 \}_input_high=\{ 255 \}_output_low=\{ -18.7 \}_output_high\{ 18.8 \}_precision=\}_fq_on_weights=\{_255_\[1\]_\{ -18.7 \}_\{ 18.7 \}\}.*)",
        // Issue: 160735
        R"(.*smoke_TestsDFT_4d/DFTLayerTest.Inference/.*TS=\{\((10.4.20.32.2|1.120.128.1.2)\)\}.*Precision=f32.*signal_size=\(\).*)",
        // by calc abs_threshold with expected value
        R"(.*smoke_.*_4D.*/GatherLayerTestCPU.CompareWithRefs.*INFERENCE_PRECISION_HINT=bf16.*)",
        R"(.*smoke.*Mvn6LayerTest.Inference/.*TS.*1.10.5.7.8.*_ModelType=f32.*_Ax=\((2.3.4|-3.-2.-1)\).*)",
        R"(.*smoke.*Mvn6LayerTest.Inference/.*TS.*2.55.*_ModelType=f32.*)",
        R"(.*smoke_ConvWithZeroPointFuse/ConvWithZeroPointFuseSubgraphTest.CompareWithRefs.*)",
        R"(.*smoke_FakeQuantize/FakeQuantizeLayerTest.Inference.*TS=.*3.49.7.5.6.*LEVELS=(255|256).*netPRC=f32.*)",
        R"(.*smoke_FakeQuantize/FakeQuantizeLayerTest.Inference.*TS=.*(2.16.4.3.18|3.10.2.5.6|3.49.5.6|2.16.3.18|2.8.5.18|3.10.5.6|2.8.1.5.18).*LEVELS=255.*netPRC=f32.*)",
        R"(.*smoke_FakeQuantize.*/FakeQuantizeLayerTest.Inference.*TS=.*3.4.2.5.*LEVELS=255.*)",
        R"(.*smoke_FakeQuantizePerChannel.*/FakeQuantizeLayerTest.Inference.*TS=.*11.10.22.19.*LEVELS=(255|256).*netPRC=f32.*)",
        R"(.*smoke_MVN_5D/Mvn6LayerTest.Inference.*TS=.*3.4.2.5.*LEVELS=255.*netPRC=f16.*)",
        R"(.*smoke_static/ConvertFqRnnToQuantizedRnn.*2.1.5.*2.1.1.*2.1.1.*)",
        R"(.*smoke_InterpolateBicubicPillow_Layout_Test/InterpolateLayerCPUTest.CompareWithRefs/ShapeCalcMode=sizes_IS=\[?.2..20.?.?\]_TS.*1.17.4.4.*2.3.10.12.*1.17.4.4.*Sizes.*4.4.*10.20.*10.4.*PARAMETER.*0.0.0.0.*0.0.1.1.*2.3.*)",
        R"(.*smoke_LoopForCommon/LoopLayerCPUTest.CompareWithRefs/.*_netType=bf16.*)",
        R"(.*smoke_FuseScaleShiftAndFakeQuantize/FuseScaleShiftAndFakeQuantizeTest.CompareWithRefs/.*Scale=\[ 30 \]_Shift=\[ 17 \]_Intervals=\[ -1 \],\[ 5 \],\[ -5 \],\[ 1 \].*)",
        R"(.*smoke_QuantizedConvolutionBatchNorm.*/QuantizedConvolutionBatchNorm.CompareWithRefs/conv_type=convolution_quantize.*)",
        R"(.*smoke_Param/RandomUniformLayerTestCPU.CompareWithRefs/IS=\{3\}_OS=\[4,3,210\]_Min=-50_Max=0_ShapePrc=.*_OutPrc=f32_GlobalSeed=8_OperationalSeed=(0|3).*)",
        R"(.*smoke_Param/RandomUniformLayerTestCPU.CompareWithRefs/IS=\{3\}_OS=\[4,3,210\]_Min=-50_Max=50_ShapePrc=.*_OutPrc=f32_GlobalSeed=8_OperationalSeed=(5|3|0).*)",
        R"(.*smoke_Param/RandomUniformLayerTestCPU.CompareWithRefs/IS=\{3\}_OS=\[4,3,210\]_Min=-50_Max=50_ShapePrc=.*_OutPrc=f32_GlobalSeed=0_OperationalSeed=5.*)",
        R"(.*smoke_Param/RandomUniformLayerTestCPU.CompareWithRefs/IS=\{1\}_OS=\[500\]_Min=-50_Max=50_ShapePrc=.*_OutPrc=f32_GlobalSeed=0_OperationalSeed=5.*)",
        R"(.*smoke.*/RNNCellCPUTest.CompareWithRefs.*activations=.*relu.*INFERENCE_PRECISION_HINT=bf16.*)",
        R"(.*smoke_InterpolateBicubicPillow_Layout_Test/InterpolateLayerCPUTest.CompareWithRefs/ShapeCalcMode=sizes_IS=\[\?.2..20.\?.\?\]_TS=\(1.17.4.4\)_\(2.3.10.12\)_\(1.17.4.4\)_Sizes=\(4.4\)_\(10.20\)_\(10.4\)_PARAMETER.*P.*.1.1.*.*)",
        R"(.*smoke_InterpolateBicubicPillow_Layout_Test/InterpolateLayerCPUTest.CompareWithRefs/ShapeCalcMode=scales_IS=\[\?.2..20.\?.\?\]_TS=\(1.11.4.4\)_\(2.7.6.5\)_\(1.11.4.4\)_Scales=\(1.25.0.75\)_CONSTANT_.*PB=\(0.0.0.0\)_PE=\(0.0.1.1\).*)",
        R"(.*smoke_Conv_Sum_Broadcast_BF16/ConvSumInPlaceTest.CompareWithRefs.*INFERENCE_PRECISION_HINT=bf16.*)",
        R"(.*smoke_Interpolate_Basic_Down_Sample_Tail/InterpolateLayerTest.Inference.*InterpolateMode=cubic_ShapeCalcMode=scales_CoordinateTransformMode=(pytorch_half_pixel|half_pixel).*netType=f32.*)",
        R"(.*smoke_basic/PermConvPermConcat.CompareWithRefs/IS=\(1.1.8.16\)_KS=\(1.5\)_OC=.*_ET=f32_targetDevice=CPU.*)",
        R"(.*smoke_basic/PermConvPermConcat.CompareWithRefs/IS=\(1.1.7.32\)_KS=\(1.3\)_OC=.*_ET=f32_targetDevice=CPU.*)",
        R"(.*smoke_BasicNegative/RangeAddSubgraphTest.*Step=-0.1_ET=f16.*)",
        R"(.*smoke_ConvertRangeSubgraphCPUTest/ConvertRangeSubgraphCPUTest.CompareWithRefs.*bf16.*)",
        R"(.*smoke_FQLayerDQBias_4D.*FQLayerDQBias.smoke_CompareWithRefs.*_TS=\(\(1.3.64.64\)_\)_layer_type=MatMul.*)",
        R"(.*smoke_Snippets_ConvMul/ConvEltwise.CompareWithRefImpl/IS\[0\]=\(1.10.16.16\)_IS\[1\]=\(1.10.16.16\)_Op=Multiply_#N=6_#S=1.*)",
        R"(.*smoke_InterpolateBicubicPillow_LayoutAlign_Test/InterpolateLayerCPUTest.CompareWithRefs/.*Sizes=\(6.8\).*)",
        R"(.*smoke_RDFT_CPU_1D/RDFTTestCPU.CompareWithRefs/prec=f32_.*TS0=\(\((106|246|245|510|1022)\)\).*)",
        R"(.*smoke_RDFT_CPU_2D/RDFTTestCPU.CompareWithRefs/prec=f32_.*_TS0=\(\((1022.64|24.39|126.32|510.64)\)\)_constAxes=true_axes=\(\(0.1\)\)_isInverse=false_primitive=jit_avx2.*)",
        R"(.*smoke_RDFT_CPU_2D/RDFTTestCPU.CompareWithRefs/prec=f32_.*_TS0=\(\((1022.64|126.32|510.64)\)\)_constAxes=true_axes=\(\(0\)\)_isInverse=false_primitive=jit_avx2.*)",
        R"(.*smoke_RDFT_CPU_2D/RDFTTestCPU.CompareWithRefs/prec=f32_.*_isInverse=false_primitive=jit_avx512.*)",
        R"(.*smoke_RDFT_CPU_2D/RDFTTestCPU.CompareWithRefs/prec=f32_.*_TS0=\(\((20.126|20.510|20.1022)\)\)_constAxes=true_axes=\(\(1\)\)_isInverse=false_primitive=jit_avx512.*)",
        R"(.*smoke_TestsDFT_3d/DFTLayerTest.Inference/.*TS=.*1.120.128.1.2.*_Precision=f32.*signal_size=\(\).*)",
        R"(.*smoke_TestsDFT_2d/DFTLayerTest.Inference.*TS=\{\(1.120.128.1.2\)\}_Precision=f32_Axes=\(2.1\)_signal_size=\(\)_Inverse=0.*)",
        R"(.*smoke_FakeQuantizeLayerCPUTest_4D_(jit|ref)/FakeQuantizeLayerCPUTest.CompareWithRefs/IS=\[\?.\?.\?.\?\]_TS=\(\(4.16.6.7\)\).*inPrec=f32.*LEVELS=255.*)",
        R"(.*smoke_FakeQuantizeLayerCPUTest_5D_(jit|ref)/FakeQuantizeLayerCPUTest.CompareWithRefs/IS=\[\?.\?.\?.\?.\?\]_TS=\(\((4|3).16.6.7.8\)\).*inPrec=f32.*LEVELS=255.*)",
        R"(.*smoke_FakeQuantizeLayerCPUTest_Decompos/FakeQuantizeLayerCPUTest.CompareWithRefs/IS.*\(\((4.5.6.7|1.1.6.7|1.1.6.1|1.5.1.6)\)\)_inPrec=f32.*LEVELS=255.*)",
        R"(.*smoke_CompareWithRefs/LRNLayerCPUTest.CompareWithRefs/f32_IS.*axes=\(1.2.3\).*)",
        R"(.*smoke_RDFT_CPU_4D/RDFTTestCPU.CompareWithRefs/prec=f32_IS0=\[\]_TS0=\(\(9.16.32.126\)\)_constAxes=true_axes=\(\((0.1.2.3|3.1|_2._1)\)\).*isInverse=false.*)",
        R"(.*smoke_RDFT_CPU_4D/RDFTTestCPU.CompareWithRefs/prec=f32_IS0=\[\]_TS0=\(\(1.192.36.64\)\)_constAxes=true_axes=\(\((0.1.2.3|3.2|_2._1|0.1|1)\)\).*isInverse=false.*)",
        R"(.*smoke_RDFT_CPU_4D/RDFTTestCPU.CompareWithRefs/prec=f32_IS0=\[\]_TS0=\(\(1.192.36.64\)\)_constAxes=true_axes=\(\((0|_2._1|0.1.2.3)\)_.*isInverse=false.*)",
        R"(.*smoke_RDFT_CPU_4D/RDFTTestCPU.CompareWithRefs/prec=f32_IS0=.*_TS0=\(\(1.192.36.64\)_.*constAxes=false.*isInverse=false.*)",
        R"(.*smoke_RDFT_CPU_4D/RDFTTestCPU.CompareWithRefs/prec=f32_IS0=\[\]_TS0=\(\(46.10.128.65\)\)_constAxes=true_axes=\(\((1.0|0.1.2.3|3.1|_2._1)\)\).*isInverse=false.*primitive=jit_avx512.*)",
        R"(.*smoke_RDFT_CPU_4D/RDFTTestCPU.CompareWithRefs/prec=f32_IS0=\[\]_TS0=\(\(10.46.128.65\)\)_constAxes=true_axes=\(\((0.1|1.2)\)\).*isInverse=false.*primitive=jit_avx512.*)",
        R"(.*smoke_RDFT_CPU_4D/RDFTTestCPU.CompareWithRefs/prec=f32_IS0=\[\?.192.36.64\]_.*_axes=\(\((0|_2._1|_1|1)\)_.*isInverse=false.*)",
        R"(.*smoke_RDFT_CPU_4D/RDFTTestCPU.CompareWithRefs/prec=f32_IS0=\[\]_TS0=\(\((1.120.64.64|1.120.96.96|\?.\?.\?.\?|1.192.\?.\?|1..2.\?.\?.1..100)\)\).*isInverse=false.*)",
        R"(.*smoke_RDFT_2d/RDFTLayerTest.Inference/IS=\(100.16\)_modelType=f32_Axes=\((0.1|_2._1|1.0)\)_SignalSize=\(\).*)",
        // Issue: 138520
        R"(.*smoke_MM_Static/MatMulLayerCPUTest.CompareWithRefs/MatMul_IS=\[\]_\[\]_TS=\(\(55.12\)\)_\(\(12.55\)\)_.*\(INFERENCE_PRECISION_HINT=bf16_\)_Fused=Multiply\(PerChannel\)_primitive=jit_gemm.*)",
        R"(.*smoke_FC_3D_BF16/MatMulLayerCPUTest.CompareWithRefs/FullyConnected_IS=\[\]_\[\]_TS=\(\(1.32.120\)\)_\(\(120.5\)\).*config=\(INFERENCE_PRECISION_HINT=bf16_\)_Fused=Multiply\(PerChannel\)_primitive=jit_gemm.*)",
        R"(.*smoke_FC_3D_BF16/MatMulLayerCPUTest.CompareWithRefs/FullyConnected_IS=\[\]_\[\]_TS=\(\(1.32.120\)\)_\(\(120.50\)\).*config=\(INFERENCE_PRECISION_HINT=bf16_\)_Fused=Multiply\(PerChannel\)_primitive=jit_gemm.*)",
        R"(.*smoke_FC_3D_BF16/MatMulLayerCPUTest.CompareWithRefs/FullyConnected_IS=\[\]_\[\]_TS=\(\(1.1.120\)\)_\(\(120.120\)\).*config=\(INFERENCE_PRECISION_HINT=bf16_\)_Fused=Multiply\(PerChannel\)_primitive=jit_gemm.*)",
        R"(.*smoke_FC_3D_BF16/MatMulLayerCPUTest.CompareWithRefs/FullyConnected_IS=\[\]_\[\]_TS=\(\(3.1.120\)\)_\(\(120.120\)\).*config=\(INFERENCE_PRECISION_HINT=bf16_\)_Fused=Multiply\(PerChannel\)_primitive=jit_gemm.*)",
        R"(.*smoke_FC_3D_BF16/MatMulLayerCPUTest.CompareWithRefs/FullyConnected_IS=\[.*\]_\[.*\]_TS=\(\(1.5.32\)_\(1.5.32\)\).*config=\(INFERENCE_PRECISION_HINT=bf16_\)_Fused=Multiply\(PerChannel\)_primitive=jit_gemm.*)",
        R"(.*smoke_FC_3D_BF16/MatMulLayerCPUTest.CompareWithRefs/FullyConnected_IS=\[..60...60...60\]_\[14.10\]_TS=\(\(1.3.14\)_\(1.7.14\)\)_\(\(14.10\)_\(14.10\)\)_.*config=\(INFERENCE_PRECISION_HINT=bf16_\)_Fused=Multiply\(PerChannel\)_primitive=jit_gemm.*)",
        R"(.*nightly_FC_3D_BF16/MatMulLayerCPUTest.CompareWithRefs/FullyConnected_IS=\[\]_\[\]_TS=\(\(1.32.120\)\)_\(\(120.5.*\)\)_.*config=\(INFERENCE_PRECISION_HINT=bf16_\)_Fused=Multiply\(PerChannel\)_primitive=jit_gemm.*)",
        R"(.*nightly_FC_3D_BF16/MatMulLayerCPUTest.CompareWithRefs/FullyConnected_IS=\[\?.\?.50\]_\[50.7\]_TS=\(\(1.2.50\)_\(1.10.50\)_\(1.2.50\)_\(2.2.50\)\)_\(\(50.7\)_\(50.7\)_\(50.7\)_\(50.7\)\)_.*config=\(INFERENCE_PRECISION_HINT=bf16_\)_Fused=Multiply\(PerChannel\)_primitive=jit_gemm.*)",
        R"(.*smoke_MM_Dynamic_Fusing/MatMulLayerCPUTest.CompareWithRefs/MatMul_IS=\[\?.\?\]_\[\?.33\]_TS=\(\(16.12\)_\(33.7\)_\(16.12\)\)_\(\(12.33\)_\(7.33\)_\(12.33\)\)_transpose_a=0_transpose_b=0_secondaryInputType=PARAMETER_.*config=\(INFERENCE_PRECISION_HINT=bf16_\)_Fused=Multiply\(PerChannel\)_primitive=jit_gemm.*)",
        R"(.*(nightly|smoke)_MM_Brgemm_Static/MatMulLayerCPUTest.CompareWithRefs/MatMul_IS=\[\]_\[\]_TS=\(\(55.12\)\)_\(\(12.55\)\)_.*config=\(INFERENCE_PRECISION_HINT=bf16_\)_Fused=Multiply\(PerChannel\)_primitive=brgemm_avx512.*)",
        R"(.*smoke_MM_Brgemm_Dynamic_Fusing/MatMulLayerCPUTest.CompareWithRefs/MatMul_IS=\[\?.\?\]_\[\?.33\]_TS=\(\(16.12\)_\(33.7\)_\(16.12\)\)_\(\(12.33\)_\(7.33\)_\(12.33\)\)_transpose_a=0_transpose_b=0_secondaryInputType=PARAMETER_netPRC=f32_inPRC=dynamic_outPRC=dynamic_trgDev=CPUconfig=\(INFERENCE_PRECISION_HINT=bf16_\)_Fused=Multiply\(PerChannel\)_primitive=brgemm_avx512.*)",
        // Issue: 140389
        R"(.*FQLayerDQBias.smoke_CompareWithRefs.*)",
        R"(.*smoke_matmulBrgemmInt8/MatmulBrgemmInt8Test.CompareWithRefs.*MatMul.*InputType=i8_OutputType=i8.*)",
        R"(.*smoke_Snippets_MHAWOTransposeOnInputs_4D/MHAWOTransposeOnInputs.CompareWithRefImpl.*)",
        // Issue: 142448
        R"(smoke_Snippets_BroadcastSelect_Dynamic.*)",
        // Issue: 141705
        R"(.*smoke_LoopForCommon/LoopLayerCPUTest.CompareWithRefs/.*trip_count=5_exec_cond=1_netType=i8.*)",
        R"(.*smoke_LoopForCommon/LoopLayerCPUTest.CompareWithRefs/Input0_IS=\[\?.1.\?\]_TS=\(10.1.10\)_\(1.1.1\)_\(1.1.1\)_\(5.1.3\)_Input1_IS=\[\?.\?.\?\]_TS=.*_Input2_IS=\[\?.1.\?\]_.*_types=0_0_1_trip_count_type=.*_trip_count=(1|5)_exec_cond=1_netType=i8.*)",
        R"(.*smoke_LoopForCommon/LoopLayerCPUTest.CompareWithRefs/Input0_IS=\[1..10.1.1..10\]_.*_Input1_IS=\[1..8.1.1..8\]_.*_Input2_IS=\[1..10.\?.1..10\]_TS=.*_types=0_0_1_trip_count_type=.*_trip_count=(1|5)_exec_cond=1_netType=i8.*)",
        // Issue: 142997
        R"(.*smoke_TestsROIAlign.*)",
        // Issue: 136881
        R"(.*smoke_CompareWithRefs_4D_BitwiseShift_overflow_i32_cast.*_eltwise_op_type=BitwiseLeft.*_model_type=.*(i16|u16).*)",
        // Issue: 163083
        // Issue: 163116
        R"(.*RandomUniformLayerTestCPU.*OutPrc=bf16.*)",
        // Issue: 163117
        R"(.*InterpolateCubic_Layout_Test.*)",
        // Issue: 163171
        R"(.*CPUDetectionOutputDynamic3InLargeTensor.*)",
        // Issue: 163168
        R"(.*UniqueLayerTestCPU.*)",
        // Issue: 163175
        R"(.*GridSampleLayerTestCPU.*dataPrc=i8.*)",
        R"(.*GridSampleLayerTestCPU.*dataPrc=bf16.*)",
        // Issue: 163177
        R"(.*NmsRotatedOpTest.*ScoreThr=0\.4.*)",
        // Issue: 163222
        R"(.*bf16.*LSTMSequenceCPUTest.*)",
        // Issue: 163223
        R"(.*bf16.*AUGRUSequenceCPUTest.*)",
        // Issue: 163224
        R"(.*bf16.*GRUSequenceCPUTest.*)",
        // Issue: 163227
        R"(.*QuantizedModelsTests\.MaxPoolFQ.*)",
        R"(.*QuantizedModelsTests\.MaxPoolQDQ.*)",
        // Issue: 163268
        R"(.*QuantizedModelsTests\.ConvolutionQDQ.*)",
        R"(.*QuantizedModelsTests\.ConvolutionFQ.*)",
        // Issue: 163230
        R"(.*ProposalLayerTest.*)",
        // Issue: 163232
        R"(.*FC_3D_BF16.*MatMulLayerCPUTest.*)",
        // Issue: 163242
        R"(.*bf16.*RNNSequenceCPUTest.*)",
        // Issue: 163250
        R"(.*OnnxModelWithExtensionFromDSO.*)",
        // Issue: 163273
        // todo: define correct area
        R"(.*Deconv_2D_Planar_FP16.*DeconvolutionLayerCPUTest.*)",
        // Issue: 163275
        R"(.*NoReshapeAndReshapeDynamic.*CodegenGelu.*)",
        // Issue: 163351
        R"(.*CoreThreadingTestsWithIter.*nightly_AsyncInfer_ShareInput.*)",
        // This transformation is disabled on CPU
        R"(.*smoke_LPT.*MultiplyToGroupConvolutionTransformation.*)",
    };

    // fp32 floor for bf16 models: conversion issue
    retVector.emplace_back(R"(.*smoke.*ActivationLayerCPUTest.*CompareWithRefs/(Floor|Ceiling)_.*netPRC=bf16.*)");

#if defined(OPENVINO_ARCH_X86)
    retVector.emplace_back(R"(.*DetectionOutputLayerTest.*)");
    // WIP: plugin cannot be loaded for some reason
    retVector.emplace_back(R"(.*IEClassBasicTestP.*)");
#elif defined(OPENVINO_ARCH_ARM64) || defined(OPENVINO_ARCH_ARM)
    {
        retVector.emplace_back(
            R"(smoke_CompareWithRefs_static_check_collapsing/EltwiseLayerTest.Inference/IS.*_eltwise_op_type=Div_secondary_input_type=PARAMETER_opType=VECTOR_model_type=i32_InType=dynamic_OutType=dynamic_trgDev=CPU.*)");
        // Issue: 123321
        retVector.emplace_back(
            R"(.*smoke_RNNSequenceCommonZeroClip/RNNSequenceTest.Inference.*hidden_size=1.*relu.*direction=reverse.*)");
        // Ticket: 134601
        retVector.emplace_back(R"(.*smoke_GroupNormalization.*)");
        // by calc abs_threshold with expected value
        retVector.emplace_back(
            R"(.*smoke_Interpolate_Basic_Down_Sample_Tail/InterpolateLayerTest.Inference.*InterpolateMode=(linear|linear_onnx)_ShapeCalcMode=scales_CoordinateTransformMode=half_pixel.*PE=\(0.0.0.0\).*netType=f32.*)");
        retVector.emplace_back(R"(.*smoke_ConversionLayerTest/ConversionLayerTest.Inference/conversionOpType=Convert_.*_inputPRC=f16_targetPRC=(u8|i8).*)");
        retVector.emplace_back(R"(.*smoke_Decomposition_4D/Mvn6LayerTest.Inference/.*TS=\{\((1.16.5.8|2.19.5.10)\)\}_ModelType=f32_.*_Ax=\(0.1.2.3\)_NormVariance=FALSE.*)");
        retVector.emplace_back(R"(.*smoke_Decomposition_4D/Mvn6LayerTest.Inference/.*TS=\{\(2.19.5.10\)\}_ModelType=f32_.*_Ax=\(1\).*)");
        retVector.emplace_back(R"(.*smoke_LogSoftmax4D/LogSoftmaxLayerTest.Inference/.*TS=\{\(2.3.4.5\)\}_modelType=f32_axis=(-4|-3|-2|0|1|2).*)");
        retVector.emplace_back(R"(.*smoke_Interpolate_Basic/InterpolateLayerTest.Inference/.*InterpolateMode=cubic_ShapeCalcMode=sizes_CoordinateTransformMode=tf_half_pixel.*PB=\(0.0.0.0\)_PE=\(0.0.1.1\)_.*netType=f32.*)");
        retVector.emplace_back(R"(.*smoke_CompareWithRefs_4D_Bitwise.*/EltwiseLayerCPUTest.*_eltwise_op_type=Bitwise.*_model_type=i32_.*)");
        // Ticket: 144845
        retVector.emplace_back(R"(.*LSTMCellFusion/LSTMCellFusionWithSplitWeights.SubgraphFusedToLSTMCell/(1|8|15))");
        // Ticket: 131541
        retVector.emplace_back(R"(.*smoke_MulticlassNmsLayerTest_dynamic2.*_outType=i32_.*)");
        // Ticket: 162434
        retVector.emplace_back(R"(smoke_LPT/MatMulTransformation.*)");
        // Ticket: 162260
        retVector.emplace_back(R"(smoke_Snippets_FQDecomposition.*netPRC=f32_D=CPU.*)");
        // Ticket: 166771
        retVector.emplace_back(R"(.*smoke_BroadcastEltwise/BroadcastEltwise.smoke_CompareWithRefs.*)");
        // Ticket: 168863
        retVector.emplace_back(R"(.*smoke_AvgPoolV14_CPU_4D/AvgPoolingV14LayerCPUTest.CompareWithRefs.*)");
        // Ticket: 168931
        retVector.emplace_back(R"(.*smoke_Reduce_OneAxis_dynamic_CPU/ReduceCPULayerTest.CompareWithRefs.*)");
    }
    // invalid test: checks u8 precision for runtime graph, while it should be f32
    retVector.emplace_back(R"(smoke_NegativeQuantizedMatMulMultiplyFusion.*)");
    // int8 specific
    retVector.emplace_back(R"(smoke_Quantized.*)");
    // Issue: 124309
    retVector.emplace_back(R"(.*InferRequestPreprocessConversionTest.*oLT=NHWC.*)");
    retVector.emplace_back(R"(.*smoke_NoReshape/OVCompiledModelGraphUniqueNodeNamesTest.CheckUniqueNodeNames.*)");
    retVector.emplace_back(R"(.*smoke_BehaviorTests/InferRequestPerfCountersTest.CheckOperationInPerfMap.*)");
    retVector.emplace_back(
        R"(smoke_ExecGraph/ExecGraphRuntimePrecision.CheckRuntimePrecision/Function=FakeQuantizeBinaryConvolution.*)");
    // Issue: 124395
    retVector.emplace_back(R"(smoke_VariableStateBasic/InferRequestVariableStateTest.*)");
    retVector.emplace_back(R"(smoke_VariableState/OVInferRequestVariableStateTest.*)");
    // Issue: 141705
    retVector.emplace_back(R"(.*smoke_arm_Deconv_2D_Planar_FP16/DeconvolutionLayerCPUTest.*INFERENCE_PRECISION_HINT=f16.*)");
    retVector.emplace_back(R"(.*ConcatMultiQuerySDPTest.*u8.*)");
    retVector.emplace_back(R"(.*smoke_ConcatSDPTransposeByChannelTest.*)");
    // Issue: 168490
    retVector.emplace_back(R"(.*CPU/CoreThreadingTest.smoke_QueryModel.*)");
#endif

#if defined(OPENVINO_ARCH_ARM)
    // Issue: 144998
    retVector.emplace_back(R"(.*smoke_CachingSupportCase_CPU.*_(i8|u8).*)");
    retVector.emplace_back(R"(.*smoke_Hetero_CachingSupportCase.*_(i8|u8).*)");
    // TODO: rounding errors
    retVector.emplace_back(R"(.*iv_secondaryInputType=PARAMETER_opType=VECTOR_NetType=i32.*)");
    // not supported
    retVector.emplace_back(R"(.*fma.*EltwiseLayerCPUTest.*)");
    retVector.emplace_back(R"(.*int_jit.*EltwiseLayerCPUTest.*)");
    retVector.emplace_back(R"(.*dyn.*EltwiseChainTest.*)");
    retVector.emplace_back(R"(.*smoke_EltwiseChain_MergeConvert_int8/.*InPRC0=i8.*Conversion=i8.*)");
    retVector.emplace_back(R"(.*smoke_EltwiseChain_MergeConvert_int8/.*InPRC0=u8.*Conversion=i8.*)");
    retVector.emplace_back(R"(.*smoke_EltwiseChain_MergeConvert_int8/.*InPRC0=i16.*Conversion=i8.*)");
    retVector.emplace_back(R"(.*smoke_EltwiseChain_MergeConvert_int8/.*InPRC0=u16.*Conversion=i8.*)");
    retVector.emplace_back(R"(.*smoke_EltwiseChain_MergeConvert_int8/.*InPRC0=i32.*Conversion=i8.*)");
    // by calc abs_threshold with expected value
    retVector.emplace_back(R"(.*smoke_CompareWithRefs_static/EltwiseLayerTest.*_eltwise_op_type=Div_.*_model_type=i32_.*)");
    // int8 / code-generation specific
    retVector.emplace_back(R"(smoke_LPT.*)");
    retVector.emplace_back(R"(.*smoke_RoPETest.*)");
#endif

#if defined(OPENVINO_ARCH_ARM64)
    // Issue: 149216. For low precision model from original framework, Snippets PropagatePrecision should insert ConvertTruncation instead
    // of ConvertSaturation when converting larger integer to smaller integer to align with c++ standard and ngraph reference.
    retVector.emplace_back(R"(.*smoke_EltwiseChain_MergeConvert_int8/.*Op0=Prod.*Conversion=i8.*)");
#endif

#if defined(OPENVINO_ARCH_RISCV64)
    // object is not initialized
    retVector.emplace_back(R"(.*StaticLoopDynamicSubgraphCPUTest.smoke_StaticLoopWithDynSubgraph.*)");
    // crash with 'double free or corruption (!prev)'
    retVector.emplace_back(R"(.*smoke_InterpolateBilinearPillow_Layout_Test.*)");
    retVector.emplace_back(R"(.*smoke_InterpolateBicubicPillow_Layout_Test.*)");
    // unsupported node type 'CausalMaskPreprocess'
    retVector.emplace_back(R"(CausalMaskPreprocessCausalMaskPreprocess.smoke_CompareWithRefs)");
    // fused op FakeQuantize has not been found
    retVector.emplace_back(R"(ConvAndFQWithSharedConstants.smoke_ConvAndFQWithSharedConstants_CPU)");
    // subgraphs code-generator is not supported on non-x64 platforms
    retVector.emplace_back(R"(SubgraphSnippetSerializationTest.smoke_SerializeSubgraph(WithScalarConst)?)");
    retVector.emplace_back(R"(.*SubgraphWithBlockedFormat.*)");
    // unsupported layout
    retVector.emplace_back(R"(GatherAddAvgpool.smoke_CompareWithRefs)");
    retVector.emplace_back(R"(smoke_StaticAdaPoolAvg(4|5)DLayoutTest/AdaPoolLayerCPUTest.*_outFmts=nd?hwc_1)");
    retVector.emplace_back(R"(.*smoke_CompareWithRefs_Mvn(4|5)D(_Static)?/MvnLayerCPUTest.CompareWithRefs.*inFmts=nd?hwc.*)");
    retVector.emplace_back(R"(.*smoke_TopK(_int32|_bubble_BLK_on_channel_horiz)?(_dynamic)?/TopKLayerCPUTest.CompareWithRefs.*inFmts=(nhwc|nChw8c|nChw16c).x.*)");
    retVector.emplace_back(R"(.*smoke_(Group)?Convolution(2|3)D/ConvConcatSubgraphTest.CompareWithRefs.*)");
    retVector.emplace_back(R"(.*smoke_FakeQuantizeCache_(4|5)D/FakeQuantizeCacheTest.CompareWithRefs.*inFmts=(nhwc|nChw8c|ndhwc|nCdhw8c).*)");
    retVector.emplace_back(R"(.*ReduceCPULayerTest.CompareWithRefs.*inFmts=nhwc.*)");
    // only infer_precision=f32 is supported on riscv64 platforms
    retVector.emplace_back(R"(.*smoke_CompareWithRefs_(4|5)D.*EltwiseLayerCPUTest.CompareWithRefs.*INFERENCE_PRECISION_HINT=f16.*)");
    retVector.emplace_back(R"(.*smoke_CompareWithRefs_Mvn[12345]D.*/MvnLayerCPUTest.CompareWithRefs.*INFERENCE_PRECISION_HINT=f16.*)");
    retVector.emplace_back(R"(.*ReduceCPULayerTest.CompareWithRefs.*INFERENCE_PRECISION_HINT=f16.*)");
    retVector.emplace_back(R"(.*RemoveUselessFP16ConvertCPUTest.*INFERENCE_PRECISION_HINT=f16.*)");
    // fused op Add has not been found
    retVector.emplace_back(R"(.*smoke_CompareWithRefs_fma_(4|5)D/EltwiseLayerCPUTest.CompareWithRefs.*)");
    // primType is unexpected
    retVector.emplace_back(R"(.*smoke_Param(Const)?/RandomUniformLayerTestCPU.CompareWithRefs.*)");
    retVector.emplace_back(R"(.*smoke_Reduce_Int32_CPU/ReduceCPULayerTest.CompareWithRefs.*)");
    retVector.emplace_back(R"(.*smoke_(static|dynamic)Shapes4D(C(16|32))?(_Transpose|_PermutePerChannels)/TransposeLayerCPUTest.CompareWithRefs.*netPRC=f32.*INFERENCE_PRECISION_HINT=f16.*)");
    retVector.emplace_back(R"(.*smoke_(static|dynamic)_1D/GatherLayerTestCPU.CompareWithRefs.*)");
    retVector.emplace_back(R"(.*smoke_RDFT_CPU_(1|2|4)D/RDFTTestCPU.CompareWithRefs.*)");
    retVector.emplace_back(R"(.*smoke_CompareWithRefs(Numpy|None)_dynamic/SelectLayerCPUTest.CompareWithRefs.*)");
    retVector.emplace_back(R"(.*smoke_Check/ConvPoolActivTest.CompareWithRefs.*)");
    retVector.emplace_back(R"(.*smoke_Conv_Sum_(1x1_)?Broadcast(_FP32|_Strided|_INT8|_Several_Consumers|_StaticShape)?/Conv(1x1)?Sum(InPlace(Test(Int8|SeveralConsumers)?|Strided)?|(Unsupported)?BroadcastTest).CompareWithRefs.*)");
    retVector.emplace_back(R"(.*smoke_ReshapeFc/ReshapeFcCPUTest.CompareWithRefs.*)");
    // dimensions of shapes are mismatched
    retVector.emplace_back(R"(.*CPUDetectionOutputDynamic3InLargeTensor/DetectionOutputLayerCPUTest.CompareWithRefs.*varEnc=0.*)");
    // cannot get dims for non static shape
    retVector.emplace_back(R"(.*nightly_/NmsRotatedOpTest.CompareWithRefs/IS=\(\[\]_\[\]\)_TS=\{\(3.11.5\)_\(3.15.11\)\}__BoxPrc=f16_MaxPrc=i64_ThrPrc=f16_OutPrc=i64_MaxBox=10.*ConstIn=\{True,True,True,True,True\}_Device=CPU.*)");
    retVector.emplace_back(R"(nightly_/NmsRotatedOpTest.CompareWithRefs/IS=\(\[\]_\[\]\)_TS=\{\(15.29.5\)_\(15.31.29\)\}__BoxPrc=f16_MaxPrc=i64_ThrPrc=f16_OutPrc=i64_MaxBox=10_IouThr=0.5_ScoreThr=0.4_SortDesc=False_Clockwise=True_ConstIn=\{True,True,True,True,True\}_Device=CPU)");
    retVector.emplace_back(R"(nightly_/NmsRotatedOpTest.CompareWithRefs/IS=\(\[\]_\[\]\)_TS=\{\(21.64.5\)_\(21.32.64\)\}__BoxPrc=f16_MaxPrc=i64_ThrPrc=f16_OutPrc=i64_MaxBox=10_IouThr=0.5_ScoreThr=0.4_SortDesc=False_Clockwise=True_ConstIn=\{True,True,True,True,True\}_Device=CPU)");
    retVector.emplace_back(R"(nightly_/NmsRotatedOpTest.CompareWithRefs/IS=\(\[\?.\?.5\]_\[\?.\?.\?\]\)_TS=\{\(7.35.5\)_\(7.30.35\)\}_\{\(7.35.5\)_\(7.100.35\)\}_\{\(7.35.5\)_\(7.133.35\)\}__BoxPrc=f16_MaxPrc=i64_ThrPrc=f16_OutPrc=i64_MaxBox=10_IouThr=0.5_ScoreThr=0.4_SortDesc=False_Clockwise=True_ConstIn=\{True,True,True,True,True\}_Device=CPU)");
    // Accuracy problem
    retVector.emplace_back(R"(.*InterpolateCubic_Layout_Test.*)");
    retVector.emplace_back(R"(.*nightly_(static|dynamic)/UniqueLayerTestCPU.*dataPrc=i8.*)");
    retVector.emplace_back(R"(.*smoke_Interpolate_Basic/InterpolateLayerTest.Inference/IS=\(\[\]\)_TS=\{\(1.4.6.6\)\}_TS=\(1.4.8.8\)_InterpolateMode=cubic_ShapeCalcMode=sizes_CoordinateTransformMode=tf_half_pixel_for_nn_NearestMode=round_prefer_floor_cube_coef=-0.75_Antialias=0_PB=\(0.0.0.0\)_PE=\(0.0.1.1\)_Axes=\(0.1.2.3\)_Scales=\(1.1.1.33333.1.33333\)_netType=f32_trgDev=CPU.*)");
    retVector.emplace_back(R"(.*smoke_MaxPool_ExplicitPad_CeilRounding.*K\(3.3\)_S\(\d.2\).*PE\(0.2\).*)");
    // Incorrect number of input or output memory formats
    retVector.emplace_back(R"(.*smoke_(static|dynamic)/RNNCellCPUTest.CompareWithRefs.*)");
    // crash with code: 11
    retVector.emplace_back(R"(.*smoke_static/ConvertFqRnnToQuantizedRnn.CompareWithRefs.*)");
    // Fused op Elu has not been found
    retVector.emplace_back(R"(.*smoke_Check/AlignMatMulInputRanksTest.CompareWithRefs/IS_A=\[\d.+\]_IS_B=\[\d.+\]__Fused=Elu.*)");
    // Unsupported node type 'ScaledDotProductAttention'
    retVector.emplace_back(R"(.*smoke_Concat(MultiQuery)?SDP(Transpose)?Test(SetState)?/.*)");
    // Unexpected transpose count
    retVector.emplace_back(R"(.*smoke_Basic/FuseTransposeAndReorderTest3.CompareWithRefs.*)");
    // Unsupported node type 'Interaction'
    retVector.emplace_back(R"(.*smoke_Interaction/IntertactionCPUTest.CompareWithRefs.*)");
    // Unexpected count of the `Reorder` nodes
    retVector.emplace_back(R"(.*smoke_MergeTransposeReorder_(static|dynamic)/MergeTransposeReorderCPUTest.CompareWithRefs.*)");
    // Unexpected value of 'num_ops'
    retVector.emplace_back(R"(.*smoke_RoPETest.*)");
    // Unsupported node type 'ScaledDotProductAttention'
    retVector.emplace_back(R"(.*smoke_SDPAGroupBeamSearchTest/SDPAGroupBeamSearchTest.CompareWithRefs.*)");
    // Crash with code: 6
    retVector.emplace_back(R"(^smoke_VirtualPlugin_BehaviorTests/OVHoldersTest.Orders/target_device=HETERO.CPU$)");
    // Crash with code: 14, but can be launched with `--gtest_filter`
    retVector.emplace_back(R"(.*CPU/CoreThreadingTest.smoke_QueryModel.*)");
    // Target Static Shape is empty
    retVector.emplace_back(R"(.*proposal_params/.*)");
    // Quantized models unsupported
    retVector.emplace_back(R"(.*Quantized.*)");

    if (!ov::intel_cpu::riscv64::mayiuse(ov::intel_cpu::riscv64::gv)) {
        // Integer division is supported only by JIT Executor which is available on platforms with GV instruction sets.
        // In other cases there might be accuracy problems.
        retVector.emplace_back(R"(.*smoke_EltwiseChain/EltwiseChainTest.CompareWithRefs.*InPRC3=i32_Op0=Div_Op1.*)");
        retVector.emplace_back(R"(.*smoke_CompareWithRefs_static.*eltwise_op_type=Div.*model_type=i32.*)");
    }
#endif

#if !defined(OPENVINO_ARCH_X86_64)
    // very time-consuming test
    retVector.emplace_back(R"(.*OVInferConsistencyTest.*)");
    // TODO: generate new 'expected' runtime graph for non-x64 CPU
    retVector.emplace_back(R"(smoke_serialization/ExecGraphSerializationTest.ExecutionGraph.*)");
    retVector.emplace_back(
        R"(smoke_ExecGraph/ExecGraphRuntimePrecision.CheckRuntimePrecision/Function=(EltwiseWithTwoDynamicInputs|FakeQuantizeRelu).*)");
    // Issue 108803: bug in CPU scalar implementation
    retVector.emplace_back(R"(smoke_TestsDFT_(1|2|3|4)d/DFTLayerTest.CompareWithRefs.*)");
    retVector.emplace_back(R"(smoke_TestsDFT_(1|2|3|4)d/DFTLayerTest.Inference.*)");
    // Issue 88764, 91647, 108802: accuracy issue
    retVector.emplace_back(R"(MultipleLSTMCellTest/MultipleLSTMCellTest.CompareWithRefs.*)");
    // Compressed weights are not supported
    retVector.emplace_back(R"(smoke_MatMulSharedCompressedWeights.*)");
    retVector.emplace_back(R"(smoke_Model_Distribution_MatMulSharedCompressedWeights.*)");
    retVector.emplace_back(R"(smoke_MatmulAndGatherSharedWeightsDecompression.*)");
    // Issue: 170863
    retVector.emplace_back(R"(smoke_Model_Distribution_MatMul_NoTranspose.*)");
    // smoke_Snippets test cases are not supported on arm32 platforms
#if !defined(OPENVINO_ARCH_ARM64)
    retVector.emplace_back(R"(smoke_Snippets.*)");
#endif
    // smoke_Snippets test cases are not supported on arm64 platforms, except for listed below
    retVector.emplace_back(R"(smoke_Snippets(?!_Eltwise|_Convert|_Transpose|_FQDecomposition_|_BroadcastSelect|_Select|_MatMul/|_Reduce|_Softmax|_AddSoftmax).*)");
    retVector.emplace_back(R"(smoke_Snippets_TransposeMatMulBias.*)");
#endif
#if defined(_WIN32)
    retVector.emplace_back(R"(.*smoke_QuantizedConvolutionBatchNormTransposeOnWeights/QuantizedConvolutionBatchNorm.CompareWithRefs/conv_type=convolution_quantize_type=fake_quantize_intervals_type=per_(tensor|channel)_transpose_on_weights=true_device=CPU.*)");
    retVector.emplace_back(R"(.*smoke_LPT/ConvolutionTransformation.CompareWithRefImpl/f32_\[(1|4),3,16,16\]_CPU_f32_rank=4D_fq_on_data=\{level=256_shape=\[1,1,1,1\]_input_low=\{ 0 \}_input_high=\{ 255 \}_output_low=\{ -12.7 \}_output_high\{ 12.8 \}_precision=\}_fq_on_weights=\{_255_\[1,1,1,1\]_\{ -12.7 \}_\{ 12.7 \}\}.*)");
    retVector.emplace_back(R"(.*smoke_LPT/FuseDequantizeToFakeQuantizeTransformation.CompareWithRefImpl/CPU_f32_0_dynamic_\[\]_f32__\{\}_\{\}__\{ 0.01, 0.1, 1 \}_f32_\[1,3\]_1_1_.*)");
    retVector.emplace_back(R"(.*smoke_QuantizedConvolutionBatchNorm/QuantizedConvolutionBatchNorm.CompareWithRefs/conv_type=convolution_quantize_.*)");
    retVector.emplace_back(R"(.*smoke_QuantizedConvolutionBatchNorm/QuantizedConvolutionBatchNorm.CompareWithRefs/conv_type=convolution_backprop_quantize_type=(quantize_dequantize_intervals|compressed_weights_intervals).*)");
    retVector.emplace_back(R"(.*smoke_LPT/MatMulTransformation.CompareWithRefImpl/f32_CPU_\[(1|8|1,1,1),4,12,2\]_level=256_shape=\[\]_input_low=\{ (0|-12.8) \}_input_high=\{ (25.5|12.7) \}_output_low=\{ (0|-12.8) \}_output_high\{ (25.5|12.7) \}_.*)");
    retVector.emplace_back(R"(.*smoke_LPT/MatMulTransformation.CompareWithRefImpl/f32_CPU_\[(1|8|1,1,1),4,12,2\]_level=256_shape=\[\]_input_low=\{ (0|-12.8) \}_input_high=\{ (25.5|12.7) \}_output_low=\{ (0|-12.8) \}_output_high\{ (25.5|12.7) \}_.*)");
    retVector.emplace_back(
        R"(.*smoke_MatMulCompressedWeights_corner_cases_basic/MatmulWeightsDecompression.CompareWithRefs/data_shape=\[\?.\?.\?\]_\(\[1,1,4096\]\)_weights_shape=\[4096,4096\]_group_size=128_weights_precision=nf4_decompression_precision=f16_scale_precision=dynamic_transpose_weights=0_decompression_subtract=full_reshape_on_decompression=1_config=\(\).*)");
    retVector.emplace_back(R"(.*smoke_RDFT_CPU_1D/RDFTTestCPU.CompareWithRefs/prec=f32_IS0=\[\]_TS0=\(\(126\)\)_constAxes=true_axes=\(\(0\)\)_isInverse=false.*)");
    retVector.emplace_back(R"(.*smoke_RDFT_CPU_2D/RDFTTestCPU.CompareWithRefs/prec=f32_IS0=\[\]_TS0=\(\(16.38\)\)_constAxes=true_axes=\(\(0.1\)\)_isInverse=false.*)");
#endif
    if (!ov::intel_cpu::hasHardwareSupport(ov::element::bf16)) {
        // on platforms which do not support bfloat16, we are disabling bf16 tests since there are no bf16 primitives,
        // tests are useless on such platforms
        retVector.emplace_back(R"(.*(BF|bf)16.*)");
        retVector.emplace_back(R"(.*bfloat16.*)");
    }
#if defined(OPENVINO_ARCH_X86) || defined(OPENVINO_ARCH_X86_64)
    if (!ov::with_cpu_x86_avx2()) {
        // MatMul in Snippets uses BRGEMM that is supported only on AVX2 (and newer) platforms
        // Disabled Snippets MHA tests as well because MHA pattern contains MatMul
        retVector.emplace_back(R"(.*Snippets.*MHA.*)");
        retVector.emplace_back(R"(.*Snippets.*(MatMul|Matmul).*)");
    }
    if (!ov::intel_cpu::hasHardwareSupport(ov::element::f16)) {
        // Skip fp16 tests for paltforms that don't support fp16 precision
        retVector.emplace_back(R"(.*INFERENCE_PRECISION_HINT=(F|f)16.*)");
        retVector.emplace_back(R"(.*ConcatMultiQuerySDPTest.*f16.*)");
        retVector.emplace_back(R"(.*ConcatSDPTest.*f16.*)");
        retVector.emplace_back(R"(.*ConvertCPULayerTest.*f16.*)");
    }
#elif defined(OPENVINO_ARCH_ARM64) || defined(OPENVINO_ARCH_ARM)
    if (!ov::intel_cpu::hasIntDotProductSupport()) {
        retVector.emplace_back(R"(.*smoke_MatMulCompressedWeights_Kleidiai.*)");
    }
    if (!ov::intel_cpu::hasHardwareSupport(ov::element::f16)) {
        // Skip fp16 tests for paltforms that don't support fp16 precision
        retVector.emplace_back(R"(.*INFERENCE_PRECISION_HINT=(F|f)16.*)");
        retVector.emplace_back(R"(.*Prc=f16.*)");
        retVector.emplace_back(R"(.*ConcatMultiQuerySDPTest.*f16.*HasShapeOf=1.*)");
        retVector.emplace_back(R"(.*ConvertCPULayerTest.*f16.*)");
    }
#endif
#if defined(OPENVINO_ARCH_ARM)
    retVector.emplace_back(R"(.*ActivationLayerTest.*Inference.*)");
    retVector.emplace_back(R"(.*AddConvertToReorderTest.*smoke_TestAddReorder_CPU.*)");
    retVector.emplace_back(R"(.*AddOutputsTest.*smoke_CheckOutputExist.*)");
    retVector.emplace_back(R"(.*CompileModelCacheRuntimePropertiesTestBase.*CanLoadFromFileWithoutException.*)");
    retVector.emplace_back(R"(.*CompileModelCacheTestBase.*CompareWithRefImpl.*2InputSubtract_f.*)");
    retVector.emplace_back(R"(.*CompileModelCacheTestBase.*CompareWithRefImpl.*ConvPoolRelu_f.*)");
    retVector.emplace_back(R"(.*CompileModelCacheTestBase.*CompareWithRefImpl.*MatMulBias_f.*)");
    retVector.emplace_back(R"(.*CompileModelCacheTestBase.*CompareWithRefImpl.*SimpleFunctionRelu_f.*)");
    retVector.emplace_back(R"(.*CompileModelCacheTestBase.*CompareWithRefImpl/MatMulBias_f32_batch1_CPU)");
    retVector.emplace_back(R"(.*CompileModelLoadFromCacheTest.*CanGetCorrectLoadedFromCacheProperty.*)");
    retVector.emplace_back(R"(.*CompileModelLoadFromFileTestBase.*CanCreateCacheDirAndDumpBinariesUnicodePath.*)");
    retVector.emplace_back(R"(.*CompileModelLoadFromFileTestBase.*CanLoadFromFileWithoutException.*)");
    retVector.emplace_back(R"(.*CompileModelLoadFromMemoryTestBase.*CanLoadFromMemoryWithoutExecption.*)");
    retVector.emplace_back(R"(.*CompileModelLoadFromMemoryTestBase.*CanLoadFromMemoryWithoutWeightsANdExecption.*)");
    retVector.emplace_back(R"(.*CompileModelWithCacheEncryptionTest.*CanImportModelWithoutException.*)");
    retVector.emplace_back(R"(.*ConcatMultiQuerySDPTest.*f16.*)");
    retVector.emplace_back(R"(.*ConcatSDPTest.*f16.*)");
    retVector.emplace_back(R"(.*FakeConvertLayerTest.*f16.*)");
    retVector.emplace_back(R"(.*CoreThreadingTestsWithCacheEnabled.*smoke_compiled_model_cache_enabled.*)");
    retVector.emplace_back(R"(.*CoreThreadingTestsWithIter.*smoke_CompileModel.*)");
    retVector.emplace_back(R"(.*CustomOpConvertI64CPUTest.*CompareWithRefs.*)");
    retVector.emplace_back(R"(.*EltwiseLayerCPUTest.*CompareWithRefs.*INFERENCE_PRECISION_HINT=f16.*)");
    retVector.emplace_back(R"(.*EltwiseLayerTest.*Inference.*)");
    retVector.emplace_back(R"(.*ExecGraphDuplicateInputsOutputsNames.*CheckOutputsMatch.*)");
    retVector.emplace_back(R"(.*ExecGraphKeepAssignNode.*KeepAssignNode.*)");
    retVector.emplace_back(R"(.*ExecGraphRemoveParameterNode.*RemoveParameterNode.*)");
    retVector.emplace_back(R"(.*IndexAddTest.*CompareWithRefs.*)");
    retVector.emplace_back(R"(.*InterpolateLayerCPUTest.*CompareWithRefs.*INFERENCE_PRECISION_HINT=f16.*)");
    retVector.emplace_back(R"(.*MatMulLayerCPUTest.*CompareWithRefs.*)");
    retVector.emplace_back(R"(.*MatmulWeightsDecompression.*CompareWithRefs.*)");
    retVector.emplace_back(R"(.*MvnLayerCPUTest.*CompareWithRefs.*INFERENCE_PRECISION_HINT=f16.*)");
    retVector.emplace_back(R"(.*NonInputInPlaceTest.*CompareWithRefs.*)");
    retVector.emplace_back(R"(.*OVClassCompiledModelGetPropertyTest_EXEC_DEVICES.*CanGetExecutionDeviceInfo.*)");
    retVector.emplace_back(R"(.*OVClassConfigTestCPU.*smoke_.*)");
    retVector.emplace_back(R"(.*OVClassConfigTestCPU.*smoke_CpuExecNetwork.*)");
    retVector.emplace_back(R"(.*OVInferenceChaining.*StaticOutputToDynamicInput.*)");
    retVector.emplace_back(R"(.*OVInferenceChaining.*StaticOutputToStaticInput.*)");
    retVector.emplace_back(R"(.*OVInferenceChainingStatic.*StaticOutputToStaticInput.*)");
    retVector.emplace_back(R"(.*ReduceCPULayerTest.*CompareWithRefs.*INFERENCE_PRECISION_HINT=f16.*)");
    // Issue: 164799
    retVector.emplace_back(R"(.*CompileModelCacheTestBase.*CompareWithRefImpl.*)");
    // Issue 167685
    retVector.emplace_back(R"(.*importExportModelWithTypeRelaxedExt.*)");
#endif
    if (!ov::test::snippets::is_i8_supported_by_brgemm()) {
        retVector.emplace_back(R"(.*Snippets.*MatMulFQ.*)");
        retVector.emplace_back(R"(.*Snippets.*MatMul.*Quantized.*)");
        retVector.emplace_back(R"(.*Snippets.*MHAFQ.*)");
        retVector.emplace_back(R"(.*Snippets.*MHAINT8.*)");
        retVector.emplace_back(R"(.*Snippets.*MHAQuant.*)");
        retVector.emplace_back(R"(.*Snippets.*MLP.*Quantized.*)");
    }
    // MHA BF16 precision is only supported on BF16 supported platform
    if (!ov::test::snippets::is_bf16_supported_by_brgemm()) {
        // ignored for not supported bf16 platforms
        retVector.emplace_back(R"(.*smoke_Snippets_EnforcePrecision_bf16.*)");
        retVector.emplace_back(R"(.*smoke_Snippets_MHAWOTransposeEnforceBF16.*)");
        retVector.emplace_back(R"(.*smoke_Snippets_FullyConnected_EnforceBF16.*)");
        retVector.emplace_back(R"(.*smoke_Snippets_MHA.*EnforceBF16.*)");
        retVector.emplace_back(R"(.*smoke_Snippets_.*MLP.*bf16.*)");
        retVector.emplace_back(R"(.*ConcatSDPTest.*bf16.*)");
    }
    if (!ov::test::snippets::is_fp16_supported_by_brgemm()) {
        retVector.emplace_back(R"(.*smoke_Snippets_MHA.*FP16.*)");
    }
    if (!ov::with_cpu_x86_avx512_core_amx_int8())
        // TODO: Issue 92895
        // on platforms which do not support AMX, we are disabling I8 input tests
        retVector.emplace_back(R"(smoke_LPT/FakeQuantizeWithNotOptimalTransformation.CompareWithRefImpl.*CPU.*i8.*)");
    // RNN/LSTM/GRU/AUGRU BF16 tests on avx512 core ISA
    if (ov::with_cpu_x86_avx512_core() && !ov::with_cpu_x86_avx512_core_amx_bf16()) {
        retVector.emplace_back(R"(smoke.*(AUGRUCellCPUTest|GRUCellCPUTest|RNNCellCPUTest|LSTMCellLayerCPUTest).CompareWithRefs.*INFERENCE_PRECISION_HINT=bf16.*)");
        retVector.emplace_back(R"(nightly.*bf16.*(AUGRUSequenceCPUTest|GRUSequenceCPUTest|LSTMSequenceCPUTest).CompareWithRefs.*INFERENCE_PRECISION_HINT=bf16.*)");
    }
#ifdef SNIPPETS_LIBXSMM_TPP
    // GN in TPP requires exposing tmp Buffer results outside the loop (ticket: 151234)
    retVector.emplace_back(R"(.*smoke_Snippets_GroupNormalization.*)");
    // TPP performs precision conversion implicitly, it makes all Convert tests irrelevant
    retVector.emplace_back(R"(.*smoke_Snippets_Convert.*)");
    // ABS and ROUND operations are needed for TPP support. Disable, since low precisions are not supported by TPP yet.
    retVector.emplace_back(R"(.*smoke_Snippets_FQ.*)");
    retVector.emplace_back(R"(.*smoke_Snippets_TransposeMatMulFQ.*)");
    // TPP doesn't support op with 2 outs, when one of them is Result (ticket: 130642)
    retVector.emplace_back(R"(.*smoke_Snippets_MaxNumParamsEltwise.*)");
    retVector.emplace_back(R"(.*smoke_Snippets_Eltwise_TwoResults.*)");
    // Accuracy problem with Exp + Reciprocal combination on TPP side (ticket: 130699)
    retVector.emplace_back(R"(.*smoke_Snippets_ExpReciprocal.*)");
    retVector.emplace_back(R"(.*smoke_Snippets_AddSoftmax.*)");
    retVector.emplace_back(R"(.*smoke_Snippets_TransposeSoftmaxEltwise.*)");
    // Low-precision Matmuls are not supported by TPP yet
    retVector.emplace_back(R"(.*smoke_Snippets.*MatMulFQ.*)");
    retVector.emplace_back(R"(.*smoke_Snippets.*MatMulBiasQuantized.*)");
    retVector.emplace_back(R"(.*smoke_Snippets.*MatMulsQuantized.*)");
    retVector.emplace_back(R"(.*smoke_Snippets.*MatMulsQuantizedSoftmax.*)");
    retVector.emplace_back(R"(.*smoke_Snippets.*MHAINT8MatMul.*)");
    retVector.emplace_back(R"(.*smoke_Snippets.*MHAQuantMatMul0.*)");
    retVector.emplace_back(R"(.*smoke_Snippets.*MHAFQ.*)");
    retVector.emplace_back(R"(.*smoke_Snippets.*PrecisionPropagation_Convertion.*)");
    retVector.emplace_back(R"(.*smoke_MHAQuant.*)");
    retVector.emplace_back(R"(.*smoke_Snippets_MLP.*)");
    if (!ov::with_cpu_x86_avx512_core_amx()) {
        // Issue: 165178
        retVector.emplace_back(R"(.*smoke_Snippets_Softmax/Softmax\.CompareWithRefImpl/IS=\[\]_TS=\(\(.*)");
        retVector.emplace_back(R"(.*smoke_Snippets_MHA.*IS\[0\]=\[\]_\(.*)");
        retVector.emplace_back(R"(.*smoke_Snippets_TransposeSoftmax/TransposeSoftmax\.CompareWithRefImpl/IS\[0\]=\[\]_TS\[0\]=\(\(.*)");
    }
#endif

    if (ov::with_cpu_x86_avx512_core_amx()) {
        // Issue: 131475
        retVector.emplace_back(R"(smoke_ExportImportTest/ExportOptimalNumStreams.OptimalNumStreams/.*)");
        // by calc abs_threshold with expected value
        retVector.emplace_back(R"(.*smoke_GatherCompressedWeights_basic/GatherWeightsDecompression.CompareWithRefs.*INFERENCE_PRECISION_HINT.*bf16.*)");
        retVector.emplace_back(R"(.*smoke_Interaction/IntertactionCPUTest.CompareWithRefs.*Prc=i32.*)");
        retVector.emplace_back(R"(.*smoke_MatMulCompressedWeights_(amx|sym_amx|corner_cases_amx)/MatmulWeightsDecompression.CompareWithRefs.*INFERENCE_PRECISION_HINT.*bf16.*)");
        retVector.emplace_back(R"(.*smoke_Snippets_EnforcePrecision_bf16/EnforcePrecisionTest.*)");
        retVector.emplace_back(R"(.*smoke_Snippets_MHABF16_4D/MHA.CompareWithRefImpl/.*\[1.58.16.34\]_IS\[1\]=\[1.58.16.34\]_IS\[2\]=\[1.1.1.58\]_IS\[3\]=\[1.58.16.34\].*)");
        retVector.emplace_back(R"(.*smoke_Snippets_MHAWOTransposeBF16/MHAWOTranspose.CompareWithRefImpl/.*IS\[0\]=\[2.\?.64\].*IS\[1\]=\[2.64.\?\].*IS\[2\]=\[2.\?.64\].*)");
        // Issue: 141705
        retVector.emplace_back(R"(.*smoke_Deconv_(2|3)D_NSPC_INT8_AMX/DeconvolutionLayerCPUTest.*)");
        retVector.emplace_back(R"(.*smoke_Deconv_(2|3)D_NSPC_INT8_AMX/DeconvolutionLayerCPUTest.*)");
    }

    if (ov::with_cpu_x86_avx512_core_fp16() || CPUTestUtils::with_cpu_x86_avx2_vnni_2()) {
        // Issue: 143852
        retVector.emplace_back(R"(smoke_ConvertRangeSubgraphCPUTest/ConvertRangeSubgraphCPUTest\.CompareWithRefs.*Prc=f16.*)");
        retVector.emplace_back(R"((smoke|nightly)_FC_3D_FP16/.*_Fused=Multiply\(PerChannel\).*)");
        retVector.emplace_back(R"((smoke|nightly)_MM_Brgemm_Static_FP16.*TS=\(\(55\.12\)\).*_Fused=Multiply\(PerChannel\).*)");
        retVector.emplace_back(R"(smoke_MM_Dynamic_Fusing_FP16/.*TS=\(\(16\.12\)_\(33\.7\)_\(16\.12\)\).*_Fused=Multiply\(PerChannel\).*)");
        retVector.emplace_back(R"(smoke_MM_Brgemm_Dynamic_Fusing_FP16/.*TS=\(\(16\.12\)_\(33\.7\)_\(16\.12\)\).*_Fused=Multiply\(PerChannel\).*)");
        retVector.emplace_back(R"(smoke_Conv_.*_FP16/.*_Fused=PRelu1D\.Multiply\(PerChannel\)\.Add\(PerChannel\).*)");
        retVector.emplace_back(R"(smoke_Conv_Sum_Broadcast_FP16/ConvSumInPlaceTest.*Relu\.Multiply\(PerChannel\)\.Add\(PerChannel\).*)");
    }

    if (CPUTestUtils::with_cpu_x86_avx2_vnni_2()) {
        // jit_gemm_BF16 kernels are not supported for conv,inner_product,matmul on avx2_vnni_2 platforms
        retVector.emplace_back(R"(smoke_Conv_.*D_GEMM_BF16.*)");
        retVector.emplace_back(
            R"(smoke_GroupConv_.*D_Gemm_BF16/GroupConvolutionLayerCPUTest.CompareWithRefs.*primitive=jit_gemm.*)");
        retVector.emplace_back(R"(smoke_.*MatMulLayerCPUTest.*INFERENCE_PRECISION_HINT=bf16.*_primitive=jit_gemm.*)");
        // by calc abs_threshold with expected value
        retVector.emplace_back(R"(smoke_Snippets_MHAWOTransposeBF16/MHAWOTranspose.CompareWithRefImpl/.*IS\[0\]=\[\]_\(\[12.128.100\]\).*)");
        retVector.emplace_back(R"(smoke_Snippets_MHAWOTransposeBF16/MHAWOTranspose.CompareWithRefImpl/.*IS\[0\]=\[2.\?.64\].*)");
        retVector.emplace_back(R"(smoke_Snippets_MHAWOTransposeBF16/MHAWOTranspose.CompareWithRefImpl/.*IS\[0\]=\[\?.\?.\?.\?\].*)");
    }

    return retVector;
}
