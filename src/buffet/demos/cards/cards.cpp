#include "cards.hpp"

namespace fb::demos::cards {

static auto layout_grid(std::span<Card> cards, uint2 window_size, uint columns) -> void {
    const uint card_count = (uint)cards.size();
    const float window_w = (float)window_size.x;
    const float window_h = (float)window_size.y;
    const float card_w = window_w / (float)columns;
    const float card_h = card_w * (window_h / window_w);
    for (uint i = 0; i < card_count; i++) {
        const float x = (float)(i % columns) * card_w;
        const float y = (float)(i / columns) * card_h;
        cards[i].position = {x, y};
        cards[i].size = {card_w, card_h};
    }
}

static auto layout_hmosaic(std::span<Card> cards, uint2 window_size) -> void {
    const uint card_count = (uint)cards.size();
    const float window_w = (float)window_size.x;
    const float window_h = (float)window_size.y;
    const float thumb_w = window_w / (float)(card_count - 1);
    const float thumb_h = thumb_w * (window_h / window_w);
    const float hero_h = window_h - thumb_h;
    const float hero_w = hero_h * (window_w / window_h);
    const float hero_y = thumb_h;
    const float hero_x = window_w * 0.5f - hero_w * 0.5f;
    for (uint i = 0; i < card_count - 1; i++) {
        cards[i].position = {thumb_w * (float)i, 0.0f};
        cards[i].size = {thumb_w, thumb_h};
    }
    cards[card_count - 1].position = {hero_x, hero_y};
    cards[card_count - 1].size = {hero_w, hero_h};
}

static auto layout_vmosaic(std::span<Card> cards, uint2 window_size) -> void {
    const uint card_count = (uint)cards.size();
    const float window_w = (float)window_size.x;
    const float window_h = (float)window_size.y;
    const float thumb_h = window_h / (float)(card_count - 1);
    const float thumb_w = thumb_h * (window_w / window_h);
    const float hero_w = window_w - thumb_w;
    const float hero_h = hero_w * (window_h / window_w);
    const float hero_x = thumb_w;
    const float hero_y = window_h * 0.5f - hero_h * 0.5f;
    for (uint i = 0; i < card_count - 1; i++) {
        cards[i].position = {0.0f, thumb_h * (float)i};
        cards[i].size = {thumb_w, thumb_h};
    }
    cards[card_count - 1].position = {hero_x, hero_y};
    cards[card_count - 1].size = {hero_w, hero_h};
}

static auto layout_exclusive(std::span<Card> cards, uint2 window_size) -> void {
    for (auto& card : cards) {
        card.position = {0.0f, 0.0f};
        card.size = {(float)window_size.x, (float)window_size.y};
    }
}

auto create(Demo& demo, const CreateDesc& desc) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Create", NAME.data());
    DebugScope debug(NAME);

    // Unpack.
    const auto& kitchen_shaders = desc.baked.kitchen.shaders;
    const auto& shaders = desc.baked.buffet.shaders;
    auto& device = desc.device;

    // Descriptors.
    for (uint i = 0; i < CARD_COUNT; i++) {
        const auto& color = desc.render_targets[i].get().color();
        demo.card_descriptors[i] = CardDescriptors {
            .src = color.srv_descriptor().index(),
            .mid = color.uav_descriptor().index() + 6,
            .dst_begin = color.uav_descriptor().index(),
        };
    }

    // Pipeline.
    GpuPipelineBuilder()
        .vertex_shader(shaders.cards_draw_vs())
        .pixel_shader(shaders.cards_draw_ps())
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .render_target_formats({device.swapchain().format()})
        .depth_stencil(GpuDepthStencilDesc {
            .depth_read = false,
            .depth_write = false,
        })
        .build(device, demo.pipeline, debug.with_name("Pipeline"));

    // Constants.
    demo.constants.create(device, 1, debug.with_name("Constants"));

    // Cards.
    demo.cards.create(device, CARD_COUNT, debug.with_name("Cards"));

    // Default card layout.
    for (uint i = 0; i < demo.cards.buffer_count(); i++) {
        layout_hmosaic(demo.cards.buffer(i).span(), device.swapchain().size());
    }

    // Geometry.
    {
        const auto vertices = std::to_array<Vertex>({
            {{0.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.0f, 1.0f}, {0.0f, 1.0f}},
            {{1.0f, 1.0f}, {1.0f, 1.0f}},
            {{1.0f, 0.0f}, {1.0f, 0.0f}},
        });
        const auto indices = std::to_array<uint16_t>({0, 1, 2, 0, 2, 3});
        demo.vertices.create_with_data(device, vertices, debug.with_name("Vertices"));
        demo.indices.create_with_data(device, indices, debug.with_name("Indices"));
    }

    // Default card indices.
    for (uint i = 0; i < CARD_COUNT; i++) {
        demo.parameters.card_indirect_indices[i] = i;
    }

