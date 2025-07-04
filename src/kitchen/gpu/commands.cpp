#include "commands.hpp"
#include "pipelines.hpp"
#include "formats.hpp"

namespace fb {

//
// GpuCommandList.
//

GpuCommandList::GpuCommandList(
    ID3D12GraphicsCommandList9* cmd,
    ID3D12RootSignature* root_signature,
    GpuDescriptors* descriptors
)
    : _cmd(cmd)
    , _root_signature(root_signature)
    , _descriptors(descriptors) {}

auto GpuCommandList::clear_rtv(const GpuDescriptor& rtv, float4 color) const -> void {
    _cmd->ClearRenderTargetView(rtv.cpu(), (const float*)&color, 0, nullptr);
}

auto GpuCommandList::clear_dsv(const GpuDescriptor& dsv, float depth, uint8_t stencil) const
    -> void {
    _cmd->ClearDepthStencilView(
        dsv.cpu(),
        D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
        depth,
        stencil,
        0,
        nullptr
    );
}

auto GpuCommandList::set_global_descriptor_heap() const -> void {
    if (_prev_engine == _engine) {
        return;
    }
    pix_marker("Set global descriptor heap");
    const auto heaps = std::to_array<ID3D12DescriptorHeap*>({
        _descriptors->cbv_srv_uav().heap(),
        _descriptors->sampler().heap(),
    });
    _cmd->SetDescriptorHeaps((uint)heaps.size(), heaps.data());
}

auto GpuCommandList::set_global_graphics_root_signature() const -> void {
    if (_prev_engine == _engine) {
        return;
    }
    pix_marker("Set global graphics root signature");
    _cmd->SetGraphicsRootSignature(_root_signature);
}

auto GpuCommandList::set_global_compute_root_signature() const -> void {
    if (_prev_engine == _engine) {
        return;
    }
    pix_marker("Set global compute root signature");
    _cmd->SetComputeRootSignature(_root_signature);
}

auto GpuCommandList::set_graphics_root_constants(Span<const uint> dwords) const -> void {
    _cmd->SetGraphicsRoot32BitConstants(0, (uint)dwords.size(), dwords.data(), 0);
}

auto GpuCommandList::set_compute_root_constants(Span<const uint> dwords) const -> void {
    _cmd->SetComputeRoot32BitConstants(0, (uint)dwords.size(), dwords.data(), 0);
}

auto GpuCommandList::copy_texture_to_buffer(
    const ComPtr<ID3D12Resource2>& dst_buffer,
    uint64_t dst_buffer_offset,
    const ComPtr<ID3D12Resource2>& src_texture,
    uint src_texture_subresource_index,
    DXGI_FORMAT src_texture_format,
    uint src_texture_width,
    uint src_texture_height
) const -> void {
    const auto unit_byte_count = dxgi_format_unit_byte_count(src_texture_format);
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT dst_footprint = {
        .Offset = dst_buffer_offset,
        .Footprint =
            D3D12_SUBRESOURCE_FOOTPRINT {
                .Format = src_texture_format,
                .Width = src_texture_width,
                .Height = src_texture_height,
                .Depth = 1,
                .RowPitch = src_texture_width * unit_byte_count,
            },
    };
    D3D12_TEXTURE_COPY_LOCATION dst_location = {
        .pResource = dst_buffer.get(),
        .Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
        .PlacedFootprint = dst_footprint,
    };
    D3D12_TEXTURE_COPY_LOCATION src_location = {
        .pResource = src_texture.get(),
        .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
        .SubresourceIndex = src_texture_subresource_index,
    };
    _cmd->CopyTextureRegion(&dst_location, 0, 0, 0, &src_location, nullptr);
}

auto GpuCommandList::resolve_resource(
    const ComPtr<ID3D12Resource2>& dst,
    const ComPtr<ID3D12Resource2>& src,
    DXGI_FORMAT src_format
) const -> void {
    _cmd->ResolveSubresource(dst.get(), 0, src.get(), 0, src_format);
}

auto GpuCommandList::global_barrier(
    D3D12_BARRIER_SYNC sync_before,
    D3D12_BARRIER_SYNC sync_after,
    D3D12_BARRIER_ACCESS access_before,
    D3D12_BARRIER_ACCESS access_after
) -> void {
    FB_ASSERT(_pending_global_barrier_count < MAX_PENDING_BARRIERS);
    _pending_global_barriers[_pending_global_barrier_count++] = D3D12_GLOBAL_BARRIER {
        .SyncBefore = sync_before,
        .SyncAfter = sync_after,
        .AccessBefore = access_before,
        .AccessAfter = access_after,
    };
}

auto GpuCommandList::buffer_barrier(
    D3D12_BARRIER_SYNC sync_before,
    D3D12_BARRIER_SYNC sync_after,
    D3D12_BARRIER_ACCESS access_before,
    D3D12_BARRIER_ACCESS access_after,
    const ComPtr<ID3D12Resource2>& resource
) -> void {
    FB_ASSERT(_pending_buffer_barrier_count < MAX_PENDING_BARRIERS);
    _pending_buffer_barriers[_pending_buffer_barrier_count++] = D3D12_BUFFER_BARRIER {
        .SyncBefore = sync_before,
        .SyncAfter = sync_after,
        .AccessBefore = access_before,
        .AccessAfter = access_after,
        .pResource = resource.get(),
        .Offset = 0,
        .Size = UINT64_MAX,
    };
}

auto GpuCommandList::texture_barrier(
    D3D12_BARRIER_SYNC sync_before,
    D3D12_BARRIER_SYNC sync_after,
    D3D12_BARRIER_ACCESS access_before,
    D3D12_BARRIER_ACCESS access_after,
    D3D12_BARRIER_LAYOUT layout_before,
    D3D12_BARRIER_LAYOUT layout_after,
    const ComPtr<ID3D12Resource2>& resource,
    D3D12_BARRIER_SUBRESOURCE_RANGE subresources
) -> void {
    FB_ASSERT(_pending_texture_barrier_count < MAX_PENDING_BARRIERS);
    _pending_texture_barriers[_pending_texture_barrier_count++] = D3D12_TEXTURE_BARRIER {
        .SyncBefore = sync_before,
        .SyncAfter = sync_after,
        .AccessBefore = access_before,
        .AccessAfter = access_after,
        .LayoutBefore = layout_before,
        .LayoutAfter = layout_after,
        .pResource = resource.get(),
        .Subresources = subresources,
        .Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE,
    };
}

auto GpuCommandList::flush_barriers() -> void {
    std::array<D3D12_BARRIER_GROUP, 3> barrier_groups = {};
    uint barrier_group_count = 0;

    if (_pending_global_barrier_count > 0) {
        barrier_groups[barrier_group_count++] = D3D12_BARRIER_GROUP {
            .Type = D3D12_BARRIER_TYPE_GLOBAL,
            .NumBarriers = (uint)_pending_global_barrier_count,
            .pGlobalBarriers = _pending_global_barriers.data(),
        };
        _pending_global_barrier_count = 0;
    }

    if (_pending_buffer_barrier_count > 0) {
        barrier_groups[barrier_group_count++] = D3D12_BARRIER_GROUP {
            .Type = D3D12_BARRIER_TYPE_BUFFER,
            .NumBarriers = (uint)_pending_buffer_barrier_count,
            .pBufferBarriers = _pending_buffer_barriers.data(),
        };
        _pending_buffer_barrier_count = 0;
    }

    if (_pending_texture_barrier_count > 0) {
        barrier_groups[barrier_group_count++] = D3D12_BARRIER_GROUP {
            .Type = D3D12_BARRIER_TYPE_TEXTURE,
            .NumBarriers = (uint)_pending_texture_barrier_count,
            .pTextureBarriers = _pending_texture_barriers.data(),
        };
        _pending_texture_barrier_count = 0;
    }

    if (barrier_group_count > 0) {
        _cmd->Barrier(barrier_group_count, barrier_groups.data());
    }
}

auto GpuCommandList::execute_indirect(
    const ComPtr<ID3D12CommandSignature>& command_signature,
    uint max_command_count,
    const ComPtr<ID3D12Resource2>& argument_buffer,
    const Option<ConstRef<ComPtr<ID3D12Resource2>>> count_buffer
) const -> void {
    _cmd->ExecuteIndirect(
        command_signature.get(),
        max_command_count,
        argument_buffer.get(),
        0,
        count_buffer.has_value() ? count_buffer.value().get().get() : nullptr,
        0
    );
}

//
// GpuGraphicsCommandList.
//

auto GpuGraphicsCommandList::set_pipeline(const GpuPipeline& pipeline) const -> void {
    _cmd->SetPipelineState(pipeline.get());
}

auto GpuGraphicsCommandList::set_rtvs_dsv(
    const Span<const GpuDescriptor>& rtvs,
    const Option<GpuDescriptor>& dsv
) const -> void {
    FB_ASSERT(rtvs.size() <= D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> rtv_descs = {};
    for (size_t i = 0; i < rtvs.size(); ++i) {
        rtv_descs[i] = rtvs[i].cpu();
    }

    _cmd->OMSetRenderTargets(
        (uint)rtvs.size(),
        rtv_descs.data(),
        false,
        dsv.has_value() ? dsv.value().cpu_ptr() : nullptr
    );
}

auto GpuGraphicsCommandList::set_viewport(
    uint left,
    uint top,
    uint right,
    uint bottom,
    float min_depth,
    float max_depth
) const -> void {
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

auto GpuGraphicsCommandList::set_scissor(uint left, uint top, uint right, uint bottom) const
    -> void {
    D3D12_RECT scissor {
        .left = (LONG)left,
        .top = (LONG)top,
        .right = (LONG)right,
        .bottom = (LONG)bottom,
    };
    _cmd->RSSetScissorRects(1, &scissor);
}

auto GpuGraphicsCommandList::set_topology(D3D12_PRIMITIVE_TOPOLOGY topology) const -> void {
    _cmd->IASetPrimitiveTopology(topology);
}

auto GpuGraphicsCommandList::set_index_buffer(D3D12_INDEX_BUFFER_VIEW ibv) const -> void {
    _cmd->IASetIndexBuffer(&ibv);
}

auto GpuGraphicsCommandList::set_blend_factor(float4 factor) const -> void {
    _cmd->OMSetBlendFactor((const float*)&factor);
}

auto GpuGraphicsCommandList::draw_instanced(
    uint vertex_count,
    uint instance_count,
    uint start_vertex,
    uint start_instance
) const -> void {
    _cmd->DrawInstanced(vertex_count, instance_count, start_vertex, start_instance);
}

auto GpuGraphicsCommandList::draw_indexed_instanced(
    uint index_count,
    uint instance_count,
    uint start_index,
    int32_t base_vertex,
    uint start_instance
) const -> void {
    _cmd->DrawIndexedInstanced(
        index_count,
        instance_count,
        start_index,
        base_vertex,
        start_instance
    );
}

//
// GpuComputeCommandList.
//

auto GpuComputeCommandList::set_pipeline(const GpuPipeline& pipeline) const -> void {
    _cmd->SetPipelineState(pipeline.get());
}

auto GpuComputeCommandList::dispatch(uint x, uint y, uint z) const -> void {
    _cmd->Dispatch(x, y, z);
}

} // namespace fb
