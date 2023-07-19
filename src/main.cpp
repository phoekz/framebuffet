// Windows Implementation Library (WIL).
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

// DirectX Shader Compiler (DXC).
#include <dxcapi.h>

// DirectXTK12.
#include <directxtk12/SimpleMath.h>

// Standard libraries.
#include <array>
#include <cstdio>

// Constants.
constexpr const char* WINDOW_TITLE = "framebuffet 😎";
constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 480;
constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
constexpr D3D_FEATURE_LEVEL MIN_FEATURE_LEVEL = D3D_FEATURE_LEVEL_12_2;

//
// Win32.
//

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
    return DefWindowProcA(window, message, w_param, l_param);
}

//
// D3D12 - Utilities.
//

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

//
// DXC.
//

struct Dxc {
    ComPtr<IDxcCompiler3> compiler;
    ComPtr<IDxcUtils> utils;
    ComPtr<IDxcIncludeHandler> include_handler;
};

static void dxc_init(Dxc& dxc) {
    DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxc.compiler));
    DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxc.utils));
    dxc.utils->CreateDefaultIncludeHandler(&dxc.include_handler);
}

enum class ShaderType {
    Vertex,
    Pixel,
};

struct ShaderResult {
    ComPtr<IDxcBlob> binary;
    ComPtr<IDxcBlob> pdb;
    ComPtr<IDxcBlobUtf16> pdb_name;
    ComPtr<IDxcBlob> reflection;
};

static void dxc_compile(
    Dxc& dxc,
    LPCWSTR shader_name,
    ShaderType shader_type,
    LPCWSTR shader_entry,
    ShaderResult& result) {
    // Shader profile.
    LPCWSTR shader_profile = nullptr;
    switch (shader_type) {
        case ShaderType::Vertex:
            shader_profile = L"vs_6_7";
            break;
        case ShaderType::Pixel:
            shader_profile = L"ps_6_7";
            break;
    }

    // Shader arguments.
    LPCWSTR shader_args[] = {
        // clang-format off
        shader_name,
        L"-E", shader_entry,
        L"-T", shader_profile,
        L"-HV", L"2021",
        L"-Zi",
        L"-Fd", L".\\shaders\\",
        L"-Qstrip_reflect",
        // clang-format on
    };

    // Shader blob.
    wchar_t shader_path[256];
    wsprintfW(shader_path, L"shaders\\%ws", shader_name);
    ComPtr<IDxcBlobEncoding> shader_blob;
    dxc.utils->LoadFile(shader_path, nullptr, &shader_blob);
    if (!shader_blob) {
        fprintf(stderr, "error: failed to load shader file\n");
        exit(1);
    }
    DxcBuffer shader_buffer = {
        .Ptr = shader_blob->GetBufferPointer(),
        .Size = shader_blob->GetBufferSize(),
        .Encoding = DXC_CP_ACP,
    };

    // Compile.
    ComPtr<IDxcResult> dxc_result;
    ComPtr<IDxcBlobUtf8> dxc_errors;
    d3d12_call(
        dxc.compiler->Compile(
            &shader_buffer,
            shader_args,
            _countof(shader_args),
            dxc.include_handler.get(),
            IID_PPV_ARGS(&dxc_result)),
        "Compile");
    dxc_result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&dxc_errors), nullptr);
    if (dxc_errors && dxc_errors->GetStringLength() != 0) {
        fprintf(stderr, "error: failed to compile %ws\n", shader_name);
        fprintf(stderr, "%s\n", dxc_errors->GetStringPointer());
        exit(1);
    }

    // Results.
    dxc_result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&result.binary), nullptr);
    dxc_result->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&result.pdb), &result.pdb_name);
    dxc_result->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&result.reflection), nullptr);
}

