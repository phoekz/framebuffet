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

// DirectXTK12.
#include <DirectXTK12/GeometricPrimitive.h>
#include <DirectXTK12/ResourceUploadBatch.h>

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

//
// Main.
//

int main() {
    // Initialize.
    fb::ComPtr<ID3D12RootSignature> d3d12_root_signature;
    fb::ComPtr<ID3D12PipelineState> d3d12_pipeline_state;
    fb::DxShader vertex_shader;
    fb::DxShader pixel_shader;

    fb::ComPtr<ID3D12DescriptorHeap> d3d12_cbv_srv_uav_heap;

    fb::ComPtr<ID3D12Resource> constant_buffer;
    UINT8* cbv_data_begin = nullptr;

    fb::ComPtr<ID3D12Resource> vertex_buffer;
    fb::ComPtr<ID3D12Resource> index_buffer;
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;
    D3D12_INDEX_BUFFER_VIEW index_buffer_view;

    fb::ComPtr<ID3D12Resource> texture;

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

        CD3DX12_DESCRIPTOR_RANGE1 cbv_range = {};
        cbv_range.Init(
            D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
            1,
            0,
            0,
            D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

        CD3DX12_DESCRIPTOR_RANGE1 srv_range = {};
        srv_range.Init(
            D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
            1,
            0,
            0,
            D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

        CD3DX12_ROOT_PARAMETER1 root_parameters[2] = {};
        root_parameters[0].InitAsDescriptorTable(1, &cbv_range, D3D12_SHADER_VISIBILITY_VERTEX);
        root_parameters[1].InitAsDescriptorTable(1, &srv_range, D3D12_SHADER_VISIBILITY_PIXEL);

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
            _countof(root_parameters),
            root_parameters,
            1,
            &sampler,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

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
        fb::dx_set_name(d3d12_root_signature.get(), "Cube Root Signature");
    }

    // DXC - Shaders.
    {
        fb::Dxc dxc;
        fb::dxc_create(&dxc);

        auto source = fb::read_whole_file("shaders/triangle.hlsl");

        fb::dxc_shader_compile(
            &dxc,
            {
                .name = "triangle",
                .type = fb::DxShaderType::Vertex,
                .entry_point = "vertex_shader",
                .source = source,
            },
            &vertex_shader);

        fb::dxc_shader_compile(
            &dxc,
            {
                .name = "triangle",
                .type = fb::DxShaderType::Pixel,
                .entry_point = "pixel_shader",
                .source = source,
            },
            &pixel_shader);

        fb::dxc_destroy(&dxc);
    }

    // D3D12 - Pipeline state.
    {
        D3D12_INPUT_ELEMENT_DESC input_element_descs[] = {
            // clang-format off
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            // clang-format on
        };
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {
            .pRootSignature = d3d12_root_signature.get(),
            .VS = fb::dx_shader_bytecode(&vertex_shader),
            .PS = fb::dx_shader_bytecode(&pixel_shader),
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
        fb::dx_set_name(d3d12_pipeline_state.get(), "Cube Pipeline State");
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
        fb::dx_set_name(d3d12_cbv_srv_uav_heap.get(), "Cube Descriptor Heap");

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
    } constant_buffer_data = {};
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
        fb::dx_set_name(constant_buffer.get(), "Cube Constant Buffer");

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
            fb::dx_set_name(vertex_buffer.get(), "Cube Vertex Buffer");
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
            fb::dx_set_name(index_buffer.get(), "Cube Index Buffer");
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
    {
        constexpr UINT TEXTURE_WIDTH = 16;
        constexpr UINT TEXTURE_HEIGHT = 16;
        constexpr UINT TEXTURE_PIXEL_SIZE = 4;
        constexpr LONG_PTR TEXTURE_ROW_PITCH = TEXTURE_WIDTH * TEXTURE_PIXEL_SIZE;
        constexpr LONG_PTR TEXTURE_SLICE_PITCH = TEXTURE_HEIGHT * TEXTURE_ROW_PITCH;
        constexpr DXGI_FORMAT TEXTURE_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

        auto texture_desc =
            CD3DX12_RESOURCE_DESC::Tex2D(TEXTURE_FORMAT, TEXTURE_WIDTH, TEXTURE_HEIGHT, 1, 1);
        CD3DX12_HEAP_PROPERTIES texture_heap(D3D12_HEAP_TYPE_DEFAULT);
        FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
            &texture_heap,
            D3D12_HEAP_FLAG_NONE,
            &texture_desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&texture)));
        fb::dx_set_name(texture.get(), "Cube Texture");

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
        DirectX::ResourceUploadBatch rub(dx.device);
        rub.Begin();
        rub.Upload(texture.get(), 0, &subresource_data, 1);
        rub.Transition(
            texture.get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        auto finish = rub.End(dx.command_queue);
        finish.wait();

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
    fb::dx_shader_destroy(&vertex_shader);
    fb::dx_shader_destroy(&pixel_shader);

    fb::gui_destroy(gui);
    fb::dx_destroy(&dx);
    fb::window_destroy(window);

    return 0;
}
