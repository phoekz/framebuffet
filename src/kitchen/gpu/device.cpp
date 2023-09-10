#include "device.hpp"
#include "descriptors.hpp"
#include "commands.hpp"
#include "samplers.hpp"

#include <d3dx12/d3dx12_root_signature.h>
#include <d3dx12/d3dx12_check_feature_support.h>

// Setup DirectX Agility SDK.
extern "C" {
    __declspec(dllexport) extern const UINT D3D12SDKVersion = 610;
    __declspec(dllexport) extern const char* D3D12SDKPath = ".\\";
}

namespace fb {

GpuDevice::~GpuDevice() {
    CloseHandle(_fence_event);
    _fence_event = nullptr;
    FreeLibrary(_pix_gpu_capturer);
    _pix_gpu_capturer = nullptr;
}

auto GpuDevice::create(const Window& window) -> void {
    DebugScope debug_scope("Device");

    // Load PIX GPU Capturer.
    {
        HMODULE module = GetModuleHandleA(FB_PIX_GPU_CAPTURER_NAME);
        if (!module) {
            module = LoadLibraryA(FB_PIX_GPU_CAPTURER_DLL_PATH);
        }
        _pix_gpu_capturer = module;
        FB_LOG_INFO(
            FB_PIX_GPU_CAPTURER_NAME ": {}",
            _pix_gpu_capturer != nullptr ? "Loaded" : "Not loaded"
        );
    }

    // Debug layer.
    UINT factory_flags = 0;
    {
#if defined(_DEBUG)
        factory_flags = DXGI_CREATE_FACTORY_DEBUG;

        ComPtr<ID3D12Debug6> debug;
        FB_ASSERT_HR(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)));
        debug->EnableDebugLayer();

        ComPtr<IDXGIInfoQueue> info_queue;
        FB_ASSERT_HR(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&info_queue)));
        info_queue
            ->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
        info_queue
            ->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

