#include "cards.hpp"
#include "../shaders.hpp"
#include <directxtk12/CommonStates.h>
#include <imgui.h>

namespace fb::cards {

Cards::Cards(Dx& dx, const Params& params) {
    // Root signature.
    {
        CD3DX12_ROOT_PARAMETER1 root_parameters[3] = {};
        root_parameters[0].InitAsConstants(NUM_32BIT_VALUES, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
        root_parameters[1].InitAsConstantBufferView(
            1,
            0,
            D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
            D3D12_SHADER_VISIBILITY_VERTEX);

        CD3DX12_DESCRIPTOR_RANGE1 srv_range = {};
        srv_range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
        root_parameters[2].InitAsDescriptorTable(1, &srv_range, D3D12_SHADER_VISIBILITY_PIXEL);

        D3D12_STATIC_SAMPLER_DESC1 sampler_desc = {
            .Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            .AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            .AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            .AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
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

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc;
        decltype(root_signature_desc)::Init_1_2(
            root_signature_desc,
            _countof(root_parameters),
            root_parameters,
            1,
            &sampler_desc,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        fb::ComPtr<ID3DBlob> signature;
        fb::ComPtr<ID3DBlob> error;
        FAIL_FAST_IF_FAILED(D3DX12SerializeVersionedRootSignature(
            &root_signature_desc,
            D3D_ROOT_SIGNATURE_VERSION_1_2,
            &signature,
            &error));
        FAIL_FAST_IF_FAILED(dx.device->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&root_signature)));
        fb::dx_set_name(root_signature, "Cards - Root Signature");
    }

    // Shaders.
    fb::Shader vertex_shader;
    fb::Shader pixel_shader;
    {
        fb::ShaderCompiler sc;
        auto source = fb::read_whole_file("shaders/cards.hlsl");
        auto name = "cards";
        vertex_shader = sc.compile(name, fb::ShaderType::Vertex, "vs_main", source);
        pixel_shader = sc.compile(name, fb::ShaderType::Pixel, "ps_main", source);
    }

    // Pipeline state.
    {
        D3D12_INPUT_ELEMENT_DESC input_element_descs[] = {
            // clang-format off
            {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            // clang-format on
        };
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {
            .pRootSignature = root_signature.get(),
            .VS = vertex_shader.bytecode(),
            .PS = pixel_shader.bytecode(),
            .BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            .SampleMask = UINT_MAX,
            .RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            .DepthStencilState = DirectX::DX12::CommonStates::DepthNone,
            .InputLayout = {input_element_descs, _countof(input_element_descs)},
            .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            .NumRenderTargets = 1,
            .RTVFormats = {DXGI_FORMAT_R8G8B8A8_UNORM},
            .SampleDesc = {.Count = 1, .Quality = 0},
        };
        FAIL_FAST_IF_FAILED(
            dx.device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipeline_state)));
        fb::dx_set_name(pipeline_state, "Cards - Pipeline State");
    }

    // Descriptor heap.
    {
        D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .NumDescriptors = 4,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
        };
        FAIL_FAST_IF_FAILED(
            dx.device->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(&descriptor_heap)));
        fb::dx_set_name(descriptor_heap, "Cards - Descriptor Heap");
    }

    // Constant buffer.
    {
        constant_buffer.create_cb(
            dx,
            D3D12_HEAP_TYPE_UPLOAD,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            dx_name("Cards", "Constant Buffer"));
        memcpy(constant_buffer.ptr, &constants, sizeof(constants));

        auto cbv_desc = constant_buffer.constant_buffer_view_desc();
        dx.device->CreateConstantBufferView(
            &cbv_desc,
            descriptor_heap->GetCPUDescriptorHandleForHeapStart());
    }

    // Geometry.
    {
        Vertex vertices[] = {
            {{0.0f, 0.0f}, {0.0f, 0.0f}},
            {{1.0f, 0.0f}, {1.0f, 0.0f}},
            {{1.0f, 1.0f}, {1.0f, 1.0f}},
            {{0.0f, 1.0f}, {0.0f, 1.0f}},
        };
        uint16_t indices[] = {0, 1, 2, 0, 2, 3};

        vertex_buffer.create_vb(
            dx,
            (uint32_t)_countof(vertices),
            D3D12_HEAP_TYPE_UPLOAD,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            dx_name("Cards", "Vertex Buffer"));
        index_buffer.create_ib(
            dx,
            (uint32_t)_countof(indices),
            D3D12_HEAP_TYPE_UPLOAD,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            dx_name("Cards", "Index Buffer"));

        memcpy(vertex_buffer.ptr, vertices, sizeof(vertices));
        memcpy(index_buffer.ptr, indices, sizeof(indices));
    }

    // Textures.
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC src_desc = {
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
            .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
            .Texture2D = D3D12_TEX2D_SRV {.MipLevels = 1},
        };
        auto descriptor_heap_type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        auto increment_size = dx.device->GetDescriptorHandleIncrementSize(descriptor_heap_type);

        auto cpu_descriptor = descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        cpu_descriptor.ptr += increment_size;
        dx.device->CreateShaderResourceView(params.cube_texture.get(), &src_desc, cpu_descriptor);
        cpu_descriptor.ptr += increment_size;
        dx.device->CreateShaderResourceView(params.rain_texture.get(), &src_desc, cpu_descriptor);
        cpu_descriptor.ptr += increment_size;
        dx.device->CreateShaderResourceView(params.tree_texture.get(), &src_desc, cpu_descriptor);

        auto gpu_descriptor = descriptor_heap->GetGPUDescriptorHandleForHeapStart();
        gpu_descriptor.ptr += increment_size;
        cube_texture_descriptor = gpu_descriptor;
        gpu_descriptor.ptr += increment_size;
        rain_texture_descriptor = gpu_descriptor;
        gpu_descriptor.ptr += increment_size;
        tree_texture_descriptor = gpu_descriptor;
    }
}

