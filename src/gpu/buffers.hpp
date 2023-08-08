#pragma once

#include <pch.hpp>
#include "device.hpp"
#include "utils.hpp"

namespace fb {

enum class GpuBufferAccessMode {
    Host,
    Device,
};

enum class GpuBufferFlags : uint32_t {
    None = 0x0,
    Cbv = 0x1,
    Srv = 0x2,
    Uav = 0x4,
    SrvUav = Srv | Uav,
    Index = 0x8,
};

constexpr auto gpu_buffer_flags_is_set(GpuBufferFlags flags, GpuBufferFlags flag) -> bool {
    return ((uint32_t)flags & (uint32_t)flag) > 0;
}

namespace detail {
    template<typename T, GpuBufferFlags FLAGS>
    constexpr auto buffer_type_is_valid() -> bool {
        static_assert(std::is_standard_layout_v<T>, "Buffer type must be standard layout");
        static_assert(std::is_trivially_copyable_v<T>, "Buffer type must be trivially copyable");
        static_assert(std::is_constructible_v<T>, "Buffer type must be constructible");
        static_assert(std::is_pointer_v<T> == false, "Buffer type cannot be a pointer");
        if constexpr (gpu_buffer_flags_is_set(FLAGS, GpuBufferFlags::Index)) {
            static_assert(
                std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t>,
                "Index buffer only supports 16-bit and 32-bit unsigned formats");
        }
        if constexpr (gpu_buffer_flags_is_set(FLAGS, GpuBufferFlags::Cbv)) {
            static_assert(sizeof(T) % size_t(256) == 0, "Constant buffer must be 256-byte aligned");
        }
        return true;
    }
}  // namespace detail

template<typename T, GpuBufferAccessMode ACCESS_MODE, GpuBufferFlags FLAGS>
    requires(detail::buffer_type_is_valid<T, FLAGS>())
class GpuBuffer {
  public:
    auto create(GpuDevice& device, uint32_t element_size, std::string_view name) -> void {
        // Format.
        if constexpr (gpu_buffer_flags_is_set(FLAGS, GpuBufferFlags::Index)) {
            if (std::is_same_v<T, uint16_t>) {
                _format = DXGI_FORMAT_R16_UINT;
            } else if (std::is_same_v<T, uint32_t>) {
                _format = DXGI_FORMAT_R32_UINT;
            }
        }

        // Resource flags.
        auto resource_flags = D3D12_RESOURCE_FLAG_NONE;
        if constexpr (gpu_buffer_flags_is_set(FLAGS, GpuBufferFlags::Uav)) {
            resource_flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }

        // Resource properties.
        auto host_visible = ACCESS_MODE == GpuBufferAccessMode::Host;
        auto heap_type = host_visible ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT;
        _element_size = element_size;
        _byte_size = _element_byte_size * element_size;
        _resource_desc = CD3DX12_RESOURCE_DESC::Buffer(_byte_size, resource_flags);
        _resource_state =
            host_visible ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_COMMON;

        // Create.
        _resource = device.create_committed_resource(
            CD3DX12_HEAP_PROPERTIES {heap_type},
            _resource_desc,
            _resource_state,
            std::nullopt,
            name);

        // Prepare host visible pointer.
        if (host_visible) {
            // Map pointer.
            CD3DX12_RANGE read_range(0, 0);
            FB_ASSERT_HR(_resource->Map(0, &read_range, &_raw));

            // Clear memory.
            memset(_raw, 0, _byte_size);

            // Inplace construct elements.
            for (uint32_t i = 0; i < element_size; i++) {
                new (reinterpret_cast<T*>(_raw) + i) T();
            }
        }

        // GPU address.
        _gpu_address = _resource->GetGPUVirtualAddress();

        // Views.
        if constexpr (gpu_buffer_flags_is_set(FLAGS, GpuBufferFlags::Cbv)) {
            _cbv_descriptor = device.descriptors().cbv_srv_uav().alloc();
            device.create_constant_buffer_view(
                D3D12_CONSTANT_BUFFER_VIEW_DESC {
                    .BufferLocation = _gpu_address,
                    .SizeInBytes = _byte_size,
                },
                _cbv_descriptor.cpu());
        }
        if constexpr (gpu_buffer_flags_is_set(FLAGS, GpuBufferFlags::Srv)) {
            _srv_descriptor = device.descriptors().cbv_srv_uav().alloc();
            device.create_shader_resource_view(
                _resource,
                D3D12_SHADER_RESOURCE_VIEW_DESC {
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
                },
                _srv_descriptor.cpu());
        }
        if constexpr (gpu_buffer_flags_is_set(FLAGS, GpuBufferFlags::Uav)) {
            _uav_descriptor = device.descriptors().cbv_srv_uav().alloc();
            device.create_unordered_access_view(
                _resource,
                std::nullopt,
                D3D12_UNORDERED_ACCESS_VIEW_DESC {
                    .Format = _format,
                    .ViewDimension = D3D12_UAV_DIMENSION_BUFFER,
                    .Buffer =
                        {
                            .FirstElement = 0,
                            .NumElements = _element_size,
                            .StructureByteStride = _element_byte_size,
                            .CounterOffsetInBytes = 0,
                            .Flags = D3D12_BUFFER_UAV_FLAG_NONE,
                        },
                },
                _uav_descriptor.cpu());
        }
    }
    auto create_with_data(GpuDevice& device, std::span<const T> data, std::string_view name)
        -> void {
        create(device, (uint32_t)data.size(), name);
        memcpy(_raw, data.data(), data.size_bytes());
    }

