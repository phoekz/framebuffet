#include "pipelines.hpp"
#include "device.hpp"

#include <d3dx12/d3dx12_pipeline_state_stream.h>

namespace fb {

static_assert(GpuPipelineBuilder::BUFFER_SIZE >= sizeof(CD3DX12_PIPELINE_STATE_STREAM5));

auto GpuPipelineBuilder::primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology)
    -> GpuPipelineBuilder& {
    static constexpr uint BIT = (1 << D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY);
    FB_ASSERT((_subobject_mask & BIT) == 0);
    new (_buffer + _buffet_offset) CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY(topology);
    _buffet_offset += sizeof(CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY);
    _subobject_mask |= BIT;
    return *this;
}

auto GpuPipelineBuilder::vertex_shader(std::span<const std::byte> dxil) -> GpuPipelineBuilder& {
    static constexpr uint BIT = (1 << D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS);
    FB_ASSERT((_subobject_mask & BIT) == 0);
    new (_buffer + _buffet_offset) CD3DX12_PIPELINE_STATE_STREAM_VS(D3D12_SHADER_BYTECODE {
        .pShaderBytecode = dxil.data(),
        .BytecodeLength = dxil.size_bytes(),
    });
    _buffet_offset += sizeof(CD3DX12_PIPELINE_STATE_STREAM_VS);
    _subobject_mask |= BIT;
    return *this;
}

auto GpuPipelineBuilder::pixel_shader(std::span<const std::byte> dxil) -> GpuPipelineBuilder& {
    static constexpr uint BIT = (1 << D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS);
    FB_ASSERT((_subobject_mask & BIT) == 0);
    new (_buffer + _buffet_offset) CD3DX12_PIPELINE_STATE_STREAM_PS(D3D12_SHADER_BYTECODE {
        .pShaderBytecode = dxil.data(),
        .BytecodeLength = dxil.size_bytes(),
    });
    _buffet_offset += sizeof(CD3DX12_PIPELINE_STATE_STREAM_PS);
    _subobject_mask |= BIT;
    return *this;
}

auto GpuPipelineBuilder::compute_shader(std::span<const std::byte> dxil) -> GpuPipelineBuilder& {
    static constexpr uint BIT = (1 << D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS);
    FB_ASSERT((_subobject_mask & BIT) == 0);
    new (_buffer + _buffet_offset) CD3DX12_PIPELINE_STATE_STREAM_CS(D3D12_SHADER_BYTECODE {
        .pShaderBytecode = dxil.data(),
        .BytecodeLength = dxil.size_bytes(),
    });
    _buffet_offset += sizeof(CD3DX12_PIPELINE_STATE_STREAM_CS);
    _subobject_mask |= BIT;
    return *this;
}

auto GpuPipelineBuilder::blend(GpuBlendDesc desc) -> GpuPipelineBuilder& {
    static constexpr uint BIT = (1 << D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND);
    FB_ASSERT((_subobject_mask & BIT) == 0);
    _blend_desc = desc;
    _subobject_mask |= BIT;
    return *this;
}

auto GpuPipelineBuilder::depth_stencil(GpuDepthStencilDesc desc) -> GpuPipelineBuilder& {
    static constexpr uint BIT = (1 << D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL2);
    FB_ASSERT((_subobject_mask & BIT) == 0);
    _depth_stencil_desc = desc;
    _subobject_mask |= BIT;
    return *this;
}

auto GpuPipelineBuilder::rasterizer(GpuRasterizerDesc desc) -> GpuPipelineBuilder& {
    static constexpr uint BIT = (1 << D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER2);
    FB_ASSERT((_subobject_mask & BIT) == 0);
    _rasterizer_desc = desc;
    _subobject_mask |= BIT;
    return *this;
}