void Cards::update(const Dx& dx) {
    float width = (float)dx.swapchain_width;
    float height = (float)dx.swapchain_height;
    float max_extent = std::max(width, height);

    if (ImGui::Begin("Cards")) {
        for (int i = 0; i < 3; i++) {
            auto& cc = card_constants[i];
            ImGui::PushID(i);
            ImGui::SliderFloat2("Position", (float*)&cc.position, -max_extent, max_extent);
            ImGui::SliderFloat2("Size", (float*)&cc.size, 0.0f, max_extent);
            ImGui::PopID();
        }
    }
    ImGui::End();

    constants.transform =
        fb::Mat4x4::CreateOrthographicOffCenter(0.0f, width, height, 0.0f, 0.0f, 1.0f);
    memcpy(constant_buffer.ptr, &constants, sizeof(constants));
}

void Cards::render(Dx& dx) {
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
    cmd->RSSetViewports(1, &viewport);
    cmd->RSSetScissorRects(1, &scissor);
    cmd->SetGraphicsRootSignature(root_signature.get());
    cmd->SetDescriptorHeaps(1, descriptor_heap.addressof());
    cmd->SetGraphicsRootConstantBufferView(1, constant_buffer.resource->GetGPUVirtualAddress());
    cmd->SetPipelineState(pipeline_state.get());
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd->IASetVertexBuffers(0, 1, &vbv);
    cmd->IASetIndexBuffer(&ibv);

    cmd->SetGraphicsRoot32BitConstants(0, NUM_32BIT_VALUES, &card_constants[0], 0);
    cmd->SetGraphicsRootDescriptorTable(2, cube_texture_descriptor);
    cmd->DrawIndexedInstanced(index_count, 1, 0, 0, 0);

    cmd->SetGraphicsRoot32BitConstants(0, NUM_32BIT_VALUES, &card_constants[1], 0);
    cmd->SetGraphicsRootDescriptorTable(2, rain_texture_descriptor);
    cmd->DrawIndexedInstanced(index_count, 1, 0, 0, 0);

    cmd->SetGraphicsRoot32BitConstants(0, NUM_32BIT_VALUES, &card_constants[2], 0);
    cmd->SetGraphicsRootDescriptorTable(2, tree_texture_descriptor);
    cmd->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
}

}  // namespace fb::cards
