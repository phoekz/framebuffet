// Windows Implementation Library (WIL).
#define UNICODE
#define WIL_SUPPRESS_EXCEPTIONS
#include <wil/com.h>
#include <wil/resource.h>
template<typename T>
using ComPtr = wil::com_ptr_nothrow<T>;

// DirectX.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
#include <d3dx12/d3dx12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#pragma clang diagnostic pop
extern "C" {
__declspec(dllexport) extern const uint32_t D3D12SDKVersion = 610;
__declspec(dllexport) extern const char* D3D12SDKPath = ".\\";
}

// D3D12 memory allocator.
#include <D3D12MemAlloc.h>

// Standard libraries.
#include <array>
#include <cstdio>

// Constants.
constexpr const char* WINDOW_TITLE = "framebuffet";
constexpr const wchar_t* WINDOW_LTITLE = L"framebuffet";
constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 480;
constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
constexpr D3D_FEATURE_LEVEL MIN_FEATURE_LEVEL = D3D_FEATURE_LEVEL_12_2;

// Win32.
static LRESULT CALLBACK
win32_window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
    switch (message) {
        case WM_KEYDOWN:
            if (w_param == VK_ESCAPE) {
                PostQuitMessage(0);
                return 0;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(window, message, w_param, l_param);
}

// Utilities.
static void d3d12_call(HRESULT hr, const char* msg) {
    if (FAILED(hr)) {
        wchar_t wsz[1024];
        FormatMessageW(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            hr,
            0,
            wsz,
            sizeof(wsz) / sizeof(wsz[0]),
            nullptr);
        printf("error: %s failed with HRESULT 0x%08lx\nmessage: %ws\n", msg, hr, wsz);
        exit(1);
    }
}

static bool d3d12_success(HRESULT hr) {
    return SUCCEEDED(hr);
}

static IDXGIAdapter4* d3d12_dxgi_adapter(IDXGIFactory7* factory) {
    IDXGIAdapter4* adapter = nullptr;
    uint32_t adapter_index = 0;
    while (factory->EnumAdapterByGpuPreference(
               adapter_index,
               DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
               IID_PPV_ARGS(&adapter))
           != DXGI_ERROR_NOT_FOUND) {
        if (d3d12_success(
                D3D12CreateDevice(adapter, MIN_FEATURE_LEVEL, __uuidof(ID3D12Device), nullptr))) {
            DXGI_ADAPTER_DESC3 desc;
            adapter->GetDesc3(&desc);
            printf("info: using adapter %u: %ws\n", adapter_index, desc.Description);
            return adapter;
        }
        adapter_index += 1;
    }
    return adapter;
}

// Main.
int main() {
    // Initialize.
    wil::unique_hwnd window_handle;
    ComPtr<ID3D12Debug6> d3d12_debug;
    ComPtr<IDXGIFactory7> dxgi_factory;
    ComPtr<IDXGIAdapter4> dxgi_adapter;
    ComPtr<ID3D12Device12> d3d12_device;
    ComPtr<ID3D12InfoQueue1> d3d12_info_queue;
    ComPtr<ID3D12DebugDevice2> d3d12_debug_device;
    ComPtr<ID3D12CommandQueue> d3d12_command_queue;
    ComPtr<ID3D12CommandAllocator> d3d12_command_allocator;
    ComPtr<ID3D12GraphicsCommandList9> d3d12_command_list;
    ComPtr<D3D12MA::Allocator> d3d12_allocator;
    ComPtr<IDXGISwapChain4> dxgi_swap_chain;
    ComPtr<ID3D12DescriptorHeap> d3d12_rtv_heap;
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, MAX_FRAMES_IN_FLIGHT> d3d12_rtv_descriptors;
    std::array<ID3D12Resource*, MAX_FRAMES_IN_FLIGHT> d3d12_rtvs;
    ComPtr<ID3D12Fence1> d3d12_fence;
    wil::unique_handle fence_event;

    {
        d3d12_call(D3D12GetDebugInterface(IID_PPV_ARGS(&d3d12_debug)), "D3D12GetDebugInterface");
        d3d12_debug->EnableDebugLayer();
    }

    {
        d3d12_call(
            CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&dxgi_factory)),
            "CreateDXGIFactory2");
    }

    dxgi_adapter = d3d12_dxgi_adapter(dxgi_factory.get());

    {
        d3d12_call(
            D3D12CreateDevice(dxgi_adapter.get(), MIN_FEATURE_LEVEL, IID_PPV_ARGS(&d3d12_device)),
            "D3D12CreateDevice");
        d3d12_device->SetName(L"Device");
    }

    {
        d3d12_device.query_to(&d3d12_info_queue);
        d3d12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        d3d12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        d3d12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        d3d12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, true);
    }

    d3d12_device.query_to(&d3d12_debug_device);

    {
        D3D12_COMMAND_QUEUE_DESC desc = {
            .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
            .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
            .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
            .NodeMask = 0,
        };
        d3d12_call(
            d3d12_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12_command_queue)),
            "CreateCommandQueue");
        d3d12_command_queue->SetName(L"Command Queue");
    }

    {
        d3d12_call(
            d3d12_device->CreateCommandAllocator(
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                IID_PPV_ARGS(&d3d12_command_allocator)),
            "CreateCommandAllocator");
        d3d12_command_allocator->SetName(L"Command Allocator");
    }

    {
        d3d12_call(
            d3d12_device->CreateCommandList1(
                0,
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                D3D12_COMMAND_LIST_FLAG_NONE,
                IID_PPV_ARGS(&d3d12_command_list)),
            "CreateCommandList1");
        d3d12_command_list->SetName(L"Command List");
    }

    {
        D3D12MA::ALLOCATOR_DESC desc = {
            .Flags = D3D12MA::ALLOCATOR_FLAG_NONE,
            .pDevice = d3d12_device.get(),
            .PreferredBlockSize = 0,
            .pAllocationCallbacks = nullptr,
            .pAdapter = dxgi_adapter.get(),
        };
        d3d12_call(D3D12MA::CreateAllocator(&desc, &d3d12_allocator), "D3D12MA::CreateAllocator");
    }

    {
        HINSTANCE module_handle = GetModuleHandle(nullptr);

        WNDCLASSEX window_class = {
            .cbSize = sizeof(WNDCLASSEX),
            .style = CS_HREDRAW | CS_VREDRAW,
            .lpfnWndProc = win32_window_proc,
            .hInstance = module_handle,
            .hIcon = LoadIcon(nullptr, IDI_WINLOGO),
            .hCursor = LoadCursor(nullptr, IDC_ARROW),
            .hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH),
            .lpszMenuName = nullptr,
            .lpszClassName = WINDOW_LTITLE,
            .hIconSm = window_class.hIcon,
        };
        RegisterClassEx(&window_class);

        RECT window_rect = {
            .left = 0,
            .top = 0,
            .right = (LONG)WINDOW_WIDTH,
            .bottom = (LONG)WINDOW_HEIGHT,
        };
        DWORD window_style = WS_OVERLAPPEDWINDOW;
        AdjustWindowRect(&window_rect, window_style, FALSE);

        HWND window = CreateWindowEx(
            WS_EX_APPWINDOW,
            WINDOW_LTITLE,
            WINDOW_LTITLE,
            window_style,
            (GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH) / 2,
            (GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT) / 2,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            nullptr,
            nullptr,
            module_handle,
            nullptr);
        ShowWindow(window, SW_SHOW);
        SetForegroundWindow(window);
        SetFocus(window);
        ShowCursor(true);
        window_handle = wil::unique_hwnd(window);
    }

    {
        ComPtr<IDXGISwapChain1> dxgi_swap_chain_1;
        DXGI_SAMPLE_DESC sample_desc = {
            .Count = 1,
            .Quality = 0,
        };
        DXGI_SWAP_CHAIN_DESC1 desc = {
            .Width = WINDOW_WIDTH,
            .Height = WINDOW_HEIGHT,
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .Stereo = FALSE,
            .SampleDesc = sample_desc,
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = MAX_FRAMES_IN_FLIGHT,
            .Scaling = DXGI_SCALING_STRETCH,
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
            .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
            .Flags = 0,
        };
        d3d12_call(
            dxgi_factory->CreateSwapChainForHwnd(
                d3d12_command_queue.get(),
                window_handle.get(),
                &desc,
                nullptr,
                nullptr,
                &dxgi_swap_chain_1),
            "CreateSwapChainForHwnd");
        dxgi_swap_chain_1.query_to(&dxgi_swap_chain);
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            .NumDescriptors = MAX_FRAMES_IN_FLIGHT,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
            .NodeMask = 0,
        };
        d3d12_call(
            d3d12_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&d3d12_rtv_heap)),
            "CreateDescriptorHeap");

        uint32_t rtv_descriptor_size =
            d3d12_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle =
            d3d12_rtv_heap->GetCPUDescriptorHandleForHeapStart();
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            d3d12_rtv_descriptors[i] = rtv_handle;
            rtv_handle.ptr += rtv_descriptor_size;
        }

        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            ID3D12Resource* d3d12_rtv = nullptr;
            d3d12_call(dxgi_swap_chain->GetBuffer(i, IID_PPV_ARGS(&d3d12_rtv)), "GetBuffer");
            d3d12_device->CreateRenderTargetView(d3d12_rtv, nullptr, d3d12_rtv_descriptors[i]);
            d3d12_rtvs[i] = d3d12_rtv;
        }
    }

    {
        d3d12_call(
            d3d12_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3d12_fence)),
            "CreateFence");
        fence_event = wil::unique_handle(CreateEvent(nullptr, FALSE, FALSE, nullptr));
    }

    // Main loop.
    bool shouldExit = false;
    uint32_t frame_index = 0;
    while (!shouldExit) {
        MSG msg = {};
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT) {
            shouldExit = true;
        }

        float clear_color[4] = {0.0f, 0.2f, 0.4f, 1.0f};

        d3d12_command_allocator->Reset();
        d3d12_command_list->Reset(d3d12_command_allocator.get(), nullptr);
        d3d12_command_list
            ->ClearRenderTargetView(d3d12_rtv_descriptors[frame_index], clear_color, 0, nullptr);
        d3d12_command_list->Close();

        frame_index = (frame_index + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    // Clean up.
    d3d12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, false);
    d3d12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false);
    d3d12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
    d3d12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, false);
    d3d12_call(
        d3d12_debug_device->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL),
        "ReportLiveDeviceObjects");

    return 0;
}
