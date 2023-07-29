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

struct Dx {
    ID3D12Device12* device = nullptr;
    ID3D12CommandQueue* command_queue = nullptr;
    std::array<ID3D12CommandAllocator*, FRAME_COUNT> command_allocators = {};
    ID3D12GraphicsCommandList9* command_list = nullptr;
    IDXGISwapChain4* swapchain = nullptr;
    ID3D12DescriptorHeap* rtv_descriptor_heap;
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, FRAME_COUNT> rtv_descriptors = {};
    std::array<ID3D12Resource*, fb::FRAME_COUNT> rtvs = {};

    uint32_t frame_index = 0;
    ID3D12Fence1* fence = nullptr;
    HANDLE fence_event = nullptr;
    std::array<uint64_t, FRAME_COUNT> fence_values = {};
};
void dx_create(Dx* dx, Window* window);
void dx_destroy(Dx* dx);
void dx_set_name(ID3D12Object* object, const char* name);
void dx_set_name(const ComPtr<ID3D12Object>& object, const char* name);
void dx_set_indexed_name(ID3D12Object* object, const char* name, uint32_t index);
void dx_set_indexed_name(const ComPtr<ID3D12Object>& object, const char* name, uint32_t index);

}  // namespace fb
