#include "device.hpp"
#include "pipelines.hpp"
#include "utils.hpp"
#include "maths.hpp"

namespace fb {

#pragma region Forward declarations

static auto dx_set_name(ID3D12Object* object, std::string_view name) -> void;
static auto dx_set_name(const ComPtr<ID3D12Object>& object, std::string_view name) -> void;

#pragma endregion

#pragma region Descriptors

static auto is_shader_visible(D3D12_DESCRIPTOR_HEAP_TYPE type) -> bool {
    return type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
        || type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
}

static auto heap_type_name(D3D12_DESCRIPTOR_HEAP_TYPE type) -> std::string_view {
    switch (type) {
        case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: return "CbvSrvUav"sv;
        case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER: return "Sampler"sv;
        case D3D12_DESCRIPTOR_HEAP_TYPE_RTV: return "Rtv"sv;
        case D3D12_DESCRIPTOR_HEAP_TYPE_DSV: return "Dsv"sv;
        default: return "Unknown"sv;
    }
}

GpuDescriptorHeap::GpuDescriptorHeap(
    GpuDevice& device,
    std::string_view name,
    D3D12_DESCRIPTOR_HEAP_TYPE type,
    uint32_t capacity) :
    _type(type),
    _capacity(capacity) {
    D3D12_DESCRIPTOR_HEAP_DESC desc = {
        .Type = type,
        .NumDescriptors = _capacity,
    };
    if (is_shader_visible(type))
        desc.Flags |= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    _heap = device.create_descriptor_heap(
        desc,
        dx_name(name, "Descriptor Heap", heap_type_name(_type)));
    _descriptor_size = device.descriptor_size(type);
    _cpu_heap_start = _heap->GetCPUDescriptorHandleForHeapStart();
    if (is_shader_visible(type))
        _gpu_heap_start = _heap->GetGPUDescriptorHandleForHeapStart();
}

auto GpuDescriptorHeap::alloc() -> GpuDescriptor {
    FB_ASSERT(_count < _capacity);
    GpuDescriptor handle;
    handle._type = _type;
    handle._index = _count++;
    handle._cpu_handle = _cpu_heap_start;
    handle._cpu_handle.ptr += handle._index * _descriptor_size;
    if (is_shader_visible(_type)) {
        handle._gpu_handle = _gpu_heap_start;
        handle._gpu_handle.ptr += handle._index * _descriptor_size;
    }
    return handle;
}

GpuDescriptors::GpuDescriptors(GpuDevice& device, std::string_view name) :
    _cbv_srv_uav_heap(
        device,
        name,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        CBV_SRV_UAV_DESCRIPTOR_CAPACITY),
    _sampler_heap(device, name, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, SAMPLER_DESCRIPTOR_CAPACITY),
    _rtv_heap(device, name, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, RTV_DESCRIPTOR_CAPACITY),
    _dsv_heap(device, name, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, DSV_DESCRIPTOR_CAPACITY) {}

auto GpuDescriptors::log_stats() -> void {
    FB_LOG_INFO(
        "Descriptor heaps: "
        "CbvSrvUav: {}/{} "
        "Sampler: {}/{} "
        "Rtv: {}/{} "
        "Dsv: {}/{}",
        _cbv_srv_uav_heap.count(),
        _cbv_srv_uav_heap.capacity(),
        _sampler_heap.count(),
        _sampler_heap.capacity(),
        _rtv_heap.count(),
        _rtv_heap.capacity(),
        _dsv_heap.count(),
        _dsv_heap.capacity());
}

class GpuBindings {
  public:
    GpuBindings() { _bindings.fill(UINT32_MAX); }

    auto push(uint32_t binding) -> void {
        assert(_count < BINDINGS_CAPACITY);
        _bindings[_count++] = binding;
    }
    auto push(const GpuDescriptor& handle) -> void { push(handle.index()); }
    auto count() const -> uint32_t { return _count; }
    auto capacity() -> uint32_t { return BINDINGS_CAPACITY; }
    auto ptr() const -> const uint32_t* { return _bindings.data(); }