#endif
    }

    // Factory.
    ComPtr<IDXGIFactory7> factory;
    FB_ASSERT_HR(CreateDXGIFactory2(factory_flags, IID_PPV_ARGS(&factory)));

    // Adapter.
    FB_ASSERT_HR(factory->EnumAdapterByGpuPreference(
        0,
        DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
        IID_PPV_ARGS(&_adapter)
    ));
    DXGI_ADAPTER_DESC3 adapter_desc;
    _adapter->GetDesc3(&adapter_desc);
    FB_LOG_INFO("Using adapter {}", fb::from_wstr(adapter_desc.Description));

    // Output.
    {
        UINT i = 0;
        ComPtr<IDXGIOutput> output;
        while (_adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND) {
            ComPtr<IDXGIOutput6> output6;
            output.query_to(&output6);

            DXGI_OUTPUT_DESC1 desc;
            output6->GetDesc1(&desc);

            Rectangle desktop_rect(desc.DesktopCoordinates);
            FB_LOG_INFO(
                "Display: {} x {} @ {}bpp, luminance: {} to {}",
                desktop_rect.width,
                desktop_rect.height,
                desc.BitsPerColor,
                desc.MinLuminance,
                desc.MaxLuminance
            );

            ++i;
        }
    }

    // Device.
    FB_ASSERT_HR(D3D12CreateDevice(_adapter.get(), MIN_FEATURE_LEVEL, IID_PPV_ARGS(&_device)));
    dx_set_name(_device, "Device");

    // Debug device.
    _device->QueryInterface(IID_PPV_ARGS(&_leak_tracker.debug_device));

    {
#if defined(_DEBUG)
        ComPtr<ID3D12InfoQueue1> info_queue;
        FB_ASSERT_HR(_device->QueryInterface(IID_PPV_ARGS(&info_queue)));
        // info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        DWORD callback_cookie;
        info_queue->RegisterMessageCallback(
            [](D3D12_MESSAGE_CATEGORY,
               D3D12_MESSAGE_SEVERITY severity,
               D3D12_MESSAGE_ID,
               const char* description,
               void*) {
                FRAME_ALLOCATION_TRAP = false;
                switch (severity) {
                    case D3D12_MESSAGE_SEVERITY_CORRUPTION: FB_LOG_ERROR("{}", description); break;
                    case D3D12_MESSAGE_SEVERITY_ERROR: FB_LOG_ERROR("{}", description); break;
                    case D3D12_MESSAGE_SEVERITY_WARNING: FB_LOG_WARN("{}", description); break;
                    case D3D12_MESSAGE_SEVERITY_INFO: FB_LOG_INFO("{}", description); break;
                    default: break;
                }
                FRAME_ALLOCATION_TRAP = true;
            },
            D3D12_MESSAGE_CALLBACK_FLAG_NONE,
            nullptr,
            &callback_cookie
        );
#endif
    }

    // Device feature support.
    {
        CD3DX12FeatureSupport support;
        FB_ASSERT_HR(support.Init(_device.get()));
        FB_ASSERT(support.MinPrecisionSupport() == D3D12_SHADER_MIN_PRECISION_SUPPORT_16_BIT);
        FB_ASSERT(support.ResourceBindingTier() == D3D12_RESOURCE_BINDING_TIER_3);
        FB_ASSERT(support.ResourceHeapTier() == D3D12_RESOURCE_HEAP_TIER_2);
        FB_ASSERT(support.MaxSupportedFeatureLevel() == D3D_FEATURE_LEVEL_12_2);
        FB_ASSERT(support.HighestShaderModel() == D3D_SHADER_MODEL_6_7);
        FB_ASSERT(support.WaveOps() == TRUE);
        FB_ASSERT(support.WaveLaneCountMin() == 32 || support.WaveLaneCountMin() == 64);
        FB_LOG_INFO("Total lane count: {}", support.TotalLaneCount());
        FB_ASSERT(support.HighestRootSignatureVersion() == D3D_ROOT_SIGNATURE_VERSION_1_2);
        FB_ASSERT(support.TileBasedRenderer() == FALSE);
        FB_ASSERT(support.UMA() == FALSE);
        FB_ASSERT(support.CacheCoherentUMA() == FALSE);
        FB_ASSERT(support.IsolatedMMU() == TRUE);
        FB_ASSERT(support.RaytracingTier() == D3D12_RAYTRACING_TIER_1_1);
        FB_ASSERT(support.VariableShadingRateTier() == D3D12_VARIABLE_SHADING_RATE_TIER_2);
        FB_ASSERT(support.MeshShaderTier() == D3D12_MESH_SHADER_TIER_1);
        FB_ASSERT(support.MeshShaderPipelineStatsSupported() == TRUE);
        FB_ASSERT(support.EnhancedBarriersSupported() == TRUE);
    }

    // Command queue.
    {
        D3D12_COMMAND_QUEUE_DESC desc = {
            .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
            .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
            .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
            .NodeMask = 0,
        };
        FB_ASSERT_HR(_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_command_queue)));
        dx_set_name(_command_queue, "Command Queue");
    }

    // Command allocators.
    for (uint i = 0; i < FRAME_COUNT; i++) {
        FB_ASSERT_HR(_device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(&_command_allocators[i])
        ));
        dx_set_name(
            _command_allocators[i],
            debug_scope.with_name(std::format("Command Allocator {}", i))
        );
    }

    // Command list.
    FB_ASSERT_HR(_device->CreateCommandList1(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        D3D12_COMMAND_LIST_FLAG_NONE,
        IID_PPV_ARGS(&_command_list)
    ));
    dx_set_name(_command_list, "Command List");

    // Global descriptor heap.
    _descriptors.create(*this);

    // Swapchain.
    _swapchain.create(factory, _device, _command_queue, window, _descriptors);

    // Synchronization.
    {
        _frame_index = _swapchain.backbuffer_index();
        _fence = this->create_fence(0, "Fence");
        _fence_event = CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
        FB_ASSERT_MSG(_fence_event != nullptr, "Failed to create fence event.");
    }

    // Global root signature.
    {
        // Allocate constants for binding slots.
        CD3DX12_ROOT_PARAMETER1 root_parameter = {};
        root_parameter.InitAsConstants(MAX_BINDING_COUNT, 0, 0);

        // Create the root signature description.
        auto flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
        flags |= D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
        flags |= D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;
        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc;
        decltype(desc)::Init_1_2(desc, 1, &root_parameter, 0, nullptr, flags);

        // Serialize the root signature.
        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        auto hr = D3DX12SerializeVersionedRootSignature(
            &desc,
            D3D_ROOT_SIGNATURE_VERSION_1_2,
            &signature,
            &error
        );
        if (FAILED(hr)) {
            if (error) {
                FB_LOG_ERROR("{}", (char*)error->GetBufferPointer());
            }
            FB_ASSERT_HR(hr);
        }

        // Create the root signature.
        _root_signature = this->create_root_signature(signature, "Global Root Signature");
    }

    // Global samplers.
    _samplers.create(*this, _descriptors);

    // Global transfer.
    _transfer.create(_device);
}

