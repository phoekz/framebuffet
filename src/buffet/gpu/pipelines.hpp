#pragma once

#include "../pch.hpp"

namespace fb {

class GpuDevice;

enum class GpuFillMode {
    Solid,
    Wireframe,
};

enum class GpuCullMode {
    None,
    Front,
    Back,
};

struct GpuRasterizerDesc {
    GpuFillMode fill_mode = GpuFillMode::Solid;
    GpuCullMode cull_mode = GpuCullMode::Back;
};

enum class GpuComparisonFunc {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always,
};

struct GpuDepthStencilDesc {
    bool depth_read = true;
    bool depth_write = true;
    GpuComparisonFunc depth_func = GpuComparisonFunc::LessEqual;
};

class GpuPipeline {
    FB_NO_COPY_MOVE(GpuPipeline);

    friend class GpuPipelineBuilder;

public:
    GpuPipeline() = default;

    auto get() const -> ID3D12PipelineState* { return _state.get(); }

private:
    ComPtr<ID3D12PipelineState> _state;
};

class GpuPipelineBuilder {
    FB_NO_COPY_MOVE(GpuPipelineBuilder);

public:
    GpuPipelineBuilder() = default;

    // clang-format off
    [[nodiscard]] auto primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology) -> GpuPipelineBuilder&;
    [[nodiscard]] auto vertex_shader(std::span<const std::byte> dxil) -> GpuPipelineBuilder&;
    [[nodiscard]] auto pixel_shader(std::span<const std::byte> dxil) -> GpuPipelineBuilder&;
    [[nodiscard]] auto compute_shader(std::span<const std::byte> dxil) -> GpuPipelineBuilder&;
    [[nodiscard]] auto blend(D3D12_BLEND_DESC blend) -> GpuPipelineBuilder&;
    [[nodiscard]] auto depth_stencil(GpuDepthStencilDesc desc) -> GpuPipelineBuilder&;
    [[nodiscard]] auto rasterizer(GpuRasterizerDesc desc) -> GpuPipelineBuilder&;
    [[nodiscard]] auto render_target_formats(std::initializer_list<DXGI_FORMAT> formats) -> GpuPipelineBuilder&;
    [[nodiscard]] auto depth_stencil_format(DXGI_FORMAT format) -> GpuPipelineBuilder&;
    [[nodiscard]] auto sample_desc(DXGI_SAMPLE_DESC desc) -> GpuPipelineBuilder&;
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
    GpuRasterizerDesc _rasterizer_desc = {};
    GpuDepthStencilDesc _depth_stencil_desc = {};
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

} // namespace fb
