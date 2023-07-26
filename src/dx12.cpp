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

void dx_shader_destroy(DxShader* shader) {
    shader->binary->Release();
}

void dxc_create(Dxc* dxc) {
    DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxc->compiler));
    DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxc->utils));
    dxc->utils->CreateDefaultIncludeHandler(&dxc->include_handler);
}

void dxc_destroy(Dxc* dxc) {
    dxc->include_handler->Release();
    dxc->utils->Release();
    dxc->compiler->Release();
}

void dxc_shader_compile(Dxc* dxc, const DxShaderDesc& desc, DxShader* shader) {
    // Note: remember to set PIX PDB search path correctly for shader debugging to work.

    // Shader profile.
    LPCWSTR shader_profile = nullptr;
    // clang-format off
    switch (desc.type) {
        case DxShaderType::Compute: shader_profile = L"cs_6_7"; break;
        case DxShaderType::Vertex: shader_profile = L"vs_6_7"; break;
        case DxShaderType::Pixel: shader_profile = L"ps_6_7"; break;
    }
    // clang-format on

    // Shader arguments.
    std::wstring shader_name = fb::to_wstr(desc.name);
    std::wstring shader_entry = fb::to_wstr(desc.entry_point);
    LPCWSTR shader_args[] = {
        // clang-format off
        shader_name.c_str(),
        L"-E", shader_entry.c_str(),
        L"-T", shader_profile,
        L"-HV", L"2021",
        L"-Zi",
        L"-Fd", L".\\shaders\\",
        L"-Qstrip_reflect",
        // clang-format on
    };

    // Compile.
    DxcBuffer source_buffer = {
        .Ptr = desc.source.data(),
        .Size = desc.source.size(),
        .Encoding = DXC_CP_ACP,
    };
    fb::ComPtr<IDxcResult> result;
    fb::ComPtr<IDxcBlobUtf8> errors;
    IDxcBlob* binary = nullptr;
    fb::ComPtr<IDxcBlob> pdb;
    fb::ComPtr<IDxcBlobUtf16> pdb_name;
    FAIL_FAST_IF_FAILED(dxc->compiler->Compile(
        &source_buffer,
        shader_args,
        _countof(shader_args),
        dxc->include_handler,
        IID_PPV_ARGS(&result)));
    FAIL_FAST_IF_FAILED(result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr));
    if (errors && errors->GetStringLength() != 0) {
        FAIL_FAST_MSG("Failed to compile %ws\n%s", shader_name.c_str(), errors->GetStringPointer());
    }
    FAIL_FAST_IF_FAILED(result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&binary), nullptr));
    FAIL_FAST_IF_FAILED(result->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&pdb), &pdb_name));

    // Write PDB.
    {
        std::wstring path;
        path.append(L"shaders\\");
        path.append(pdb_name->GetStringPointer());

        FILE* file = nullptr;
        _wfopen_s(&file, path.c_str(), L"wb");
        FAIL_FAST_IF_NULL_MSG(file, "Failed to create file %ws", path.c_str());

        fwrite(pdb->GetBufferPointer(), pdb->GetBufferSize(), 1, file);

        fclose(file);
    }

    // Result.
    shader->binary = binary;
}