auto GpuPipelineBuilder::render_target_formats(std::initializer_list<DXGI_FORMAT> formats)
    -> GpuPipelineBuilder& {
    static constexpr uint BIT = (1 << D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS);
    FB_ASSERT((_subobject_mask & BIT) == 0);
    FB_ASSERT(formats.size() <= D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);
    D3D12_RT_FORMAT_ARRAY array = {};
    for (auto format : formats) {
        array.RTFormats[array.NumRenderTargets++] = format;
    }
    new (_buffer + _buffet_offset) CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS(array);
    _buffet_offset += sizeof(CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS);
    _subobject_mask |= BIT;
    return *this;
}

auto GpuPipelineBuilder::depth_stencil_format(Option<DXGI_FORMAT> format) -> GpuPipelineBuilder& {
    if (!format.has_value()) {
        return *this;
    }
    static constexpr uint BIT = (1 << D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT);
    FB_ASSERT((_subobject_mask & BIT) == 0);
    new (_buffer + _buffet_offset)
        CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT(format.value());
    _buffet_offset += sizeof(CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT);
    _subobject_mask |= BIT;
    return *this;
}

auto GpuPipelineBuilder::sample_desc(DXGI_SAMPLE_DESC desc) -> GpuPipelineBuilder& {
    static constexpr uint BIT = (1 << D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC);
    FB_ASSERT((_subobject_mask & BIT) == 0);
    new (_buffer + _buffet_offset) CD3DX12_PIPELINE_STATE_STREAM_SAMPLE_DESC(desc);
    _buffet_offset += sizeof(CD3DX12_PIPELINE_STATE_STREAM_SAMPLE_DESC);
    _subobject_mask |= BIT;
    _samples_count = desc.Count;
    return *this;
}

