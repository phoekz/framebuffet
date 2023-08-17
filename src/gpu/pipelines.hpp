#pragma once

#include <pch.hpp>
#include "device.hpp"

namespace fb {

class GpuPipeline {
    friend class GpuPipelineBuilder;

  public:
    auto get() const -> ID3D12PipelineState* { return _state.get(); }

  private:
    ComPtr<ID3D12PipelineState> _state;
};

class GpuPipelineBuilder {
  public:
    // clang-format off
    [[nodiscard]] auto primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology) -> GpuPipelineBuilder&;
    [[nodiscard]] auto vertex_shader(std::span<const std::byte> dxil) -> GpuPipelineBuilder&;
    [[nodiscard]] auto pixel_shader(std::span<const std::byte> dxil) -> GpuPipelineBuilder&;
    [[nodiscard]] auto compute_shader(std::span<const std::byte> dxil) -> GpuPipelineBuilder&;
    [[nodiscard]] auto blend(D3D12_BLEND_DESC blend) -> GpuPipelineBuilder&;
    [[nodiscard]] auto depth_stencil(D3D12_DEPTH_STENCIL_DESC2 depth_stencil) -> GpuPipelineBuilder&;
    [[nodiscard]] auto rasterizer(D3D12_RASTERIZER_DESC2 rasterizer) -> GpuPipelineBuilder&;
    [[nodiscard]] auto render_target_formats(std::initializer_list<DXGI_FORMAT> formats) -> GpuPipelineBuilder&;
    [[nodiscard]] auto depth_stencil_format(DXGI_FORMAT format) -> GpuPipelineBuilder&;
    auto build(GpuDevice& device, GpuPipeline& pipeline, std::string_view name) -> void;
    // clang-format on

  private:
    static constexpr size_t BUFFER_SIZE = 1024;
    static_assert(BUFFER_SIZE >= sizeof(CD3DX12_PIPELINE_STATE_STREAM5));
    static_assert(8 * sizeof(uint32_t) >= D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MAX_VALID);

    std::string_view _name;
    std::byte _buffer[BUFFER_SIZE] = {};
    size_t _buffet_offset = 0;
    uint32_t _subobject_mask = 0;
};

inline constexpr D3D12_BLEND_DESC GPU_PIPELINE_BLEND_ADDITIVE = {
    .AlphaToCoverageEnable = FALSE,
    .IndependentBlendEnable = FALSE,
    .RenderTarget = {D3D12_RENDER_TARGET_BLEND_DESC {
        .BlendEnable = TRUE,
        .LogicOpEnable = FALSE,
        .SrcBlend = D3D12_BLEND_SRC_ALPHA,
        .DestBlend = D3D12_BLEND_ONE,
        .BlendOp = D3D12_BLEND_OP_ADD,
        .SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA,
        .DestBlendAlpha = D3D12_BLEND_ONE,
        .BlendOpAlpha = D3D12_BLEND_OP_ADD,
        .LogicOp = D3D12_LOGIC_OP_NOOP,
        .RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL,
    }},
};

inline constexpr D3D12_BLEND_DESC GPU_PIPELINE_BLEND_ALPHA = {
    .AlphaToCoverageEnable = FALSE,
    .IndependentBlendEnable = FALSE,
    .RenderTarget = {D3D12_RENDER_TARGET_BLEND_DESC {
        .BlendEnable = TRUE,
        .LogicOpEnable = FALSE,
        .SrcBlend = D3D12_BLEND_SRC_ALPHA,
        .DestBlend = D3D12_BLEND_INV_SRC_ALPHA,
        .BlendOp = D3D12_BLEND_OP_ADD,
        .SrcBlendAlpha = D3D12_BLEND_ONE,
        .DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA,
        .BlendOpAlpha = D3D12_BLEND_OP_ADD,
        .LogicOp = D3D12_LOGIC_OP_NOOP,
        .RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL,
    }},
};

inline constexpr D3D12_DEPTH_STENCIL_DESC2 GPU_PIPELINE_DEPTH_DEFAULT = {
    .DepthEnable = TRUE,
    .DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,
    .DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL,
    .StencilEnable = FALSE,
    .FrontFace =
        D3D12_DEPTH_STENCILOP_DESC1 {
            .StencilFailOp = D3D12_STENCIL_OP_KEEP,
            .StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
            .StencilPassOp = D3D12_STENCIL_OP_KEEP,
            .StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS,
            .StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK,
            .StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK,
        },
    .BackFace =
        D3D12_DEPTH_STENCILOP_DESC1 {
            .StencilFailOp = D3D12_STENCIL_OP_KEEP,
            .StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
            .StencilPassOp = D3D12_STENCIL_OP_KEEP,
            .StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS,
            .StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK,
            .StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK,
        },
    .DepthBoundsTestEnable = FALSE,
};

inline constexpr D3D12_DEPTH_STENCIL_DESC2 GPU_PIPELINE_DEPTH_NONE = {
    .DepthEnable = FALSE,
    .DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO,
    .DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL,
    .StencilEnable = FALSE,
    .FrontFace =
        D3D12_DEPTH_STENCILOP_DESC1 {
            .StencilFailOp = D3D12_STENCIL_OP_KEEP,
            .StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
            .StencilPassOp = D3D12_STENCIL_OP_KEEP,
            .StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS,
            .StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK,
            .StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK,
        },
    .BackFace =
        D3D12_DEPTH_STENCILOP_DESC1 {
            .StencilFailOp = D3D12_STENCIL_OP_KEEP,
            .StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
            .StencilPassOp = D3D12_STENCIL_OP_KEEP,
            .StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS,
            .StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK,
            .StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK,
        },
    .DepthBoundsTestEnable = FALSE,
};

inline constexpr D3D12_RASTERIZER_DESC2 GPU_PIPELINE_CULL_NONE = {
    .FillMode = D3D12_FILL_MODE_SOLID,
    .CullMode = D3D12_CULL_MODE_NONE,
    .FrontCounterClockwise = FALSE,
    .DepthBias = D3D12_DEFAULT_DEPTH_BIAS,
    .DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
    .SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
    .DepthClipEnable = TRUE,
    .LineRasterizationMode = D3D12_LINE_RASTERIZATION_MODE_ALIASED,
    .ForcedSampleCount = 0,
    .ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF,
};

inline constexpr D3D12_RASTERIZER_DESC2 GPU_PIPELINE_WIREFRAME = {
    .FillMode = D3D12_FILL_MODE_WIREFRAME,
    .CullMode = D3D12_CULL_MODE_NONE,
    .FrontCounterClockwise = FALSE,
    .DepthBias = D3D12_DEFAULT_DEPTH_BIAS,
    .DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
    .SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
    .DepthClipEnable = TRUE,
    .LineRasterizationMode = D3D12_LINE_RASTERIZATION_MODE_ALIASED,
    .ForcedSampleCount = 0,
    .ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF,
};

}  // namespace fb
