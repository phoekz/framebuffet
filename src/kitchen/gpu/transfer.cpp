#include "transfer.hpp"
#include "debug.hpp"
#include "device.hpp"

namespace fb {

struct GpuTransferImpl {
    ~GpuTransferImpl() {
        CloseHandle(fence_event);
        fence_event = nullptr;
        free(subresource_data_buffer);
    }

    ComPtr<ID3D12CommandAllocator> command_allocator;
    ComPtr<ID3D12GraphicsCommandList9> command_list;
    ComPtr<ID3D12Fence> fence;
    HANDLE fence_event = nullptr;
    ComPtr<ID3D12QueryHeap> query_heap;
    ComPtr<ID3D12Resource> query_buffer;

    struct SubresourceData {
        uint64_t offset;
        uint64_t row_pitch;
        uint64_t slice_pitch;
    };
    std::byte* subresource_data_buffer = nullptr;
    size_t subresource_data_buffer_offset = 0;
    size_t subresource_data_buffer_capacity = 1024ull * 1024ull * 1024ull;
    std::vector<SubresourceData> subresource_datas;

    struct ResourceData {
        ComPtr<ID3D12Resource> resource;
        uint64_t subresource_offset;
        uint64_t subresource_count;
        D3D12_RESOURCE_STATES before_state;
        D3D12_RESOURCE_STATES after_state;
    };
    std::vector<ResourceData> resource_datas;
};

GpuTransfer::~GpuTransfer() {
    if (_impl) {
        delete _impl;
        _impl = nullptr;
    }
}

auto GpuTransfer::create(const ComPtr<ID3D12Device12>& device) -> void {
    DebugScope debug("Transfer");

    // Create implementation.
    _impl = new GpuTransferImpl();

    // Command allocator.
    FB_ASSERT_HR(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&_impl->command_allocator)
    ));
    dx_set_name(_impl->command_allocator, debug.with_name("Command Allocator"));

    // Command list.
    FB_ASSERT_HR(device->CreateCommandList1(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        D3D12_COMMAND_LIST_FLAG_NONE,
        IID_PPV_ARGS(&_impl->command_list)
    ));
    dx_set_name(_impl->command_list, debug.with_name("Command List"));

    // Fence.
    FB_ASSERT_HR(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_impl->fence)));
    dx_set_name(_impl->fence, debug.with_name("Fence"));

    // Fence event.
    _impl->fence_event = CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
    FB_ASSERT(_impl->fence_event);

    // Query heap.
    const auto query_heap_desc = D3D12_QUERY_HEAP_DESC {
        .Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP,
        .Count = 2,
        .NodeMask = 0,
    };
    FB_ASSERT_HR(device->CreateQueryHeap(&query_heap_desc, IID_PPV_ARGS(&_impl->query_heap)));

    // Query buffer.
    const auto query_buffer_desc = D3D12_RESOURCE_DESC {
        .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Alignment = 0,
        .Width = 2 * sizeof(uint64_t),
        .Height = 1,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .Format = DXGI_FORMAT_UNKNOWN,
        .SampleDesc = {1, 0},
        .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        .Flags = D3D12_RESOURCE_FLAG_NONE,
    };
    const auto query_buffer_heap_properties = D3D12_HEAP_PROPERTIES {
        .Type = D3D12_HEAP_TYPE_READBACK,
        .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
        .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
        .CreationNodeMask = 1,
        .VisibleNodeMask = 1,
    };
    FB_ASSERT_HR(device->CreateCommittedResource(
        &query_buffer_heap_properties,
        D3D12_HEAP_FLAG_NONE,
        &query_buffer_desc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&_impl->query_buffer)
    ));

    // Subresource data buffer.
    _impl->subresource_data_buffer = (std::byte*)malloc(_impl->subresource_data_buffer_capacity);
}

auto GpuTransfer::resource(
    const ComPtr<ID3D12Resource>& resource,
    const D3D12_SUBRESOURCE_DATA& data,
    D3D12_RESOURCE_STATES before_state,
    D3D12_RESOURCE_STATES after_state
) -> void {
    this->resource(resource, std::span(&data, 1), before_state, after_state);
}

