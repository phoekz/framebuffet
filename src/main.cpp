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
#include <cstdio>

// Constants.
constexpr const char* WINDOW_TITLE = "framebuffet";
constexpr const wchar_t* WINDOW_LTITLE = L"framebuffet";
constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 480;
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
    ComPtr<D3D12MA::Allocator> d3d12_allocator;
    ComPtr<IDXGISwapChain4> dxgi_swap_chain;

    {
        HINSTANCE module_handle = GetModuleHandle(nullptr);

        WNDCLASSEX window_class = {};
        window_class.cbSize = sizeof(WNDCLASSEX);
        window_class.style = CS_HREDRAW | CS_VREDRAW;
        window_class.lpfnWndProc = win32_window_proc;
        window_class.hInstance = module_handle;
        window_class.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
        window_class.hIconSm = window_class.hIcon;
        window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
        window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        window_class.lpszMenuName = nullptr;
        window_class.lpszClassName = WINDOW_LTITLE;
        window_class.cbSize = sizeof(WNDCLASSEX);
        RegisterClassEx(&window_class);

        RECT window_rect = {0, 0, (LONG)WINDOW_WIDTH, (LONG)WINDOW_HEIGHT};
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
        d3d12_call(d3d12_device->QueryInterface(IID_PPV_ARGS(&d3d12_info_queue)), "QueryInterface");
        d3d12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        d3d12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        d3d12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        d3d12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, true);
    }

    d3d12_call(d3d12_device->QueryInterface(IID_PPV_ARGS(&d3d12_debug_device)), "QueryInterface");

    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        d3d12_call(
            d3d12_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12_command_queue)),
            "CreateCommandQueue");
        d3d12_command_queue->SetName(L"Command Queue");
    }

    {
        D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
        allocatorDesc.pDevice = d3d12_device.get();
        allocatorDesc.pAdapter = dxgi_adapter.get();
        allocatorDesc.pAllocationCallbacks = nullptr;
        allocatorDesc.Flags = D3D12MA::ALLOCATOR_FLAG_NONE;
        d3d12_call(
            D3D12MA::CreateAllocator(&allocatorDesc, &d3d12_allocator),
            "D3D12MA::CreateAllocator");
    }

    {
        ComPtr<IDXGISwapChain1> dxgi_swap_chain_1;
        DXGI_SWAP_CHAIN_DESC1 desc = {};
        desc.Width = WINDOW_WIDTH;
        desc.Height = WINDOW_HEIGHT;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.Stereo = FALSE;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferCount = 2;
        desc.Scaling = DXGI_SCALING_STRETCH;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        desc.Flags = 0;
        d3d12_call(
            dxgi_factory->CreateSwapChainForHwnd(
                d3d12_command_queue.get(),
                window_handle.get(),
                &desc,
                nullptr,
                nullptr,
                &dxgi_swap_chain_1),
            "CreateSwapChainForHwnd");
        dxgi_swap_chain_1->QueryInterface(IID_PPV_ARGS(&dxgi_swap_chain));
    }

    // Main loop.
    bool shouldExit = false;
    while (!shouldExit) {
        MSG msg = {};
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT) {
            shouldExit = true;
        }
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
