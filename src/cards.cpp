#include "cards.hpp"

namespace fb::cards {

Cards::Cards(Dx& dx, const Params& params) :
    root_signature(dx, Cards::NAME),
    descriptors(dx, Cards::NAME),
    samplers(dx, descriptors) {
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
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {
            .pRootSignature = root_signature.get(),
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
        };
        FAIL_FAST_IF_FAILED(
            dx.device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipeline_state)));
        dx_set_name(pipeline_state, dx_name(Cards::NAME, "Pipeline State"));
    }

    // Constant buffer.
    {
        constant_buffer.create(dx, 1, dx_name(Cards::NAME, "Constant Buffer"));

        auto cbv_desc = constant_buffer.cbv_desc();
        dx.device->CreateConstantBufferView(&cbv_desc, constant_buffer_descriptor.cpu());
    }

    // Cards buffer.
    {
        card_buffer.create(dx, CARD_COUNT, dx_name(Cards::NAME, "Cards Buffer"));

        auto srv_desc = card_buffer.srv_desc();
        dx.device->CreateShaderResourceView(
            card_buffer.resource(),
            &srv_desc,
            card_buffer_descriptor.cpu());

        Card* cards = card_buffer.ptr();
        cards[0] = {{0.0f, 0.0f}, {640.0f, 400.0f}};
        cards[1] = {{640.0f, 0.0f}, {640.0f, 400.0f}};
        cards[2] = {{0.0f, 400.0f}, {640.0f, 400.0f}};
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

        vertex_buffer.create(dx, vertex_count, dx_name(Cards::NAME, "Vertex Buffer"));
        index_buffer.create(dx, index_count, dx_name(Cards::NAME, "Index Buffer"));

        memcpy(vertex_buffer.ptr(), vertices, sizeof(vertices));
        memcpy(index_buffer.ptr(), indices, sizeof(indices));

        auto srv_desc = vertex_buffer.srv_desc();
        dx.device->CreateShaderResourceView(
            vertex_buffer.resource(),
            &srv_desc,
            vertex_buffer_descriptor.cpu());
    }

    // Textures.
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC src_desc = {
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
            .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
            .Texture2D = D3D12_TEX2D_SRV {.MipLevels = 1},
        };
        for (uint32_t i = 0; i < CARD_COUNT; i++) {
            dx.device->CreateShaderResourceView(
                params.card_textures[i].get().get(),
                &src_desc,
                card_texture_descriptors[i].cpu());
        }
    }
}

void Cards::update(const Dx& dx) {
    float width = (float)dx.swapchain_width;
    float height = (float)dx.swapchain_height;
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

void Cards::render(Dx& dx) {
    auto* cmd = dx.command_list.get();

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
    cmd->RSSetViewports(1, &viewport);
    cmd->RSSetScissorRects(1, &scissor);

    auto ibv = index_buffer.index_buffer_view();
    auto index_count = index_buffer.element_size();

    ID3D12DescriptorHeap* heaps[] = {
        descriptors.cbv_srv_uav().heap(),
        descriptors.sampler().heap()};
    cmd->SetDescriptorHeaps(_countof(heaps), heaps);
    cmd->SetGraphicsRootSignature(root_signature.get());

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
