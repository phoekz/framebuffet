// Windows Implementation Library (WIL).
#define WIL_SUPPRESS_EXCEPTIONS
#include <wil/com.h>
#include <wil/resource.h>
template<typename T>
using ComPtr = wil::com_ptr_nothrow<T>;

// DirectX.
#include <d3dx12/d3dx12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
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

// WinPixEventRuntime.
#define USE_PIX
#include <WinPixEventRuntime/pix3.h>

// Standard libraries.
#include <array>
#include <cstdio>

// Constants.
constexpr const char* WINDOW_TITLE = "framebuffet 😎";
constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 480;
constexpr uint32_t FRAME_COUNT = 2;
constexpr D3D_FEATURE_LEVEL MIN_FEATURE_LEVEL = D3D_FEATURE_LEVEL_12_2;
constexpr float CLEAR_COLOR[4] = {0.1f, 0.1f, 0.1f, 1.0f};

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

static IDXGIAdapter4* d3d12_dxgi_adapter(IDXGIFactory7* factory) {
    IDXGIAdapter4* adapter = nullptr;
    uint32_t adapter_index = 0;
    while (factory->EnumAdapterByGpuPreference(
               adapter_index,
               DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
               IID_PPV_ARGS(&adapter))
           != DXGI_ERROR_NOT_FOUND) {
        if (SUCCEEDED(
                D3D12CreateDevice(adapter, MIN_FEATURE_LEVEL, __uuidof(ID3D12Device), nullptr))) {
            DXGI_ADAPTER_DESC3 desc;
            adapter->GetDesc3(&desc);
            fprintf(stderr, "info: using adapter %u: %ws\n", adapter_index, desc.Description);
            return adapter;
        }
        adapter_index += 1;
    }
    return adapter;
}

static void d3d12_message_func(
    D3D12_MESSAGE_CATEGORY category,
    D3D12_MESSAGE_SEVERITY severity,
    D3D12_MESSAGE_ID /*id*/,
    LPCSTR description,
    void* /*context*/) {
    // clang-format off

    // Message category.
    const char* category_name = "unknown";
    switch (category) {
        case D3D12_MESSAGE_CATEGORY_APPLICATION_DEFINED: category_name = "APPLICATION_DEFINED"; break;
        case D3D12_MESSAGE_CATEGORY_MISCELLANEOUS: category_name = "MISCELLANEOUS"; break;
        case D3D12_MESSAGE_CATEGORY_INITIALIZATION: category_name = "INITIALIZATION"; break;
        case D3D12_MESSAGE_CATEGORY_CLEANUP: category_name = "CLEANUP"; break;
        case D3D12_MESSAGE_CATEGORY_COMPILATION: category_name = "COMPILATION"; break;
        case D3D12_MESSAGE_CATEGORY_STATE_CREATION: category_name = "STATE_CREATION"; break;
        case D3D12_MESSAGE_CATEGORY_STATE_SETTING: category_name = "STATE_SETTING"; break;
        case D3D12_MESSAGE_CATEGORY_STATE_GETTING: category_name = "STATE_GETTING"; break;
        case D3D12_MESSAGE_CATEGORY_RESOURCE_MANIPULATION: category_name = "RESOURCE_MANIPULATION"; break;
        case D3D12_MESSAGE_CATEGORY_EXECUTION: category_name = "EXECUTION"; break;
        case D3D12_MESSAGE_CATEGORY_SHADER: category_name = "SHADER"; break;
    }

    // Message severity.
    const char* severity_name = "unknown";
    switch (severity) {
        case D3D12_MESSAGE_SEVERITY_CORRUPTION: severity_name = "CORRUPTION"; break;
        case D3D12_MESSAGE_SEVERITY_ERROR: severity_name = "ERROR"; break;
        case D3D12_MESSAGE_SEVERITY_WARNING: severity_name = "WARNING"; break;
        case D3D12_MESSAGE_SEVERITY_INFO: severity_name = "INFO"; break;
        case D3D12_MESSAGE_SEVERITY_MESSAGE: severity_name = "MESSAGE"; break;
    }

    // clang-format on

    fprintf(stderr, "[%s] %s: %s\n", category_name, severity_name, description);
}

