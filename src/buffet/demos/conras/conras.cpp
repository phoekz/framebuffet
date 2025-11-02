#include "conras.hpp"

namespace fb::demos::conras {

auto create(Demo& demo, const CreateDesc& desc) -> void {
    FB_PERF_FUNC();
    DebugScope debug(NAME);

    // Unpack.
    const auto& kitchen_shaders = desc.baked.kitchen.shaders;
    const auto& shaders = desc.baked.buffet.shaders;
    auto& device = desc.device;

    // Render targets.
    demo.render_target.create(
        device,
        {
            .size = device.swapchain().size(),
            .sample_count = SAMPLE_COUNT,
            .colors = COLOR_ATTACHMENTS,
            .depth_stencil = DEPTH_STENCIL_ATTACHMENT,
        }
    );
    demo.render_target_view.create(demo.render_target.view_desc());

    // Debug draw.
    demo.debug_draw.create(device, kitchen_shaders, demo.render_target_view);

    // Constants.
    demo.constants.create(device, 1, debug.with_name("Constants"));

    // Star.
    demo.star_indices.create_and_transfer(
        device,
        STAR_INDICES,
        D3D12_BARRIER_SYNC_INDEX_INPUT,
        D3D12_BARRIER_ACCESS_INDEX_BUFFER,
        debug.with_name("Indices")
    );

    // Texture.
    const uint BUFFER_DOWNSCALE_FACTOR = 16;
    FB_ASSERT(device.swapchain().size().x % BUFFER_DOWNSCALE_FACTOR == 0);
    FB_ASSERT(device.swapchain().size().y % BUFFER_DOWNSCALE_FACTOR == 0);
    const uint BUFFER_WIDTH = device.swapchain().size().x / BUFFER_DOWNSCALE_FACTOR;
    const uint BUFFER_HEIGHT = device.swapchain().size().y / BUFFER_DOWNSCALE_FACTOR;
    demo.raster_buffer.create(device, BUFFER_WIDTH * BUFFER_HEIGHT, debug.with_name("Raster"));
    demo.raster_buffer_size = {BUFFER_WIDTH, BUFFER_HEIGHT};

    // Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.conras_clear_vs())
        .pixel_shader(shaders.conras_clear_ps())
        .depth_stencil({.depth_read = false, .depth_write = false})
        .rasterizer({.conservative_rasterization = false})
        .sample_desc(
            DXGI_SAMPLE_DESC {
                .Count = 1,
                .Quality = 0,
            }
        )
        .build(device, demo.clear_pipeline, debug.with_name("Clear"));
    using Desc = std::tuple<uint, bool, Span<const std::byte>, GpuPipeline&>;
    for (auto [index, conservative_rasterization, pixel_shader, pipeline] : {
             Desc {0, false, shaders.conras_raster_cr_off_ps(), demo.raster_pipelines[0]},
             Desc {1, true, shaders.conras_raster_cr_on_ps(), demo.raster_pipelines[1]},
         }) {
        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(shaders.conras_raster_vs())
            .pixel_shader(pixel_shader)
            .depth_stencil({.depth_read = false, .depth_write = false})
            .rasterizer({.conservative_rasterization = conservative_rasterization})
            .sample_desc(
                DXGI_SAMPLE_DESC {
                    .Count = 1,
                    .Quality = 0,
                }
            )
            .build(device, pipeline, debug.with_name(std::format("Raster {}", index)));
    }
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.conras_display_vs())
        .pixel_shader(shaders.conras_display_ps())
        .depth_stencil({.depth_read = false, .depth_write = false})
        .render_target_formats({demo.render_target.color_format(0)})
        .sample_desc(demo.render_target.sample_desc())
        .build(device, demo.display_pipeline, debug.with_name("Display"));
}

auto gui(Demo& demo, const GuiDesc&) -> void {
    FB_PERF_FUNC();
    auto& params = demo.parameters;
    ImGui::Combo(
        "Conservative Rasterization",
        (int*)&params.conservative_rasterization,
        "Off\0Over\0Under\0OverUnder\0"
    );
}

