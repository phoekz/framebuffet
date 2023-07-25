// Framebuffet
#include "maths.hpp"
#include "utils.hpp"

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
__declspec(dllexport) extern const UINT D3D12SDKVersion = 610;
__declspec(dllexport) extern const char* D3D12SDKPath = ".\\";
}

// D3D12 memory allocator.
#include <D3D12MemAlloc.h>

// DirectX Shader Compiler (DXC).
#include <dxcapi.h>

// DirectXTK12.
#include <DirectXTK12/GeometricPrimitive.h>

// WinPixEventRuntime.
#define USE_PIX
#include <WinPixEventRuntime/pix3.h>

// Dear ImGui.
#include <imgui.h>

// Standard libraries.
#include <array>
#include <vector>

// Constants.
constexpr const char* WINDOW_TITLE = "framebuffet 😎";
constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 480;
constexpr float WINDOW_ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
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
            log_info("Using adapter {}: {}", adapter_index, fb::from_wstr(desc.Description));
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

    log_info("[{}] {}: {}", category_name, severity_name, description);
}

#if defined(_DEBUG) || defined(DBG)
inline void d3d12_set_name(ID3D12Object* object, LPCWSTR name) {
    object->SetName(name);
}
inline void d3d12_set_indexed_name(ID3D12Object* object, LPCWSTR name, UINT index) {
    WCHAR indexed_name[256] = {};
    swprintf_s(indexed_name, L"%s[%u]", name, index);
    object->SetName(indexed_name);
}
#else
inline void d3d12_set_name(ID3D12Object*, LPCWSTR) {}
inline void d3d12_set_indexed_name(ID3D12Object*, LPCWSTR, UINT) {}
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
        FAIL_FAST_MSG("Failed to compile %ws\n%s", shader_name, dxc_errors->GetStringPointer());
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
    UINT dxgi_factory_flags = 0;
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

    ComPtr<ID3D12DescriptorHeap> d3d12_cbv_srv_uav_heap;

    ComPtr<ID3D12Resource> constant_buffer;
    UINT8* cbv_data_begin = nullptr;

    ComPtr<ID3D12Resource> vertex_buffer;
    ComPtr<ID3D12Resource> index_buffer;
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;
    D3D12_INDEX_BUFFER_VIEW index_buffer_view;

    ComPtr<ID3D12Resource> texture;

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

    // D3D12 - DXGIOutput.
    {
        UINT i = 0;
        ComPtr<IDXGIOutput> output;
        while (dxgi_adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND) {
            ComPtr<IDXGIOutput6> output6;
            output.query_to(&output6);

            DXGI_OUTPUT_DESC1 desc;
            output6->GetDesc1(&desc);

            fb::Rect desktop_rect(desc.DesktopCoordinates);
            log_info(
                "display: {} x {} @ {}bpp, luminance: {} to {}",
                desktop_rect.width,
                desktop_rect.height,
                desc.BitsPerColor,
                desc.MinLuminance,
                desc.MaxLuminance);

            ++i;
        }
    }

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

        UINT rtv_descriptor_size =
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
        // Check support.
        constexpr D3D_ROOT_SIGNATURE_VERSION ROOT_SIGNATURE_VERSION =
            D3D_ROOT_SIGNATURE_VERSION_1_2;
        D3D12_FEATURE_DATA_ROOT_SIGNATURE feature_data = {.HighestVersion = ROOT_SIGNATURE_VERSION};
        FAIL_FAST_IF_FAILED(d3d12_device->CheckFeatureSupport(
            D3D12_FEATURE_ROOT_SIGNATURE,
            &feature_data,
            sizeof(feature_data)));

        CD3DX12_DESCRIPTOR_RANGE1 cbv_range;
        cbv_range.Init(
            /* rangeType */ D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
            /* numDescriptors */ 1,
            /* baseShaderRegister */ 0,
            /* registerSpace */ 0,
            /* flags */ D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

        CD3DX12_DESCRIPTOR_RANGE1 srv_range;
        srv_range.Init(
            /* rangeType */ D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
            /* numDescriptors */ 1,
            /* baseShaderRegister */ 0,
            /* registerSpace */ 0,
            /* flags */ D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

        CD3DX12_ROOT_PARAMETER1 root_parameters[2];
        root_parameters[0].InitAsDescriptorTable(
            /* numDescriptorRanges */ 1,
            /* pDescriptorRanges */ &cbv_range,
            /* visibility */ D3D12_SHADER_VISIBILITY_VERTEX);
        root_parameters[1].InitAsDescriptorTable(
            /* numDescriptorRanges */ 1,
            /* pDescriptorRanges */ &srv_range,
            /* visibility */ D3D12_SHADER_VISIBILITY_PIXEL);

        D3D12_STATIC_SAMPLER_DESC1 sampler = {
            .Filter = D3D12_FILTER_MIN_MAG_MIP_POINT,
            .AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            .AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            .AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            .MipLODBias = 0.0f,
            .MaxAnisotropy = 0,
            .ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
            .BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK,
            .MinLOD = 0.0f,
            .MaxLOD = D3D12_FLOAT32_MAX,
            .ShaderRegister = 0,
            .RegisterSpace = 0,
            .ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL,
            .Flags = D3D12_SAMPLER_FLAG_NONE,
        };

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc;
        decltype(desc)::Init_1_2(
            desc,
            /* numParameters */ _countof(root_parameters),
            /* _pParameters */ root_parameters,
            /* numStaticSamplers */ 1,
            /* _pStaticSamplers */ &sampler,
            /* flags */ D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        FAIL_FAST_IF_FAILED(D3DX12SerializeVersionedRootSignature(
            &desc,
            ROOT_SIGNATURE_VERSION,
            &signature,
            &error));
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
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
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
            .SampleDesc = {.Count = 1, .Quality = 0},
        };
        FAIL_FAST_IF_FAILED(
            d3d12_device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&d3d12_pipeline_state)));
        d3d12_set_name(d3d12_pipeline_state.get(), L"Pipeline State");
    }

    // D3D12 - CBV_SRV_UAV Heap.
    D3D12_GPU_DESCRIPTOR_HANDLE cbv_gpu_descriptor_handle;
    D3D12_GPU_DESCRIPTOR_HANDLE srv_gpu_descriptor_handle;
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .NumDescriptors = 2,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            .NodeMask = 0,
        };
        FAIL_FAST_IF_FAILED(
            d3d12_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&d3d12_cbv_srv_uav_heap)));
        d3d12_set_name(d3d12_cbv_srv_uav_heap.get(), L"CBV_SRV_UAV Heap");

        uint32_t increment_size =
            d3d12_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        cbv_gpu_descriptor_handle = d3d12_cbv_srv_uav_heap->GetGPUDescriptorHandleForHeapStart();
        srv_gpu_descriptor_handle = d3d12_cbv_srv_uav_heap->GetGPUDescriptorHandleForHeapStart();
        srv_gpu_descriptor_handle.ptr += increment_size;
    }

    // D3D12 - Constants.
    struct ConstantBuffer {
        fb::Mat4x4 transform;
        float padding[48];
    } constant_buffer_data;
    static_assert(
        (sizeof(ConstantBuffer) % 256) == 0,
        "Constant Buffer size must be 256-byte aligned");
    {
        uint32_t constant_buffer_size = (uint32_t)sizeof(ConstantBuffer);
        auto heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(constant_buffer_size);
        FAIL_FAST_IF_FAILED(d3d12_device->CreateCommittedResource(
            &heap_properties,
            D3D12_HEAP_FLAG_NONE,
            &buffer_desc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&constant_buffer)));

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc = {
            .BufferLocation = constant_buffer->GetGPUVirtualAddress(),
            .SizeInBytes = constant_buffer_size,
        };
        d3d12_device->CreateConstantBufferView(
            &cbv_desc,
            d3d12_cbv_srv_uav_heap->GetCPUDescriptorHandleForHeapStart());

        auto read_range = CD3DX12_RANGE(0, 0);
        FAIL_FAST_IF_FAILED(constant_buffer->Map(0, &read_range, (void**)&cbv_data_begin));
        memcpy(cbv_data_begin, &constant_buffer_data, sizeof(constant_buffer_data));
    }

    // D3D12 - Geometry.
    uint32_t index_count = 0;
    {
        DirectX::DX12::GeometricPrimitive::VertexCollection vertices;
        DirectX::DX12::GeometricPrimitive::IndexCollection indices;
        DirectX::DX12::GeometricPrimitive::CreateCube(vertices, indices);

        uint32_t vertices_size = (uint32_t)vertices.size() * sizeof(vertices[0]);
        uint32_t indices_size = (uint32_t)indices.size() * sizeof(indices[0]);
        uint32_t vertex_size = (uint32_t)sizeof(vertices[0]);
        index_count = (uint32_t)indices.size();

        auto heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

        {
            auto vertex_buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(vertices_size);
            FAIL_FAST_IF_FAILED(d3d12_device->CreateCommittedResource(
                &heap_properties,
                D3D12_HEAP_FLAG_NONE,
                &vertex_buffer_desc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&vertex_buffer)));
            d3d12_set_name(vertex_buffer.get(), L"Vertex Buffer");
            UINT8* vertex_data;
            auto read_range = CD3DX12_RANGE(0, 0);
            FAIL_FAST_IF_FAILED(vertex_buffer->Map(0, &read_range, (void**)&vertex_data));
            memcpy(vertex_data, vertices.data(), vertices_size);
            vertex_buffer->Unmap(0, nullptr);
        }

        {
            auto index_buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(indices_size);
            FAIL_FAST_IF_FAILED(d3d12_device->CreateCommittedResource(
                &heap_properties,
                D3D12_HEAP_FLAG_NONE,
                &index_buffer_desc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&index_buffer)));
            d3d12_set_name(index_buffer.get(), L"Index Buffer");
            UINT8* index_data;
            auto read_range = CD3DX12_RANGE(0, 0);
            FAIL_FAST_IF_FAILED(index_buffer->Map(0, &read_range, (void**)&index_data));
            memcpy(index_data, indices.data(), indices_size);
            index_buffer->Unmap(0, nullptr);
        }

        vertex_buffer_view = {
            .BufferLocation = vertex_buffer->GetGPUVirtualAddress(),
            .SizeInBytes = vertices_size,
            .StrideInBytes = vertex_size,
        };
        index_buffer_view = {
            .BufferLocation = index_buffer->GetGPUVirtualAddress(),
            .SizeInBytes = indices_size,
            .Format = DXGI_FORMAT_R16_UINT,
        };
    }

    // D3D12 - Texture - Resource.
    ComPtr<ID3D12Resource> texture_upload_heap;
    {
        constexpr UINT TEXTURE_WIDTH = 32;
        constexpr UINT TEXTURE_HEIGHT = 32;
        constexpr UINT TEXTURE_PIXEL_SIZE = 4;
        constexpr LONG_PTR TEXTURE_ROW_PITCH = TEXTURE_WIDTH * TEXTURE_PIXEL_SIZE;
        constexpr LONG_PTR TEXTURE_SLICE_PITCH = TEXTURE_HEIGHT * TEXTURE_ROW_PITCH;
        constexpr DXGI_FORMAT TEXTURE_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

        auto texture_desc = CD3DX12_RESOURCE_DESC::Tex2D(
            /* format */ TEXTURE_FORMAT,
            /* width */ TEXTURE_WIDTH,
            /* height */ TEXTURE_HEIGHT,
            /* arraySize */ 1,
            /* mipLevels */ 1);
        CD3DX12_HEAP_PROPERTIES default_heap(D3D12_HEAP_TYPE_DEFAULT);
        FAIL_FAST_IF_FAILED(d3d12_device->CreateCommittedResource(
            /* pHeapProperties */ &default_heap,
            /* HeapFlags */ D3D12_HEAP_FLAG_NONE,
            /* pDesc */ &texture_desc,
            /* InitialResourceState */ D3D12_RESOURCE_STATE_COPY_DEST,
            /* pOptimizedClearValue */ nullptr,
            /* riidResource */ IID_PPV_ARGS(&texture)));
        d3d12_set_name(texture.get(), L"Texture");

        UINT64 upload_buffer_size = GetRequiredIntermediateSize(texture.get(), 0, 1);
        auto buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(upload_buffer_size);
        CD3DX12_HEAP_PROPERTIES upload_heap(D3D12_HEAP_TYPE_UPLOAD);
        FAIL_FAST_IF_FAILED(d3d12_device->CreateCommittedResource(
            /* pHeapProperties */ &upload_heap,
            /* HeapFlags */ D3D12_HEAP_FLAG_NONE,
            /* pDesc */ &buffer_desc,
            /* InitialResourceState */ D3D12_RESOURCE_STATE_GENERIC_READ,
            /* pOptimizedClearValue */ nullptr,
            /* riidResource */ IID_PPV_ARGS(&texture_upload_heap)));
        d3d12_set_name(texture_upload_heap.get(), L"Texture Upload Heap");

        std::vector<uint8_t> texture_data;
        texture_data.resize(TEXTURE_SLICE_PITCH);
        for (uint32_t y = 0; y < TEXTURE_HEIGHT; y++) {
            for (uint32_t x = 0; x < TEXTURE_WIDTH; x++) {
                uint32_t* p =
                    (uint32_t*)&texture_data[TEXTURE_PIXEL_SIZE * (x + y * TEXTURE_WIDTH)];
                *p = ((x ^ y) & 1) == 0 ? 0xffffffff : 0xffaaaaaa;
            }
        }
        D3D12_SUBRESOURCE_DATA subresource_data = {
            .pData = texture_data.data(),
            .RowPitch = TEXTURE_ROW_PITCH,
            .SlicePitch = TEXTURE_SLICE_PITCH,
        };
        d3d12_command_allocators[frame_index]->Reset();
        d3d12_command_list->Reset(d3d12_command_allocators[frame_index].get(), nullptr);
        FAIL_FAST_IF(
            UpdateSubresources(
                /* pCmdList */ d3d12_command_list.get(),
                /* pDestinationResource */ texture.get(),
                /* pIntermediate */ texture_upload_heap.get(),
                /* IntermediateOffset */ 0,
                /* FirstSubresource */ 0,
                /* NumSubresources */ 1,
                /* pSrcData */ &subresource_data)
            == 0);
        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            texture.get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        d3d12_command_list->ResourceBarrier(1, &barrier);

        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {
            .Format = TEXTURE_FORMAT,
            .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
            .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
            .Texture2D =
                D3D12_TEX2D_SRV {
                    .MipLevels = 1,
                },
        };
        D3D12_CPU_DESCRIPTOR_HANDLE heap_start =
            d3d12_cbv_srv_uav_heap->GetCPUDescriptorHandleForHeapStart();
        heap_start.ptr +=
            d3d12_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        d3d12_device->CreateShaderResourceView(texture.get(), &srv_desc, heap_start);
    }

    // Wait for pending GPU work to complete.
    {
        d3d12_command_list->Close();
        ID3D12CommandList* command_lists[] = {(ID3D12CommandList*)d3d12_command_list.get()};
        d3d12_command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

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
    fb::FrameTiming ft = {};
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

        // Update frame timing.
        ft.update();

        // Update.
        {
            fb::Mat4x4 perspective = fb::Mat4x4::CreatePerspectiveFieldOfView(
                fb::rad_from_deg(45.0f),
                WINDOW_ASPECT_RATIO,
                0.1f,
                100.0f);
            fb::Vec3 eye = fb::Vec3(
                2.5f * std::sin(ft.elapsed_time()),
                1.5f,
                2.5f * std::cos(ft.elapsed_time()));
            fb::Mat4x4 view = fb::Mat4x4::CreateLookAt(
                eye,
                fb::Vec3(0.0f, 0.0f, 0.0f),
                fb::Vec3(0.0f, 1.0f, 0.0f));
            constant_buffer_data.transform = view * perspective;
            memcpy(cbv_data_begin, &constant_buffer_data, sizeof(constant_buffer_data));
        }

        // Populate command list.
        d3d12_command_allocators[frame_index]->Reset();
        d3d12_command_list->Reset(
            d3d12_command_allocators[frame_index].get(),
            d3d12_pipeline_state.get());
        PIXBeginEvent(d3d12_command_list.get(), PIX_COLOR_DEFAULT, "Render");

        d3d12_command_list->SetGraphicsRootSignature(d3d12_root_signature.get());
        ID3D12DescriptorHeap* descriptor_heaps[] = {d3d12_cbv_srv_uav_heap.get()};
        d3d12_command_list->SetDescriptorHeaps(_countof(descriptor_heaps), descriptor_heaps);
        d3d12_command_list->SetGraphicsRootDescriptorTable(0, cbv_gpu_descriptor_handle);
        d3d12_command_list->SetGraphicsRootDescriptorTable(1, srv_gpu_descriptor_handle);
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
        d3d12_command_list->IASetIndexBuffer(&index_buffer_view);
        d3d12_command_list->DrawIndexedInstanced(index_count, 1, 0, 0, 0);

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
        debug_device->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL);
    }
#endif

    return 0;
}