void dx_create(Dx* dx, Window* window) {
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
    ID3D12Device12* device = nullptr;
    FAIL_FAST_IF_FAILED(D3D12CreateDevice(adapter.get(), MIN_FEATURE_LEVEL, IID_PPV_ARGS(&device)));
    dx_set_name(device, "Device");

    // Command queue.
    ID3D12CommandQueue* command_queue = nullptr;
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
    std::array<ID3D12CommandAllocator*, FRAME_COUNT> command_allocators = {};
    for (uint32_t i = 0; i < fb::FRAME_COUNT; i++) {
        FAIL_FAST_IF_FAILED(device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(&command_allocators[i])));
        dx_set_indexed_name(command_allocators[i], "Command Allocator", i);
    }

    // Command list.
    ID3D12GraphicsCommandList9* command_list = nullptr;
    FAIL_FAST_IF_FAILED(device->CreateCommandList1(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        D3D12_COMMAND_LIST_FLAG_NONE,
        IID_PPV_ARGS(&command_list)));
    dx_set_name(command_list, "Command List");

    // Swapchain.
    IDXGISwapChain4* swapchain = nullptr;
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
            command_queue,
            (HWND)window_handle(window),
            &desc,
            nullptr,
            nullptr,
            &swapchain1));
        swapchain1.query_to(&swapchain);
    }

    // Render target views.
    ID3D12DescriptorHeap* rtv_descriptor_heap = nullptr;
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, FRAME_COUNT> rtv_descriptors = {};
    std::array<ID3D12Resource*, fb::FRAME_COUNT> rtvs = {};
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            .NumDescriptors = FRAME_COUNT,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
            .NodeMask = 0,
        };
        FAIL_FAST_IF_FAILED(
            device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtv_descriptor_heap)));
        dx_set_name(rtv_descriptor_heap, "RTV Descriptor Heap");

        uint32_t rtv_descriptor_size =
            device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtv_descriptor_handle =
            rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        for (uint32_t i = 0; i < fb::FRAME_COUNT; i++) {
            FAIL_FAST_IF_FAILED(swapchain->GetBuffer(i, IID_PPV_ARGS(&rtvs[i])));
            dx_set_indexed_name(rtvs[i], "RTV", i);

            device->CreateRenderTargetView(rtvs[i], nullptr, rtv_descriptor_handle);
            rtv_descriptors[i] = rtv_descriptor_handle;
            rtv_descriptor_handle.ptr += rtv_descriptor_size;
        }
    }

    // Synchronization.
    uint32_t frame_index = 0;
    ID3D12Fence1* fence = nullptr;
    HANDLE fence_event = nullptr;
    std::array<uint64_t, FRAME_COUNT> fence_values = {};
    {
        frame_index = swapchain->GetCurrentBackBufferIndex();
        FAIL_FAST_IF_FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
        dx_set_name(fence, "Fence");
        fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        FAIL_FAST_IF_NULL_MSG(fence_event, "Failed to create fence event.");
    }

    // Result.
    dx->device = device;
    dx->command_queue = command_queue;
    dx->command_allocators = command_allocators;
    dx->command_list = command_list;
    dx->swapchain = swapchain;
    dx->rtv_descriptor_heap = rtv_descriptor_heap;
    dx->rtv_descriptors = rtv_descriptors;
    dx->rtvs = rtvs;
    dx->frame_index = frame_index;
    dx->fence = fence;
    dx->fence_event = fence_event;
    dx->fence_values = fence_values;
}

void dx_destroy(Dx* dx) {
    // Detect resource leaks.
#if defined(_DEBUG)
    {
        fb::ComPtr<ID3D12DebugDevice2> debug_device;
        dx->device->QueryInterface(IID_PPV_ARGS(&debug_device));
        debug_device->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL);
    }
#endif

    // Release resources.
    CloseHandle(dx->fence_event);
    for (auto rtv : dx->rtvs) {
        rtv->Release();
    }
    dx->rtv_descriptor_heap->Release();
    dx->swapchain->Release();
    dx->command_list->Release();
    for (auto allocator : dx->command_allocators) {
        allocator->Release();
    }
    dx->command_queue->Release();
    dx->device->Release();
}

void dx_set_name(ID3D12Object* object, const char* name) {
#if defined(_DEBUG)
    std::wstring wname = fb::to_wstr(name);
    object->SetName(wname.c_str());
#else
    (void)object;
    (void)name;
#endif
}

void dx_set_indexed_name(ID3D12Object* object, const char* name, uint32_t index) {
#if defined(_DEBUG)
    std::wstring wname = fb::to_wstr(name);
    std::wstring windexed_name = std::format(L"{}[{}]", wname, index);
    object->SetName(windexed_name.c_str());
#else
    (void)object;
    (void)name;
    (void)index;
#endif
}

}  // namespace fb
