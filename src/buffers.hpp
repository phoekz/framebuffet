#pragma once

#include "dx12.hpp"
#include "utils.hpp"
#include <pch.hpp>

namespace fb {

enum class GpuBufferAccessMode {
    HostWritable,
    GpuExclusive,
};

template<typename T>
    requires std::is_standard_layout<T>::value
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
        std::string_view name) -> void {
        // Resource.
        auto host_visible = access_mode == GpuBufferAccessMode::HostWritable;
        auto heap_type = host_visible ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT;
        auto resource_state =
            host_visible ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_COPY_DEST;
        auto heap_properties = CD3DX12_HEAP_PROPERTIES(heap_type);
        _resource_desc = CD3DX12_RESOURCE_DESC::Buffer(byte_size, resource_flags, alignment);
        FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
            &heap_properties,
            D3D12_HEAP_FLAG_NONE,
            &_resource_desc,
            resource_state,
            nullptr,
            IID_PPV_ARGS(&_resource)));

        // Debug.
        dx_set_name(_resource, name);

        // Map host visible pointer.
        if (host_visible) {
            CD3DX12_RANGE read_range(0, 0);
            FAIL_FAST_IF_FAILED(_resource->Map(0, &read_range, &_raw));
            memset(_raw, 0, byte_size);
        }

        // GPU address.
        _gpu_address = _resource->GetGPUVirtualAddress();

        // Save.
        this->_element_byte_size = element_byte_size;
        this->_element_size = element_size;
        this->_byte_size = byte_size;
        this->_format = format;
        this->_resource_state = resource_state;
    }

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

    auto create_srv(
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

    auto vertex_buffer_view() const -> D3D12_VERTEX_BUFFER_VIEW {
        return D3D12_VERTEX_BUFFER_VIEW {
            .BufferLocation = _resource->GetGPUVirtualAddress(),
            .SizeInBytes = _byte_size,
            .StrideInBytes = _element_byte_size,
        };
    }
    auto index_buffer_view() const -> D3D12_INDEX_BUFFER_VIEW {
        return D3D12_INDEX_BUFFER_VIEW {
            .BufferLocation = _resource->GetGPUVirtualAddress(),
            .SizeInBytes = _byte_size,
            .Format = _format,
        };
    }
    auto constant_buffer_view_desc() const -> D3D12_CONSTANT_BUFFER_VIEW_DESC {
        return D3D12_CONSTANT_BUFFER_VIEW_DESC {
            .BufferLocation = _resource->GetGPUVirtualAddress(),
            .SizeInBytes = _byte_size,
        };
    }
    auto shader_resource_view_desc() const -> D3D12_SHADER_RESOURCE_VIEW_DESC {
        return D3D12_SHADER_RESOURCE_VIEW_DESC {
            .Format = _format,
            .ViewDimension = D3D12_SRV_DIMENSION_BUFFER,
            .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
            .Buffer =
                {
                    .FirstElement = 0,
                    .NumElements = _element_size,
                    .StructureByteStride = _element_byte_size,
                    .Flags = D3D12_BUFFER_SRV_FLAG_NONE,
                },
        };
    }

    auto element_byte_size() const -> uint32_t { return _element_byte_size; }
    auto element_size() const -> uint32_t { return _element_size; }
    auto byte_size() const -> uint32_t { return _byte_size; }
    auto format() const -> DXGI_FORMAT { return _format; }
    auto resource() const -> ID3D12Resource* { return _resource.get(); }
    auto resource_state() const -> D3D12_RESOURCE_STATES { return _resource_state; }
    auto raw() const -> void* { return _raw; }
    auto ptr() const -> T* { return reinterpret_cast<T*>(raw()); }
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

}  // namespace fb