auto GpuDevice::flush_transfers() -> void {
    _transfer.flush(*this);
}

auto GpuDevice::begin_frame() -> GpuCommandList {
    auto* cmd_alloc = _command_allocators[_frame_index].get();
    cmd_alloc->Reset();
    _command_list->Reset(cmd_alloc, nullptr);
    return GpuCommandList {
        _command_list.get(),
        _root_signature.get(),
        &_descriptors,
    };
}

auto GpuDevice::end_frame(GpuCommandList&&) -> void {
    // Close.
    _command_list->Close();

    // Execute command list.
    ID3D12CommandList* command_lists[] = {(ID3D12CommandList*)_command_list.get()};
    _command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

    // Present.
    _swapchain.present();

    // Schedule a Signal command in the queue.
    uint64_t current_fence_value = _fence_values[_frame_index];
    FB_ASSERT_HR(_command_queue->Signal(_fence.get(), _fence_values[_frame_index]));

    // Update the frame index.
    _frame_index = _swapchain.backbuffer_index();
    uint64_t* fence_value = &_fence_values[_frame_index];

    // If the next frame is not ready to be rendered yet, wait until it is ready.
    if (_fence->GetCompletedValue() < *fence_value) {
        FB_ASSERT_HR(_fence->SetEventOnCompletion(*fence_value, _fence_event));
        WaitForSingleObjectEx(_fence_event, INFINITE, FALSE);
    }

    // Set the fence value for the next frame.
    *fence_value = current_fence_value + 1;
}

auto GpuDevice::wait() -> void {
    // Schedule a Signal command in the queue.
    FB_ASSERT_HR(_command_queue->Signal(_fence.get(), _fence_values[_frame_index]));

    // Wait until the fence has been processed.
    FB_ASSERT_HR(_fence->SetEventOnCompletion(_fence_values[_frame_index], _fence_event));
    WaitForSingleObjectEx(_fence_event, INFINITE, FALSE);
}

auto GpuDevice::create_root_signature(const ComPtr<ID3DBlob>& signature, std::string_view name)
    const -> ComPtr<ID3D12RootSignature> {
    ComPtr<ID3D12RootSignature> result;
    FB_ASSERT_HR(_device->CreateRootSignature(
        0,
        signature->GetBufferPointer(),
        signature->GetBufferSize(),
        IID_PPV_ARGS(&result)
    ));
    dx_set_name(result, name);
    return result;
}

auto GpuDevice::create_descriptor_heap(
    const D3D12_DESCRIPTOR_HEAP_DESC& desc,
    std::string_view name
) const -> ComPtr<ID3D12DescriptorHeap> {
    ComPtr<ID3D12DescriptorHeap> result;
    FB_ASSERT_HR(_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&result)));
    dx_set_name(result, name);
    return result;
}

