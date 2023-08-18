#include "pipelines.hpp"
#include "device.hpp"

namespace fb {

auto GpuPipelineBuilder::primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology)
    -> GpuPipelineBuilder& {
    static constexpr uint32_t BIT = (1 << D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY);
    FB_ASSERT((_subobject_mask & BIT) == 0);
    new (_buffer + _buffet_offset) CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY(topology);
    _buffet_offset += sizeof(CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY);
    _subobject_mask |= BIT;
    return *this;
}

auto GpuPipelineBuilder::vertex_shader(std::span<const std::byte> dxil) -> GpuPipelineBuilder& {
    static constexpr uint32_t BIT = (1 << D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS);
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
    static constexpr uint32_t BIT = (1 << D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS);
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
    static constexpr uint32_t BIT = (1 << D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS);
    FB_ASSERT((_subobject_mask & BIT) == 0);
    new (_buffer + _buffet_offset) CD3DX12_PIPELINE_STATE_STREAM_CS(D3D12_SHADER_BYTECODE {
        .pShaderBytecode = dxil.data(),
        .BytecodeLength = dxil.size_bytes(),
    });
    _buffet_offset += sizeof(CD3DX12_PIPELINE_STATE_STREAM_CS);
    _subobject_mask |= BIT;
    return *this;
}

auto GpuPipelineBuilder::blend(D3D12_BLEND_DESC blend) -> GpuPipelineBuilder& {
    static constexpr uint32_t BIT = (1 << D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND);
    FB_ASSERT((_subobject_mask & BIT) == 0);
    CD3DX12_BLEND_DESC cdesc(blend);
    new (_buffer + _buffet_offset) CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC(cdesc);
    _buffet_offset += sizeof(CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC);
    _subobject_mask |= BIT;
    return *this;
}

auto GpuPipelineBuilder::depth_stencil(D3D12_DEPTH_STENCIL_DESC2 depth_stencil)
    -> GpuPipelineBuilder& {
    static constexpr uint32_t BIT = (1 << D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL2);
    FB_ASSERT((_subobject_mask & BIT) == 0);
    CD3DX12_DEPTH_STENCIL_DESC2 cdesc(depth_stencil);
    new (_buffer + _buffet_offset) CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL2(cdesc);
    _buffet_offset += sizeof(CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL2);
    _subobject_mask |= BIT;
    return *this;
}

auto GpuPipelineBuilder::rasterizer(D3D12_RASTERIZER_DESC2 rasterizer) -> GpuPipelineBuilder& {
    static constexpr uint32_t BIT = (1 << D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER2);
    FB_ASSERT((_subobject_mask & BIT) == 0);
    CD3DX12_RASTERIZER_DESC2 cdesc(rasterizer);
    new (_buffer + _buffet_offset) CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER2(cdesc);
    _buffet_offset += sizeof(CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER2);
    _subobject_mask |= BIT;
    return *this;
}

auto GpuPipelineBuilder::render_target_formats(std::initializer_list<DXGI_FORMAT> formats)
    -> GpuPipelineBuilder& {
    static constexpr uint32_t BIT =
        (1 << D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS);
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

auto GpuPipelineBuilder::depth_stencil_format(DXGI_FORMAT format) -> GpuPipelineBuilder& {
    static constexpr uint32_t BIT = (1 << D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT);
    FB_ASSERT((_subobject_mask & BIT) == 0);
    new (_buffer + _buffet_offset) CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT(format);
    _buffet_offset += sizeof(CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT);
    _subobject_mask |= BIT;
    return *this;
}

auto GpuPipelineBuilder::build(GpuDevice& device, GpuPipeline& pipeline, std::string_view name)
    -> void {
    // Add global root signature.
    new (_buffer + _buffet_offset)
        CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE(device.root_signature());
    _buffet_offset += sizeof(CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE);

    // Create pipeline state.
    pipeline._state = device.create_pipeline_state(
        D3D12_PIPELINE_STATE_STREAM_DESC {
            .SizeInBytes = _buffet_offset,
            .pPipelineStateSubobjectStream = _buffer,
        },
        name);
}

} // namespace fb