auto GpuPipelineBuilder::build(GpuDevice& device, GpuPipeline& pipeline, std::string_view name)
    -> void {
    ZoneScoped;

    // Add global root signature.
    {
        new (_buffer + _buffet_offset)
            CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE(device.root_signature());
        _buffet_offset += sizeof(CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE);
    }

    // Rasterizer.
    {
        // D3D12 defaults.
        D3D12_RASTERIZER_DESC2 d3d12_desc = {
            .FillMode = D3D12_FILL_MODE_SOLID,
            .CullMode = D3D12_CULL_MODE_BACK,
            .FrontCounterClockwise = TRUE,
            .DepthBias = D3D12_DEFAULT_DEPTH_BIAS,
            .DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
            .SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
            .DepthClipEnable = TRUE,
            .LineRasterizationMode = D3D12_LINE_RASTERIZATION_MODE_ALIASED,
            .ForcedSampleCount = 0,
            .ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF,
        };

        // Override `FillMode`.
        switch (_rasterizer_desc.fill_mode) {
            case GpuFillMode::Solid: d3d12_desc.FillMode = D3D12_FILL_MODE_SOLID; break;
            case GpuFillMode::Wireframe: d3d12_desc.FillMode = D3D12_FILL_MODE_WIREFRAME; break;
        }

        // Override `CullMode`.
        switch (_rasterizer_desc.cull_mode) {
            case GpuCullMode::None: d3d12_desc.CullMode = D3D12_CULL_MODE_NONE; break;
            case GpuCullMode::Front: d3d12_desc.CullMode = D3D12_CULL_MODE_FRONT; break;
            case GpuCullMode::Back: d3d12_desc.CullMode = D3D12_CULL_MODE_BACK; break;
        }

        // Override `LineRasterizationMode`.
        if (_samples_count > 1) {
            // Note: https://microsoft.github.io/DirectX-Specs/d3d/VulkanOn12.html#line-rasterization-updates
            d3d12_desc.LineRasterizationMode = D3D12_LINE_RASTERIZATION_MODE_QUADRILATERAL_WIDE;
        }

        // Override `ConservativeRaster`.
        if (_rasterizer_desc.conservative_rasterization) {
            d3d12_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON;
        } else {
            d3d12_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
        }

        // Add.
        CD3DX12_RASTERIZER_DESC2 cdesc(d3d12_desc);
        new (_buffer + _buffet_offset) CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER2(cdesc);
        _buffet_offset += sizeof(CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER2);
    }

    // Depth stencil.
    {
        // D3D12 defaults.
        const D3D12_DEPTH_STENCILOP_DESC1 stencil_op_desc = {
            .StencilFailOp = D3D12_STENCIL_OP_KEEP,
            .StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
            .StencilPassOp = D3D12_STENCIL_OP_KEEP,
            .StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS,
            .StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK,
            .StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK,
        };
        D3D12_DEPTH_STENCIL_DESC2 d3d12_desc = {
            .DepthEnable = TRUE,
            .DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,
            .DepthFunc = D3D12_COMPARISON_FUNC_LESS,
            .StencilEnable = FALSE,
            .FrontFace = stencil_op_desc,
            .BackFace = stencil_op_desc,
            .DepthBoundsTestEnable = FALSE,
        };

        // Override `DepthEnable`.
        if (_depth_stencil_desc.depth_read) {
            d3d12_desc.DepthEnable = TRUE;
        } else {
            d3d12_desc.DepthEnable = FALSE;
        }

        // Override `DepthWriteMask`.
        if (_depth_stencil_desc.depth_write) {
            d3d12_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        } else {
            d3d12_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        }

        // Override `DepthFunc`.
        switch (_depth_stencil_desc.depth_func) {
            using enum GpuComparisonFunc;
            case Never: d3d12_desc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER; break;
            case Less: d3d12_desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS; break;
            case Equal: d3d12_desc.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL; break;
            case LessEqual: d3d12_desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; break;
            case Greater: d3d12_desc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER; break;
            case NotEqual: d3d12_desc.DepthFunc = D3D12_COMPARISON_FUNC_NOT_EQUAL; break;
            case GreaterEqual: d3d12_desc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL; break;
            case Always: d3d12_desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS; break;
        }

        // Add.
        CD3DX12_DEPTH_STENCIL_DESC2 cdesc(d3d12_desc);
        new (_buffer + _buffet_offset) CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL2(cdesc);
        _buffet_offset += sizeof(CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL2);
    }

    // Blend.
    {
        // D3D12 defaults.
        const D3D12_RENDER_TARGET_BLEND_DESC render_target_blend_desc = {
            .BlendEnable = FALSE,
            .LogicOpEnable = FALSE,
            .SrcBlend = D3D12_BLEND_ONE,
            .DestBlend = D3D12_BLEND_ZERO,
            .BlendOp = D3D12_BLEND_OP_ADD,
            .SrcBlendAlpha = D3D12_BLEND_ONE,
            .DestBlendAlpha = D3D12_BLEND_ZERO,
            .BlendOpAlpha = D3D12_BLEND_OP_ADD,
            .LogicOp = D3D12_LOGIC_OP_NOOP,
            .RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL,
        };
        D3D12_BLEND_DESC d3d12_desc = {
            .AlphaToCoverageEnable = FALSE,
            .IndependentBlendEnable = FALSE,
            .RenderTarget = {render_target_blend_desc},
        };

        // Conversions.
        auto d3d12_from_gpu_blend = [](GpuBlend blend) {
            using enum GpuBlend;
            switch (blend) {
                case Zero: return D3D12_BLEND_ZERO;
                case One: return D3D12_BLEND_ONE;
                case SrcColor: return D3D12_BLEND_SRC_COLOR;
                case InvSrcColor: return D3D12_BLEND_INV_SRC_COLOR;
                case SrcAlpha: return D3D12_BLEND_SRC_ALPHA;
                case InvSrcAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
                case DstAlpha: return D3D12_BLEND_DEST_ALPHA;
                case InvDstAlpha: return D3D12_BLEND_INV_DEST_ALPHA;
                case DstColor: return D3D12_BLEND_DEST_COLOR;
                case InvDstColor: return D3D12_BLEND_INV_DEST_COLOR;
                default: FB_FATAL();
            }
        };
        auto d3d12_from_gpu_blend_op = [](GpuBlendOp blend_op) {
            using enum GpuBlendOp;
            switch (blend_op) {
                case Add: return D3D12_BLEND_OP_ADD;
                case Subtract: return D3D12_BLEND_OP_SUBTRACT;
                case RevSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
                case Min: return D3D12_BLEND_OP_MIN;
                case Max: return D3D12_BLEND_OP_MAX;
                default: FB_FATAL();
            }
        };
        auto d3d12_from_gpu_logic_op = [](GpuLogicOp logic_op) {
            using enum GpuLogicOp;
            switch (logic_op) {
                case Clear: return D3D12_LOGIC_OP_CLEAR;
                case Set: return D3D12_LOGIC_OP_SET;
                case Copy: return D3D12_LOGIC_OP_COPY;
                case CopyInverted: return D3D12_LOGIC_OP_COPY_INVERTED;
                case Noop: return D3D12_LOGIC_OP_NOOP;
                case Invert: return D3D12_LOGIC_OP_INVERT;
                case And: return D3D12_LOGIC_OP_AND;
                case Nand: return D3D12_LOGIC_OP_NAND;
                case Or: return D3D12_LOGIC_OP_OR;
                case Nor: return D3D12_LOGIC_OP_NOR;
                case Xor: return D3D12_LOGIC_OP_XOR;
                case Equiv: return D3D12_LOGIC_OP_EQUIV;
                case AndReverse: return D3D12_LOGIC_OP_AND_REVERSE;
                case AndInverted: return D3D12_LOGIC_OP_AND_INVERTED;
                case OrReverse: return D3D12_LOGIC_OP_OR_REVERSE;
                case OrInverted: return D3D12_LOGIC_OP_OR_INVERTED;
                default: FB_FATAL();
            }
        };

        // Override `AlphaToCoverageEnable`.
        if (_blend_desc.alpha_to_coverage_enable) {
            d3d12_desc.AlphaToCoverageEnable = TRUE;
        } else {
            d3d12_desc.AlphaToCoverageEnable = FALSE;
        }

        // Override `BlendEnable`.
        if (_blend_desc.blend_enable) {
            d3d12_desc.RenderTarget[0].BlendEnable = TRUE;
        } else {
            d3d12_desc.RenderTarget[0].BlendEnable = FALSE;
        }

        // Override `LogicOpEnable`.
        if (_blend_desc.logic_op_enable) {
            d3d12_desc.RenderTarget[0].LogicOpEnable = TRUE;
        } else {
            d3d12_desc.RenderTarget[0].LogicOpEnable = FALSE;
        }

        // Override `SrcBlend`, `DestBlend`, `BlendOp`.
        d3d12_desc.RenderTarget[0].SrcBlend = d3d12_from_gpu_blend(_blend_desc.rgb_blend_src);
        d3d12_desc.RenderTarget[0].DestBlend = d3d12_from_gpu_blend(_blend_desc.rgb_blend_dst);
        d3d12_desc.RenderTarget[0].BlendOp = d3d12_from_gpu_blend_op(_blend_desc.rgb_blend_op);

        // Override `SrcBlendAlpha`, `DestBlendAlpha`, `BlendOpAlpha`.
        d3d12_desc.RenderTarget[0].SrcBlendAlpha =
            d3d12_from_gpu_blend(_blend_desc.alpha_blend_src);
        d3d12_desc.RenderTarget[0].DestBlendAlpha =
            d3d12_from_gpu_blend(_blend_desc.alpha_blend_dst);
        d3d12_desc.RenderTarget[0].BlendOpAlpha =
            d3d12_from_gpu_blend_op(_blend_desc.alpha_blend_op);

        // Override `LogicOp`.
        d3d12_desc.RenderTarget[0].LogicOp = d3d12_from_gpu_logic_op(_blend_desc.logic_op);

        // Add.
        CD3DX12_BLEND_DESC cdesc(d3d12_desc);
        new (_buffer + _buffet_offset) CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC(cdesc);
        _buffet_offset += sizeof(CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC);
    }

    // Create pipeline state.
    pipeline._state = device.create_pipeline_state(
        D3D12_PIPELINE_STATE_STREAM_DESC {
            .SizeInBytes = _buffet_offset,
            .pPipelineStateSubobjectStream = _buffer,
        },
        name
    );
}

} // namespace fb