#if defined(_DEBUG) || defined(DBG)
inline void d3d12_set_name(ID3D12Object* object, LPCWSTR name) {
    object->SetName(name);
}
inline void d3d12_set_indexed_name(ID3D12Object* object, LPCWSTR name, UINT index) {
    WCHAR indexed_name[256];
    if (swprintf_s(indexed_name, L"%s[%u]", name, index) > 0) {
        object->SetName(indexed_name);
    }
}
#else
inline void d3d12_set_name(ID3D12Object*, LPCWSTR) {}
inline void d3d12_set_name_indexed(ID3D12Object*, LPCWSTR, UINT) {}
#endif

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
    FAIL_FAST_IF_FAILED(dxc.utils->LoadFile(shader_path, nullptr, &shader_blob));
    FAIL_FAST_IF_NULL_MSG(shader_blob, "Failed to load shader file");
    DxcBuffer shader_buffer = {
        .Ptr = shader_blob->GetBufferPointer(),
        .Size = shader_blob->GetBufferSize(),
        .Encoding = DXC_CP_ACP,
    };

    // Compile.
    ComPtr<IDxcResult> dxc_result;
    ComPtr<IDxcBlobUtf8> dxc_errors;
    FAIL_FAST_IF_FAILED(dxc.compiler->Compile(
        &shader_buffer,
        shader_args,
        _countof(shader_args),
        dxc.include_handler.get(),
        IID_PPV_ARGS(&dxc_result)));
    FAIL_FAST_IF_FAILED(dxc_result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&dxc_errors), nullptr));
    if (dxc_errors && dxc_errors->GetStringLength() != 0) {
        FAIL_FAST_MSG("Failed to compile %ws\n%s", dxc_errors->GetStringPointer());
    }

    // Results.
    FAIL_FAST_IF_FAILED(
        dxc_result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&result.binary), nullptr));
    FAIL_FAST_IF_FAILED(
        dxc_result->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&result.pdb), &result.pdb_name));
    FAIL_FAST_IF_FAILED(
        dxc_result->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&result.reflection), nullptr));
}

//
// Main.
//

int main() {
    // Initialize.
    ComPtr<IDXGIFactory7> dxgi_factory;
    uint32_t dxgi_factory_flags = 0;
    ComPtr<IDXGIAdapter4> dxgi_adapter;
    ComPtr<ID3D12Device12> d3d12_device;
    ComPtr<ID3D12CommandQueue> d3d12_command_queue;
    ComPtr<ID3D12GraphicsCommandList9> d3d12_command_list;
    ComPtr<D3D12MA::Allocator> d3d12_allocator;
    wil::unique_hwnd window_handle;
    ComPtr<IDXGISwapChain4> dxgi_swap_chain;
    CD3DX12_VIEWPORT viewport;
    CD3DX12_RECT scissor_rect;
    ComPtr<ID3D12DescriptorHeap> d3d12_rtv_heap;
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, FRAME_COUNT> d3d12_rtv_descriptors;
    std::array<ID3D12Resource*, FRAME_COUNT> d3d12_rtvs;
    std::array<ComPtr<ID3D12CommandAllocator>, FRAME_COUNT> d3d12_command_allocators;
    ComPtr<ID3D12Fence1> d3d12_fence;
    uint32_t frame_index = 0;
    wil::unique_handle fence_event;
    std::array<uint64_t, FRAME_COUNT> fence_values = {};

    ComPtr<ID3D12RootSignature> d3d12_root_signature;
    ComPtr<ID3D12PipelineState> d3d12_pipeline_state;
    ShaderResult vertex_shader;
    ShaderResult pixel_shader;
    ComPtr<ID3D12Resource> vertex_buffer;
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;

    // D3D12 - DebugInterface.
#if defined(_DEBUG)
    {
        ComPtr<ID3D12Debug6> debug;
        FAIL_FAST_IF_FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)));
        debug->EnableDebugLayer();
        dxgi_factory_flags = DXGI_CREATE_FACTORY_DEBUG;
    }
