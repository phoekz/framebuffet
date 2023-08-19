#include "commands.hpp"
#include "pipelines.hpp"

namespace fb {

auto GpuCommandList::begin_pix(std::string_view name) const -> void {
    PIXBeginEvent(_cmd, PIX_COLOR_DEFAULT, name.data());
}

auto GpuCommandList::end_pix() const -> void {
    PIXEndEvent(_cmd);
}

auto GpuCommandList::set_global_descriptor_heap() -> void {
    const auto heaps = std::to_array<ID3D12DescriptorHeap*>({
        _descriptors->cbv_srv_uav().heap(),
        _descriptors->sampler().heap(),
    });
    _cmd->SetDescriptorHeaps((uint32_t)heaps.size(), heaps.data());
}

auto GpuCommandList::set_graphics() -> void {
    if (_engine == GpuCommandEngine::Graphics) {
        return;
    }
    _engine = GpuCommandEngine::Graphics;
    set_global_descriptor_heap();
    _cmd->SetGraphicsRootSignature(_root_signature);
}

auto GpuCommandList::set_compute() -> void {
    if (_engine == GpuCommandEngine::Compute) {
        return;
    }
    _engine = GpuCommandEngine::Compute;
    set_global_descriptor_heap();
    _cmd->SetComputeRootSignature(_root_signature);
}

auto GpuCommandList::set_viewport(
    uint32_t left,
    uint32_t top,
    uint32_t right,
    uint32_t bottom,
    float min_depth,
    float max_depth
) const -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    D3D12_VIEWPORT viewport {
        .TopLeftX = (FLOAT)left,
        .TopLeftY = (FLOAT)top,
        .Width = (FLOAT)(right - left),
        .Height = (FLOAT)(bottom - top),
        .MinDepth = min_depth,
        .MaxDepth = max_depth,
    };
    _cmd->RSSetViewports(1, &viewport);
}

auto GpuCommandList::set_scissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom) const
    -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    D3D12_RECT scissor {
        .left = (LONG)left,
        .top = (LONG)top,
        .right = (LONG)right,
        .bottom = (LONG)bottom,
    };
    _cmd->RSSetScissorRects(1, &scissor);
}

auto GpuCommandList::set_rtv_dsv(
    const std::optional<GpuDescriptor>& rtv,
    const std::optional<GpuDescriptor>& dsv
) const -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    _cmd->OMSetRenderTargets(
        rtv.has_value() ? 1 : 0,
        rtv.has_value() ? rtv.value().cpu_ptr() : nullptr,
        false,
        dsv.has_value() ? dsv.value().cpu_ptr() : nullptr
    );
}

auto GpuCommandList::set_topology(D3D12_PRIMITIVE_TOPOLOGY topology) const -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    _cmd->IASetPrimitiveTopology(topology);
}

auto GpuCommandList::set_index_buffer(D3D12_INDEX_BUFFER_VIEW ibv) const -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    _cmd->IASetIndexBuffer(&ibv);
}

auto GpuCommandList::set_blend_factor(Float4 factor) const -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    _cmd->OMSetBlendFactor((const float*)&factor);
}

auto GpuCommandList::set_pipeline(const GpuPipeline& pipeline) const -> void {
    _cmd->SetPipelineState(pipeline.get());
}

auto GpuCommandList::clear_rtv(const GpuDescriptor& rtv, Float4 color) const -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    _cmd->ClearRenderTargetView(rtv.cpu(), (const float*)&color, 0, nullptr);
}

auto GpuCommandList::clear_dsv(const GpuDescriptor& dsv, float depth) const -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    _cmd->ClearDepthStencilView(dsv.cpu(), D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
}

auto GpuCommandList::draw_instanced(
    uint32_t vertex_count,
    uint32_t instance_count,
    uint32_t start_vertex,
    uint32_t start_instance
) const -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    _cmd->DrawInstanced(vertex_count, instance_count, start_vertex, start_instance);
}

auto GpuCommandList::draw_indexed_instanced(
    uint32_t index_count,
    uint32_t instance_count,
    uint32_t start_index,
    int32_t base_vertex,
    uint32_t start_instance
) const -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    _cmd->DrawIndexedInstanced(
        index_count,
        instance_count,
        start_index,
        base_vertex,
        start_instance
    );
}

auto GpuCommandList::dispatch(uint32_t x, uint32_t y, uint32_t z) const -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Compute);
    _cmd->Dispatch(x, y, z);
}

auto GpuCommandList::resolve_resource(
    const ComPtr<ID3D12Resource>& dst,
    const ComPtr<ID3D12Resource>& src,
    DXGI_FORMAT src_format
) const -> void {
    _cmd->ResolveSubresource(dst.get(), 0, src.get(), 0, src_format);
}

auto GpuCommandList::transition_barrier(
    const ComPtr<ID3D12Resource>& resource,
    D3D12_RESOURCE_STATES before,
    D3D12_RESOURCE_STATES after
) const -> void {
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.get(), before, after);
    _cmd->ResourceBarrier(1, &barrier);
}

auto GpuCommandList::uav_barrier(const ComPtr<ID3D12Resource>& resource) const -> void {
    auto barrier = CD3DX12_RESOURCE_BARRIER::UAV(resource.get());
    _cmd->ResourceBarrier(1, &barrier);
}

} // namespace fb
