#include "cube.hpp"
#include "../shaders.hpp"
#include <DirectXTK12/GeometricPrimitive.h>
#include <DirectXTK12/ResourceUploadBatch.h>

namespace fb::cube {

Demo::Demo(Dx& dx) {
    // Root signature.
    {
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
            D3D_ROOT_SIGNATURE_VERSION_1_2,
            &signature,
            &error));
        FAIL_FAST_IF_FAILED(dx.device->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&root_signature)));
        fb::dx_set_name(root_signature.get(), "Cube Root Signature");
    }

    // Shaders.
    fb::Shader vertex_shader;
    fb::Shader pixel_shader;
    {
        fb::ShaderCompiler sc;
        auto source = fb::read_whole_file("shaders/triangle.hlsl");
        const char* name = "triangle";
        vertex_shader = sc.compile(name, fb::ShaderType::Vertex, "vs_main", source);
        pixel_shader = sc.compile(name, fb::ShaderType::Pixel, "ps_main", source);
    }

    // Pipeline state.
    {
        D3D12_INPUT_ELEMENT_DESC input_element_descs[] = {
            // clang-format off
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            // clang-format on
        };
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {
            .pRootSignature = root_signature.get(),
            .VS = vertex_shader.bytecode(),
            .PS = pixel_shader.bytecode(),
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
            dx.device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipeline_state)));
        fb::dx_set_name(pipeline_state.get(), "Cube Pipeline State");
    }

    // Descriptors.
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .NumDescriptors = 2,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            .NodeMask = 0,
        };
        FAIL_FAST_IF_FAILED(dx.device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptor_heap)));
        fb::dx_set_name(descriptor_heap.get(), "Cube Descriptor Heap");

        uint32_t increment_size =
            dx.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        constant_buffer_descriptor = descriptor_heap->GetGPUDescriptorHandleForHeapStart();
        texture_descriptor = descriptor_heap->GetGPUDescriptorHandleForHeapStart();
        texture_descriptor.ptr += increment_size;
    }

    // Constant buffer.
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
            descriptor_heap->GetCPUDescriptorHandleForHeapStart());

        CD3DX12_RANGE read_range(0, 0);
        FAIL_FAST_IF_FAILED(constant_buffer->Map(0, &read_range, (void**)&constant_buffer_ptr));
        memcpy(constant_buffer_ptr, &constant_buffer_data, sizeof(constant_buffer_data));
    }

    // Geometry.
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

    // Texture.
    {
        constexpr UINT TEXTURE_WIDTH = 16;
        constexpr UINT TEXTURE_HEIGHT = 16;
        constexpr UINT TEXTURE_PIXEL_SIZE = 4;
        constexpr LONG_PTR TEXTURE_ROW_PITCH = TEXTURE_WIDTH * TEXTURE_PIXEL_SIZE;
        constexpr LONG_PTR TEXTURE_SLICE_PITCH = TEXTURE_HEIGHT * TEXTURE_ROW_PITCH;
        constexpr DXGI_FORMAT TEXTURE_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

        // Create.
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

        // Texels.
        std::vector<uint8_t> texture_data;
        texture_data.resize(TEXTURE_SLICE_PITCH);
        for (uint32_t y = 0; y < TEXTURE_HEIGHT; y++) {
            for (uint32_t x = 0; x < TEXTURE_WIDTH; x++) {
                uint32_t* p =
                    (uint32_t*)&texture_data[TEXTURE_PIXEL_SIZE * (x + y * TEXTURE_WIDTH)];
                *p = ((x ^ y) & 1) == 0 ? 0xffffffff : 0xffaaaaaa;
            }
        }

        // Upload.
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

        // Descriptor.
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
            descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        heap_start.ptr +=
            dx.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        dx.device->CreateShaderResourceView(texture.get(), &srv_desc, heap_start);
    }
}

Demo::~Demo() {}

void Demo::update(const UpdateParams& params) {
    float aspect_ratio = params.aspect_ratio;
    float elapsed_time = params.elapsed_time;
    fb::Mat4x4 perspective = fb::Mat4x4::CreatePerspectiveFieldOfView(
        fb::rad_from_deg(45.0f),
        aspect_ratio,
        0.1f,
        100.0f);
    fb::Vec3 eye = fb::Vec3(2.5f * std::sin(elapsed_time), 1.5f, 2.5f * std::cos(elapsed_time));
    fb::Mat4x4 view =
        fb::Mat4x4::CreateLookAt(eye, fb::Vec3(0.0f, 0.0f, 0.0f), fb::Vec3(0.0f, 1.0f, 0.0f));
    constant_buffer_data.transform = view * perspective;
    memcpy(constant_buffer_ptr, &constant_buffer_data, sizeof(constant_buffer_data));
}

void Demo::render(ID3D12GraphicsCommandList9* cmd) {
    cmd->SetGraphicsRootSignature(root_signature.get());
    cmd->SetDescriptorHeaps(1, descriptor_heap.addressof());
    cmd->SetGraphicsRootDescriptorTable(0, constant_buffer_descriptor);
    cmd->SetGraphicsRootDescriptorTable(1, texture_descriptor);
    cmd->SetPipelineState(pipeline_state.get());
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd->IASetVertexBuffers(0, 1, &vertex_buffer_view);
    cmd->IASetIndexBuffer(&index_buffer_view);
    cmd->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
}

}  // namespace fb::cube
