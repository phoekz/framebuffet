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
    friend class GpuDevice;

public:
    GpuCommandList(GpuCommandList&& o) { *this = std::move(o); }
    GpuCommandList& operator=(GpuCommandList&& o) {
        _cmd = std::exchange(o._cmd, nullptr);
        _engine = std::exchange(o._engine, GpuCommandEngine::Unknown);
        _root_signature = std::exchange(o._root_signature, nullptr);
        _descriptors = std::exchange(o._descriptors, nullptr);
        return *this;
    }

    auto begin_pix(std::string_view name) const -> void;
    auto end_pix() const -> void;

    auto set_graphics() -> void;
    auto set_compute() -> void;

    auto set_viewport(
        uint32_t left,
        uint32_t top,
        uint32_t right,
        uint32_t bottom,
        float min_depth = 0.0f,
        float max_depth = 1.0f
    ) const -> void;
    auto set_scissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom) const -> void;
    auto set_rtv_dsv(
        const std::optional<GpuDescriptor>& rtv,
        const std::optional<GpuDescriptor>& dsv
    ) const -> void;
    auto set_topology(D3D12_PRIMITIVE_TOPOLOGY topology) const -> void;
    auto set_index_buffer(D3D12_INDEX_BUFFER_VIEW ibv) const -> void;
    auto set_blend_factor(Float4 factor) const -> void;
    auto set_pipeline(const GpuPipeline& pipeline) const -> void;

    template<GpuBindable T>
    auto set_graphics_constants(T t) const -> void {
        const auto arr = into_dword_array(t);
        _cmd->SetGraphicsRoot32BitConstants(0, (uint32_t)arr.size(), arr.data(), 0);
    }

    template<GpuBindable T>
    auto set_compute_constants(T t) const -> void {
        const auto arr = into_dword_array(t);
        _cmd->SetComputeRoot32BitConstants(0, (uint32_t)arr.size(), arr.data(), 0);
    }

    auto clear_rtv(const GpuDescriptor& rtv, Float4 color) const -> void;
    auto clear_dsv(const GpuDescriptor& dsv, float depth) const -> void;
    auto draw_instanced(
        uint32_t vertex_count,
        uint32_t instance_count,
        uint32_t start_vertex,
        uint32_t start_instance
    ) const -> void;
    auto draw_indexed_instanced(
        uint32_t index_count,
        uint32_t instance_count,
        uint32_t start_index,
        int32_t base_vertex,
        uint32_t start_instance
    ) const -> void;
    auto dispatch(uint32_t x, uint32_t y, uint32_t z) const -> void;

    auto transition_barrier(
        const ComPtr<ID3D12Resource>& resource,
        D3D12_RESOURCE_STATES before,
        D3D12_RESOURCE_STATES after
    ) const -> void;
    auto uav_barrier(const ComPtr<ID3D12Resource>& resource) const -> void;

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
    GpuCommandList(GpuCommandList& o) = delete;
    GpuCommandList& operator=(GpuCommandList& o) = delete;

    auto set_global_descriptor_heap() -> void;

    ID3D12GraphicsCommandList9* _cmd = nullptr;
    GpuCommandEngine _engine = GpuCommandEngine::Unknown;
    ID3D12RootSignature* _root_signature = nullptr;
    GpuDescriptors* _descriptors = nullptr;
};

} // namespace fb
