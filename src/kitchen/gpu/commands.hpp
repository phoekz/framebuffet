#pragma once

#include "../pch.hpp"
#include "descriptors.hpp"

namespace fb {

class GpuGraphicsCommandList;
template<typename Scope>
concept GpuGraphicsCommandScope =
    requires(Scope scope, GpuGraphicsCommandList& graphics) { scope(graphics); };

class GpuComputeCommandList;
template<typename Scope>
concept GpuComputeCommandScope =
    requires(Scope scope, GpuComputeCommandList& compute) { scope(compute); };

//
// GpuCommandList.
//

enum class GpuCommandEngine {
    Generic,
    Graphics,
    Compute,
};

class GpuCommandList {
    FB_NO_COPY(GpuCommandList);

public:
    // Move constructor and assignment operator.
    GpuCommandList(GpuCommandList&& o) { *this = std::move(o); }
    GpuCommandList& operator=(GpuCommandList&& o) {
        _cmd = std::exchange(o._cmd, nullptr);
        _engine = std::exchange(o._engine, GpuCommandEngine::Generic);
        _prev_engine = std::exchange(o._prev_engine, GpuCommandEngine::Generic);
        _root_signature = std::exchange(o._root_signature, nullptr);
        _descriptors = std::exchange(o._descriptors, nullptr);
        _pending_barriers = std::exchange(o._pending_barriers, {});
        _pending_barrier_count = std::exchange(o._pending_barrier_count, 0);
        return *this;
    }

    // Scope functions.
    template<GpuGraphicsCommandScope Scope>
    auto graphics_scope(Scope scope) -> void {
        FB_ASSERT(_engine != GpuCommandEngine::Graphics);
        _engine = GpuCommandEngine::Graphics;
        set_global_descriptor_heap();
        set_global_graphics_root_signature();
        scope(reinterpret_cast<GpuGraphicsCommandList&>(*this));
        _engine = GpuCommandEngine::Generic;
        _prev_engine = GpuCommandEngine::Graphics;
    }

    template<GpuComputeCommandScope Scope>
    auto compute_scope(Scope scope) -> void {
        FB_ASSERT(_engine != GpuCommandEngine::Compute);
        _engine = GpuCommandEngine::Compute;
        set_global_descriptor_heap();
        set_global_compute_root_signature();
        scope(reinterpret_cast<GpuComputeCommandList&>(*this));
        _engine = GpuCommandEngine::Generic;
        _prev_engine = GpuCommandEngine::Compute;
    }

    //
    // PIX.
    //

    template<typename... Args>
    auto begin_pix(const char* fmt, Args... args) const -> void {
        PIXBeginEvent(_cmd, PIX_COLOR_DEFAULT, fmt, args...);
    }

    auto end_pix() const -> void { PIXEndEvent(_cmd); }

    template<typename... Args>
    auto marker_pix(const char* fmt, Args... args) const -> void {
        PIXSetMarker(_cmd, PIX_COLOR_DEFAULT, fmt, args...);
    }

    //
    // Command list functions.
    //

    auto clear_rtv(const GpuDescriptor& rtv, float4 color) const -> void;

    auto clear_dsv(const GpuDescriptor& dsv, float depth) const -> void;

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

protected:
    auto set_global_descriptor_heap() const -> void;
    auto set_global_graphics_root_signature() const -> void;
    auto set_global_compute_root_signature() const -> void;
    auto set_graphics_root_constants(std::span<const uint> dwords) const -> void;
    auto set_compute_root_constants(std::span<const uint> dwords) const -> void;

protected:
    ID3D12GraphicsCommandList9* _cmd = nullptr;
    GpuCommandEngine _engine = GpuCommandEngine::Generic;
    GpuCommandEngine _prev_engine = GpuCommandEngine::Generic;
    ID3D12RootSignature* _root_signature = nullptr;
    GpuDescriptors* _descriptors = nullptr;

    static constexpr size_t MAX_PENDING_BARRIERS = 32;
    std::array<D3D12_RESOURCE_BARRIER, MAX_PENDING_BARRIERS> _pending_barriers;
    size_t _pending_barrier_count = 0;

private:
    // GpuDevice is the only class that can create GpuCommandList.
    friend class GpuDevice;
    GpuCommandList(
        ID3D12GraphicsCommandList9* cmd,
        ID3D12RootSignature* root_signature,
        GpuDescriptors* descriptors
    )
        : _cmd(cmd)
        , _root_signature(root_signature)
        , _descriptors(descriptors) {}
};

//
// GpuGraphicsCommandList.
//

class GpuPipeline;

class GpuGraphicsCommandList final: public GpuCommandList {
    FB_NO_COPY(GpuGraphicsCommandList);

public:
    auto set_pipeline(const GpuPipeline& pipeline) const -> void;

    auto set_rtv_dsv(
        const std::optional<GpuDescriptor>& rtv,
        const std::optional<GpuDescriptor>& dsv
    ) const -> void;

    auto set_viewport(
        uint left,
        uint top,
        uint right,
        uint bottom,
        float min_depth = 0.0f,
        float max_depth = 1.0f
    ) const -> void;

    auto set_scissor(uint left, uint top, uint right, uint bottom) const -> void;

    auto set_topology(D3D12_PRIMITIVE_TOPOLOGY topology) const -> void;

    auto set_index_buffer(D3D12_INDEX_BUFFER_VIEW ibv) const -> void;

    auto set_blend_factor(float4 factor) const -> void;

    template<GpuBindable Values>
    auto set_constants(Values values) const -> void {
        const auto dwords = into_dword_array(values);
        set_graphics_root_constants(dwords);
    }

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
};

//
// GpuComputeCommandList.
//

class GpuComputeCommandList final: public GpuCommandList {
    FB_NO_COPY(GpuComputeCommandList);

public:
    auto set_pipeline(const GpuPipeline& pipeline) const -> void;

    template<GpuBindable Values>
    auto set_constants(Values values) const -> void {
        const auto dwords = into_dword_array(values);
        set_compute_root_constants(dwords);
    }

    auto dispatch(uint x, uint y, uint z) const -> void;
};

} // namespace fb