  private:
    std::array<uint32_t, BINDINGS_CAPACITY> _bindings;
    uint32_t _count = 0;
};

#pragma endregion

#pragma region Samplers

GpuSamplers::GpuSamplers(GpuDevice& device, GpuDescriptors& descriptors) {
    std::tuple<GpuSamplerType, D3D12_SAMPLER_DESC, GpuDescriptor> samplers[] = {
        {
            GpuSamplerType::LinearClamp,
            {
                .Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                .AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                .AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                .AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                .MipLODBias = 0.0f,
                .MaxAnisotropy = 0,
                .ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
                .BorderColor = {0.0f, 0.0f, 0.0f, 0.0f},
                .MinLOD = 0.0f,
                .MaxLOD = D3D12_FLOAT32_MAX,
            },
            descriptors.sampler().alloc(),
        },
        {
            GpuSamplerType::LinearWrap,
            {
                .Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                .AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                .AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                .AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                .MipLODBias = 0.0f,
                .MaxAnisotropy = 0,
                .ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
                .BorderColor = {0.0f, 0.0f, 0.0f, 0.0f},
                .MinLOD = 0.0f,
                .MaxLOD = D3D12_FLOAT32_MAX,
            },
            descriptors.sampler().alloc(),
        },
        {
            GpuSamplerType::Shadow,
            {
                .Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT,
                .AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                .AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                .AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                .MipLODBias = 0.0f,
                .MaxAnisotropy = 0,
                .ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL,
                .BorderColor = {1.0f, 1.0f, 1.0f, 1.0f},
                .MinLOD = 0.0f,
                .MaxLOD = D3D12_FLOAT32_MAX,
            },
            descriptors.sampler().alloc(),
        },
    };

    for (const auto& [type, desc, handle] : samplers) {
        device.create_sampler(desc, handle.cpu());
        _handles[(size_t)type] = handle;
    }
}

#pragma endregion

#pragma region Commands

auto GpuCommandList::begin_pix(std::string_view name) const -> void {
    PIXBeginEvent(_cmd, PIX_COLOR_DEFAULT, name.data());
}

auto GpuCommandList::end_pix() const -> void {
    PIXEndEvent(_cmd);
}

auto GpuCommandList::set_global_descriptor_heap() -> void {
    const auto heaps = std::to_array<ID3D12DescriptorHeap*>({
        _descriptors->cbv_srv_uav().heap(),
        _descriptors->sampler().heap(),
    });
    _cmd->SetDescriptorHeaps((uint32_t)heaps.size(), heaps.data());
}

auto GpuCommandList::set_graphics() -> void {
    if (_engine == GpuCommandEngine::Graphics)
        return;
    _engine = GpuCommandEngine::Graphics;
    set_global_descriptor_heap();
    _cmd->SetGraphicsRootSignature(_root_signature);
}

auto GpuCommandList::set_compute() -> void {
    if (_engine == GpuCommandEngine::Compute)
        return;
    _engine = GpuCommandEngine::Compute;
    set_global_descriptor_heap();
    _cmd->SetComputeRootSignature(_root_signature);
}

auto GpuCommandList::set_viewport(
    uint32_t left,
    uint32_t top,
    uint32_t right,
    uint32_t bottom,
    float min_depth,
    float max_depth) const -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    D3D12_VIEWPORT viewport {
        .TopLeftX = (FLOAT)left,
        .TopLeftY = (FLOAT)top,
        .Width = (FLOAT)(right - left),
        .Height = (FLOAT)(bottom - top),
        .MinDepth = min_depth,
        .MaxDepth = max_depth,
    };
    _cmd->RSSetViewports(1, &viewport);
}

auto GpuCommandList::set_scissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom) const
    -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    D3D12_RECT scissor {
        .left = (LONG)left,
        .top = (LONG)top,
        .right = (LONG)right,
        .bottom = (LONG)bottom,
    };
    _cmd->RSSetScissorRects(1, &scissor);
}

auto GpuCommandList::set_rtv_dsv(
    const std::optional<GpuDescriptor>& rtv,
    const std::optional<GpuDescriptor>& dsv) const -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    _cmd->OMSetRenderTargets(
        rtv.has_value() ? 1 : 0,
        rtv.has_value() ? rtv.value().cpu_ptr() : nullptr,
        false,
        dsv.has_value() ? dsv.value().cpu_ptr() : nullptr);
}

