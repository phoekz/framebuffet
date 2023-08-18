#pragma once

#include "../pch.hpp"
#include "debug.hpp"

namespace fb {

inline constexpr uint32_t FRAME_COUNT = 2;
inline constexpr D3D_FEATURE_LEVEL MIN_FEATURE_LEVEL = D3D_FEATURE_LEVEL_12_2;

class GpuCommandList;
class GpuDescriptors;
class GpuSamplers;

class GpuDevice {
    FB_NO_COPY_MOVE(GpuDevice);

  public:
    // Device state.
    GpuDevice(const Window& window);
    auto begin_frame() -> GpuCommandList;
    auto begin_main_pass() -> void;
    auto end_main_pass() -> void;
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
        const CD3DX12_HEAP_PROPERTIES& heap_props,
        const D3D12_RESOURCE_DESC& desc,
        D3D12_RESOURCE_STATES init_state,
        const std::optional<D3D12_CLEAR_VALUE>& clear_value,
        std::string_view name) const -> ComPtr<ID3D12Resource>;
    [[nodiscard]] auto create_fence(uint64_t init_value, std::string_view name) const
        -> ComPtr<ID3D12Fence1>;
    auto create_constant_buffer_view(
        const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc,
        D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const -> void;
    auto create_shader_resource_view(
        const ComPtr<ID3D12Resource>& resource,
        const D3D12_SHADER_RESOURCE_VIEW_DESC& desc,
        D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const -> void;
    auto create_unordered_access_view(
        const ComPtr<ID3D12Resource>& resource,
        const std::optional<std::reference_wrapper<ComPtr<ID3D12Resource>>> counter,
        const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc,
        D3D12_CPU_DESCRIPTOR_HANDLE descriptor) -> void;
    auto create_render_target_view(
        const ComPtr<ID3D12Resource>& resource,
        const std::optional<D3D12_RENDER_TARGET_VIEW_DESC>& desc,
        D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const -> void;
    auto create_depth_stencil_view(
        const ComPtr<ID3D12Resource>& resource,
        const std::optional<D3D12_DEPTH_STENCIL_VIEW_DESC>& desc,
        D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const -> void;
    auto
    create_sampler(const D3D12_SAMPLER_DESC& desc, D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const
        -> void;
    auto descriptor_size(D3D12_DESCRIPTOR_HEAP_TYPE heap_type) const -> uint32_t;

    // Resource utilities.
    auto easy_upload(
        const D3D12_SUBRESOURCE_DATA& data,
        const ComPtr<ID3D12Resource>& resource,
        D3D12_RESOURCE_STATES before_state,
        D3D12_RESOURCE_STATES after_state) const -> void;
    auto easy_multi_upload(
        std::span<const D3D12_SUBRESOURCE_DATA> datas,
        const ComPtr<ID3D12Resource>& resource,
        D3D12_RESOURCE_STATES before_state,
        D3D12_RESOURCE_STATES after_state) const -> void;

    // Getters.
    auto swapchain_size() const -> Uint2 { return _swapchain_size; }
    auto frame_index() const -> uint32_t { return _frame_index; }
    auto root_signature() const -> ID3D12RootSignature* { return _root_signature.get(); }
    auto descriptors() -> GpuDescriptors& { return *_descriptors; }

    // Debugging.
    auto log_stats() -> void;

  private:
    LeakTracker _leak_tracker;
    ComPtr<ID3D12Device12> _device;
    ComPtr<ID3D12CommandQueue> _command_queue;
    std::array<ComPtr<ID3D12CommandAllocator>, FRAME_COUNT> _command_allocators;
    ComPtr<ID3D12GraphicsCommandList9> _command_list;
    ComPtr<IDXGISwapChain4> _swapchain;
    Uint2 _swapchain_size = {};
    ComPtr<ID3D12DescriptorHeap> _rtv_descriptor_heap;
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, FRAME_COUNT> _rtv_descriptors = {};
    std::array<ComPtr<ID3D12Resource>, FRAME_COUNT> _rtvs = {};

    uint32_t _frame_index = 0;
    ComPtr<ID3D12Fence1> _fence;
    wil::unique_handle _fence_event;
    std::array<uint64_t, FRAME_COUNT> _fence_values = {};

    ComPtr<ID3D12RootSignature> _root_signature;
    std::unique_ptr<GpuDescriptors> _descriptors;
    std::unique_ptr<GpuSamplers> _samplers;
};

} // namespace fb
