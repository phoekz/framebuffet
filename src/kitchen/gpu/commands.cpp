#include "commands.hpp"
#include "pipelines.hpp"
#include "formats.hpp"

namespace fb {

//
// GpuCommandList.
//

auto GpuCommandList::clear_rtv(const GpuDescriptor& rtv, float4 color) const -> void {
    _cmd->ClearRenderTargetView(rtv.cpu(), (const float*)&color, 0, nullptr);
}

auto GpuCommandList::clear_dsv(const GpuDescriptor& dsv, float depth) const -> void {
    _cmd->ClearDepthStencilView(dsv.cpu(), D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
}

auto GpuCommandList::set_global_descriptor_heap() const -> void {
    if (_prev_engine == _engine) {
        return;
    }
    marker_pix("Set global descriptor heap");
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
    marker_pix("Set global graphics root signature");
    _cmd->SetGraphicsRootSignature(_root_signature);
}

auto GpuCommandList::set_global_compute_root_signature() const -> void {
    if (_prev_engine == _engine) {
        return;
    }
    marker_pix("Set global compute root signature");
    _cmd->SetComputeRootSignature(_root_signature);
}

auto GpuCommandList::set_graphics_root_constants(std::span<const uint> dwords) const -> void {
    _cmd->SetGraphicsRoot32BitConstants(0, (uint)dwords.size(), dwords.data(), 0);
}

auto GpuCommandList::set_compute_root_constants(std::span<const uint> dwords) const -> void {
    _cmd->SetComputeRoot32BitConstants(0, (uint)dwords.size(), dwords.data(), 0);
}

auto GpuCommandList::copy_texture_to_buffer(
    const ComPtr<ID3D12Resource>& dst_buffer,
    uint64_t dst_buffer_offset,
    const ComPtr<ID3D12Resource>& src_texture,
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
) -> void {
    FB_ASSERT(_pending_barrier_count < MAX_PENDING_BARRIERS);
    _pending_barriers[_pending_barrier_count++] = D3D12_RESOURCE_BARRIER {
        .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
        .Transition =
            D3D12_RESOURCE_TRANSITION_BARRIER {
                .pResource = resource.get(),
                .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                .StateBefore = before,
                .StateAfter = after,
            },
    };
}

auto GpuCommandList::uav_barrier(const ComPtr<ID3D12Resource>& resource) -> void {
    FB_ASSERT(_pending_barrier_count < MAX_PENDING_BARRIERS);
    _pending_barriers[_pending_barrier_count++] = D3D12_RESOURCE_BARRIER {
        .Type = D3D12_RESOURCE_BARRIER_TYPE_UAV,
        .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
        .UAV =
            D3D12_RESOURCE_UAV_BARRIER {
                .pResource = resource.get(),
            },
    };
}

auto GpuCommandList::flush_barriers() -> void {
    if (_pending_barrier_count == 0) {
        return;
    }
    _cmd->ResourceBarrier((uint)_pending_barrier_count, _pending_barriers.data());
    _pending_barrier_count = 0;
}

auto GpuCommandList::execute_indirect(
    const ComPtr<ID3D12CommandSignature>& command_signature,
    uint max_command_count,
    const ComPtr<ID3D12Resource>& argument_buffer,
    const std::optional<std::reference_wrapper<const ComPtr<ID3D12Resource>>> count_buffer
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

auto GpuGraphicsCommandList::set_rtv_dsv(
    const std::optional<GpuDescriptor>& rtv,
    const std::optional<GpuDescriptor>& dsv
) const -> void {
    _cmd->OMSetRenderTargets(
        rtv.has_value() ? 1 : 0,
        rtv.has_value() ? rtv.value().cpu_ptr() : nullptr,
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
