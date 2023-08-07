#include "cards.hpp"

namespace fb::cards {

Cards::Cards(GpuDevice& device, const Params& params) {
    // Descriptors.
    _card_texture_descriptors = params.card_texture_descriptors;

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
    _pipeline_state = device.create_graphics_pipeline_state(
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
    _constant_buffer.create(device, 1, dx_name(Cards::NAME, "Constant Buffer"));

    // Cards buffer.
    {
        _card_buffer.create(device, CARD_COUNT, dx_name(Cards::NAME, "Cards Buffer"));

        Card* cards = _card_buffer.ptr();
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

        _vertex_buffer.create(device, vertex_count, dx_name(Cards::NAME, "Vertex Buffer"));
        _index_buffer.create(device, index_count, dx_name(Cards::NAME, "Index Buffer"));

        memcpy(_vertex_buffer.ptr(), vertices, sizeof(vertices));
        memcpy(_index_buffer.ptr(), indices, sizeof(indices));
    }
}

void Cards::update(const GpuDevice& device) {
    Uint2 swapchain_size = device.swapchain_size();
    float width = (float)swapchain_size.x;
    float height = (float)swapchain_size.y;
    float max_extent = std::max(width, height);

    if (ImGui::Begin(Cards::NAME.data())) {
        Card* cards = _card_buffer.ptr();
        for (uint32_t i = 0; i < CARD_COUNT; i++) {
            ImGui::SliderFloat4(
                std::format("Card {}", i).c_str(),
                (float*)&cards[i],
                -max_extent,
                max_extent);
        }
    }
    ImGui::End();

    auto& constants = *_constant_buffer.ptr();
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

    auto ibv = _index_buffer.index_buffer_view();
    auto index_count = _index_buffer.element_size();

    cmd->SetPipelineState(_pipeline_state.get());
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd->IASetIndexBuffer(&ibv);

    for (uint32_t i = 0; i < CARD_COUNT; ++i) {
        GpuBindings bindings;
        bindings.push(i);
        bindings.push(_constant_buffer.cbv_descriptor());
        bindings.push(_card_buffer.srv_descriptor());
        bindings.push(_vertex_buffer.srv_descriptor());
        bindings.push(_card_texture_descriptors[i]);
        device.cmd_set_graphics();
        cmd->SetGraphicsRoot32BitConstants(0, bindings.capacity(), bindings.ptr(), 0);
        cmd->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
    }
}

}  // namespace fb::cards
