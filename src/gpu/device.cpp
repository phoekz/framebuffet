#include "device.hpp"
#include "utils.hpp"
#include "maths.hpp"

// Setup DirectX Agility SDK.
extern "C" {
__declspec(dllexport) extern const UINT D3D12SDKVersion = 610;
__declspec(dllexport) extern const char* D3D12SDKPath = ".\\";
}

// Constants.
constexpr D3D_FEATURE_LEVEL MIN_FEATURE_LEVEL = D3D_FEATURE_LEVEL_12_2;

namespace fb {

DxLeakTracker::~DxLeakTracker() {
#if defined(_DEBUG)
    if (debug_device) {
        debug_device->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
    }
#endif
}

GpuDevice::GpuDevice(const Window& window) {
    // Debug layer.
    UINT factory_flags = 0;
    {
#if defined(_DEBUG)
        ComPtr<ID3D12Debug6> debug;
        FB_ASSERT_HR(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)));
        debug->EnableDebugLayer();
        factory_flags = DXGI_CREATE_FACTORY_DEBUG;
#endif
    }

    // Factory.
    ComPtr<IDXGIFactory7> factory;
    FB_ASSERT_HR(CreateDXGIFactory2(factory_flags, IID_PPV_ARGS(&factory)));

    // Adapter.
    ComPtr<IDXGIAdapter4> adapter;
    {
        IDXGIAdapter4* temp_adapter = nullptr;
        uint32_t adapter_index = 0;
        while (factory->EnumAdapterByGpuPreference(
                   adapter_index,
                   DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                   IID_PPV_ARGS(&temp_adapter))
               != DXGI_ERROR_NOT_FOUND) {
            if (SUCCEEDED(D3D12CreateDevice(
                    temp_adapter,
                    MIN_FEATURE_LEVEL,
                    __uuidof(ID3D12Device),
                    nullptr))) {
                DXGI_ADAPTER_DESC3 desc;
                temp_adapter->GetDesc3(&desc);
                FB_LOG_INFO("Using adapter {}: {}", adapter_index, fb::from_wstr(desc.Description));
                adapter = temp_adapter;
                break;
            }
            adapter_index += 1;
        }
    }

    // Output.
    {
        UINT i = 0;
        ComPtr<IDXGIOutput> output;
        while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND) {
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
                desc.MaxLuminance);

            ++i;
        }
    }

    // Device.
    FB_ASSERT_HR(D3D12CreateDevice(adapter.get(), MIN_FEATURE_LEVEL, IID_PPV_ARGS(&_device)));
    dx_set_name(_device, "Device");

    // Debug device.
    _device->QueryInterface(IID_PPV_ARGS(&_leak_tracker.debug_device));

    // Root signature support.
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE feature_data = {
            .HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_2};
        FB_ASSERT_HR(_device->CheckFeatureSupport(
            D3D12_FEATURE_ROOT_SIGNATURE,
            &feature_data,
            sizeof(feature_data)));
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
    for (uint32_t i = 0; i < FRAME_COUNT; i++) {
        FB_ASSERT_HR(_device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(&_command_allocators[i])));
        dx_set_name(_command_allocators[i], dx_name("Command Allocator", i));
    }

    // Command list.
    FB_ASSERT_HR(_device->CreateCommandList1(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        D3D12_COMMAND_LIST_FLAG_NONE,
        IID_PPV_ARGS(&_command_list)));
    dx_set_name(_command_list, "Command List");

    // Swapchain.
    {
        ComPtr<IDXGISwapChain1> swapchain1;
        DXGI_SWAP_CHAIN_DESC1 desc = {
            .Width = 0,
            .Height = 0,
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .Stereo = FALSE,
            .SampleDesc =
                {
                    .Count = 1,
                    .Quality = 0,
                },
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = FRAME_COUNT,
            .Scaling = DXGI_SCALING_STRETCH,
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
            .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
            .Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT,
        };
        FB_ASSERT_HR(factory->CreateSwapChainForHwnd(
            _command_queue.get(),
            window.hwnd(),
            &desc,
            nullptr,
            nullptr,
            &swapchain1));
        swapchain1.query_to(&_swapchain);
    }

    // Swapchain size.
    {
        DXGI_SWAP_CHAIN_DESC1 desc;
        _swapchain->GetDesc1(&desc);
        _swapchain_size = {desc.Width, desc.Height};
    }

    // Render target views.
    {
        _rtv_descriptor_heap = this->create_descriptor_heap(
            D3D12_DESCRIPTOR_HEAP_DESC {
                .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
                .NumDescriptors = FRAME_COUNT},
            dx_name("Render Target", "Descriptor Heap"));

        for (uint32_t i = 0; i < FRAME_COUNT; i++) {
            FB_ASSERT_HR(_swapchain->GetBuffer(i, IID_PPV_ARGS(&_rtvs[i])));
            dx_set_name(_rtvs[i], dx_name("Render Target", i));

            auto rtv_descriptor_handle = _rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
            rtv_descriptor_handle.ptr += i * this->descriptor_size(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            _device->CreateRenderTargetView(_rtvs[i].get(), nullptr, rtv_descriptor_handle);
            _rtv_descriptors[i] = rtv_descriptor_handle;
        }
    }

    // Synchronization.
    {
        _frame_index = _swapchain->GetCurrentBackBufferIndex();
        _fence = this->create_fence(0, "Fence");
        _fence_event = wil::unique_handle(CreateEvent(nullptr, FALSE, FALSE, nullptr));
        FB_ASSERT_MSG(_fence_event != nullptr, "Failed to create fence event.");
    }
}

