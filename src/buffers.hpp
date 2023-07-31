#pragma once

#include "dx12.hpp"
#include "utils.hpp"
#include <string_view>
#include <type_traits>

#pragma warning(push)
#pragma warning(disable : 4458)  // declaration hides class member

namespace fb {

struct GpuRawBuffer {
    void create_raw(
        Dx& dx,
        uint32_t element_byte_size,
        uint32_t element_size,
        uint32_t byte_size,
        uint32_t alignment,
        DXGI_FORMAT format,
        D3D12_HEAP_TYPE heap_type,
        D3D12_RESOURCE_FLAGS resource_flags,
        D3D12_RESOURCE_STATES resource_state,
        std::string_view name);
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view() const;
    D3D12_INDEX_BUFFER_VIEW index_buffer_view() const;
    D3D12_CONSTANT_BUFFER_VIEW_DESC constant_buffer_view_desc() const;

    uint32_t element_byte_size = 0;
    uint32_t element_size = 0;
    uint32_t byte_size = 0;
    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
    ComPtr<ID3D12Resource> resource;
    CD3DX12_RESOURCE_DESC resource_desc;
    D3D12_RESOURCE_STATES resource_state = D3D12_RESOURCE_STATE_COMMON;
    void* ptr = nullptr;
};

template<typename T>
    requires std::is_standard_layout<T>::value
struct GpuBuffer: public GpuRawBuffer {
    void create_vb(
        Dx& dx,
        uint32_t element_size,
        D3D12_HEAP_TYPE heap_type,
        D3D12_RESOURCE_STATES resource_state,
        std::string_view name) {
        create_raw(
            dx,
            sizeof(T),
            element_size,
            sizeof(T) * element_size,
            0,
            DXGI_FORMAT_UNKNOWN,
            heap_type,
            D3D12_RESOURCE_FLAG_NONE,
            resource_state,
            name);
    }

    void create_ib(
        Dx& dx,
        uint32_t element_size,
        D3D12_HEAP_TYPE heap_type,
        D3D12_RESOURCE_STATES resource_state,
        std::string_view name) {
        static_assert(
            sizeof(T) == 2 || sizeof(T) == 4,
            "Only 16-bit and 32-bit index formats are supported");
        static_assert(std::is_unsigned_v<T>, "Index type must be unsigned");

        DXGI_FORMAT format;
        if constexpr (sizeof(T) == 2) {
            format = DXGI_FORMAT_R16_UINT;
        } else if constexpr (sizeof(T) == 4) {
            format = DXGI_FORMAT_R32_UINT;
        }

        create_raw(
            dx,
            sizeof(T),
            element_size,
            sizeof(T) * element_size,
            0,
            format,
            heap_type,
            D3D12_RESOURCE_FLAG_NONE,
            resource_state,
            name);
    }

    void create_cb(
        Dx& dx,
        D3D12_HEAP_TYPE heap_type,
        D3D12_RESOURCE_STATES resource_state,
        std::string_view name) {
        static_assert(
            sizeof(T) % size_t(256) == 0,
            "Constant buffer size must be 256-byte aligned");
        create_raw(
            dx,
            sizeof(T),
            1,
            sizeof(T),
            0,
            DXGI_FORMAT_UNKNOWN,
            heap_type,
            D3D12_RESOURCE_FLAG_NONE,
            resource_state,
            name);
    }

    void create_uav(
        Dx& dx,
        uint32_t element_size,
        D3D12_HEAP_TYPE heap_type,
        D3D12_RESOURCE_STATES resource_state,
        std::string_view name) {
        create_raw(
            dx,
            sizeof(T),
            element_size,
            sizeof(T) * element_size,
            0,
            DXGI_FORMAT_UNKNOWN,
            heap_type,
            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
            resource_state,
            name);
    }

    T* data() const {
        return reinterpret_cast<T*>(ptr);
    }
};

}  // namespace fb

#pragma warning(pop)
