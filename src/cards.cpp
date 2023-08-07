#include "cards.hpp"

namespace fb::cards {

Cards::Cards(GpuDevice& device, const Params& params) :
    descriptors(device, Cards::NAME),
    samplers(device, descriptors) {
    // Descriptors.
    {
        constant_buffer_descriptor = descriptors.cbv_srv_uav().alloc();
        card_buffer_descriptor = descriptors.cbv_srv_uav().alloc();
        vertex_buffer_descriptor = descriptors.cbv_srv_uav().alloc();
        for (auto& card_texture_descriptor : card_texture_descriptors) {
            card_texture_descriptor = descriptors.cbv_srv_uav().alloc();
        }
    }

    // Shaders.
    GpuShader vertex_shader;
    GpuShader pixel_shader;
    {
        GpuShaderCompiler sc;
        auto source = read_whole_file("shaders/cards.hlsl");
        vertex_shader = sc.compile(Cards::NAME, GpuShaderType::Vertex, "vs_main", source);
        pixel_shader = sc.compile(Cards::NAME, GpuShaderType::Pixel, "ps_main", source);
    }

    // Pipeline state.
    pipeline_state = device.create_graphics_pipeline_state(
        D3D12_GRAPHICS_PIPELINE_STATE_DESC {
            .pRootSignature = device.root_signature(),
            .VS = vertex_shader.bytecode(),
            .PS = pixel_shader.bytecode(),
            .BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            .SampleMask = UINT_MAX,
            .RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            .DepthStencilState = DirectX::DX12::CommonStates::DepthNone,
            .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            .NumRenderTargets = 1,
            .RTVFormats = {DXGI_FORMAT_R8G8B8A8_UNORM},
            .SampleDesc = {.Count = 1, .Quality = 0},
        },
        dx_name(Cards::NAME, "Pipeline State"));

    // Constant buffer.
    {
        constant_buffer.create(device, 1, dx_name(Cards::NAME, "Constant Buffer"));
        device.create_constant_buffer_view(
            constant_buffer.cbv_desc(),
            constant_buffer_descriptor.cpu());
    }

    // Cards buffer.
    {
        card_buffer.create(device, CARD_COUNT, dx_name(Cards::NAME, "Cards Buffer"));
        device.create_shader_resource_view(
            card_buffer.resource(),
            card_buffer.srv_desc(),
            card_buffer_descriptor.cpu());

        Card* cards = card_buffer.ptr();
        cards[0] = {{0.0f, 0.0f}, {640.0f, 400.0f}};
        cards[1] = {{640.0f, 0.0f}, {640.0f, 400.0f}};
        cards[2] = {{0.0f, 400.0f}, {640.0f, 400.0f}};
        cards[3] = {{640.0f, 400.0f}, {640.0f, 400.0f}};
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
        uint32_t vertex_count = (uint32_t)_countof(vertices);
        uint32_t index_count = (uint32_t)_countof(indices);

        vertex_buffer.create(device, vertex_count, dx_name(Cards::NAME, "Vertex Buffer"));
        index_buffer.create(device, index_count, dx_name(Cards::NAME, "Index Buffer"));

        memcpy(vertex_buffer.ptr(), vertices, sizeof(vertices));
        memcpy(index_buffer.ptr(), indices, sizeof(indices));

        device.create_shader_resource_view(
            vertex_buffer.resource(),
            vertex_buffer.srv_desc(),
            vertex_buffer_descriptor.cpu());
    }

    // Textures.
    for (uint32_t i = 0; i < CARD_COUNT; i++) {
        device.create_shader_resource_view(
            params.card_textures[i],
            D3D12_SHADER_RESOURCE_VIEW_DESC {
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
                .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                .Texture2D = D3D12_TEX2D_SRV {.MipLevels = 1},
            },
            card_texture_descriptors[i].cpu());
    }
}

void Cards::update(const GpuDevice& device) {
    Uint2 swapchain_size = device.swapchain_size();
    float width = (float)swapchain_size.x;
    float height = (float)swapchain_size.y;
    float max_extent = std::max(width, height);

    if (ImGui::Begin(Cards::NAME.data())) {
        Card* cards = card_buffer.ptr();
        for (uint32_t i = 0; i < CARD_COUNT; i++) {
            ImGui::SliderFloat4(
                std::format("Card {}", i).c_str(),
                (float*)&cards[i],
                -max_extent,
                max_extent);
        }
    }
    ImGui::End();

    auto& constants = *constant_buffer.ptr();
    constants.transform =
        Matrix::CreateOrthographicOffCenter(0.0f, width, height, 0.0f, 0.0f, 1.0f);
}

void Cards::render(GpuDevice& device) {
    auto* cmd = device.command_list();

    D3D12_VIEWPORT viewport = {
        .TopLeftX = 0.0f,
        .TopLeftY = 0.0f,
        .Width = (float)device.swapchain_size().x,
        .Height = (float)device.swapchain_size().y,
        .MinDepth = 0.0f,
        .MaxDepth = 1.0f,
    };
    D3D12_RECT scissor = {
        .left = 0,
        .top = 0,
        .right = (LONG)device.swapchain_size().x,
        .bottom = (LONG)device.swapchain_size().y,
    };
    cmd->RSSetViewports(1, &viewport);
    cmd->RSSetScissorRects(1, &scissor);

    auto ibv = index_buffer.index_buffer_view();
    auto index_count = index_buffer.element_size();

    ID3D12DescriptorHeap* heaps[] = {
        descriptors.cbv_srv_uav().heap(),
        descriptors.sampler().heap()};
    cmd->SetDescriptorHeaps(_countof(heaps), heaps);
    cmd->SetGraphicsRootSignature(device.root_signature());

    cmd->SetPipelineState(pipeline_state.get());
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd->IASetIndexBuffer(&ibv);

    for (uint32_t i = 0; i < CARD_COUNT; ++i) {
        GpuBindings bindings;
        bindings.push(i);
        bindings.push(constant_buffer_descriptor);
        bindings.push(card_buffer_descriptor);
        bindings.push(vertex_buffer_descriptor);
        bindings.push(card_texture_descriptors[i]);
        cmd->SetGraphicsRoot32BitConstants(0, bindings.capacity(), bindings.ptr(), 0);
        cmd->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
    }
}

}  // namespace fb::cards