auto GpuDevice::create_root_signature(const ComPtr<ID3DBlob>& signature, std::string_view name)
    const -> ComPtr<ID3D12RootSignature> {
    ComPtr<ID3D12RootSignature> result;
    FB_ASSERT_HR(_device->CreateRootSignature(
        0,
        signature->GetBufferPointer(),
        signature->GetBufferSize(),
        IID_PPV_ARGS(&result)));
    dx_set_name(result, name);
    return result;
}

auto GpuDevice::create_descriptor_heap(
    const D3D12_DESCRIPTOR_HEAP_DESC& desc,
    std::string_view name) const -> ComPtr<ID3D12DescriptorHeap> {
    ComPtr<ID3D12DescriptorHeap> result;
    FB_ASSERT_HR(_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&result)));
    dx_set_name(result, name);
    return result;
}

auto GpuDevice::create_graphics_pipeline_state(
    const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc,
    std::string_view name) const -> ComPtr<ID3D12PipelineState> {
    ComPtr<ID3D12PipelineState> result;
    FB_ASSERT_HR(_device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&result)));
    dx_set_name(result, name);
    return result;
}

auto GpuDevice::create_compute_pipeline_state(
    const D3D12_COMPUTE_PIPELINE_STATE_DESC& desc,
    std::string_view name) -> ComPtr<ID3D12PipelineState> {
    ComPtr<ID3D12PipelineState> result;
    FB_ASSERT_HR(_device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&result)));
    dx_set_name(result, name);
    return result;
}

auto GpuDevice::create_committed_resource(
    const CD3DX12_HEAP_PROPERTIES& heap_props,
    const D3D12_RESOURCE_DESC& desc,
    D3D12_RESOURCE_STATES init_state,
    const std::optional<D3D12_CLEAR_VALUE>& clear_value,
    std::string_view name) const -> ComPtr<ID3D12Resource> {
    ComPtr<ID3D12Resource> result;
    const D3D12_CLEAR_VALUE* clear_value_ptr = nullptr;
    if (clear_value) {
        clear_value_ptr = &clear_value.value();
    }
    FB_ASSERT_HR(_device->CreateCommittedResource(
        &heap_props,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        init_state,
        clear_value_ptr,
        IID_PPV_ARGS(&result)));
    dx_set_name(result, name);
    return result;
}

auto GpuDevice::create_fence(uint64_t init_value, std::string_view name) const
    -> ComPtr<ID3D12Fence1> {
    ComPtr<ID3D12Fence1> result;
    FB_ASSERT_HR(_device->CreateFence(init_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&result)));
    dx_set_name(result, name);
    return result;
}

