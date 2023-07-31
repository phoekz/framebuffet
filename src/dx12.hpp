#pragma once

#include "win32.hpp"
#include "utils.hpp"
#include <d3dx12/d3dx12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>
#include <array>
#include <span>
#include <string_view>

namespace fb {

constexpr uint32_t FRAME_COUNT = 2;

struct DxLeakTracker {
    ~DxLeakTracker();
    fb::ComPtr<ID3D12DebugDevice2> debug_device;
};

struct Dx {
    Dx(Window* window);

    ComPtr<ID3D12Device12> device;
    DxLeakTracker leak_tracker;
    ComPtr<ID3D12CommandQueue> command_queue;
    std::array<ComPtr<ID3D12CommandAllocator>, FRAME_COUNT> command_allocators;
    ComPtr<ID3D12GraphicsCommandList9> command_list;
    ComPtr<IDXGISwapChain4> swapchain;
    uint32_t swapchain_width = 0;
    uint32_t swapchain_height = 0;
    ComPtr<ID3D12DescriptorHeap> rtv_descriptor_heap;
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, FRAME_COUNT> rtv_descriptors = {};
    std::array<ComPtr<ID3D12Resource>, fb::FRAME_COUNT> rtvs = {};

    uint32_t frame_index = 0;
    ComPtr<ID3D12Fence1> fence;
    wil::unique_handle fence_event;
    std::array<uint64_t, FRAME_COUNT> fence_values = {};

    void transition(
        const ComPtr<ID3D12Resource>& resource,
        D3D12_RESOURCE_STATES before,
        D3D12_RESOURCE_STATES after);
};

template<typename... Args>
std::string dx_name(Args&&... args) noexcept {
    constexpr std::string_view DELIMITER = " - ";
    std::ostringstream oss;
    ((oss << args << DELIMITER), ...);
    std::string str = oss.str();
    str.erase(str.size() - DELIMITER.size());
    return str;
}
void dx_set_name(ID3D12Object* object, std::string_view name);
void dx_set_name(const ComPtr<ID3D12Object>& object, std::string_view name);

}  // namespace fb
