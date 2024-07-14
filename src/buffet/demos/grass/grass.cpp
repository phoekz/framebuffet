#include "grass.hpp"

namespace fb::demos::grass {

auto create(Demo& demo, const CreateDesc& desc) -> void {
    ZoneScoped;
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Create", NAME.data());
    DebugScope debug(NAME);

    // Unpack.
    const auto& kitchen_shaders = desc.baked.kitchen.shaders;
    const auto& shaders = desc.baked.buffet.shaders;
    const auto& assets = desc.baked.buffet.assets;
    auto& device = desc.device;

    // Render targets.
    demo.render_targets.create(
        device,
        {
            .size = device.swapchain().size(),
            .color_format = COLOR_FORMAT,
            .color_clear_value = COLOR_CLEAR_VALUE,
            .depth_format = DEPTH_FORMAT,
            .depth_clear_value = DEPTH_CLEAR_VALUE,
            .sample_count = SAMPLE_COUNT,
        }
    );

    // Debug draw.
    demo.debug_draw.create(device, kitchen_shaders, demo.render_targets);

    // Constants.
    demo.constants.create(device, 1, debug.with_name("Constants"));

    // Geometry.
    const auto grass = assets.grass_mesh();
    demo.vertices.create_with_data(device, grass.vertices, debug.with_name("Vertices"));
    demo.indices.create_with_data(device, grass.indices, debug.with_name("Indices"));

    // Texture.
    demo.texture.create_and_transfer_baked(
        device,
        assets.grass_base_color_texture(),
        D3D12_BARRIER_SYNC_PIXEL_SHADING,
        D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
        D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE,
        debug.with_name("Texture")
    );

    // Material.
    demo.material = assets.grass_material();

    // Pipelines.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.grass_draw_vs())
        .pixel_shader(shaders.grass_draw_naive_ps())
        .rasterizer(GpuRasterizerDesc {
            .fill_mode = GpuFillMode::Solid,
            .cull_mode = GpuCullMode::None,
        })
        .render_target_formats({demo.render_targets.color_format()})
        .depth_stencil_format(demo.render_targets.depth_format())
        .sample_desc(demo.render_targets.sample_desc())
        .build(device, demo.pipeline_naive, debug.with_name("Pipeline - Naive"));
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.grass_draw_vs())
        .pixel_shader(shaders.grass_draw_atoc_ps())
        .blend(GpuBlendDesc {
            .alpha_to_coverage_enable = true,
        })
        .rasterizer(GpuRasterizerDesc {
            .fill_mode = GpuFillMode::Solid,
            .cull_mode = GpuCullMode::None,
        })
        .render_target_formats({demo.render_targets.color_format()})
        .depth_stencil_format(demo.render_targets.depth_format())
        .sample_desc(demo.render_targets.sample_desc())
        .build(device, demo.pipeline_atoc, debug.with_name("Pipeline - AtoC"));
}

auto gui(Demo& demo, const GuiDesc&) -> void {
    ZoneScoped;
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Gui", NAME.data());
    auto& params = demo.parameters;
    ImGui::SliderFloat("Camera Distance", &params.camera_distance, 0.0f, 10.0f);
    ImGui::SliderAngle("Camera FOV", &params.camera_fov, 0.0f, 90.0f);
    ImGui::SliderAngle("Camera Latitude", &params.camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera Longitude", &params.camera_longitude, 0.0f, 360.0f);
    ImGui::SliderFloat("Camera Rotation Speed", &params.camera_rotation_speed, 0.0f, 2.0f);
    ImGui::SliderFloat("Camera Height", &params.camera_height, -1.0f, 1.0f);
    ImGui::Checkbox("Enable AtoC", &params.enable_atoc);
}

auto update(Demo& demo, const UpdateDesc& desc) -> void {
    ZoneScoped;
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Update", NAME.data());
    auto& params = demo.parameters;

    // Update camera.
    params.camera_longitude += params.camera_rotation_speed * desc.delta_time;
    if (params.camera_longitude > PI * 2.0f) {
        params.camera_longitude -= PI * 2.0f;
    }
    const auto projection =
        float4x4::CreatePerspectiveFieldOfView(params.camera_fov, desc.aspect_ratio, 0.1f, 100.0f);
    const auto eye =
        params.camera_distance * dir_from_lonlat(params.camera_longitude, params.camera_latitude)
        + float3::Up * params.camera_height;
    const auto target = float3::Up * params.camera_height;
    const auto view = float4x4::CreateLookAt(eye, target, float3::Up);
    const auto camera_transform = view * projection;

    // Update debug draw.
    demo.debug_draw.begin(desc.frame_index);
    demo.debug_draw.transform(camera_transform);
    demo.debug_draw.axes();
    demo.debug_draw.end();

    // Update constants.
    demo.constants.buffer(desc.frame_index).ref() = Constants {
        .transform = camera_transform,
        .alpha_cutoff = demo.material.alpha_cutoff,
    };
}

auto render(Demo& demo, const RenderDesc& desc) -> void {
    ZoneScoped;
    auto& [cmd, device, frame_index] = desc;
    cmd.pix_begin("%s - Render", NAME.data());

    cmd.graphics_scope([&demo, frame_index](GpuGraphicsCommandList& cmd) {
        demo.render_targets.set(cmd);

        demo.debug_draw.render(cmd);

        const GpuPipeline& pipeline =
            demo.parameters.enable_atoc ? demo.pipeline_atoc : demo.pipeline_naive;

        cmd.pix_begin("Draw");
        cmd.set_pipeline(pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_constants(Bindings {
            demo.constants.buffer(frame_index).cbv_descriptor().index(),
            demo.vertices.srv_descriptor().index(),
            demo.texture.srv_descriptor().index(),
        });
        cmd.set_index_buffer(demo.indices.index_buffer_view());
        cmd.draw_indexed_instanced(demo.indices.element_count(), 1, 0, 0, 0);
        cmd.pix_end();
    });

    cmd.pix_end();
}

} // namespace fb::demos::grass