auto GpuCommandList::set_topology(D3D12_PRIMITIVE_TOPOLOGY topology) const -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    _cmd->IASetPrimitiveTopology(topology);
}

auto GpuCommandList::set_index_buffer(D3D12_INDEX_BUFFER_VIEW ibv) const -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    _cmd->IASetIndexBuffer(&ibv);
}

auto GpuCommandList::set_blend_factor(Float4 factor) const -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    _cmd->OMSetBlendFactor((const float*)&factor);
}

auto GpuCommandList::set_pipeline(const GpuPipeline& pipeline) const -> void {
    _cmd->SetPipelineState(pipeline.get());
}

auto GpuCommandList::set_graphics_constants(std::initializer_list<uint32_t> constants) const
    -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    GpuBindings bindings;
    for (auto constant : constants)
        bindings.push(constant);
    _cmd->SetGraphicsRoot32BitConstants(0, bindings.capacity(), bindings.ptr(), 0);
}

auto GpuCommandList::set_compute_constants(std::initializer_list<uint32_t> constants) const
    -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Compute);
    GpuBindings bindings;
    for (auto constant : constants)
        bindings.push(constant);
    _cmd->SetComputeRoot32BitConstants(0, bindings.capacity(), bindings.ptr(), 0);
}

auto GpuCommandList::clear_rtv(const GpuDescriptor& rtv, Float4 color) const -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    _cmd->ClearRenderTargetView(rtv.cpu(), (const float*)&color, 0, nullptr);
}

auto GpuCommandList::clear_dsv(const GpuDescriptor& dsv, float depth) const -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    _cmd->ClearDepthStencilView(dsv.cpu(), D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
}

auto GpuCommandList::draw_instanced(
    uint32_t vertex_count,
    uint32_t instance_count,
    uint32_t start_vertex,
    uint32_t start_instance) const -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    _cmd->DrawInstanced(vertex_count, instance_count, start_vertex, start_instance);
}

auto GpuCommandList::draw_indexed_instanced(
    uint32_t index_count,
    uint32_t instance_count,
    uint32_t start_index,
    int32_t base_vertex,
    uint32_t start_instance) const -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Graphics);
    _cmd->DrawIndexedInstanced(
        index_count,
        instance_count,
        start_index,
        base_vertex,
        start_instance);
}

auto GpuCommandList::dispatch(uint32_t x, uint32_t y, uint32_t z) const -> void {
    FB_ASSERT(_engine == GpuCommandEngine::Compute);
    _cmd->Dispatch(x, y, z);
}

auto GpuCommandList::transition_barrier(
    const ComPtr<ID3D12Resource>& resource,
    D3D12_RESOURCE_STATES before,
    D3D12_RESOURCE_STATES after) const -> void {
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.get(), before, after);
    _cmd->ResourceBarrier(1, &barrier);
}

#pragma endregion

#pragma region Device

#pragma region Device - State

GpuDevice::GpuDevice(const Window& window) {
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
        info_queue->SetBreakOnSeverity(
            DXGI_DEBUG_ALL,
            DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR,
            true);
        info_queue->SetBreakOnSeverity(
            DXGI_DEBUG_ALL,
            DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION,
            true);

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
                switch (severity) {
                    case D3D12_MESSAGE_SEVERITY_CORRUPTION: FB_LOG_ERROR("[{}", description); break;
                    case D3D12_MESSAGE_SEVERITY_ERROR: FB_LOG_ERROR("{}", description); break;
                    case D3D12_MESSAGE_SEVERITY_WARNING: FB_LOG_WARN("{}", description); break;
                    case D3D12_MESSAGE_SEVERITY_INFO: FB_LOG_INFO("{}", description); break;
                    default: break;
                }
            },
            D3D12_MESSAGE_CALLBACK_FLAG_NONE,
            nullptr,
            &callback_cookie);
