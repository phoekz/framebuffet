#pragma once

#include "../pch.hpp"
#include "descriptors.hpp"

namespace fb {

class GpuPipeline;

enum class GpuCommandEngine {
    Unknown,
    Graphics,
    Compute,
    Copy,
};

class GpuCommandList {
    FB_NO_COPY(GpuCommandList);

    friend class GpuDevice;

public:
    GpuCommandList(GpuCommandList&& o) { *this = std::move(o); }
    GpuCommandList& operator=(GpuCommandList&& o) {
        _cmd = std::exchange(o._cmd, nullptr);
        _engine = std::exchange(o._engine, GpuCommandEngine::Unknown);
        _root_signature = std::exchange(o._root_signature, nullptr);
        _descriptors = std::exchange(o._descriptors, nullptr);
        _pending_barriers = std::exchange(o._pending_barriers, {});
        _pending_barrier_count = std::exchange(o._pending_barrier_count, 0);
        return *this;
    }

    template<typename... Args>
    auto begin_pix(const char* fmt, Args... args) const -> void {
        PIXBeginEvent(_cmd, PIX_COLOR_DEFAULT, fmt, args...);
    }
    auto end_pix() const -> void { PIXEndEvent(_cmd); }

    auto set_graphics() -> void;
    auto set_compute() -> void;

    auto set_viewport(
        uint left,
        uint top,
        uint right,
        uint bottom,
        float min_depth = 0.0f,
        float max_depth = 1.0f
    ) const -> void;
    auto set_scissor(uint left, uint top, uint right, uint bottom) const -> void;
    auto set_rtv_dsv(
        const std::optional<GpuDescriptor>& rtv,
        const std::optional<GpuDescriptor>& dsv
    ) const -> void;
    auto set_topology(D3D12_PRIMITIVE_TOPOLOGY topology) const -> void;
    auto set_index_buffer(D3D12_INDEX_BUFFER_VIEW ibv) const -> void;
    auto set_blend_factor(float4 factor) const -> void;
    auto set_pipeline(const GpuPipeline& pipeline) const -> void;

    template<GpuBindable T>
    auto set_graphics_constants(T t) const -> void {
        const auto arr = into_dword_array(t);
        _cmd->SetGraphicsRoot32BitConstants(0, (uint)arr.size(), arr.data(), 0);
    }

    template<GpuBindable T>
    auto set_compute_constants(T t) const -> void {
        const auto arr = into_dword_array(t);
        _cmd->SetComputeRoot32BitConstants(0, (uint)arr.size(), arr.data(), 0);
    }

    auto clear_rtv(const GpuDescriptor& rtv, float4 color) const -> void;
    auto clear_dsv(const GpuDescriptor& dsv, float depth) const -> void;
    auto
    draw_instanced(uint vertex_count, uint instance_count, uint start_vertex, uint start_instance)
        const -> void;
    auto draw_indexed_instanced(
        uint index_count,
        uint instance_count,
        uint start_index,
        int32_t base_vertex,
        uint start_instance
    ) const -> void;
    auto dispatch(uint x, uint y, uint z) const -> void;

    auto copy_texture_to_buffer(
        const ComPtr<ID3D12Resource>& dst_buffer,
        uint64_t dst_buffer_offset,
        const ComPtr<ID3D12Resource>& src_texture,
        uint src_texture_subresource_index,
        DXGI_FORMAT src_texture_format,
        uint src_texture_width,
        uint src_texture_height
    ) const -> void;

    auto resolve_resource(
        const ComPtr<ID3D12Resource>& dst,
        const ComPtr<ID3D12Resource>& src,
        DXGI_FORMAT src_format
    ) const -> void;

    auto transition_barrier(
        const ComPtr<ID3D12Resource>& resource,
        D3D12_RESOURCE_STATES before,
        D3D12_RESOURCE_STATES after
    ) -> void;
    auto uav_barrier(const ComPtr<ID3D12Resource>& resource) -> void;
    auto flush_barriers() -> void;

private:
    GpuCommandList(
        ID3D12GraphicsCommandList9* cmd,
        GpuCommandEngine engine,
        ID3D12RootSignature* root_signature,
        GpuDescriptors* descriptors
    )
        : _cmd(cmd)
        , _engine(engine)
        , _root_signature(root_signature)
        , _descriptors(descriptors) {}

    auto set_global_descriptor_heap() -> void;

    ID3D12GraphicsCommandList9* _cmd = nullptr;
    GpuCommandEngine _engine = GpuCommandEngine::Unknown;
    ID3D12RootSignature* _root_signature = nullptr;
    GpuDescriptors* _descriptors = nullptr;

    static constexpr size_t MAX_PENDING_BARRIERS = 32;
    std::array<D3D12_RESOURCE_BARRIER, MAX_PENDING_BARRIERS> _pending_barriers;
    size_t _pending_barrier_count = 0;
};

} // namespace fb