    auto element_byte_size() const -> uint32_t { return _element_byte_size; }
    auto element_size() const -> uint32_t { return _element_size; }
    auto byte_size() const -> uint32_t { return _byte_size; }
    auto format() const -> DXGI_FORMAT { return _format; }
    auto resource() const -> ID3D12Resource* { return _resource.get(); }
    auto resource_state() const -> D3D12_RESOURCE_STATES { return _resource_state; }
    auto raw() const -> void* { return _raw; }
    auto ptr() const -> T* { return reinterpret_cast<T*>(raw()); }
    auto span() const -> std::span<T> { return std::span<T>(ptr(), element_size()); }
    auto gpu_address() const -> D3D12_GPU_VIRTUAL_ADDRESS { return _gpu_address; }
    auto access_mode() const -> GpuBufferAccessMode { return ACCESS_MODE; }
    auto flags() const -> GpuBufferFlags { return FLAGS; }
    auto index_buffer_view() const -> D3D12_INDEX_BUFFER_VIEW {
        static_assert(
            gpu_buffer_flags_is_set(FLAGS, GpuBufferFlags::Index),
            "Buffer is not an index buffer");
        return D3D12_INDEX_BUFFER_VIEW {
            .BufferLocation = _gpu_address,
            .SizeInBytes = _byte_size,
            .Format = _format,
        };
    }
    auto cbv_descriptor() const -> GpuDescriptorHandle {
        static_assert(
            gpu_buffer_flags_is_set(FLAGS, GpuBufferFlags::Cbv),
            "Buffer does not support CBV");
        return _cbv_descriptor;
    }
    auto srv_descriptor() const -> GpuDescriptorHandle {
        static_assert(
            gpu_buffer_flags_is_set(FLAGS, GpuBufferFlags::Srv),
            "Buffer does not support SRV");
        return _srv_descriptor;
    }
    auto uav_descriptor() const -> GpuDescriptorHandle {
        static_assert(
            gpu_buffer_flags_is_set(FLAGS, GpuBufferFlags::Uav),
            "Buffer does not support UAV");
        return _uav_descriptor;
    }

    auto transition(const GpuCommandList& cmd, D3D12_RESOURCE_STATES after) -> void {
        if (_resource_state != after) {
            cmd.transition_barrier(_resource, _resource_state, after);
            _resource_state = after;
        }
    }

  private:
    uint32_t _element_byte_size = (uint32_t)sizeof(T);
    uint32_t _element_size = 0;
    uint32_t _byte_size = 0;
    DXGI_FORMAT _format = DXGI_FORMAT_UNKNOWN;
    ComPtr<ID3D12Resource> _resource;
    CD3DX12_RESOURCE_DESC _resource_desc;
    D3D12_RESOURCE_STATES _resource_state = D3D12_RESOURCE_STATE_COMMON;
    void* _raw = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS _gpu_address = 0;
    GpuDescriptorHandle _cbv_descriptor = {};
    GpuDescriptorHandle _srv_descriptor = {};
    GpuDescriptorHandle _uav_descriptor = {};
};

template<typename T>
using GpuBufferHostCbv = GpuBuffer<T, GpuBufferAccessMode::Host, GpuBufferFlags::Cbv>;
template<typename T>
using GpuBufferHostSrv = GpuBuffer<T, GpuBufferAccessMode::Host, GpuBufferFlags::Srv>;
template<typename T>
using GpuBufferHostUav = GpuBuffer<T, GpuBufferAccessMode::Host, GpuBufferFlags::Uav>;
template<typename T>
using GpuBufferHostIndex = GpuBuffer<T, GpuBufferAccessMode::Host, GpuBufferFlags::Index>;

template<typename T>
using GpuBufferDeviceSrv = GpuBuffer<T, GpuBufferAccessMode::Device, GpuBufferFlags::Srv>;
template<typename T>
using GpuBufferDeviceUav = GpuBuffer<T, GpuBufferAccessMode::Device, GpuBufferFlags::Uav>;
template<typename T>
using GpuBufferDeviceIndex = GpuBuffer<T, GpuBufferAccessMode::Device, GpuBufferFlags::Index>;
template<typename T>
using GpuBufferDeviceSrvUav = GpuBuffer<T, GpuBufferAccessMode::Device, GpuBufferFlags::SrvUav>;

}  // namespace fb
