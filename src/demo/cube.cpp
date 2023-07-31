#include "cube.hpp"
#include "../shaders.hpp"
#include "../gltf.hpp"
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
        fb::dx_set_name(root_signature, "Cube - Root Signature");
    }

    // Shaders.
    fb::Shader vertex_shader;
    fb::Shader pixel_shader;
    {
        fb::ShaderCompiler sc;
        auto name = "cube";
        auto source = fb::read_whole_file("shaders/cube.hlsl");
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
            .DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT),
            .InputLayout = {input_element_descs, _countof(input_element_descs)},
            .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            .NumRenderTargets = 1,
            .RTVFormats = {DXGI_FORMAT_R8G8B8A8_UNORM},
            .DSVFormat = DXGI_FORMAT_D32_FLOAT,
            .SampleDesc = {.Count = 1, .Quality = 0},
        };
        FAIL_FAST_IF_FAILED(
            dx.device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipeline_state)));
        fb::dx_set_name(pipeline_state, "Cube - Pipeline State");
    }

    // Descriptor heap.
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .NumDescriptors = 2,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            .NodeMask = 0,
        };
        FAIL_FAST_IF_FAILED(dx.device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptor_heap)));
        fb::dx_set_name(descriptor_heap, "Cube - Descriptor Heap");

        uint32_t increment_size =
            dx.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        constant_buffer_descriptor = descriptor_heap->GetGPUDescriptorHandleForHeapStart();
        texture_descriptor = descriptor_heap->GetGPUDescriptorHandleForHeapStart();
        texture_descriptor.ptr += increment_size;
    }

    // Constant buffer.
    {
        constant_buffer.create_cb(
            dx,
            D3D12_HEAP_TYPE_UPLOAD,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            dx_name("Cube", "Constant Buffer"));
        memcpy(constant_buffer.ptr, &constants, sizeof(constants));

        auto cbv_desc = constant_buffer.constant_buffer_view_desc();
        dx.device->CreateConstantBufferView(
            &cbv_desc,
            descriptor_heap->GetCPUDescriptorHandleForHeapStart());
    }

    // Model.
    auto model = fb::GltfModel::load("models/stylized_crate.glb");

    // Geometry.
    {
        vertex_buffer.create_vb(
            dx,
            model.vertex_count(),
            D3D12_HEAP_TYPE_UPLOAD,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            dx_name("Cube", "Vertex Buffer"));
        index_buffer.create_ib(
            dx,
            model.index_count(),
            D3D12_HEAP_TYPE_UPLOAD,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            dx_name("Cube", "Index Buffer"));

        memcpy(vertex_buffer.ptr, model.vertex_data(), model.vertex_buffer_size());
        memcpy(index_buffer.ptr, model.index_data(), model.index_buffer_size());
    }

    // Texture.
    {
        // Create.
        const auto& image = model.base_color_texture;
        auto texture_format = DXGI_FORMAT_R8G8B8A8_UNORM;
        auto texture_desc =
            CD3DX12_RESOURCE_DESC::Tex2D(texture_format, image.width, image.height, 1, 1);
        CD3DX12_HEAP_PROPERTIES texture_heap(D3D12_HEAP_TYPE_DEFAULT);
        FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
            &texture_heap,
            D3D12_HEAP_FLAG_NONE,
            &texture_desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&texture)));
        fb::dx_set_name(texture, "Cube - Texture");

        // Upload.
        D3D12_SUBRESOURCE_DATA subresource_data = {
            .pData = image.data(),
            .RowPitch = image.row_pitch(),
            .SlicePitch = image.slice_pitch(),
        };
        DirectX::ResourceUploadBatch rub(dx.device.get());
        rub.Begin();
        rub.Upload(texture.get(), 0, &subresource_data, 1);
        rub.Transition(
            texture.get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        auto finish = rub.End(dx.command_queue.get());
        finish.wait();

        // Descriptor.
        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {
            .Format = texture_format,
            .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
            .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
            .Texture2D = D3D12_TEX2D_SRV {.MipLevels = 1},
        };
        D3D12_CPU_DESCRIPTOR_HANDLE heap_start =
            descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        heap_start.ptr +=
            dx.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        dx.device->CreateShaderResourceView(texture.get(), &srv_desc, heap_start);
    }

    // Color target.
    {
        CD3DX12_HEAP_PROPERTIES color_target_heap(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC color_target_desc = CD3DX12_RESOURCE_DESC::Tex2D(
            DXGI_FORMAT_R8G8B8A8_UNORM,
            dx.swapchain_width,
            dx.swapchain_height,
            1,
            1,
            1,
            0,
            D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
        D3D12_CLEAR_VALUE color_clear_value = {
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .Color = {0.0f, 0.0f, 0.0f, 1.0f},
        };
        FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
            &color_target_heap,
            D3D12_HEAP_FLAG_NONE,
            &color_target_desc,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            &color_clear_value,
            IID_PPV_ARGS(&color_target)));
        fb::dx_set_name(color_target, "Cube - Color Target");

        D3D12_DESCRIPTOR_HEAP_DESC descriptors_desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            .NumDescriptors = 1,
        };
        FAIL_FAST_IF_FAILED(dx.device->CreateDescriptorHeap(
            &descriptors_desc,
            IID_PPV_ARGS(&color_target_descriptor_heap)));
        fb::dx_set_name(color_target_descriptor_heap, "Cube - Color Target Descriptor Heap");
        color_target_descriptor =
            color_target_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        dx.device->CreateRenderTargetView(color_target.get(), nullptr, color_target_descriptor);
    }

    // Depth target.
    {
        CD3DX12_HEAP_PROPERTIES depth_target_heap(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC depth_target_desc = CD3DX12_RESOURCE_DESC::Tex2D(
            DXGI_FORMAT_D32_FLOAT,
            dx.swapchain_width,
            dx.swapchain_height,
            1,
            1,
            1,
            0,
            D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
        D3D12_CLEAR_VALUE depth_clear_value = {
            .Format = DXGI_FORMAT_D32_FLOAT,
            .DepthStencil = {.Depth = 1.0f, .Stencil = 0}};
        FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
            &depth_target_heap,
            D3D12_HEAP_FLAG_NONE,
            &depth_target_desc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depth_clear_value,
            IID_PPV_ARGS(&depth_target)));
        fb::dx_set_name(depth_target, "Cube - Depth Target");

        D3D12_DESCRIPTOR_HEAP_DESC descriptors_desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
            .NumDescriptors = 1,
        };
        FAIL_FAST_IF_FAILED(dx.device->CreateDescriptorHeap(
            &descriptors_desc,
            IID_PPV_ARGS(&depth_target_descriptor_heap)));
        fb::dx_set_name(depth_target_descriptor_heap, "Cube - Depth Target Descriptor Heap");
        depth_target_descriptor =
            depth_target_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        dx.device->CreateDepthStencilView(depth_target.get(), nullptr, depth_target_descriptor);
    }
}

