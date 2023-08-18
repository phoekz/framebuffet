#include "cards.hpp"

namespace fb::demos::cards {

static auto layout_grid(std::span<Card> cards, Uint2 window_size, uint32_t columns) -> void {
    const uint32_t card_count = (uint32_t)cards.size();
    const float window_w = (float)window_size.x;
    const float window_h = (float)window_size.y;
    const float card_w = window_w / (float)columns;
    const float card_h = card_w * (window_h / window_w);
    for (uint32_t i = 0; i < card_count; i++) {
        const float x = (float)(i % columns) * card_w;
        const float y = (float)(i / columns) * card_h;
        cards[i].position = {x, y};
        cards[i].size = {card_w, card_h};
    }
}

static auto layout_hmosaic(std::span<Card> cards, Uint2 window_size) -> void {
    const uint32_t card_count = (uint32_t)cards.size();
    const float window_w = (float)window_size.x;
    const float window_h = (float)window_size.y;
    const float thumb_w = window_w / (float)(card_count - 1);
    const float thumb_h = thumb_w * (window_h / window_w);
    const float hero_h = window_h - thumb_h;
    const float hero_w = hero_h * (window_w / window_h);
    const float hero_y = thumb_h;
    const float hero_x = window_w * 0.5f - hero_w * 0.5f;
    for (uint32_t i = 0; i < card_count - 1; i++) {
        cards[i].position = {thumb_w * (float)i, 0.0f};
        cards[i].size = {thumb_w, thumb_h};
    }
    cards[card_count - 1].position = {hero_x, hero_y};
    cards[card_count - 1].size = {hero_w, hero_h};
}

static auto layout_vmosaic(std::span<Card> cards, Uint2 window_size) -> void {
    const uint32_t card_count = (uint32_t)cards.size();
    const float window_w = (float)window_size.x;
    const float window_h = (float)window_size.y;
    const float thumb_h = window_h / (float)(card_count - 1);
    const float thumb_w = thumb_h * (window_w / window_h);
    const float hero_w = window_w - thumb_w;
    const float hero_h = hero_w * (window_h / window_w);
    const float hero_x = thumb_w;
    const float hero_y = window_h * 0.5f - hero_h * 0.5f;
    for (uint32_t i = 0; i < card_count - 1; i++) {
        cards[i].position = {0.0f, thumb_h * (float)i};
        cards[i].size = {thumb_w, thumb_h};
    }
    cards[card_count - 1].position = {hero_x, hero_y};
    cards[card_count - 1].size = {hero_w, hero_h};
}

Cards::Cards(GpuDevice& device, const baked::Shaders& shaders, const Params& params) {
    // Descriptors.
    _card_texture_descriptors = params.card_texture_descriptors;

    // Pipeline.
    GpuPipelineBuilder()
        .vertex_shader(shaders.cards_draw_vs())
        .pixel_shader(shaders.cards_draw_ps())
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .render_target_formats({DXGI_FORMAT_R8G8B8A8_UNORM})
        .depth_stencil(GPU_PIPELINE_DEPTH_NONE)
        .build(device, _pipeline, dx_name(Cards::NAME, "Pipeline"));

    // Constants.
    _constants.create(device, 1, dx_name(Cards::NAME, "Constants"));

    // Cards.
    _cards.create(device, CARD_COUNT, dx_name(Cards::NAME, "Cards"));

    // Default card layout.
    layout_hmosaic(_cards.span(), device.swapchain_size());

    // Geometry.
    {
        const auto vertices = std::to_array<Vertex>({
            {{0.0f, 0.0f}, {0.0f, 0.0f}},
            {{1.0f, 0.0f}, {1.0f, 0.0f}},
            {{1.0f, 1.0f}, {1.0f, 1.0f}},
            {{0.0f, 1.0f}, {0.0f, 1.0f}},
        });
        const auto indices = std::to_array<uint16_t>({0, 1, 2, 0, 2, 3});
        _vertices.create_with_data(device, vertices, dx_name(Cards::NAME, "Vertices"));
        _indices.create_with_data(device, indices, dx_name(Cards::NAME, "Indices"));
    }

    // Card indices.
    for (uint32_t i = 0; i < CARD_COUNT; i++) {
        _card_indirect_indices[i] = i;
    }
}

auto Cards::gui(const demos::GuiDesc& desc) -> void {
    std::span<Card> cards = {_cards.ptr(), CARD_COUNT};

    if (ImGui::Button("Grid")) {
        layout_grid(cards, desc.window_size, CARD_GRID_COLUMNS);
    }
    ImGui::SameLine();
    if (ImGui::Button("HMosaic")) {
        layout_hmosaic(cards, desc.window_size);
    }
    ImGui::SameLine();
    if (ImGui::Button("VMosaic")) {
        layout_vmosaic(cards, desc.window_size);
    }
    if (ImGui::Button("Rotate Left")) {
        std::rotate(
            _card_indirect_indices.begin(),
            _card_indirect_indices.begin() + 1,
            _card_indirect_indices.end());
    }
    ImGui::SameLine();
    if (ImGui::Button("Rotate Right")) {
        std::rotate(
            _card_indirect_indices.rbegin(),
            _card_indirect_indices.rbegin() + 1,
            _card_indirect_indices.rend());
    }
}

void Cards::update(const GpuDevice& device) {
    Uint2 swapchain_size = device.swapchain_size();
    float width = (float)swapchain_size.x;
    float height = (float)swapchain_size.y;

    auto& constants = *_constants.ptr();
    constants.transform =
        Float4x4::CreateOrthographicOffCenter(0.0f, width, height, 0.0f, 0.0f, 1.0f);
}

void Cards::render(GpuDevice& device, GpuCommandList& cmd) {
    cmd.set_graphics();
    cmd.set_viewport(0, 0, device.swapchain_size().x, device.swapchain_size().y);
    cmd.set_scissor(0, 0, device.swapchain_size().x, device.swapchain_size().y);
    cmd.set_pipeline(_pipeline);
    cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd.set_index_buffer(_indices.index_buffer_view());

    for (uint32_t card_index = 0; card_index < CARD_COUNT; ++card_index) {
        uint32_t card_indirect = _card_indirect_indices[card_index];
        cmd.set_graphics_constants({
            card_index,
            _constants.cbv_descriptor().index(),
            _cards.srv_descriptor().index(),
            _vertices.srv_descriptor().index(),
            _card_texture_descriptors[card_indirect].index(),
        });
        cmd.draw_indexed_instanced(_indices.element_size(), 1, 0, 0, 0);
    }
}

} // namespace fb::demos::cards
