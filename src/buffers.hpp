#pragma once

#include "dx12.hpp"
#include "utils.hpp"
#include <string_view>
#include <type_traits>

#pragma warning(push)
#pragma warning(disable : 4458)  // declaration hides class member

namespace fb {

enum class GpuBufferAccessMode {
    HostWritable,
    GpuExclusive,
};

class GpuBuffer {
  public:
    auto create_raw(
        Dx& dx,
        uint32_t element_byte_size,
        uint32_t element_size,
        uint32_t byte_size,
        uint32_t alignment,
        DXGI_FORMAT format,
        GpuBufferAccessMode access_mode,
        D3D12_RESOURCE_FLAGS resource_flags,
        std::string_view name) -> void;

    auto vertex_buffer_view() const -> D3D12_VERTEX_BUFFER_VIEW;
    auto index_buffer_view() const -> D3D12_INDEX_BUFFER_VIEW;
    auto constant_buffer_view_desc() const -> D3D12_CONSTANT_BUFFER_VIEW_DESC;
    auto element_byte_size() const -> uint32_t { return _element_byte_size; }
    auto element_size() const -> uint32_t { return _element_size; }
    auto byte_size() const -> uint32_t { return _byte_size; }
    auto format() const -> DXGI_FORMAT { return _format; }
    auto resource() const -> ID3D12Resource* { return _resource.get(); }
    auto resource_state() const -> D3D12_RESOURCE_STATES { return _resource_state; }
    auto raw() const -> void* { return _raw; }
    auto gpu_address() const -> D3D12_GPU_VIRTUAL_ADDRESS { return _gpu_address; }

  private:
    uint32_t _element_byte_size = 0;
    uint32_t _element_size = 0;
    uint32_t _byte_size = 0;
    DXGI_FORMAT _format = DXGI_FORMAT_UNKNOWN;
    ComPtr<ID3D12Resource> _resource;
    CD3DX12_RESOURCE_DESC _resource_desc;
    D3D12_RESOURCE_STATES _resource_state = D3D12_RESOURCE_STATE_COMMON;
    void* _raw = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS _gpu_address = 0;
};

template<typename T>
    requires std::is_standard_layout<T>::value
class GpuTypedBuffer final: public GpuBuffer {
  public:
    auto
    create_vb(Dx& dx, uint32_t element_size, GpuBufferAccessMode access_mode, std::string_view name)
        -> void {
        create_raw(
            dx,
            sizeof(T),
            element_size,
            sizeof(T) * element_size,
            0,
            DXGI_FORMAT_UNKNOWN,
            access_mode,
            D3D12_RESOURCE_FLAG_NONE,
            name);
    }

    auto
    create_ib(Dx& dx, uint32_t element_size, GpuBufferAccessMode access_mode, std::string_view name)
        -> void {
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
            access_mode,
            D3D12_RESOURCE_FLAG_NONE,
            name);
    }

    auto create_cb(Dx& dx, GpuBufferAccessMode access_mode, std::string_view name) -> void {
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
            access_mode,
            D3D12_RESOURCE_FLAG_NONE,
            name);
    }

    auto create_uav(
        Dx& dx,
        uint32_t element_size,
        GpuBufferAccessMode access_mode,
        std::string_view name) -> void {
        create_raw(
            dx,
            sizeof(T),
            element_size,
            sizeof(T) * element_size,
            0,
            DXGI_FORMAT_UNKNOWN,
            access_mode,
            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
            name);
    }

    auto ptr() const -> T* { return reinterpret_cast<T*>(raw()); }
};

}  // namespace fb

#pragma warning(pop)