void Demo::update(const UpdateParams& params) {
    float aspect_ratio = params.aspect_ratio;
    float elapsed_time = params.elapsed_time;
    fb::Mat4x4 perspective = fb::Mat4x4::CreatePerspectiveFieldOfView(
        fb::rad_from_deg(45.0f),
        aspect_ratio,
        0.1f,
        100.0f);
    fb::Vec3 eye = fb::Vec3(4.0f * std::sin(elapsed_time), 3.0f, 4.0f * std::cos(elapsed_time));
    fb::Mat4x4 view =
        fb::Mat4x4::CreateLookAt(eye, fb::Vec3(0.0f, 0.0f, 0.0f), fb::Vec3(0.0f, 1.0f, 0.0f));
    constants.transform = view * perspective;
    memcpy(constant_buffer.ptr, &constants, sizeof(constants));
}

void Demo::render(Dx& dx) {
    constexpr float CLEAR_COLOR[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    D3D12_VIEWPORT viewport = {
        .TopLeftX = 0.0f,
        .TopLeftY = 0.0f,
        .Width = (float)dx.swapchain_width,
        .Height = (float)dx.swapchain_height,
        .MinDepth = 0.0f,
        .MaxDepth = 1.0f,
    };
    D3D12_RECT scissor = {
        .left = 0,
        .top = 0,
        .right = (LONG)dx.swapchain_width,
        .bottom = (LONG)dx.swapchain_height,
    };
    auto vbv = vertex_buffer.vertex_buffer_view();
    auto ibv = index_buffer.index_buffer_view();
    auto index_count = index_buffer.element_size;

    auto* cmd = dx.command_list.get();
    dx.transition(
        color_target,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
    cmd->RSSetViewports(1, &viewport);
    cmd->RSSetScissorRects(1, &scissor);
    cmd->OMSetRenderTargets(1, &color_target_descriptor, FALSE, &depth_target_descriptor);
    cmd->ClearRenderTargetView(color_target_descriptor, CLEAR_COLOR, 0, nullptr);
    cmd->ClearDepthStencilView(
        depth_target_descriptor,
        D3D12_CLEAR_FLAG_DEPTH,
        1.0f,
        0,
        0,
        nullptr);
    cmd->SetGraphicsRootSignature(root_signature.get());
    cmd->SetDescriptorHeaps(1, descriptor_heap.addressof());
    cmd->SetGraphicsRootDescriptorTable(0, constant_buffer_descriptor);
    cmd->SetGraphicsRootDescriptorTable(1, texture_descriptor);
    cmd->SetPipelineState(pipeline_state.get());
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd->IASetVertexBuffers(0, 1, &vbv);
    cmd->IASetIndexBuffer(&ibv);
    cmd->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
    dx.transition(
        color_target,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

}  // namespace fb::cube
