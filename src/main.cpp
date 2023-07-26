// Framebuffet
#include "maths.hpp"
#include "utils.hpp"
#include "win32.hpp"
#include "dx12.hpp"
#include "gui.hpp"

// DirectX.
#include <d3dx12/d3dx12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

// DirectX Shader Compiler (DXC).
#include <dxcapi.h>

// DirectXTK12.
#include <DirectXTK12/GeometricPrimitive.h>

// WinPixEventRuntime.
#define USE_PIX
#include <WinPixEventRuntime/pix3.h>

// Standard libraries.
#include <array>
#include <vector>

// Constants.
constexpr const char* WINDOW_TITLE = "framebuffet 😎";
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 800;
constexpr float WINDOW_ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
constexpr float CLEAR_COLOR[4] = {0.1f, 0.1f, 0.1f, 1.0f};

//
// D3D12 - Utilities.
//

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
#else
inline void d3d12_set_name(ID3D12Object*, LPCWSTR) {}
#endif

//
// DXC.
//

struct Dxc {
    fb::ComPtr<IDxcCompiler3> compiler;
    fb::ComPtr<IDxcUtils> utils;
    fb::ComPtr<IDxcIncludeHandler> include_handler;
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
    fb::ComPtr<IDxcBlob> binary;
    fb::ComPtr<IDxcBlob> pdb;
    fb::ComPtr<IDxcBlobUtf16> pdb_name;
    fb::ComPtr<IDxcBlob> reflection;
};

static void dxc_compile(
    Dxc& dxc,
    LPCWSTR shader_name,
    ShaderType shader_type,
    LPCWSTR shader_entry,
    ShaderResult& result) {
    // Note: remember to set PIX PDB search path correctly for shader debugging to work.

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
    fb::ComPtr<IDxcBlobEncoding> shader_blob;
    FAIL_FAST_IF_FAILED(dxc.utils->LoadFile(shader_path, nullptr, &shader_blob));
    FAIL_FAST_IF_NULL_MSG(shader_blob, "Failed to load shader file");
    DxcBuffer shader_buffer = {
        .Ptr = shader_blob->GetBufferPointer(),
        .Size = shader_blob->GetBufferSize(),
        .Encoding = DXC_CP_ACP,
    };

    // Compile.
    fb::ComPtr<IDxcResult> dxc_result;
    fb::ComPtr<IDxcBlobUtf8> dxc_errors;
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

    // Write PDB.
    {
        std::wstring s;
        s.append(L"shaders\\");
        s.append(result.pdb_name->GetStringPointer());

        FILE* file = nullptr;
        _wfopen_s(&file, s.c_str(), L"wb");
        FAIL_FAST_IF_NULL_MSG(file, "Failed to create file %ws", s.c_str());

        fwrite(result.pdb->GetBufferPointer(), result.pdb->GetBufferSize(), 1, file);

        fclose(file);
    }
}

//
// Main.
//

