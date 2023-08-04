#pragma once

#include "win32.hpp"
#include "utils.hpp"
#include "maths.hpp"
#include <pch.hpp>

namespace fb {

constexpr uint32_t FRAME_COUNT = 2;

struct DxLeakTracker {
    ~DxLeakTracker();
    ComPtr<ID3D12DebugDevice2> debug_device;
};

class GpuDevice {
    FB_NO_COPY_MOVE(GpuDevice);

  public:
    GpuDevice(const Window& window);

    // Direct3D 12 wrappers.
    // clang-format off
    [[nodiscard]] auto create_root_signature(const ComPtr<ID3DBlob>& signature, std::string_view name) const -> ComPtr<ID3D12RootSignature>;
    [[nodiscard]] auto create_descriptor_heap(const D3D12_DESCRIPTOR_HEAP_DESC& desc, std::string_view name) const -> ComPtr<ID3D12DescriptorHeap>;
    [[nodiscard]] auto create_graphics_pipeline_state(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, std::string_view name) const -> ComPtr<ID3D12PipelineState>;
    [[nodiscard]] auto create_compute_pipeline_state(const D3D12_COMPUTE_PIPELINE_STATE_DESC& desc, std::string_view name) -> ComPtr<ID3D12PipelineState>;
    [[nodiscard]] auto create_committed_resource(const CD3DX12_HEAP_PROPERTIES& heap_props, const D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES init_state, const std::optional<D3D12_CLEAR_VALUE>& clear_value, std::string_view name) const -> ComPtr<ID3D12Resource>;
    [[nodiscard]] auto create_fence(uint64_t init_value, std::string_view name) const -> ComPtr<ID3D12Fence1>;
    auto create_constant_buffer_view(const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc, D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const -> void;
    auto create_shader_resource_view(const ComPtr<ID3D12Resource>& resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc, D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const -> void;
    auto create_unordered_access_view(const ComPtr<ID3D12Resource>& resource, const std::optional<std::reference_wrapper<ComPtr<ID3D12Resource>>> counter, const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc, D3D12_CPU_DESCRIPTOR_HANDLE descriptor) -> void;
    auto create_render_target_view(const ComPtr<ID3D12Resource>& resource, const std::optional<D3D12_RENDER_TARGET_VIEW_DESC>& desc, D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const -> void;
    auto create_depth_stencil_view(const ComPtr<ID3D12Resource>& resource, const std::optional<D3D12_DEPTH_STENCIL_VIEW_DESC>& desc, D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const -> void;
    auto create_sampler(const D3D12_SAMPLER_DESC& desc, D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const -> void;
    auto descriptor_size(D3D12_DESCRIPTOR_HEAP_TYPE heap_type) const -> uint32_t;
    // clang-format on

    auto begin_frame() -> void;
    auto begin_main_pass() -> void;
    auto end_main_pass() -> void;
    auto end_frame() -> void;
    auto wait() -> void;

    auto command_list() const -> ID3D12GraphicsCommandList9* { return _command_list.get(); }
    auto swapchain_size() const -> Uint2 { return _swapchain_size; }
    auto frame_index() const -> uint32_t { return _frame_index; }

    auto transition(
        const ComPtr<ID3D12Resource>& resource,
        D3D12_RESOURCE_STATES before,
        D3D12_RESOURCE_STATES after) const -> void;
    auto easy_upload(
        const D3D12_SUBRESOURCE_DATA& data,
        const ComPtr<ID3D12Resource>& resource,
        D3D12_RESOURCE_STATES before_state,
        D3D12_RESOURCE_STATES after_state) const -> void;

  private:
    ComPtr<ID3D12Device12> _device;
    DxLeakTracker _leak_tracker;
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
};

template<typename... Args>
[[nodiscard]] auto dx_name(Args&&... args) -> std::string {
    constexpr std::string_view DELIMITER = " - ";
    std::ostringstream oss;
    ((oss << args << DELIMITER), ...);
    std::string str = oss.str();
    str.erase(str.size() - DELIMITER.size());
    return str;
}
auto dx_set_name(ID3D12Object* object, std::string_view name) -> void;
auto dx_set_name(const ComPtr<ID3D12Object>& object, std::string_view name) -> void;

}  // namespace fb