auto GpuTransfer::resource(
    const ComPtr<ID3D12Resource>& resource,
    std::span<const D3D12_SUBRESOURCE_DATA> datas,
    D3D12_RESOURCE_STATES before_state,
    D3D12_RESOURCE_STATES after_state
) -> void {
    // Add resource data.
    _impl->resource_datas.push_back(GpuTransferImpl::ResourceData {
        .resource = resource,
        .subresource_offset = _impl->subresource_datas.size(),
        .subresource_count = (uint64_t)datas.size(),
        .before_state = before_state,
        .after_state = after_state,
    });

    // Avoid calling `resize` once per subresource by calculating the total byte
    // count and resizing once.
    const auto subresource_data_offset = _impl->subresource_data_buffer_offset;
    {
        auto resource_byte_count = 0ull;
        for (const auto& data : datas) {
            resource_byte_count += data.SlicePitch;
        }
        FB_ASSERT(
            _impl->subresource_data_buffer_offset + resource_byte_count
            <= _impl->subresource_data_buffer_capacity
        );
        _impl->subresource_data_buffer_offset += resource_byte_count;
    }

    // Copy subresource data into buffer.
    {
        auto dst_offset = subresource_data_offset;
        for (const auto& data : datas) {
            memcpy(_impl->subresource_data_buffer + dst_offset, data.pData, data.SlicePitch);
            _impl->subresource_datas.push_back(GpuTransferImpl::SubresourceData {
                .offset = dst_offset,
                .row_pitch = (uint64_t)data.RowPitch,
                .slice_pitch = (uint64_t)data.SlicePitch,
            });
            dst_offset += data.SlicePitch;
        }
    }
}