int main() {
    // Initialize.
    fb::ComPtr<ID3D12RootSignature> d3d12_root_signature;
    fb::ComPtr<ID3D12PipelineState> d3d12_pipeline_state;
    ShaderResult vertex_shader;
    ShaderResult pixel_shader;

    fb::ComPtr<ID3D12DescriptorHeap> d3d12_cbv_srv_uav_heap;

    fb::ComPtr<ID3D12Resource> constant_buffer;
    UINT8* cbv_data_begin = nullptr;

    fb::ComPtr<ID3D12Resource> vertex_buffer;
    fb::ComPtr<ID3D12Resource> index_buffer;
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;
    D3D12_INDEX_BUFFER_VIEW index_buffer_view;

    fb::ComPtr<ID3D12Resource> texture;

    fb::ComPtr<ID3D12DescriptorHeap> d3d12_imgui_heap;

    // Window.
    fb::Window* window = fb::window_create({
        .title = WINDOW_TITLE,
        .width = WINDOW_WIDTH,
        .height = WINDOW_HEIGHT,
    });

    // D3D12.
    fb::Dx dx;
    fb::dx_create(&dx, window);

    // Gui.
    fb::Gui* gui = fb::gui_create(window, &dx);

    // D3D12 - Root signature.
    {
        // Check support.
        constexpr D3D_ROOT_SIGNATURE_VERSION ROOT_SIGNATURE_VERSION =
            D3D_ROOT_SIGNATURE_VERSION_1_2;
        D3D12_FEATURE_DATA_ROOT_SIGNATURE feature_data = {.HighestVersion = ROOT_SIGNATURE_VERSION};
        FAIL_FAST_IF_FAILED(dx.device->CheckFeatureSupport(
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

        fb::ComPtr<ID3DBlob> signature;
        fb::ComPtr<ID3DBlob> error;
        FAIL_FAST_IF_FAILED(D3DX12SerializeVersionedRootSignature(
            &desc,
            ROOT_SIGNATURE_VERSION,
            &signature,
            &error));
        FAIL_FAST_IF_FAILED(dx.device->CreateRootSignature(
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
            dx.device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&d3d12_pipeline_state)));
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
            dx.device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&d3d12_cbv_srv_uav_heap)));
        d3d12_set_name(d3d12_cbv_srv_uav_heap.get(), L"CBV_SRV_UAV Heap");

        uint32_t increment_size =
            dx.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

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
        FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
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
        dx.device->CreateConstantBufferView(
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
            FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
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
            FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
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
    fb::ComPtr<ID3D12Resource> texture_upload_heap;
    {
        constexpr UINT TEXTURE_WIDTH = 16;
        constexpr UINT TEXTURE_HEIGHT = 16;
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
        FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
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
        FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
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
        dx.command_allocators[dx.frame_index]->Reset();
        dx.command_list->Reset(dx.command_allocators[dx.frame_index], nullptr);
        FAIL_FAST_IF(
            UpdateSubresources(
                /* pCmdList */ dx.command_list,
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
        dx.command_list->ResourceBarrier(1, &barrier);

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
            dx.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        dx.device->CreateShaderResourceView(texture.get(), &srv_desc, heap_start);
    }

    // Dear ImGui.
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .NumDescriptors = 1,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            .NodeMask = 0,
        };
        FAIL_FAST_IF_FAILED(
            dx.device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&d3d12_imgui_heap)));
        d3d12_set_name(d3d12_imgui_heap.get(), L"ImGui Heap");
    }

    // Wait for pending GPU work to complete.
    {
        dx.command_list->Close();
        ID3D12CommandList* command_lists[] = {(ID3D12CommandList*)dx.command_list};
        dx.command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

        // Schedule a Signal command in the queue.
        uint64_t* fence_value = &dx.fence_values[dx.frame_index];
        FAIL_FAST_IF_FAILED(dx.command_queue->Signal(dx.fence, *fence_value));

        // Wait until the fence has been processed.
        FAIL_FAST_IF_FAILED(dx.fence->SetEventOnCompletion(*fence_value, dx.fence_event));
        WaitForSingleObjectEx(dx.fence_event, INFINITE, FALSE);

        // Increment the fence value for the current frame.
        fence_value++;
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

        // Update Dear ImGui.
        fb::gui_update(gui);

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
        dx.command_allocators[dx.frame_index]->Reset();
        dx.command_list->Reset(dx.command_allocators[dx.frame_index], d3d12_pipeline_state.get());

        dx.command_list->SetGraphicsRootSignature(d3d12_root_signature.get());
        ID3D12DescriptorHeap* descriptor_heaps[] = {d3d12_cbv_srv_uav_heap.get()};
        dx.command_list->SetDescriptorHeaps(_countof(descriptor_heaps), descriptor_heaps);
        dx.command_list->SetGraphicsRootDescriptorTable(0, cbv_gpu_descriptor_handle);
        dx.command_list->SetGraphicsRootDescriptorTable(1, srv_gpu_descriptor_handle);
        CD3DX12_VIEWPORT viewport(0.0f, 0.0f, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);
        dx.command_list->RSSetViewports(1, &viewport);
        CD3DX12_RECT scissor_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        dx.command_list->RSSetScissorRects(1, &scissor_rect);
        dx.command_list->OMSetRenderTargets(1, &dx.rtv_descriptors[dx.frame_index], FALSE, nullptr);

        {
            auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                dx.rtvs[dx.frame_index],
                D3D12_RESOURCE_STATE_PRESENT,
                D3D12_RESOURCE_STATE_RENDER_TARGET);
            dx.command_list->ResourceBarrier(1, &barrier);
        }

        dx.command_list
            ->ClearRenderTargetView(dx.rtv_descriptors[dx.frame_index], CLEAR_COLOR, 0, nullptr);

        {
            PIXBeginEvent(dx.command_list, PIX_COLOR_DEFAULT, "Cube");
            dx.command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            dx.command_list->IASetVertexBuffers(0, 1, &vertex_buffer_view);
            dx.command_list->IASetIndexBuffer(&index_buffer_view);
            dx.command_list->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
            PIXEndEvent(dx.command_list);
        }

        {
            PIXBeginEvent(dx.command_list, PIX_COLOR_DEFAULT, "Gui");
            fb::gui_render(gui, &dx);
            PIXEndEvent(dx.command_list);
        }

        {
            auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                dx.rtvs[dx.frame_index],
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PRESENT);
            dx.command_list->ResourceBarrier(1, &barrier);
        }

        dx.command_list->Close();

        // Execute command list.
        ID3D12CommandList* command_lists[] = {(ID3D12CommandList*)dx.command_list};
        dx.command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

        // Present.
        dx.swapchain->Present(1, 0);

        // Move to next frame.
        {
            // Schedule a Signal command in the queue.
            uint64_t current_fence_value = dx.fence_values[dx.frame_index];
            FAIL_FAST_IF_FAILED(
                dx.command_queue->Signal(dx.fence, dx.fence_values[dx.frame_index]));

            // Update the frame index.
            dx.frame_index = dx.swapchain->GetCurrentBackBufferIndex();
            uint64_t* fence_value = &dx.fence_values[dx.frame_index];

            // If the next frame is not ready to be rendered yet, wait until it is ready.
            if (dx.fence->GetCompletedValue() < *fence_value) {
                FAIL_FAST_IF_FAILED(dx.fence->SetEventOnCompletion(*fence_value, dx.fence_event));
                WaitForSingleObjectEx(dx.fence_event, INFINITE, FALSE);
            }

            // Set the fence value for the next frame.
            *fence_value = current_fence_value + 1;
        }
    }

    // Wait for pending GPU work to complete.
    {
        // Schedule a Signal command in the queue.
        FAIL_FAST_IF_FAILED(dx.command_queue->Signal(dx.fence, dx.fence_values[dx.frame_index]));

        // Wait until the fence has been processed.
        FAIL_FAST_IF_FAILED(
            dx.fence->SetEventOnCompletion(dx.fence_values[dx.frame_index], dx.fence_event));
        WaitForSingleObjectEx(dx.fence_event, INFINITE, FALSE);
    }

    // Cleanup.
    fb::gui_destroy(gui);
    fb::dx_destroy(&dx);
    fb::window_destroy(window);

    return 0;
}