auto update(Demo& demo, const UpdateDesc& desc) -> void {
    FB_PERF_FUNC();

    // Projection.
    const float projection_scale = 1.0f;
    const float4x4 projection = float4x4_orthographic(
        -0.5f * desc.aspect_ratio * projection_scale,
        0.5f * desc.aspect_ratio * projection_scale,
        -0.5f * projection_scale,
        0.5f * projection_scale,
        0.0f,
        1.0f
    );
    const float rotation_angle = 0.5f * desc.elapsed_time;
    const float4x4 view = float4x4_rotation_z(rotation_angle);
    const float4x4 transform = projection * view;

    // Debug draw - begin.
    demo.debug_draw.begin(desc.frame_index);
    demo.debug_draw.transform(transform);

    // Star.
    for (uint i = 0; i < STAR_TRIANGLE_COUNT; i++) {
        const uint16_t v0 = STAR_INDICES[i * 3 + 0];
        const uint16_t v1 = STAR_INDICES[i * 3 + 1];
        demo.debug_draw.line(STAR_VERTICES[v0], STAR_VERTICES[v1], {255, 255, 255, 255});

        const uint16_t i0 = STAR_INDICES[i * 3 + 1];
        const uint16_t i1 = STAR_INDICES[i * 3 + 2];
        demo.debug_draw.line(STAR_VERTICES[i0], STAR_VERTICES[i1], {255, 255, 255, 255});
    }

    // Axis.
    demo.debug_draw.line({0.0f, 0.0f, 0.0f}, {0.1f, 0.0f, 0.0f}, {255, 0, 0, 255});
    demo.debug_draw.line({0.0f, 0.0f, 0.0f}, {0.0f, 0.1f, 0.0f}, {0, 255, 0, 255});

    // Debug draw - end.
    demo.debug_draw.end();

    // Update constants.
    demo.constants.buffer(desc.frame_index).ref() = Constants {
        .transform = transform,
        .raster_buffer_size = demo.raster_buffer_size,
    };
}

auto render(Demo& demo, const RenderDesc& desc) -> void {
    FB_PERF_FUNC();
    auto& [cmd, device, frame_index] = desc;
    cmd.pix_begin("%s - Render", NAME.data());

    cmd.graphics_scope([&demo, frame_index](GpuGraphicsCommandList& cmd) {
        const uint raster_buffer_width = demo.raster_buffer_size.x;
        const uint raster_buffer_height = demo.raster_buffer_size.y;

        demo.raster_buffer.transition(
            cmd,
            D3D12_BARRIER_SYNC_PIXEL_SHADING,
            D3D12_BARRIER_ACCESS_UNORDERED_ACCESS
        );
        cmd.flush_barriers();
        cmd.set_viewport(0, 0, raster_buffer_width, raster_buffer_height);
        cmd.set_scissor(0, 0, raster_buffer_width, raster_buffer_height);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_rtvs_dsv({}, std::nullopt);

        // Clear.
        cmd.pix_begin("Clear");
        cmd.set_pipeline(demo.clear_pipeline);
        cmd.set_constants(
            Bindings {
                .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
                .raster_buffer = demo.raster_buffer.uav_descriptor().index(),
                .conservative_rasterization = ConservativeRasterization::Off,
            }
        );
        cmd.draw_instanced(3, 1, 0, 0);
        cmd.pix_end();

        // Raster.
        cmd.pix_begin("Raster");
        demo.raster_buffer.transition(
            cmd,
            D3D12_BARRIER_SYNC_PIXEL_SHADING,
            D3D12_BARRIER_ACCESS_UNORDERED_ACCESS
        );
        cmd.flush_barriers();
        const uint pipeline_index =
            demo.parameters.conservative_rasterization == ConservativeRasterization::Off ? 0 : 1;
        cmd.set_pipeline(demo.raster_pipelines[pipeline_index]);
        cmd.set_constants(
            Bindings {
                .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
                .raster_buffer = demo.raster_buffer.uav_descriptor().index(),
                .conservative_rasterization = demo.parameters.conservative_rasterization,
            }
        );
        cmd.set_index_buffer(demo.star_indices.index_buffer_view());
        cmd.draw_indexed_instanced(demo.star_indices.element_count(), 1, 0, 0, 0);
        cmd.pix_end();

        // Display.
        cmd.pix_begin("Display");
        demo.render_target_view.set_graphics(cmd);
        demo.raster_buffer.transition(
            cmd,
            D3D12_BARRIER_SYNC_PIXEL_SHADING,
            D3D12_BARRIER_ACCESS_SHADER_RESOURCE
        );
        cmd.flush_barriers();
        cmd.set_constants(
            Bindings {
                .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
                .raster_buffer = demo.raster_buffer.srv_descriptor().index(),
                .conservative_rasterization = demo.parameters.conservative_rasterization,
            }
        );
        cmd.set_pipeline(demo.display_pipeline);
        cmd.draw_instanced(3, 1, 0, 0);
        cmd.pix_end();

        demo.debug_draw.render(cmd);
    });

    cmd.pix_end();
}

} // namespace fb::demos::conras
