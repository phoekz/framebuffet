#pragma once

#include "device.hpp"

namespace fb {

enum class GpuBufferAccessMode {
    Host,
    Device,
};

enum class GpuBufferFlags : uint {
    None = 0x0,
    Cbv = 0x1,
    Srv = 0x2,
    Uav = 0x4,
    SrvUav = Srv | Uav,
    Index = 0x8,
    Readback = 0x10,
};

inline constexpr auto gpu_buffer_flags_contains(GpuBufferFlags flags, GpuBufferFlags flag) -> bool {
    return ((uint)flags & (uint)flag) == (uint)flag;
}

namespace detail {
    template<typename T, GpuBufferAccessMode ACCESS_MODE, GpuBufferFlags FLAGS>
    inline constexpr auto buffer_type_is_valid() -> bool {
        using enum GpuBufferAccessMode;
        using enum GpuBufferFlags;
        static_assert(std::is_standard_layout_v<T>, "Buffer type must be standard layout");
        static_assert(std::is_trivially_copyable_v<T>, "Buffer type must be trivially copyable");
        static_assert(std::is_constructible_v<T>, "Buffer type must be constructible");
        static_assert(std::is_pointer_v<T> == false, "Buffer type cannot be a pointer");
        if constexpr (gpu_buffer_flags_contains(FLAGS, Index)) {
            static_assert(
                std::is_same_v<T, uint16_t> || std::is_same_v<T, uint>,
                "Index buffer only supports 16-bit and 32-bit unsigned formats"
            );
        }
        if constexpr (gpu_buffer_flags_contains(FLAGS, Cbv)) {
            static_assert(
                sizeof(T) % size_t(256) == 0,
                "Constant buffers must be 256-byte aligned"
            );
        }
        if constexpr (gpu_buffer_flags_contains(FLAGS, Readback)) {
            static_assert(FLAGS == Readback, "A readback buffer cannot have any other flags.");
            static_assert(ACCESS_MODE == Host, "A readback buffer must be host accessible.");
            static_assert(std::is_same_v<T, std::byte>, "A readback buffer must be byte type.");
        }
        return true;
    }
} // namespace detail

template<typename T, GpuBufferAccessMode ACCESS_MODE, GpuBufferFlags FLAGS>
    requires(detail::buffer_type_is_valid<T, ACCESS_MODE, FLAGS>())
class GpuBuffer {
    FB_NO_COPY_MOVE(GpuBuffer);

public:
    using enum GpuBufferAccessMode;
    using enum GpuBufferFlags;

    GpuBuffer() = default;