auto GpuDevice::create_pipeline_state(
    const D3D12_PIPELINE_STATE_STREAM_DESC& desc,
    std::string_view name
) -> ComPtr<ID3D12PipelineState> {
    ComPtr<ID3D12PipelineState> result;
    FB_ASSERT_HR(_device->CreatePipelineState(&desc, IID_PPV_ARGS(&result)));
    dx_set_name(result, name);
    return result;
}

auto GpuDevice::create_committed_resource(
    D3D12_HEAP_TYPE heap_type,
    const D3D12_RESOURCE_DESC1& desc,
    D3D12_BARRIER_LAYOUT initial_layout,
    const std::optional<D3D12_CLEAR_VALUE>& optimized_clear_value,
    std::string_view name
) const -> ComPtr<ID3D12Resource2> {
    ComPtr<ID3D12Resource2> result;

    const D3D12_CLEAR_VALUE* optimized_clear_value_ptr = nullptr;
    if (optimized_clear_value) {
        optimized_clear_value_ptr = &optimized_clear_value.value();
    }

    D3D12_HEAP_PROPERTIES heap_properties = {
        .Type = heap_type,
        .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
        .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
        .CreationNodeMask = 1,
        .VisibleNodeMask = 1,
    };

    FB_ASSERT_HR(_device->CreateCommittedResource3(
        &heap_properties,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        initial_layout,
        optimized_clear_value_ptr,
        nullptr,
        0,
        nullptr,
        IID_PPV_ARGS(&result)
    ));

    dx_set_name(result, name);
    return result;
}

auto GpuDevice::create_command_signature(
    uint constant_count,
    D3D12_INDIRECT_ARGUMENT_TYPE argument_type,
    std::string_view name
) const -> ComPtr<ID3D12CommandSignature> {
    FB_ASSERT(constant_count <= MAX_BINDING_COUNT);

    uint arguments_byte_count = 0;
    switch (argument_type) {
        case D3D12_INDIRECT_ARGUMENT_TYPE_DRAW:
            arguments_byte_count = sizeof(D3D12_DRAW_ARGUMENTS);
            break;
        case D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED:
            arguments_byte_count = sizeof(D3D12_DRAW_INDEXED_ARGUMENTS);
            break;
        case D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH:
        case D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH_RAYS:
        case D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH_MESH:
            arguments_byte_count = sizeof(D3D12_DISPATCH_ARGUMENTS);
            break;
        default: FB_ASSERT_MSG(false, "Invalid argument type."); break;
    }

    const auto argument_descs = std::to_array({
        D3D12_INDIRECT_ARGUMENT_DESC {
            .Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT,
            .Constant =
                {
                    .RootParameterIndex = 0,
                    .DestOffsetIn32BitValues = 0,
                    .Num32BitValuesToSet = constant_count,
                },
        },
        D3D12_INDIRECT_ARGUMENT_DESC {
            .Type = argument_type,
        },
    });

    ComPtr<ID3D12CommandSignature> result;
    D3D12_COMMAND_SIGNATURE_DESC desc {
        .ByteStride = sizeof(uint) * constant_count + arguments_byte_count,
        .NumArgumentDescs = (uint)argument_descs.size(),
        .pArgumentDescs = argument_descs.data(),
        .NodeMask = 0,
    };
    FB_ASSERT_HR(
        _device->CreateCommandSignature(&desc, _root_signature.get(), IID_PPV_ARGS(&result))
    );
    dx_set_name(result, name);
    return result;
}

auto GpuDevice::create_fence(uint64_t initial_value, std::string_view name) const
    -> ComPtr<ID3D12Fence1> {
    ComPtr<ID3D12Fence1> result;
    FB_ASSERT_HR(_device->CreateFence(initial_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&result)));
    dx_set_name(result, name);
    return result;
}

auto GpuDevice::create_render_target_view(
    const ComPtr<ID3D12Resource2>& resource,
    const std::optional<D3D12_RENDER_TARGET_VIEW_DESC>& desc,
    D3D12_CPU_DESCRIPTOR_HANDLE descriptor
) const -> void {
    const D3D12_RENDER_TARGET_VIEW_DESC* desc_ptr = nullptr;
    if (desc) {
        desc_ptr = &desc.value();
    }
    _device->CreateRenderTargetView(resource.get(), desc_ptr, descriptor);
}

