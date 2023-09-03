#pragma once

#include "../pch.hpp"
#include "debug.hpp"
#include "samplers.hpp"
#include "transfer.hpp"
#include "swapchain.hpp"

namespace fb {

inline constexpr D3D_FEATURE_LEVEL MIN_FEATURE_LEVEL = D3D_FEATURE_LEVEL_12_2;

class Window;

struct GpuVideoMemoryInfo {
    struct {
        uint64_t budget;
        uint64_t current_usage;
        uint64_t available_for_reservation;
        uint64_t current_reservation;
    } local, non_local;
};

class GpuDevice {
    FB_NO_COPY_MOVE(GpuDevice);

public:
    GpuDevice() = default;
    ~GpuDevice();

    // Device state.
    auto create(const Window& window) -> void;
    auto begin_transfer() -> void;
    auto end_transfer() -> void;
    auto begin_frame() -> GpuCommandList;
    auto end_frame(GpuCommandList&& cmd) -> void;
    auto wait() -> void;

    // Wrappers.
    [[nodiscard]] auto
    create_root_signature(const ComPtr<ID3DBlob>& signature, std::string_view name) const
        -> ComPtr<ID3D12RootSignature>;
    [[nodiscard]] auto
    create_descriptor_heap(const D3D12_DESCRIPTOR_HEAP_DESC& desc, std::string_view name) const
        -> ComPtr<ID3D12DescriptorHeap>;
    [[nodiscard]] auto
    create_pipeline_state(const D3D12_PIPELINE_STATE_STREAM_DESC& desc, std::string_view name)
        -> ComPtr<ID3D12PipelineState>;
    [[nodiscard]] auto create_committed_resource(
        D3D12_HEAP_TYPE heap_type,
        const D3D12_RESOURCE_DESC& desc,
        D3D12_RESOURCE_STATES init_state,
        const std::optional<D3D12_CLEAR_VALUE>& clear_value,
        std::string_view name
    ) const -> ComPtr<ID3D12Resource>;
    [[nodiscard]] auto create_fence(uint64_t init_value, std::string_view name) const
        -> ComPtr<ID3D12Fence1>;
    auto create_constant_buffer_view(
        const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc,
        D3D12_CPU_DESCRIPTOR_HANDLE descriptor
    ) const -> void;
    auto create_shader_resource_view(
        const ComPtr<ID3D12Resource>& resource,
        const D3D12_SHADER_RESOURCE_VIEW_DESC& desc,
        D3D12_CPU_DESCRIPTOR_HANDLE descriptor
    ) const -> void;
    auto create_unordered_access_view(
        const ComPtr<ID3D12Resource>& resource,
        const std::optional<std::reference_wrapper<ComPtr<ID3D12Resource>>> counter,
        const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc,
        D3D12_CPU_DESCRIPTOR_HANDLE descriptor
    ) -> void;
    auto create_render_target_view(
        const ComPtr<ID3D12Resource>& resource,
        const std::optional<D3D12_RENDER_TARGET_VIEW_DESC>& desc,
        D3D12_CPU_DESCRIPTOR_HANDLE descriptor
    ) const -> void;
    auto create_depth_stencil_view(
        const ComPtr<ID3D12Resource>& resource,
        const std::optional<D3D12_DEPTH_STENCIL_VIEW_DESC>& desc,
        D3D12_CPU_DESCRIPTOR_HANDLE descriptor
    ) const -> void;
    auto
    create_sampler(const D3D12_SAMPLER_DESC2& desc, D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const
        -> void;
    auto descriptor_size(D3D12_DESCRIPTOR_HEAP_TYPE heap_type) const -> uint;

    // Getters.
    auto transfer() -> GpuTransfer& { return _transfer; }
    auto swapchain() -> GpuSwapchain& { return _swapchain; }
    auto swapchain() const -> const GpuSwapchain& { return _swapchain; }
    auto frame_index() const -> uint { return _frame_index; }
    auto root_signature() const -> ID3D12RootSignature* { return _root_signature.get(); }
    auto descriptors() -> GpuDescriptors& { return _descriptors; }

    // Debugging.
    auto log_stats() -> void;
    auto video_memory_info() -> GpuVideoMemoryInfo;
    auto pix_capture() -> void;

private:
    LeakTracker _leak_tracker;
    HMODULE _pix_gpu_capturer = nullptr;
    ComPtr<IDXGIAdapter4> _adapter;
    ComPtr<ID3D12Device12> _device;
    ComPtr<ID3D12CommandQueue> _command_queue;
    std::array<ComPtr<ID3D12CommandAllocator>, FRAME_COUNT> _command_allocators;
    ComPtr<ID3D12GraphicsCommandList9> _command_list;
    GpuSwapchain _swapchain;

    uint _frame_index = 0;
    ComPtr<ID3D12Fence1> _fence;
    HANDLE _fence_event = nullptr;
    std::array<uint64_t, FRAME_COUNT> _fence_values = {};

    ComPtr<ID3D12RootSignature> _root_signature;
    GpuDescriptors _descriptors;
    GpuSamplers _samplers;
    GpuTransfer _transfer;
};

} // namespace fb