    auto create(GpuDevice& device, uint element_count, std::string_view name) -> void {
        // Format.
        if constexpr (gpu_buffer_flags_contains(FLAGS, Index)) {
            if (std::is_same_v<T, uint16_t>) {
                _format = DXGI_FORMAT_R16_UINT;
            } else if (std::is_same_v<T, uint>) {
                _format = DXGI_FORMAT_R32_UINT;
            }
        }

        // Resource flags.
        auto resource_flags = D3D12_RESOURCE_FLAG_NONE;
        if constexpr (gpu_buffer_flags_contains(FLAGS, Uav)) {
            resource_flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }
        if constexpr (FLAGS == Readback) {
            resource_flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
        }

        // Resource desc.
        _element_count = element_count;
        _byte_count = _element_byte_count * element_count;
        _resource_desc = D3D12_RESOURCE_DESC1 {
            .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
            .Alignment = 0,
            .Width = _byte_count,
            .Height = 1,
            .DepthOrArraySize = 1,
            .MipLevels = 1,
            .Format = DXGI_FORMAT_UNKNOWN,
            .SampleDesc = {1, 0},
            .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
            .Flags = resource_flags,
            .SamplerFeedbackMipRegion = {},
        };

        // Heap type.
        D3D12_HEAP_TYPE heap_type = D3D12_HEAP_TYPE_DEFAULT;
        if constexpr (FLAGS == Readback) {
            heap_type = D3D12_HEAP_TYPE_READBACK;
        } else if constexpr (ACCESS_MODE == Host) {
            heap_type = D3D12_HEAP_TYPE_UPLOAD;
        }

        // Create.
        _resource = device.create_committed_resource(
            heap_type,
            _resource_desc,
            D3D12_BARRIER_LAYOUT_UNDEFINED,
            std::nullopt,
            name
        );

        // Prepare host visible pointer.
        if constexpr (ACCESS_MODE == Host) {
            // Map pointer.
            D3D12_RANGE read_range {.Begin = 0, .End = 0};
            FB_ASSERT_HR(_resource->Map(0, &read_range, &_raw));

            // Clear memory.
            memset(_raw, 0, _byte_count);
        }

        // GPU address.
        _gpu_address = _resource->GetGPUVirtualAddress();

        // Views.
        if constexpr (gpu_buffer_flags_contains(FLAGS, Cbv)) {
            _cbv_descriptor = device.descriptors().cbv_srv_uav().alloc();
            device.create_constant_buffer_view(
                D3D12_CONSTANT_BUFFER_VIEW_DESC {
                    .BufferLocation = _gpu_address,
                    .SizeInBytes = _byte_count,
                },
                _cbv_descriptor.cpu()
            );
        }
        if constexpr (gpu_buffer_flags_contains(FLAGS, Srv)) {
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
                            .NumElements = _element_count,
                            .StructureByteStride = _element_byte_count,
                            .Flags = D3D12_BUFFER_SRV_FLAG_NONE,
                        },
                },
                _srv_descriptor.cpu()
            );
        }
        if constexpr (gpu_buffer_flags_contains(FLAGS, Uav)) {
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
                            .NumElements = _element_count,
                            .StructureByteStride = _element_byte_count,
                            .CounterOffsetInBytes = 0,
                            .Flags = D3D12_BUFFER_UAV_FLAG_NONE,
                        },
                },
                _uav_descriptor.cpu()
            );
        }
    }
    auto create_with_data(GpuDevice& device, Span<const T> data, std::string_view name) -> void {
        static_assert(ACCESS_MODE == Host);
        create(device, (uint)data.size(), name);
        memcpy(_raw, data.data(), data.size_bytes());
    }
    auto create_and_transfer(
        GpuDevice& device,
        Span<const T> data,
        D3D12_BARRIER_SYNC sync_after,
        D3D12_BARRIER_ACCESS access_after,
        std::string_view name
    ) -> void {
        static_assert(ACCESS_MODE == Device);
        create(device, (uint)data.size(), name);
        device.transfer().resource(
            _resource,
            D3D12_SUBRESOURCE_DATA {
                .pData = data.data(),
                .RowPitch = (LONG_PTR)data.size_bytes(),
                .SlicePitch = (LONG_PTR)data.size_bytes(),
            },
            _sync_before,
            sync_after,
            _access_before,
            access_after
        );
    }

    auto element_count() const -> uint { return _element_count; }
    auto byte_count() const -> uint { return _byte_count; }
    auto resource() const -> const ComPtr<ID3D12Resource2>& { return _resource; }
    auto raw() const -> void* { return _raw; }
    auto ptr() const -> T* { return reinterpret_cast<T*>(raw()); }
    auto ref() const -> T& { return *ptr(); }
    auto span() const -> Span<T> { return Span<T>(ptr(), element_count()); }
    auto index_buffer_view() const -> D3D12_INDEX_BUFFER_VIEW {
        static_assert(gpu_buffer_flags_contains(FLAGS, Index));
        return D3D12_INDEX_BUFFER_VIEW {
            .BufferLocation = _gpu_address,
            .SizeInBytes = _byte_count,
            .Format = _format,
        };
    }
    auto cbv_descriptor() const -> GpuDescriptor {
        static_assert(gpu_buffer_flags_contains(FLAGS, Cbv));
        return _cbv_descriptor;
    }
    auto srv_descriptor() const -> GpuDescriptor {
        static_assert(gpu_buffer_flags_contains(FLAGS, Srv));
        return _srv_descriptor;
    }
    auto uav_descriptor() const -> GpuDescriptor {
        static_assert(gpu_buffer_flags_contains(FLAGS, Uav));
        return _uav_descriptor;
    }

    auto transition(
        GpuCommandList& cmd,
        D3D12_BARRIER_SYNC sync_after,
        D3D12_BARRIER_ACCESS access_after
    ) -> void {
        explicit_transition(cmd, _sync_before, sync_after, _access_before, access_after);
    }

    auto explicit_transition(
        GpuCommandList& cmd,
        D3D12_BARRIER_SYNC sync_before,
        D3D12_BARRIER_SYNC sync_after,
        D3D12_BARRIER_ACCESS access_before,
        D3D12_BARRIER_ACCESS access_after
    ) -> void {
        cmd.buffer_barrier(sync_before, sync_after, access_before, access_after, _resource);

        _sync_before = sync_after;
        _access_before = access_after;
    }

private:
    uint _element_byte_count = (uint)sizeof(T);
    uint _element_count = 0;
    uint _byte_count = 0;
    DXGI_FORMAT _format = DXGI_FORMAT_UNKNOWN;
    ComPtr<ID3D12Resource2> _resource;
    D3D12_RESOURCE_DESC1 _resource_desc;
    D3D12_BARRIER_SYNC _sync_before = D3D12_BARRIER_SYNC_NONE;
    D3D12_BARRIER_ACCESS _access_before = D3D12_BARRIER_ACCESS_NO_ACCESS;
    void* _raw = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS _gpu_address = 0;
    GpuDescriptor _cbv_descriptor = {};
    GpuDescriptor _srv_descriptor = {};
    GpuDescriptor _uav_descriptor = {};
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
using GpuBufferHostReadback = GpuBuffer<T, GpuBufferAccessMode::Host, GpuBufferFlags::Readback>;

template<typename T>
using GpuBufferDeviceSrv = GpuBuffer<T, GpuBufferAccessMode::Device, GpuBufferFlags::Srv>;
template<typename T>
using GpuBufferDeviceUav = GpuBuffer<T, GpuBufferAccessMode::Device, GpuBufferFlags::Uav>;
template<typename T>
using GpuBufferDeviceIndex = GpuBuffer<T, GpuBufferAccessMode::Device, GpuBufferFlags::Index>;
template<typename T>
using GpuBufferDeviceSrvUav = GpuBuffer<T, GpuBufferAccessMode::Device, GpuBufferFlags::SrvUav>;

} // namespace fb
