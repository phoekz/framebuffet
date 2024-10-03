#pragma once

#include <common/common.hpp>

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
    bool conservative_rasterization = false;
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

enum class GpuBlend {
    Zero,
    One,
    SrcColor,
    InvSrcColor,
    SrcAlpha,
    InvSrcAlpha,
    DstAlpha,
    InvDstAlpha,
    DstColor,
    InvDstColor,
};

enum class GpuBlendOp {
    Add,
    Subtract,
    RevSubtract,
    Min,
    Max,
};

enum class GpuLogicOp {
    Clear,
    Set,
    Copy,
    CopyInverted,
    Noop,
    Invert,
    And,
    Nand,
    Or,
    Nor,
    Xor,
    Equiv,
    AndReverse,
    AndInverted,
    OrReverse,
    OrInverted,
};

struct GpuBlendDesc {
    bool alpha_to_coverage_enable = false;
    bool blend_enable = false;
    bool logic_op_enable = false;
    GpuBlend rgb_blend_src = GpuBlend::One;
    GpuBlend rgb_blend_dst = GpuBlend::Zero;
    GpuBlendOp rgb_blend_op = GpuBlendOp::Add;
    GpuBlend alpha_blend_src = GpuBlend::One;
    GpuBlend alpha_blend_dst = GpuBlend::Zero;
    GpuBlendOp alpha_blend_op = GpuBlendOp::Add;
    GpuLogicOp logic_op = GpuLogicOp::Clear;
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
    static constexpr size_t BUFFER_SIZE = 1024;

    GpuPipelineBuilder() = default;

    auto primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology) -> GpuPipelineBuilder&;
    auto vertex_shader(std::span<const std::byte> dxil) -> GpuPipelineBuilder&;
    auto pixel_shader(std::span<const std::byte> dxil) -> GpuPipelineBuilder&;
    auto compute_shader(std::span<const std::byte> dxil) -> GpuPipelineBuilder&;
    auto blend(GpuBlendDesc desc) -> GpuPipelineBuilder&;
    auto depth_stencil(GpuDepthStencilDesc desc) -> GpuPipelineBuilder&;
    auto rasterizer(GpuRasterizerDesc desc) -> GpuPipelineBuilder&;
    auto render_target_formats(std::initializer_list<DXGI_FORMAT> formats) -> GpuPipelineBuilder&;
    auto depth_stencil_format(Option<DXGI_FORMAT> format) -> GpuPipelineBuilder&;
    auto sample_desc(DXGI_SAMPLE_DESC desc) -> GpuPipelineBuilder&;
    auto build(GpuDevice& device, GpuPipeline& pipeline, std::string_view name) -> void;

private:
    static_assert(8 * sizeof(uint) >= D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MAX_VALID);

    std::string_view _name;
    std::byte _buffer[BUFFER_SIZE] = {};
    size_t _buffet_offset = 0;
    uint _subobject_mask = 0;
    GpuRasterizerDesc _rasterizer_desc = {};
    GpuDepthStencilDesc _depth_stencil_desc = {};
    GpuBlendDesc _blend_desc = {};
    uint _samples_count = 1;
};

} // namespace fb