auto GpuDevice::create_render_target_view(
    const ComPtr<ID3D12Resource>& resource,
    const std::optional<D3D12_RENDER_TARGET_VIEW_DESC>& desc,
    D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const -> void {
    const D3D12_RENDER_TARGET_VIEW_DESC* desc_ptr = nullptr;
    if (desc) {
        desc_ptr = &desc.value();
    }
    _device->CreateRenderTargetView(resource.get(), desc_ptr, descriptor);
}

auto GpuDevice::create_depth_stencil_view(
    const ComPtr<ID3D12Resource>& resource,
    const std::optional<D3D12_DEPTH_STENCIL_VIEW_DESC>& desc,
    D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const -> void {
    const D3D12_DEPTH_STENCIL_VIEW_DESC* desc_ptr = nullptr;
    if (desc) {
        desc_ptr = &desc.value();
    }
    _device->CreateDepthStencilView(resource.get(), desc_ptr, descriptor);
}

auto GpuDevice::create_shader_resource_view(
    const ComPtr<ID3D12Resource>& resource,
    const D3D12_SHADER_RESOURCE_VIEW_DESC& desc,
    D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const -> void {
    _device->CreateShaderResourceView(resource.get(), &desc, descriptor);
}

auto GpuDevice::create_unordered_access_view(
    const ComPtr<ID3D12Resource>& resource,
    const std::optional<std::reference_wrapper<ComPtr<ID3D12Resource>>> counter,
    const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc,
    D3D12_CPU_DESCRIPTOR_HANDLE descriptor) -> void {
    _device->CreateUnorderedAccessView(
        resource.get(),
        counter ? counter->get().get() : nullptr,
        &desc,
        descriptor);
}

auto GpuDevice::create_sampler(
    const D3D12_SAMPLER_DESC& desc,
    D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const -> void {
    _device->CreateSampler(&desc, descriptor);
}

auto GpuDevice::create_constant_buffer_view(
    const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc,
    D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const -> void {
    _device->CreateConstantBufferView(&desc, descriptor);
}

auto GpuDevice::descriptor_size(D3D12_DESCRIPTOR_HEAP_TYPE heap_type) const -> uint32_t {
    return _device->GetDescriptorHandleIncrementSize(heap_type);
}

auto GpuDevice::begin_frame() -> void {
    // Reset.
    auto* cmd_alloc = _command_allocators[_frame_index].get();
    cmd_alloc->Reset();
    _command_list->Reset(cmd_alloc, nullptr);
}

auto GpuDevice::begin_main_pass() -> void {
    static constexpr float CLEAR_COLOR[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    transition(
        _rtvs[_frame_index],
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
    _command_list->ClearRenderTargetView(_rtv_descriptors[_frame_index], CLEAR_COLOR, 0, nullptr);
    _command_list->OMSetRenderTargets(1, &_rtv_descriptors[_frame_index], FALSE, nullptr);
}

auto GpuDevice::end_main_pass() -> void {
    transition(
        _rtvs[_frame_index],
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);
}

auto GpuDevice::end_frame() -> void {
    // Close.
    _command_list->Close();

    // Execute command list.
    ID3D12CommandList* command_lists[] = {(ID3D12CommandList*)_command_list.get()};
    _command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

    // Present.
    _swapchain->Present(1, 0);

    // Schedule a Signal command in the queue.
    uint64_t current_fence_value = _fence_values[_frame_index];
    FB_ASSERT_HR(_command_queue->Signal(_fence.get(), _fence_values[_frame_index]));

    // Update the frame index.
    _frame_index = _swapchain->GetCurrentBackBufferIndex();
    uint64_t* fence_value = &_fence_values[_frame_index];

    // If the next frame is not ready to be rendered yet, wait until it is ready.
    if (_fence->GetCompletedValue() < *fence_value) {
        FB_ASSERT_HR(_fence->SetEventOnCompletion(*fence_value, _fence_event.get()));
        WaitForSingleObjectEx(_fence_event.get(), INFINITE, FALSE);
    }

    // Set the fence value for the next frame.
    *fence_value = current_fence_value + 1;
}

auto GpuDevice::wait() -> void {
    // Schedule a Signal command in the queue.
    FB_ASSERT_HR(_command_queue->Signal(_fence.get(), _fence_values[_frame_index]));

    // Wait until the fence has been processed.
    FB_ASSERT_HR(_fence->SetEventOnCompletion(_fence_values[_frame_index], _fence_event.get()));
    WaitForSingleObjectEx(_fence_event.get(), INFINITE, FALSE);
}

auto GpuDevice::transition(
    const ComPtr<ID3D12Resource>& resource,
    D3D12_RESOURCE_STATES before,
    D3D12_RESOURCE_STATES after) const -> void {
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.get(), before, after);
    _command_list->ResourceBarrier(1, &barrier);
}

auto GpuDevice::easy_upload(
    const D3D12_SUBRESOURCE_DATA& data,
    const ComPtr<ID3D12Resource>& resource,
    D3D12_RESOURCE_STATES before_state,
    D3D12_RESOURCE_STATES after_state) const -> void {
    DirectX::ResourceUploadBatch rub((ID3D12Device*)_device.get());
    rub.Begin();
    rub.Transition(resource.get(), before_state, D3D12_RESOURCE_STATE_COPY_DEST);
    rub.Upload(resource.get(), 0, &data, 1);
    rub.Transition(resource.get(), D3D12_RESOURCE_STATE_COPY_DEST, after_state);
    auto finish = rub.End(_command_queue.get());
    finish.wait();
}

auto dx_set_name(ID3D12Object* object, std::string_view name) -> void {
#if defined(_DEBUG)
    std::wstring wname = fb::to_wstr(name);
    object->SetName(wname.c_str());
#else
    (void)object;
    (void)name;
#endif
}

auto dx_set_name(const ComPtr<ID3D12Object>& object, std::string_view name) -> void {
    dx_set_name(object.get(), name);
}

}  // namespace fb