    // Single pass downsampler.
    {
        DebugScope spd_debug("Spd");

        const auto width = device.swapchain().size().x;
        const auto height = device.swapchain().size().y;
        const auto mip_count = mip_count_from_size(width, height);
        const auto end_index_x = (width - 1u) / 64u;
        const auto end_index_y = (height - 1u) / 64u;
        const auto threadgroup_count_x = end_index_x + 1u;
        const auto threadgroup_count_y = end_index_y + 1u;
        const auto threadgroup_count = threadgroup_count_x * threadgroup_count_y;
        const auto inv_texture_size = float2(1.0f / (float)width, 1.0f / (float)height);
        demo.spd_dispatch = uint3(threadgroup_count_x, threadgroup_count_y, 1);

        // Constants.
        demo.spd_constants.create(device, 1, spd_debug.with_name("Constants"));
        demo.spd_constants.ref() = spd::Constants {
            .mip_count = mip_count,
            .threadgroup_count = threadgroup_count,
            .inv_texture_size = inv_texture_size,
        };

        // Atomics.
        spd::Atomics atomics = {};
        demo.spd_atomics.create_and_transfer(
            device,
            std::span(&atomics, 1),
            D3D12_BARRIER_SYNC_COMPUTE_SHADING,
            D3D12_BARRIER_ACCESS_UNORDERED_ACCESS,
            spd_debug.with_name("Slice Atomics")
        );

        // Pipeline.
        GpuPipelineBuilder()
            .compute_shader(kitchen_shaders.spd_downsample_cs())
            .build(device, demo.spd_pipeline, spd_debug.with_name("Pipeline"));
    }
}

auto gui(Demo& demo, const GuiDesc& desc) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Gui", NAME.data());
    auto& params = demo.parameters;
    auto cards = demo.cards.buffer(desc.frame_index).span();
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
    ImGui::SameLine();
    if (ImGui::Button("Exclusive")) {
        layout_exclusive(cards, desc.window_size);
    }
    if (ImGui::Button("Rotate Left")) {
        std::rotate(
            params.card_indirect_indices.begin(),
            params.card_indirect_indices.begin() + 1,
            params.card_indirect_indices.end()
        );
    }
    ImGui::SameLine();
    if (ImGui::Button("Rotate Right")) {
        std::rotate(
            params.card_indirect_indices.rbegin(),
            params.card_indirect_indices.rbegin() + 1,
            params.card_indirect_indices.rend()
        );
    }
}

auto update(Demo& demo, const UpdateDesc& desc) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Update", NAME.data());
    const auto width = (float)desc.window_size.x;
    const auto height = (float)desc.window_size.y;
    const auto projection =
        float4x4::CreateOrthographicOffCenter(0.0f, width, height, 0.0f, 0.0f, 1.0f);
    demo.constants.buffer(desc.frame_index).ref() = Constants {
        .transform = projection,
    };
}

auto render(Demo& demo, const RenderDesc& desc) -> void {
    auto& [cmd, device, frame_index] = desc;
    cmd.begin_pix("%s - Render", NAME.data());

    cmd.compute_scope([&demo, frame_index](GpuComputeCommandList& cmd) {
        cmd.begin_pix("Spd");
        cmd.set_pipeline(demo.spd_pipeline);
        for (uint i = 0; i < CARD_COUNT; i++) {
            const auto& card = demo.card_descriptors[i];
            cmd.set_constants(spd::Bindings {
                .constants = demo.spd_constants.cbv_descriptor().index(),
                .atomics = demo.spd_atomics.uav_descriptor().index(),
                .texture_src = card.src,
                .texture_mid = card.mid,
                .texture_dst_begin = card.dst_begin,
            });
            cmd.dispatch(demo.spd_dispatch.x, demo.spd_dispatch.y, demo.spd_dispatch.z);
        }
        cmd.end_pix();
    });

    cmd.graphics_scope([&demo, &device, frame_index](GpuGraphicsCommandList& cmd) {
        cmd.begin_pix("Render");
        const auto& params = demo.parameters;
        cmd.set_pipeline(demo.pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(demo.indices.index_buffer_view());
        for (uint i = 0; i < CARD_COUNT; ++i) {
            const auto card_indirect = params.card_indirect_indices[i];
            cmd.set_constants(Bindings {
                .card_index = i,
                .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
                .cards = demo.cards.buffer(frame_index).srv_descriptor().index(),
                .vertices = demo.vertices.srv_descriptor().index(),
                .texture = demo.card_descriptors[card_indirect].src,
            });
            cmd.draw_indexed_instanced(demo.indices.element_count(), 1, 0, 0, 0);
        }
        cmd.end_pix();
    });
    cmd.end_pix();
}

} // namespace fb::demos::cards
