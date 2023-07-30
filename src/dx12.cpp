#include "dx12.hpp"
#include "utils.hpp"
#include "maths.hpp"
#include <wil/com.h>
#include <wil/result_macros.h>
#include <dxgidebug.h>

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

Dx::Dx(Window* window) {
    // Debug layer.
    UINT factory_flags = 0;
    {
#if defined(_DEBUG)
        fb::ComPtr<ID3D12Debug6> debug;
        FAIL_FAST_IF_FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)));
        debug->EnableDebugLayer();
        factory_flags = DXGI_CREATE_FACTORY_DEBUG;
#endif
    }

    // Factory.
    fb::ComPtr<IDXGIFactory7> factory;
    FAIL_FAST_IF_FAILED(CreateDXGIFactory2(factory_flags, IID_PPV_ARGS(&factory)));

    // Adapter.
    fb::ComPtr<IDXGIAdapter4> adapter;
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
                log_info("Using adapter {}: {}", adapter_index, fb::from_wstr(desc.Description));
                adapter = temp_adapter;
                break;
            }
            adapter_index += 1;
        }
    }

    // Output.
    {
        UINT i = 0;
        fb::ComPtr<IDXGIOutput> output;
        while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND) {
            fb::ComPtr<IDXGIOutput6> output6;
            output.query_to(&output6);

            DXGI_OUTPUT_DESC1 desc;
            output6->GetDesc1(&desc);

            fb::Rect desktop_rect(desc.DesktopCoordinates);
            log_info(
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
    FAIL_FAST_IF_FAILED(D3D12CreateDevice(adapter.get(), MIN_FEATURE_LEVEL, IID_PPV_ARGS(&device)));
    dx_set_name(device, "Device");

    // Debug device.
    device->QueryInterface(IID_PPV_ARGS(&leak_tracker.debug_device));

    // Root signature support.
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE feature_data = {
            .HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_2};
        FAIL_FAST_IF_FAILED(device->CheckFeatureSupport(
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
        FAIL_FAST_IF_FAILED(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&command_queue)));
        dx_set_name(command_queue, "Command Queue");
    }

    // Command allocators.
    for (uint32_t i = 0; i < fb::FRAME_COUNT; i++) {
        FAIL_FAST_IF_FAILED(device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(&command_allocators[i])));
        dx_set_indexed_name(command_allocators[i], "Command Allocator", i);
    }

    // Command list.
    FAIL_FAST_IF_FAILED(device->CreateCommandList1(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        D3D12_COMMAND_LIST_FLAG_NONE,
        IID_PPV_ARGS(&command_list)));
    dx_set_name(command_list, "Command List");

    // Swapchain.
    {
        fb::ComPtr<IDXGISwapChain1> swapchain1;
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
        FAIL_FAST_IF_FAILED(factory->CreateSwapChainForHwnd(
            command_queue.get(),
            (HWND)window_handle(window),
            &desc,
            nullptr,
            nullptr,
            &swapchain1));
        swapchain1.query_to(&swapchain);
    }

    // Swapchain size.
    {
        DXGI_SWAP_CHAIN_DESC1 desc;
        swapchain->GetDesc1(&desc);
        swapchain_width = desc.Width;
        swapchain_height = desc.Height;
    }

    // Render target views.
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            .NumDescriptors = FRAME_COUNT,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
            .NodeMask = 0,
        };
        FAIL_FAST_IF_FAILED(
            device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtv_descriptor_heap)));
        dx_set_name(rtv_descriptor_heap, "Render Target View Descriptor Heap");

        uint32_t rtv_descriptor_size =
            device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtv_descriptor_handle =
            rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        for (uint32_t i = 0; i < fb::FRAME_COUNT; i++) {
            FAIL_FAST_IF_FAILED(swapchain->GetBuffer(i, IID_PPV_ARGS(&rtvs[i])));
            dx_set_indexed_name(rtvs[i], "Render Target View", i);

            device->CreateRenderTargetView(rtvs[i].get(), nullptr, rtv_descriptor_handle);
            rtv_descriptors[i] = rtv_descriptor_handle;
            rtv_descriptor_handle.ptr += rtv_descriptor_size;
        }
    }

    // Synchronization.
    {
        frame_index = swapchain->GetCurrentBackBufferIndex();
        FAIL_FAST_IF_FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
        dx_set_name(fence, "Fence");
        fence_event = wil::unique_handle(CreateEvent(nullptr, FALSE, FALSE, nullptr));
        FAIL_FAST_IF_NULL_MSG(fence_event, "Failed to create fence event.");
    }
}

void Dx::transition(
    const ComPtr<ID3D12Resource>& resource,
    D3D12_RESOURCE_STATES before,
    D3D12_RESOURCE_STATES after) {
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.get(), before, after);
    command_list->ResourceBarrier(1, &barrier);
}

void dx_set_name(ID3D12Object* object, std::string_view name) {
#if defined(_DEBUG)
    std::wstring wname = fb::to_wstr(name);
    object->SetName(wname.c_str());
#else
    (void)object;
    (void)name;
#endif
}

void dx_set_name(const ComPtr<ID3D12Object>& object, std::string_view name) {
    dx_set_name(object.get(), name);
}

void dx_set_indexed_name(ID3D12Object* object, std::string_view name, uint32_t index) {
#if defined(_DEBUG)
    std::wstring wname = fb::to_wstr(name);
    std::wstring windexed_name = std::format(L"{} {}", wname, index);
    object->SetName(windexed_name.c_str());
#else
    (void)object;
    (void)name;
    (void)index;
#endif
}

void dx_set_indexed_name(
    const ComPtr<ID3D12Object>& object,
    std::string_view name,
    uint32_t index) {
    dx_set_indexed_name(object.get(), name, index);
}

}  // namespace fb
