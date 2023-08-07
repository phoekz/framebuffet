#include "cards.hpp"

namespace fb::cards {

Cards::Cards(GpuDevice& device, const Params& params) :
    _descriptors(device, Cards::NAME),
    _samplers(device, _descriptors) {
    // Descriptors.
    {
        _constant_buffer_descriptor = _descriptors.cbv_srv_uav().alloc();
        _card_buffer_descriptor = _descriptors.cbv_srv_uav().alloc();
        _vertex_buffer_descriptor = _descriptors.cbv_srv_uav().alloc();
        for (auto& card_texture_descriptor : _card_texture_descriptors) {
            card_texture_descriptor = _descriptors.cbv_srv_uav().alloc();
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
    {
        _constant_buffer.create(device, 1, dx_name(Cards::NAME, "Constant Buffer"));
        device.create_constant_buffer_view(
            _constant_buffer.cbv_desc(),
            _constant_buffer_descriptor.cpu());
    }

    // Cards buffer.
    {
        _card_buffer.create(device, CARD_COUNT, dx_name(Cards::NAME, "Cards Buffer"));
        device.create_shader_resource_view(
            _card_buffer.resource(),
            _card_buffer.srv_desc(),
            _card_buffer_descriptor.cpu());

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

        device.create_shader_resource_view(
            _vertex_buffer.resource(),
            _vertex_buffer.srv_desc(),
            _vertex_buffer_descriptor.cpu());
    }

    // Textures.
    for (uint32_t i = 0; i < CARD_COUNT; i++) {
        // Todo: global descriptor heap gets rid of this.
        device.create_shader_resource_view(
            params.card_textures[i],
            D3D12_SHADER_RESOURCE_VIEW_DESC {
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
                .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                .Texture2D = D3D12_TEX2D_SRV {.MipLevels = 1},
            },
            _card_texture_descriptors[i].cpu());
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

    ID3D12DescriptorHeap* heaps[] = {
        _descriptors.cbv_srv_uav().heap(),
        _descriptors.sampler().heap()};
    cmd->SetDescriptorHeaps(_countof(heaps), heaps);
    cmd->SetGraphicsRootSignature(device.root_signature());

    cmd->SetPipelineState(_pipeline_state.get());
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd->IASetIndexBuffer(&ibv);

    for (uint32_t i = 0; i < CARD_COUNT; ++i) {
        GpuBindings bindings;
        bindings.push(i);
        bindings.push(_constant_buffer_descriptor);
        bindings.push(_card_buffer_descriptor);
        bindings.push(_vertex_buffer_descriptor);
        bindings.push(_card_texture_descriptors[i]);
        cmd->SetGraphicsRoot32BitConstants(0, bindings.capacity(), bindings.ptr(), 0);
        cmd->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
    }
}

}  // namespace fb::cards