#endif
    }

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

    // Global root signature.
    {
        // Allocate constants for binding slots.
        CD3DX12_ROOT_PARAMETER1 root_parameter = {};
        root_parameter.InitAsConstants(BINDINGS_CAPACITY, 0, 0);

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
            &error);
        if (FAILED(hr)) {
            if (error) {
                FB_LOG_ERROR("{}", (char*)error->GetBufferPointer());
            }
            FB_ASSERT_HR(hr);
        }

        // Create the root signature.
        _root_signature = this->create_root_signature(signature, "Global Root Signature");
    }

    // Global descriptor heap.
    _descriptors = std::make_unique<GpuDescriptors>(*this, "Global Descriptor Heap");

    // Global samplers.
    _samplers = std::make_unique<GpuSamplers>(*this, *_descriptors);
}

auto GpuDevice::begin_frame() -> GpuCommandList {
    auto* cmd_alloc = _command_allocators[_frame_index].get();
    cmd_alloc->Reset();
    _command_list->Reset(cmd_alloc, nullptr);
    return GpuCommandList {
        _command_list.get(),
        GpuCommandEngine::Unknown,
        _root_signature.get(),
        _descriptors.get(),
    };
}

auto GpuDevice::begin_main_pass() -> void {
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        _rtvs[_frame_index].get(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
    _command_list->ResourceBarrier(1, &barrier);

    static constexpr float CLEAR_COLOR[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    _command_list->ClearRenderTargetView(_rtv_descriptors[_frame_index], CLEAR_COLOR, 0, nullptr);
    _command_list->OMSetRenderTargets(1, &_rtv_descriptors[_frame_index], FALSE, nullptr);
}

auto GpuDevice::end_main_pass() -> void {
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        _rtvs[_frame_index].get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);
    _command_list->ResourceBarrier(1, &barrier);
}

auto GpuDevice::end_frame(GpuCommandList&&) -> void {
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

#pragma endregion  // Device - State

#pragma region Device - Wrappers

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

auto GpuDevice::create_pipeline_state(
    const D3D12_PIPELINE_STATE_STREAM_DESC& desc,
    std::string_view name) -> ComPtr<ID3D12PipelineState> {
    ComPtr<ID3D12PipelineState> result;
    FB_ASSERT_HR(_device->CreatePipelineState(&desc, IID_PPV_ARGS(&result)));
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

#pragma endregion  // Device - Wrappers

#pragma region Device - Resources

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

auto GpuDevice::easy_multi_upload(
    std::span<const D3D12_SUBRESOURCE_DATA> datas,
    const ComPtr<ID3D12Resource>& resource,
    D3D12_RESOURCE_STATES before_state,
    D3D12_RESOURCE_STATES after_state) const -> void {
    DirectX::ResourceUploadBatch rub((ID3D12Device*)_device.get());
    rub.Begin();
    rub.Transition(resource.get(), before_state, D3D12_RESOURCE_STATE_COPY_DEST);
    for (uint32_t i = 0; i < (uint32_t)datas.size(); i++) {
        const auto& data = datas[i];
        rub.Upload(resource.get(), i, &data, 1);
    }
    rub.Transition(resource.get(), D3D12_RESOURCE_STATE_COPY_DEST, after_state);
    auto finish = rub.End(_command_queue.get());
    finish.wait();
}

auto GpuDevice::log_stats() -> void {
    _descriptors->log_stats();
}

#pragma endregion  // Device - Resources

#pragma endregion  // Device

#pragma region Debugging

GpuDevice::LeakTracker::~LeakTracker() {
#if defined(_DEBUG)
    if (debug_device) {
        debug_device->ReportLiveDeviceObjects(
            D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
    }
#endif
}

static auto dx_set_name(ID3D12Object* object, std::string_view name) -> void {
#if defined(_DEBUG)
    std::wstring wname = fb::to_wstr(name);
    object->SetName(wname.c_str());
#else
    (void)object;
    (void)name;
#endif
}

static auto dx_set_name(const ComPtr<ID3D12Object>& object, std::string_view name) -> void {
    dx_set_name(object.get(), name);
}

#pragma endregion

}  // namespace fb

// Setup DirectX Agility SDK.
extern "C" {
__declspec(dllexport) extern const UINT D3D12SDKVersion = 610;
__declspec(dllexport) extern const char* D3D12SDKPath = ".\\";
}
