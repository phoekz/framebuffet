#pragma once

#include "descriptors.hpp"

namespace fb {

//
// Bindings.
//

inline constexpr uint MAX_BINDING_COUNT = 16;

template<typename T>
inline constexpr auto dword_count() -> uint {
    return sizeof(T) / sizeof(uint);
}

template<typename T>
concept GpuBindable =
    (sizeof(T) > 0) && (sizeof(T) % sizeof(uint) == 0) && (dword_count<T>() <= MAX_BINDING_COUNT)
    && std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T>;

template<GpuBindable T>
using GpuBindableArray = std::array<uint, dword_count<T>()>;

template<GpuBindable T>
inline constexpr auto into_dword_array(T t) -> GpuBindableArray<T> {
    return std::bit_cast<GpuBindableArray<T>>(t);
}

//
// GpuCommandList.
//

class GpuGraphicsCommandList;
template<typename Scope>
concept GpuGraphicsCommandScope =
    requires(Scope scope, GpuGraphicsCommandList& graphics) { scope(graphics); };

class GpuComputeCommandList;
template<typename Scope>
concept GpuComputeCommandScope =
    requires(Scope scope, GpuComputeCommandList& compute) { scope(compute); };

enum class GpuCommandEngine {
    Generic,
    Graphics,
    Compute,
};

class GpuCommandList {
    FB_NO_COPY(GpuCommandList);

public:
    // Constructor.
    GpuCommandList(
        ID3D12GraphicsCommandList9* cmd,
        ID3D12RootSignature* root_signature,
        GpuDescriptors* descriptors
    );

    // Move constructor and assignment operator.
    GpuCommandList(GpuCommandList&& o) noexcept { *this = std::move(o); }
    GpuCommandList& operator=(GpuCommandList&& o) noexcept {
        _cmd = std::exchange(o._cmd, nullptr);
        _engine = std::exchange(o._engine, GpuCommandEngine::Generic);
        _prev_engine = std::exchange(o._prev_engine, GpuCommandEngine::Generic);
        _root_signature = std::exchange(o._root_signature, nullptr);
        _descriptors = std::exchange(o._descriptors, nullptr);
        _pending_global_barriers = std::exchange(o._pending_global_barriers, {});
        _pending_buffer_barriers = std::exchange(o._pending_buffer_barriers, {});
        _pending_texture_barriers = std::exchange(o._pending_texture_barriers, {});
        _pending_global_barrier_count = std::exchange(o._pending_global_barrier_count, 0);
        _pending_buffer_barrier_count = std::exchange(o._pending_buffer_barrier_count, 0);
        _pending_texture_barrier_count = std::exchange(o._pending_texture_barrier_count, 0);
        return *this;
    }

    // Scope functions.
    template<GpuGraphicsCommandScope Scope>
    FB_INLINE auto graphics_scope(Scope scope) -> void {
        FB_ASSERT(_engine != GpuCommandEngine::Graphics);
        _engine = GpuCommandEngine::Graphics;
        set_global_descriptor_heap();
        set_global_graphics_root_signature();
        scope(reinterpret_cast<GpuGraphicsCommandList&>(*this));
        _engine = GpuCommandEngine::Generic;
        _prev_engine = GpuCommandEngine::Graphics;
    }

    template<GpuComputeCommandScope Scope>
    FB_INLINE auto compute_scope(Scope scope) -> void {
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
    FB_INLINE auto pix_begin(const char* fmt, Args... args) const -> void {
        PIXBeginEvent(_cmd, PIX_COLOR_DEFAULT, fmt, args...);
    }

    FB_INLINE auto pix_end() const -> void { PIXEndEvent(_cmd); }

    template<typename... Args>
    FB_INLINE auto pix_marker(const char* fmt, Args... args) const -> void {
        PIXSetMarker(_cmd, PIX_COLOR_DEFAULT, fmt, args...);
    }

    //
    // Command list functions.
    //

    auto clear_rtv(const GpuDescriptor& rtv, float4 color) const -> void;

    auto clear_dsv(const GpuDescriptor& dsv, float depth, uint8_t stencil) const -> void;

    auto copy_texture_to_buffer(
        const ComPtr<ID3D12Resource2>& dst_buffer,
        uint64_t dst_buffer_offset,
        const ComPtr<ID3D12Resource2>& src_texture,
        uint src_texture_subresource_index,
        DXGI_FORMAT src_texture_format,
        uint src_texture_width,
        uint src_texture_height
    ) const -> void;

    auto resolve_resource(
        const ComPtr<ID3D12Resource2>& dst,
        const ComPtr<ID3D12Resource2>& src,
        DXGI_FORMAT src_format
    ) const -> void;

    auto global_barrier(
        D3D12_BARRIER_SYNC sync_before,
        D3D12_BARRIER_SYNC sync_after,
        D3D12_BARRIER_ACCESS access_before,
        D3D12_BARRIER_ACCESS access_after
    ) -> void;
    auto buffer_barrier(
        D3D12_BARRIER_SYNC sync_before,
        D3D12_BARRIER_SYNC sync_after,
        D3D12_BARRIER_ACCESS access_before,
        D3D12_BARRIER_ACCESS access_after,
        const ComPtr<ID3D12Resource2>& resource
    ) -> void;
    auto texture_barrier(
        D3D12_BARRIER_SYNC sync_before,
        D3D12_BARRIER_SYNC sync_after,
        D3D12_BARRIER_ACCESS access_before,
        D3D12_BARRIER_ACCESS access_after,
        D3D12_BARRIER_LAYOUT layout_before,
        D3D12_BARRIER_LAYOUT layout_after,
        const ComPtr<ID3D12Resource2>& resource,
        D3D12_BARRIER_SUBRESOURCE_RANGE subresources
    ) -> void;
    auto flush_barriers() -> void;

    auto execute_indirect(
        const ComPtr<ID3D12CommandSignature>& command_signature,
        uint max_command_count,
        const ComPtr<ID3D12Resource2>& argument_buffer,
        const Option<ConstRef<ComPtr<ID3D12Resource2>>> count_buffer
    ) const -> void;

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
    std::array<D3D12_GLOBAL_BARRIER, MAX_PENDING_BARRIERS> _pending_global_barriers = {};
    std::array<D3D12_BUFFER_BARRIER, MAX_PENDING_BARRIERS> _pending_buffer_barriers = {};
    std::array<D3D12_TEXTURE_BARRIER, MAX_PENDING_BARRIERS> _pending_texture_barriers = {};
    size_t _pending_global_barrier_count = 0;
    size_t _pending_buffer_barrier_count = 0;
    size_t _pending_texture_barrier_count = 0;
};

//
// GpuGraphicsCommandList.
//

class GpuPipeline;

class GpuGraphicsCommandList final: public GpuCommandList {
    FB_NO_COPY(GpuGraphicsCommandList);

public:
    auto set_pipeline(const GpuPipeline& pipeline) const -> void;

    auto set_rtvs_dsv(const std::span<const GpuDescriptor>& rtvs, const Option<GpuDescriptor>& dsv)
        const -> void;

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

//
// Indirect commands.
//

template<GpuBindable T>
struct IndirectDrawCommand {
    T bindings;
    D3D12_DRAW_ARGUMENTS draw;
};

template<GpuBindable T>
struct IndirectDrawIndexedCommand {
    T bindings;
    D3D12_DRAW_INDEXED_ARGUMENTS draw_indexed;
};

template<GpuBindable T>
struct IndirectDispatchCommand {
    T bindings;
    D3D12_DISPATCH_ARGUMENTS dispatch;
};

} // namespace fb