auto GpuDevice::create_depth_stencil_view(
    const ComPtr<ID3D12Resource2>& resource,
    const std::optional<D3D12_DEPTH_STENCIL_VIEW_DESC>& desc,
    D3D12_CPU_DESCRIPTOR_HANDLE descriptor
) const -> void {
    const D3D12_DEPTH_STENCIL_VIEW_DESC* desc_ptr = nullptr;
    if (desc) {
        desc_ptr = &desc.value();
    }
    _device->CreateDepthStencilView(resource.get(), desc_ptr, descriptor);
}

auto GpuDevice::create_shader_resource_view(
    const ComPtr<ID3D12Resource2>& resource,
    const D3D12_SHADER_RESOURCE_VIEW_DESC& desc,
    D3D12_CPU_DESCRIPTOR_HANDLE descriptor
) const -> void {
    _device->CreateShaderResourceView(resource.get(), &desc, descriptor);
}

auto GpuDevice::create_unordered_access_view(
    const ComPtr<ID3D12Resource2>& resource,
    const std::optional<std::reference_wrapper<ComPtr<ID3D12Resource2>>> counter,
    const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc,
    D3D12_CPU_DESCRIPTOR_HANDLE descriptor
) -> void {
    _device->CreateUnorderedAccessView(
        resource.get(),
        counter ? counter->get().get() : nullptr,
        &desc,
        descriptor
    );
}

auto GpuDevice::create_sampler(
    const D3D12_SAMPLER_DESC2& desc,
    D3D12_CPU_DESCRIPTOR_HANDLE descriptor
) const -> void {
    _device->CreateSampler2(&desc, descriptor);
}

auto GpuDevice::create_constant_buffer_view(
    const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc,
    D3D12_CPU_DESCRIPTOR_HANDLE descriptor
) const -> void {
    _device->CreateConstantBufferView(&desc, descriptor);
}

auto GpuDevice::descriptor_size(D3D12_DESCRIPTOR_HEAP_TYPE heap_type) const -> uint {
    return _device->GetDescriptorHandleIncrementSize(heap_type);
}

auto GpuDevice::get_copyable_footprints(
    const D3D12_RESOURCE_DESC1& desc,
    uint subresource_offset,
    uint subresource_count,
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT* subresource_footprints,
    uint* subresource_row_counts,
    uint64_t* subresource_row_byte_counts,
    uint64_t* byte_count
) const -> void {
    _device->GetCopyableFootprints1(
        &desc,
        subresource_offset,
        subresource_count,
        0,
        subresource_footprints,
        subresource_row_counts,
        subresource_row_byte_counts,
        byte_count
    );
}

auto GpuDevice::log_stats() -> void {
    _descriptors.log_stats();
}

auto GpuDevice::video_memory_info() -> GpuVideoMemoryInfo {
    DXGI_QUERY_VIDEO_MEMORY_INFO local, non_local;
    FB_ASSERT_HR(_adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &local));
    FB_ASSERT_HR(_adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &non_local)
    );
    return GpuVideoMemoryInfo {
        .local =
            {
                .budget = local.Budget,
                .current_usage = local.CurrentUsage,
                .available_for_reservation = local.AvailableForReservation,
                .current_reservation = local.CurrentReservation,
            },
        .non_local =
            {
                .budget = non_local.Budget,
                .current_usage = non_local.CurrentUsage,
                .available_for_reservation = non_local.AvailableForReservation,
                .current_reservation = non_local.CurrentReservation,
            },
    };
}

auto GpuDevice::pix_capture() -> void {
    FB_ASSERT_HR(PIXGpuCaptureNextFrames(FB_PIX_GPU_CAPTURE_FILE_NAME, 1));
    ShellExecuteW(nullptr, L"open", FB_PIX_GPU_CAPTURE_FILE_NAME, nullptr, nullptr, SW_SHOW);
}

} // namespace fb