#endif

    // D3D12 - DXGIFactory.
    FAIL_FAST_IF_FAILED(CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&dxgi_factory)));

    // D3D12 - DXGIAdapter.
    dxgi_adapter = d3d12_dxgi_adapter(dxgi_factory.get());

    // D3D12 - D3D12Device12.
    {
        FAIL_FAST_IF_FAILED(
            D3D12CreateDevice(dxgi_adapter.get(), MIN_FEATURE_LEVEL, IID_PPV_ARGS(&d3d12_device)));
        d3d12_set_name(d3d12_device.get(), L"Device");
    }

    // D3D12 - D3D12InfoQueue.
    {
        ComPtr<ID3D12InfoQueue1> info_queue;
        d3d12_device.query_to(&info_queue);
        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, true);
        info_queue->RegisterMessageCallback(
            d3d12_message_func,
            D3D12_MESSAGE_CALLBACK_FLAG_NONE,
            nullptr,
            nullptr);
    }

    // D3D12 - D3D12CommandQueue.
    {
        D3D12_COMMAND_QUEUE_DESC desc = {
            .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
            .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
            .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
            .NodeMask = 0,
        };
        FAIL_FAST_IF_FAILED(
            d3d12_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12_command_queue)));
        d3d12_set_name(d3d12_command_queue.get(), L"Command Queue");
    }

    // D3D12 - D3D12CommandAllocator.
    for (uint32_t i = 0; i < FRAME_COUNT; i++) {
        FAIL_FAST_IF_FAILED(d3d12_device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(&d3d12_command_allocators[i])));
        d3d12_set_indexed_name(d3d12_command_allocators[i].get(), L"Command Allocator", i);
    }

    // D3D12 - D3D12GraphicsCommandList.
    {
        FAIL_FAST_IF_FAILED(d3d12_device->CreateCommandList1(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            D3D12_COMMAND_LIST_FLAG_NONE,
            IID_PPV_ARGS(&d3d12_command_list)));
        d3d12_set_name(d3d12_command_list.get(), L"Command List");
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
        FAIL_FAST_IF_FAILED(D3D12MA::CreateAllocator(&desc, &d3d12_allocator));
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

        window_handle = wil::unique_hwnd(CreateWindowExA(
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
            nullptr));
        FAIL_FAST_IF_NULL_MSG(window_handle, "Failed to create window.");
        ShowWindow(window_handle.get(), SW_SHOW);
        SetForegroundWindow(window_handle.get());
        SetFocus(window_handle.get());
        ShowCursor(true);
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
            .BufferCount = FRAME_COUNT,
            .Scaling = DXGI_SCALING_STRETCH,
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
            .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
            .Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT,
        };
        FAIL_FAST_IF_FAILED(dxgi_factory->CreateSwapChainForHwnd(
            d3d12_command_queue.get(),
            window_handle.get(),
            &desc,
            nullptr,
            nullptr,
            &dxgi_swap_chain_1));
        dxgi_swap_chain_1.query_to(&dxgi_swap_chain);

        viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);
        scissor_rect = CD3DX12_RECT(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        frame_index = dxgi_swap_chain->GetCurrentBackBufferIndex();
    }

    // D3D12 - Render target views.
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            .NumDescriptors = FRAME_COUNT,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
            .NodeMask = 0,
        };
        FAIL_FAST_IF_FAILED(
            d3d12_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&d3d12_rtv_heap)));

        uint32_t rtv_descriptor_size =
            d3d12_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle =
            d3d12_rtv_heap->GetCPUDescriptorHandleForHeapStart();
        for (uint32_t i = 0; i < FRAME_COUNT; i++) {
            d3d12_rtv_descriptors[i] = rtv_handle;
            rtv_handle.ptr += rtv_descriptor_size;
        }

        for (uint32_t i = 0; i < FRAME_COUNT; i++) {
            FAIL_FAST_IF_FAILED(dxgi_swap_chain->GetBuffer(i, IID_PPV_ARGS(&d3d12_rtvs[i])));
            d3d12_device->CreateRenderTargetView(d3d12_rtvs[i], nullptr, d3d12_rtv_descriptors[i]);
            d3d12_set_indexed_name(d3d12_rtvs[i], L"Swap Chain RTV", i);
        }
    }

    // D3D12 - Fences.
    {
        FAIL_FAST_IF_FAILED(
            d3d12_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3d12_fence)));
        d3d12_set_name(d3d12_fence.get(), L"Fence");
        fence_event = wil::unique_handle(CreateEvent(nullptr, FALSE, FALSE, nullptr));
        FAIL_FAST_IF_NULL_MSG(fence_event, "Failed to create fence event.\n");
        fence_values[frame_index]++;
    }

    // D3D12 - Root signature.
    {
        CD3DX12_ROOT_SIGNATURE_DESC desc;
        desc.Init(
            0,
            nullptr,
            0,
            nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
        FAIL_FAST_IF_FAILED(d3d12_device->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&d3d12_root_signature)));
        d3d12_set_name(d3d12_root_signature.get(), L"Root Signature");
    }

    // DXC - Shaders.
    {
        Dxc dxc;
        dxc_init(dxc);
        dxc_compile(dxc, L"triangle.hlsl", ShaderType::Vertex, L"vertex_shader", vertex_shader);
        dxc_compile(dxc, L"triangle.hlsl", ShaderType::Pixel, L"pixel_shader", pixel_shader);
    }

    // D3D12 - Pipeline state.
    {
        D3D12_INPUT_ELEMENT_DESC input_element_descs[] = {
            // clang-format off
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
            // clang-format on
        };
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {
            .pRootSignature = d3d12_root_signature.get(),
            .VS = CD3DX12_SHADER_BYTECODE(
                vertex_shader.binary->GetBufferPointer(),
                vertex_shader.binary->GetBufferSize()),
            .PS = CD3DX12_SHADER_BYTECODE(
                pixel_shader.binary->GetBufferPointer(),
                pixel_shader.binary->GetBufferSize()),
            .BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            .SampleMask = UINT_MAX,
            .RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            .DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(),
            .InputLayout = {input_element_descs, _countof(input_element_descs)},
            .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            .NumRenderTargets = 1,
            .RTVFormats = {DXGI_FORMAT_R8G8B8A8_UNORM},
            .SampleDesc =
                {
                    .Count = 1,
                    .Quality = 0,
                },
        };
        FAIL_FAST_IF_FAILED(
            d3d12_device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&d3d12_pipeline_state)));
        d3d12_set_name(d3d12_pipeline_state.get(), L"Pipeline State");
    }

    // D3D12 - Triangle.
    {
        using namespace DirectX::SimpleMath;

        struct Vertex {
            Vector3 position;
            Vector4 color;
        };

        Vertex triangle_vertices[] = {
            {{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
            {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}};

        uint32_t vertex_buffer_size = (uint32_t)sizeof(triangle_vertices);

        auto heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(vertex_buffer_size);
        FAIL_FAST_IF_FAILED(d3d12_device->CreateCommittedResource(
            &heap_properties,
            D3D12_HEAP_FLAG_NONE,
            &buffer_desc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&vertex_buffer)));
        d3d12_set_name(vertex_buffer.get(), L"Vertex Buffer");

        UINT8* vertex_data;
        auto read_range = CD3DX12_RANGE(0, 0);
        FAIL_FAST_IF_FAILED(vertex_buffer->Map(0, &read_range, (void**)&vertex_data));
        memcpy(vertex_data, triangle_vertices, sizeof(triangle_vertices));
        vertex_buffer->Unmap(0, nullptr);

        vertex_buffer_view = {
            .BufferLocation = vertex_buffer->GetGPUVirtualAddress(),
            .SizeInBytes = vertex_buffer_size,
            .StrideInBytes = sizeof(Vertex),
        };
    }

    // Wait for pending GPU work to complete.
    {
        // Schedule a Signal command in the queue.
        FAIL_FAST_IF_FAILED(
            d3d12_command_queue->Signal(d3d12_fence.get(), fence_values[frame_index]));

        // Wait until the fence has been processed.
        FAIL_FAST_IF_FAILED(
            d3d12_fence->SetEventOnCompletion(fence_values[frame_index], fence_event.get()));
        WaitForSingleObjectEx(fence_event.get(), INFINITE, FALSE);

        // Increment the fence value for the current frame.
        fence_values[frame_index]++;
    }

    // Main loop.
    bool running = true;
    while (running) {
        // Handle window messages.
        {
            MSG msg = {};
            if (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageA(&msg);
            }
            if (msg.message == WM_QUIT) {
                running = false;
            }
        }

        // Populate command list.
        d3d12_command_allocators[frame_index]->Reset();
        d3d12_command_list->Reset(
            d3d12_command_allocators[frame_index].get(),
            d3d12_pipeline_state.get());
        PIXBeginEvent(d3d12_command_list.get(), PIX_COLOR_DEFAULT, "Render");

        d3d12_command_list->SetGraphicsRootSignature(d3d12_root_signature.get());
        d3d12_command_list->RSSetViewports(1, &viewport);
        d3d12_command_list->RSSetScissorRects(1, &scissor_rect);
        d3d12_command_list
            ->OMSetRenderTargets(1, &d3d12_rtv_descriptors[frame_index], FALSE, nullptr);

        {
            auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                d3d12_rtvs[frame_index],
                D3D12_RESOURCE_STATE_PRESENT,
                D3D12_RESOURCE_STATE_RENDER_TARGET);
            d3d12_command_list->ResourceBarrier(1, &barrier);
        }

        d3d12_command_list
            ->ClearRenderTargetView(d3d12_rtv_descriptors[frame_index], CLEAR_COLOR, 0, nullptr);
        d3d12_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        d3d12_command_list->IASetVertexBuffers(0, 1, &vertex_buffer_view);
        d3d12_command_list->DrawInstanced(3, 1, 0, 0);

        {
            auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                d3d12_rtvs[frame_index],
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PRESENT);
            d3d12_command_list->ResourceBarrier(1, &barrier);
        }

        PIXEndEvent(d3d12_command_list.get());
        d3d12_command_list->Close();

        // Execute command list.
        ID3D12CommandList* command_lists[] = {(ID3D12CommandList*)d3d12_command_list.get()};
        d3d12_command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

        // Present.
        dxgi_swap_chain->Present(1, 0);

        // Move to next frame.
        {
            // Schedule a Signal command in the queue.
            uint64_t current_fence_value = fence_values[frame_index];
            FAIL_FAST_IF_FAILED(
                d3d12_command_queue->Signal(d3d12_fence.get(), current_fence_value));

            // Update the frame index.
            frame_index = dxgi_swap_chain->GetCurrentBackBufferIndex();

            // If the next frame is not ready to be rendered yet, wait until it is ready.
            if (d3d12_fence->GetCompletedValue() < fence_values[frame_index]) {
                FAIL_FAST_IF_FAILED(d3d12_fence->SetEventOnCompletion(
                    fence_values[frame_index],
                    fence_event.get()));
                WaitForSingleObjectEx(fence_event.get(), INFINITE, FALSE);
            }

            // Set the fence value for the next frame.
            fence_values[frame_index] = current_fence_value + 1;
        }
    }

    // Wait for pending GPU work to complete.
    {
        // Schedule a Signal command in the queue.
        FAIL_FAST_IF_FAILED(
            d3d12_command_queue->Signal(d3d12_fence.get(), fence_values[frame_index]));

        // Wait until the fence has been processed.
        FAIL_FAST_IF_FAILED(
            d3d12_fence->SetEventOnCompletion(fence_values[frame_index], fence_event.get()));
        WaitForSingleObjectEx(fence_event.get(), INFINITE, FALSE);
    }

    // Detect resource leaks.
#if defined(_DEBUG)
    {
        ComPtr<ID3D12DebugDevice2> debug_device;
        d3d12_device.query_to(&debug_device);
        FAIL_FAST_IF_FAILED(
            debug_device->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL));
    }
#endif

    return 0;
}
