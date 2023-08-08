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

    // Pipeline.
    GpuPipelineBuilder()
        .vertex_shader(vertex_shader.bytecode())
        .pixel_shader(pixel_shader.bytecode())
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .render_target_formats({DXGI_FORMAT_R8G8B8A8_UNORM})
        .depth_stencil(GPU_PIPELINE_DEPTH_NONE)
        .build(device, _pipeline, dx_name(Cards::NAME, "Pipeline"));

    // Constant buffer.
    _constant_buffer.create(device, 1, dx_name(Cards::NAME, "Constant Buffer"));

    // Cards buffer.
    {
        _card_buffer.create(device, CARD_COUNT, dx_name(Cards::NAME, "Cards Buffer"));
        auto cards = _card_buffer.span();
        cards[0] = {{0.0f, 0.0f}, {640.0f, 400.0f}};
        cards[1] = {{640.0f, 0.0f}, {640.0f, 400.0f}};
        cards[2] = {{0.0f, 400.0f}, {640.0f, 400.0f}};
        cards[3] = {{640.0f, 400.0f}, {640.0f, 400.0f}};
    }

    // Geometry.
    {
        const auto vertices = std::to_array<Vertex>({
            {{0.0f, 0.0f}, {0.0f, 0.0f}},
            {{1.0f, 0.0f}, {1.0f, 0.0f}},
            {{1.0f, 1.0f}, {1.0f, 1.0f}},
            {{0.0f, 1.0f}, {0.0f, 1.0f}},
        });
        const auto indices = std::to_array<uint16_t>({0, 1, 2, 0, 2, 3});
        _vertex_buffer.create_with_data(device, vertices, dx_name(Cards::NAME, "Vertex Buffer"));
        _index_buffer.create_with_data(device, indices, dx_name(Cards::NAME, "Index Buffer"));
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

void Cards::render(GpuDevice& device, GpuCommandList& cmd) {
    cmd.set_graphics();
    cmd.set_viewport(0, 0, device.swapchain_size().x, device.swapchain_size().y);
    cmd.set_scissor(0, 0, device.swapchain_size().x, device.swapchain_size().y);
    cmd.set_pipeline(_pipeline);
    cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd.set_index_buffer(_index_buffer.index_buffer_view());

    for (uint32_t i = 0; i < CARD_COUNT; ++i) {
        cmd.set_graphics_constants({
            i,
            _constant_buffer.cbv_descriptor().index(),
            _card_buffer.srv_descriptor().index(),
            _vertex_buffer.srv_descriptor().index(),
            _card_texture_descriptors[i].index(),
        });
        cmd.draw_indexed_instanced(_index_buffer.element_size(), 1, 0, 0, 0);
    }
}

}  // namespace fb::cards