//
// Main.
//

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
    ComPtr<ID3D12GraphicsCommandList9> d3d12_command_list;
    ComPtr<D3D12MA::Allocator> d3d12_allocator;
    ComPtr<IDXGISwapChain4> dxgi_swap_chain;
    HANDLE dxgi_swap_chain_waitable_object = nullptr;
    ComPtr<ID3D12DescriptorHeap> d3d12_rtv_heap;
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, MAX_FRAMES_IN_FLIGHT> d3d12_rtv_descriptors;
    std::array<ID3D12Resource*, MAX_FRAMES_IN_FLIGHT> d3d12_rtvs;
    ComPtr<ID3D12Fence1> d3d12_fence;
    wil::unique_handle fence_event;
    uint64_t fence_last_signaled_value = 0;
    struct Frame {
        ComPtr<ID3D12CommandAllocator> command_allocator;
        uint64_t fence_value = 0;
    };
    std::array<Frame, MAX_FRAMES_IN_FLIGHT> frames;
    uint64_t frame_index = 0;

    // D3D12 - DebugInterface.
    {
        d3d12_call(D3D12GetDebugInterface(IID_PPV_ARGS(&d3d12_debug)), "D3D12GetDebugInterface");
        d3d12_debug->EnableDebugLayer();
    }

    // D3D12 - DXGIFactory.
    {
        d3d12_call(
            CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&dxgi_factory)),
            "CreateDXGIFactory2");
    }

    // D3D12 - DXGIAdapter.
    dxgi_adapter = d3d12_dxgi_adapter(dxgi_factory.get());

    // D3D12 - D3D12Device12.
    {
        d3d12_call(
            D3D12CreateDevice(dxgi_adapter.get(), MIN_FEATURE_LEVEL, IID_PPV_ARGS(&d3d12_device)),
            "D3D12CreateDevice");
        d3d12_device->SetName(L"Device");
    }

    // D3D12 - D3D12InfoQueue.
    {
        d3d12_device.query_to(&d3d12_info_queue);
        d3d12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        d3d12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        d3d12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        d3d12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, true);
    }

    // D3D12 - D3D12DebugDevice2.
    d3d12_device.query_to(&d3d12_debug_device);

    // D3D12 - D3D12CommandQueue.
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

    // D3D12 - D3D12CommandAllocator.
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        ID3D12CommandAllocator* d3d12_command_allocator = nullptr;
        d3d12_call(
            d3d12_device->CreateCommandAllocator(
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                IID_PPV_ARGS(&d3d12_command_allocator)),
            "CreateCommandAllocator");
        wchar_t name[32];
        swprintf_s(name, L"Command Allocator %u", i);
        d3d12_command_allocator->SetName(name);
        frames[i].command_allocator = d3d12_command_allocator;
    }

    // D3D12 - D3D12GraphicsCommandList.
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

    // D3D12MA - Allocator.
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

    // Win32 - Window.
    {
        HINSTANCE module_handle = GetModuleHandleA(nullptr);

        WNDCLASSEXA window_class = {
            .cbSize = sizeof(WNDCLASSEXA),
            .style = CS_HREDRAW | CS_VREDRAW,
            .lpfnWndProc = win32_window_proc,
            .hInstance = module_handle,
            .hIcon = LoadIconA(nullptr, IDI_WINLOGO),
            .hCursor = LoadCursorA(nullptr, IDC_ARROW),
            .hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH),
            .lpszMenuName = nullptr,
            .lpszClassName = WINDOW_TITLE,
            .hIconSm = window_class.hIcon,
        };
        RegisterClassExA(&window_class);

        RECT window_rect = {
            .left = 0,
            .top = 0,
            .right = (LONG)WINDOW_WIDTH,
            .bottom = (LONG)WINDOW_HEIGHT,
        };
        DWORD window_style = WS_OVERLAPPEDWINDOW;
        AdjustWindowRect(&window_rect, window_style, FALSE);

        HWND window = CreateWindowExA(
            WS_EX_APPWINDOW,
            WINDOW_TITLE,
            WINDOW_TITLE,
            window_style,
            (GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH) / 2,
            (GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT) / 2,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            nullptr,
            nullptr,
            module_handle,
            nullptr);
        if (!window) {
            printf("Failed to create window.\n");
            exit(1);
        }
        ShowWindow(window, SW_SHOW);
        SetForegroundWindow(window);
        SetFocus(window);
        ShowCursor(true);
        window_handle = wil::unique_hwnd(window);
    }

    // D3D12 - DXGISwapChain.
    {
        ComPtr<IDXGISwapChain1> dxgi_swap_chain_1;
        DXGI_SWAP_CHAIN_DESC1 desc = {
            .Width = WINDOW_WIDTH,
            .Height = WINDOW_HEIGHT,
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .Stereo = FALSE,
            .SampleDesc =
                {
                    .Count = 1,
                    .Quality = 0,
                },
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = MAX_FRAMES_IN_FLIGHT,
            .Scaling = DXGI_SCALING_STRETCH,
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
            .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
            .Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT,
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
        dxgi_swap_chain_waitable_object = dxgi_swap_chain->GetFrameLatencyWaitableObject();
    }

    // D3D12 - Render target views.
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
            wchar_t name[32];
            swprintf_s(name, L"Swap Chain RTV %u", i);
            d3d12_rtv->SetName(name);
            d3d12_rtvs[i] = d3d12_rtv;
        }
    }

    // D3D12 - Fences.
    {
        d3d12_call(
            d3d12_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3d12_fence)),
            "CreateFence");
        d3d12_fence->SetName(L"Fence");
        fence_event = wil::unique_handle(CreateEvent(nullptr, FALSE, FALSE, nullptr));
    }

    // DXC - Shaders.
    ShaderResult vertex_shader;
    ShaderResult pixel_shader;
    {
        Dxc dxc;
        dxc_init(dxc);
        dxc_compile(dxc, L"triangle.hlsl", ShaderType::Vertex, L"vertex_shader", vertex_shader);
        dxc_compile(dxc, L"triangle.hlsl", ShaderType::Pixel, L"pixel_shader", pixel_shader);
    }

    // Main loop.
    bool should_exit = false;
    while (!should_exit) {
        MSG msg = {};
        if (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }

        if (msg.message == WM_QUIT) {
            should_exit = true;
        }

        Frame* current_frame = nullptr;
        {
            HANDLE waitable_objects[] = {
                dxgi_swap_chain_waitable_object,
                nullptr,
            };
            int waitable_object_count = 1;
            uint32_t next_frame_index = frame_index + 1;
            current_frame = &frames[next_frame_index % MAX_FRAMES_IN_FLIGHT];
            uint64_t fence_value = current_frame->fence_value;
            if (fence_value != 0) {
                current_frame->fence_value = 0;
                d3d12_fence->SetEventOnCompletion(fence_value, fence_event.get());
                waitable_objects[1] = fence_event.get();
                waitable_object_count = 2;
            }
            WaitForMultipleObjects(waitable_object_count, waitable_objects, TRUE, INFINITE);
            frame_index = next_frame_index;
        }

        uint32_t back_buffer_index = dxgi_swap_chain->GetCurrentBackBufferIndex();

        current_frame->command_allocator->Reset();
        d3d12_command_list->Reset(current_frame->command_allocator.get(), nullptr);

        {
            auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                d3d12_rtvs[back_buffer_index],
                D3D12_RESOURCE_STATE_PRESENT,
                D3D12_RESOURCE_STATE_RENDER_TARGET);
            d3d12_command_list->ResourceBarrier(1, &barrier);
        }
        float clear_color[4] = {1.0f, 0.3f, 0.3f, 1.0f};
        d3d12_command_list->ClearRenderTargetView(
            d3d12_rtv_descriptors[back_buffer_index],
            clear_color,
            0,
            nullptr);
        {
            auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                d3d12_rtvs[back_buffer_index],
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PRESENT);
            d3d12_command_list->ResourceBarrier(1, &barrier);
        }
        d3d12_command_list->Close();

        ID3D12CommandList* command_lists[] = {(ID3D12CommandList*)d3d12_command_list.get()};
        d3d12_command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

        uint64_t fence_value = fence_last_signaled_value + 1;
        d3d12_command_queue->Signal(d3d12_fence.get(), fence_value);
        fence_last_signaled_value = fence_value;
        current_frame->fence_value = fence_value;

        dxgi_swap_chain->Present(1, 0);
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