auto GpuTransfer::flush(const GpuDevice& device) -> void {
    DebugScope debug("Transfer");

    // Gather required metadata for all subresources.
    const auto total_subresource_count = (uint)_impl->subresource_datas.size();
    const auto total_resource_count = (uint)_impl->resource_datas.size();
    auto subresource_footprints =
        std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>(total_subresource_count);
    auto subresource_row_counts = std::vector<uint>(total_subresource_count);
    auto subresource_row_byte_counts = std::vector<uint64_t>(total_subresource_count);
    auto resource_offsets = std::vector<uint64_t>(total_resource_count);
    auto resource_dimensions = std::vector<D3D12_RESOURCE_DIMENSION>(total_resource_count);
    auto total_byte_count = 0ull;
    {
        auto resource_offset = 0ull;
        auto subresource_offset = 0ull;
        for (const auto& resource_data : _impl->resource_datas) {
            const auto subresource_count = (uint)resource_data.subresource_count;
            auto byte_count = 0ull;
            const auto desc = resource_data.resource->GetDesc();
            device.get_copyable_footprints(
                desc,
                0,
                subresource_count,
                subresource_footprints.data() + subresource_offset,
                subresource_row_counts.data() + subresource_offset,
                subresource_row_byte_counts.data() + subresource_offset,
                &byte_count
            );
            resource_offsets[resource_offset] = total_byte_count;
            resource_dimensions[resource_offset] = desc.Dimension;
            total_byte_count += byte_count;
            resource_offset += 1;
            subresource_offset += subresource_count;
        }
    }

    // Create upload buffer.
    const auto upload_buffer_desc = D3D12_RESOURCE_DESC {
        .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Alignment = 0,
        .Width = total_byte_count,
        .Height = 1,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .Format = DXGI_FORMAT_UNKNOWN,
        .SampleDesc = {1, 0},
        .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        .Flags = D3D12_RESOURCE_FLAG_NONE,
    };
    const auto upload_buffer = device.create_committed_resource(
        D3D12_HEAP_TYPE_UPLOAD,
        upload_buffer_desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        std::nullopt,
        debug.with_name("Upload Buffer")
    );

    // Map upload buffer.
    std::byte* upload_buffer_ptr = nullptr;
    FB_ASSERT_HR(upload_buffer->Map(0, nullptr, (void**)&upload_buffer_ptr));

    // Copy subresource data into upload buffer.
    auto dst_offsets = std::vector<uint64_t>(total_subresource_count);
    auto dst_offset = 0ull;
    for (uint subresource_id = 0; subresource_id < total_subresource_count; subresource_id++) {
        dst_offsets[subresource_id] = dst_offset;
        const auto& src_data = _impl->subresource_datas[subresource_id];
        const auto& dst_footprint = subresource_footprints[subresource_id];
        const auto slice_count = (uint)dst_footprint.Footprint.Depth;
        const auto row_count = subresource_row_counts[subresource_id];

        for (uint slice_id = 0; slice_id < slice_count; slice_id++) {
            auto* dst_slice = upload_buffer_ptr + dst_offset;
            const auto dst_row_pitch = dst_footprint.Footprint.RowPitch;
            const auto dst_slice_pitch = dst_footprint.Footprint.RowPitch * row_count;

            const auto* src_slice = _impl->subresource_data_buffer + src_data.offset;
            const auto src_row_pitch = src_data.row_pitch;
            const auto src_slice_pitch = src_data.slice_pitch;

            for (uint row_id = 0; row_id < row_count; row_id++) {
                memcpy(
                    dst_slice + dst_slice_pitch * slice_id + dst_row_pitch * row_id,
                    src_slice + src_slice_pitch * slice_id + src_row_pitch * row_id,
                    src_row_pitch
                );
            }
            dst_offset += dst_slice_pitch;
        }
    }

    // Unmap.
    upload_buffer->Unmap(0, nullptr);

    // Reset commands.
    _impl->command_allocator->Reset();
    _impl->command_list->Reset(_impl->command_allocator.get(), nullptr);

    // Begin commands.
    PIXBeginEvent(
        _impl->command_list.get(),
        PIX_COLOR_DEFAULT,
        "Upload - %.3f MB",
        total_byte_count / 1e6f
    );

    // Query timestamp - Begin.
    _impl->command_list->EndQuery(_impl->query_heap.get(), D3D12_QUERY_TYPE_TIMESTAMP, 0);

    // Combined resource barriers.
    // - Transition all resources into COPY_DEST.
    // - Transition upload buffer into COPY_SOURCE.
    // - Transition query buffer into COPY_DEST.
    auto resource_barriers = std::vector<D3D12_RESOURCE_BARRIER>();
    resource_barriers.reserve(total_resource_count + 1 + 1);
    for (uint resource_id = 0; resource_id < total_resource_count; resource_id++) {
        const auto resource_data = _impl->resource_datas[resource_id];
        resource_barriers.push_back(D3D12_RESOURCE_BARRIER {
            .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
            .Transition =
                D3D12_RESOURCE_TRANSITION_BARRIER {
                    .pResource = resource_data.resource.get(),
                    .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                    .StateBefore = resource_data.before_state,
                    .StateAfter = D3D12_RESOURCE_STATE_COPY_DEST,
                },
        });
    }
    resource_barriers.push_back(D3D12_RESOURCE_BARRIER {
        .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
        .Transition =
            D3D12_RESOURCE_TRANSITION_BARRIER {
                .pResource = upload_buffer.get(),
                .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                .StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ,
                .StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE,
            },
    });
    resource_barriers.push_back(D3D12_RESOURCE_BARRIER {
        .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
        .Transition =
            D3D12_RESOURCE_TRANSITION_BARRIER {
                .pResource = _impl->query_buffer.get(),
                .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                .StateBefore = D3D12_RESOURCE_STATE_COMMON,
                .StateAfter = D3D12_RESOURCE_STATE_COPY_DEST,
            },
    });
    _impl->command_list->ResourceBarrier((uint)resource_barriers.size(), resource_barriers.data());
    resource_barriers.clear();

    // Issue commands.
    for (uint resource_id = 0; resource_id < total_resource_count; resource_id++) {
        const auto resource_dimension = resource_dimensions[resource_id];
        const auto resource_data = _impl->resource_datas[resource_id];
        const auto subresource_count = resource_data.subresource_count;
        const auto dst_resource = resource_data.resource.get();
        const auto src_resource = upload_buffer.get();

        PIXBeginEvent(
            _impl->command_list.get(),
            PIX_COLOR_DEFAULT,
            "Resource %d - Subresources %d",
            resource_id,
            subresource_count
        );
        switch (resource_dimension) {
            case D3D12_RESOURCE_DIMENSION_BUFFER: {
                const auto& footprint = subresource_footprints[resource_data.subresource_offset];
                const auto src_offset = dst_offsets[resource_data.subresource_offset];
                const auto src_byte_count = footprint.Footprint.Width;
                _impl->command_list
                    ->CopyBufferRegion(dst_resource, 0, src_resource, src_offset, src_byte_count);
                break;
            }
            case D3D12_RESOURCE_DIMENSION_TEXTURE2D: {
                for (uint subresource_id = 0; subresource_id < subresource_count;
                     subresource_id++) {
                    const auto dst = D3D12_TEXTURE_COPY_LOCATION {
                        .pResource = dst_resource,
                        .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
                        .SubresourceIndex = subresource_id,
                    };
                    const auto& footprint =
                        subresource_footprints[resource_data.subresource_offset + subresource_id];
                    const auto src = D3D12_TEXTURE_COPY_LOCATION {
                        .pResource = src_resource,
                        .Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
                        .PlacedFootprint =
                            D3D12_PLACED_SUBRESOURCE_FOOTPRINT {
                                .Offset =
                                    dst_offsets[resource_data.subresource_offset + subresource_id],
                                .Footprint = footprint.Footprint,
                            },
                    };
                    _impl->command_list->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
                }
                break;
            }
            default: FB_FATAL(); break;
        }
        PIXEndEvent(_impl->command_list.get());
    }

    // Transition all resources from COPY_DEST to after_state.
    for (uint resource_id = 0; resource_id < total_resource_count; resource_id++) {
        const auto resource_data = _impl->resource_datas[resource_id];
        resource_barriers.push_back(D3D12_RESOURCE_BARRIER {
            .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
            .Transition =
                D3D12_RESOURCE_TRANSITION_BARRIER {
                    .pResource = resource_data.resource.get(),
                    .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                    .StateBefore = D3D12_RESOURCE_STATE_COPY_DEST,
                    .StateAfter = resource_data.after_state,
                },
        });
    }
    _impl->command_list->ResourceBarrier((uint)resource_barriers.size(), resource_barriers.data());

    // Query timestamp - End.
    _impl->command_list->EndQuery(_impl->query_heap.get(), D3D12_QUERY_TYPE_TIMESTAMP, 1);
    _impl->command_list->ResolveQueryData(
        _impl->query_heap.get(),
        D3D12_QUERY_TYPE_TIMESTAMP,
        0,
        2,
        _impl->query_buffer.get(),
        0
    );

    // Close command list.
    PIXEndEvent(_impl->command_list.get());
    FB_ASSERT_HR(_impl->command_list->Close());

    // Execute command list.
    const auto command_lists = std::to_array<ID3D12CommandList*>({_impl->command_list.get()});
    device.command_queue()->ExecuteCommandLists((uint)command_lists.size(), command_lists.data());

    // Signal fence.
    const auto fence_value = 1ull;
    device.command_queue()->Signal(_impl->fence.get(), fence_value);
    _impl->fence->SetEventOnCompletion(fence_value, _impl->fence_event);

    // Wait for fence.
    WaitForSingleObject(_impl->fence_event, INFINITE);

    // Read query buffer.
    std::array<uint64_t, 2> timestamps;
    {
        void* mapped_query_buffer = nullptr;
        FB_ASSERT_HR(_impl->query_buffer->Map(0, nullptr, &mapped_query_buffer));
        memcpy(timestamps.data(), mapped_query_buffer, sizeof(timestamps));
        _impl->query_buffer->Unmap(0, nullptr);
    }

    // Calculate time.
    uint64_t frequency = 0;
    FB_ASSERT_HR(device.command_queue()->GetTimestampFrequency(&frequency));
    const auto seconds = (timestamps[1] - timestamps[0]) / (double)frequency;

    // Log stats.
    FB_LOG_INFO("Resource count: {}", total_resource_count);
    FB_LOG_INFO("Subresource count: {}", total_subresource_count);
    FB_LOG_INFO("Upload time: {:.3f} ms", seconds * 1000.0);
    FB_LOG_INFO("Upload bytes: {:.3f} MB", total_byte_count / 1e6);
    FB_LOG_INFO("Upload speed: {:.3f} GB/s", total_byte_count / seconds / 1e9);
}

} // namespace fb
