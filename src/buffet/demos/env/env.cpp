#include "env.hpp"

namespace fb::demos::env {

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

    // Pbr.
    {
        const auto lut = assets.winter_evening_lut();
        const auto irr = assets.winter_evening_irr();
        const auto rad = assets.winter_evening_rad();

        demo.pbr.lut.create_and_transfer_baked(
            device,
            lut,
            D3D12_BARRIER_SYNC_PIXEL_SHADING,
            D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
            D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE,
            debug.with_name("LUT")
        );
        demo.pbr.irr.create_and_transfer_baked(
            device,
            irr,
            D3D12_BARRIER_SYNC_PIXEL_SHADING,
            D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
            D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE,
            debug.with_name("Irradiance")
        );
        demo.pbr.rad.create_and_transfer_baked(
            device,
            rad,
            D3D12_BARRIER_SYNC_PIXEL_SHADING,
            D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
            D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE,
            debug.with_name("Radiance")
        );
    }

    // Background.
    {
        DebugScope pass_debug("Background");
        auto& pass = demo.background;

        // Constants.
        pass.constants.create(device, 1, pass_debug.with_name("Constants"));

        // Geometry.
        const auto mesh = assets.skybox_mesh();
        pass.vertices.create_and_transfer(
            device,
            mesh.vertices,
            D3D12_BARRIER_SYNC_VERTEX_SHADING,
            D3D12_BARRIER_ACCESS_VERTEX_BUFFER,
            pass_debug.with_name("Vertices")
        );
        pass.indices.create_and_transfer(
            device,
            mesh.indices,
            D3D12_BARRIER_SYNC_INDEX_INPUT,
            D3D12_BARRIER_ACCESS_INDEX_BUFFER,
            pass_debug.with_name("Indices")
        );

        // Pipeline.
        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(shaders.env_background_vs())
            .pixel_shader(shaders.env_background_ps())
            .depth_stencil(GpuDepthStencilDesc {
                .depth_read = true,
                .depth_write = true,
                .depth_func = GpuComparisonFunc::LessEqual,
            })
            .render_target_formats({demo.render_targets.color_format()})
            .depth_stencil_format(demo.render_targets.depth_format())
            .sample_desc(demo.render_targets.sample_desc())
            .build(device, pass.pipeline, pass_debug.with_name("Pipeline"));
    }

    // Model.
    {
        DebugScope pass_debug("Model");
        auto& pass = demo.model;

        // Constants.
        pass.constants.create(device, 1, pass_debug.with_name("Constants"));

        // Geometry.
        const auto mesh = assets.sphere_mesh();
        pass.vertices.create_and_transfer(
            device,
            mesh.vertices,
            D3D12_BARRIER_SYNC_VERTEX_SHADING,
            D3D12_BARRIER_ACCESS_VERTEX_BUFFER,
            pass_debug.with_name("Vertices")
        );
        pass.indices.create_and_transfer(
            device,
            mesh.indices,
            D3D12_BARRIER_SYNC_INDEX_INPUT,
            D3D12_BARRIER_ACCESS_INDEX_BUFFER,
            pass_debug.with_name("Indices")
        );

        // Pipeline.
        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(shaders.env_model_vs())
            .pixel_shader(shaders.env_model_ps())
            .depth_stencil(GpuDepthStencilDesc {
                .depth_read = true,
                .depth_write = true,
                .depth_func = GpuComparisonFunc::LessEqual,
            })
            .render_target_formats({demo.render_targets.color_format()})
            .depth_stencil_format(demo.render_targets.depth_format())
            .sample_desc(demo.render_targets.sample_desc())
            .build(device, pass.pipeline, pass_debug.with_name("Pipeline"));
    }
}

auto gui(Demo& demo, const GuiDesc&) -> void {
    ZoneScoped;
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Gui", NAME.data());
    auto& params = demo.parameters;
    ImGui::SliderFloat("Model Metallic", &params.model_metallic, 0.0f, 1.0f);
    ImGui::SliderFloat("Model Roughness", &params.model_roughness, 0.0f, 1.0f);
    ImGui::Checkbox("Tonemap", (bool*)&params.tonemap);
    ImGui::SliderFloat("Exposure", &params.exposure, -10.0f, 10.0f);
    ImGui::SliderFloat("BG Roughness", &params.background_roughness, 0.0f, 1.0f);
    ImGui::SliderAngle("Camera FOV", &params.camera_fov, 1.0f, 90.0f);
    ImGui::SliderFloat("Camera Distance", &params.camera_distance, 1.0f, 10.0f);
    ImGui::SliderAngle("Camera Longitude", &params.camera_longitude, -360.0f, 360.0f);
    ImGui::SliderAngle("Camera Latitude", &params.camera_latitude, -90.0f + 1.0f, 90.0f - 1.0f);
    ImGui::SliderFloat("Camera Rotation Speed", &params.camera_rotation_speed, 0.0f, 1.0f);
}

auto update(Demo& demo, const UpdateDesc& desc) -> void {
    ZoneScoped;
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Update", NAME.data());
    auto& params = demo.parameters;

    // Update camera rotation.
    {
        params.camera_longitude += params.camera_rotation_speed * desc.delta_time;
        if (params.camera_longitude > PI * 2.0f) {
            params.camera_longitude -= PI * 2.0f;
        }
    }

    // Update camera transforms.
    const auto aspect_ratio = desc.aspect_ratio;
    const auto perspective =
        float4x4::CreatePerspectiveFieldOfView(params.camera_fov, aspect_ratio, 0.1f, 100.0f);
    const auto eye =
        params.camera_distance * dir_from_lonlat(params.camera_longitude, params.camera_latitude);
    const auto view = float4x4::CreateLookAt(eye, float3::Zero, float3::Up);
    const auto camera_transform = view * perspective;

    // Update exposure.
    const auto exposure = std::pow(2.0f, -params.exposure);

    // Update debug.
    {
        demo.debug_draw.begin(desc.frame_index);
        demo.debug_draw.transform(camera_transform);
        demo.debug_draw.scaled_axes(2.0f);
        demo.debug_draw.end();
    }

    // Update background constants.
    {
        auto env_view = view;
        env_view.m[3][0] = 0.0f;
        env_view.m[3][1] = 0.0f;
        env_view.m[3][2] = 0.0f;
        env_view.m[3][3] = 1.0f;
        const auto env_transform = env_view * perspective;

        demo.background.constants.buffer(desc.frame_index).ref() = BackgroundConstants {
            .transform = env_transform,
            .tonemap = params.tonemap,
            .exposure = exposure,
            .roughness = params.background_roughness,
            .mip_count = demo.pbr.rad.mip_count(),
        };
    }

    // Update model constants.
    {
        demo.model.constants.buffer(desc.frame_index).ref() = ModelConstants {
            .transform = camera_transform,
            .camera_position = eye,
            .tonemap = params.tonemap,
            .metallic = params.model_metallic,
            .roughness = params.model_roughness,
            .mip_count = demo.pbr.rad.mip_count(),
        };
    }
}

auto render(Demo& demo, const RenderDesc& desc) -> void {
    ZoneScoped;
    auto& [cmd, device, frame_index] = desc;
    cmd.graphics_scope([&demo, frame_index](GpuGraphicsCommandList& cmd) {
        cmd.pix_begin("%s - Render", NAME.data());

        // Render targets.
        demo.render_targets.set(cmd);

        // Debug.
        demo.debug_draw.render(cmd);

        // Background.
        {
            const auto& pbr = demo.pbr;
            const auto& pass = demo.background;
            cmd.pix_begin("Background");
            cmd.set_constants(BackgroundBindings {
                .constants = pass.constants.buffer(frame_index).cbv_descriptor().index(),
                .vertices = pass.vertices.srv_descriptor().index(),
                .texture = pbr.rad.srv_descriptor().index(),
            });
            cmd.set_pipeline(pass.pipeline);
            cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            cmd.set_index_buffer(pass.indices.index_buffer_view());
            cmd.draw_indexed_instanced(pass.indices.element_count(), 1, 0, 0, 0);
            cmd.pix_end();
        }

        // Model.
        {
            const auto& pbr = demo.pbr;
            const auto& pass = demo.model;
            cmd.pix_begin("Model");
            cmd.set_constants(ModelBindings {
                .constants = pass.constants.buffer(frame_index).cbv_descriptor().index(),
                .vertices = pass.vertices.srv_descriptor().index(),
                .lut_texture = pbr.lut.srv_descriptor().index(),
                .irr_texture = pbr.irr.srv_descriptor().index(),
                .rad_texture = pbr.rad.srv_descriptor().index(),
            });
            cmd.set_pipeline(pass.pipeline);
            cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            cmd.set_index_buffer(pass.indices.index_buffer_view());
            cmd.draw_indexed_instanced(pass.indices.element_count(), 1, 0, 0, 0);
            cmd.pix_end();
        }

        cmd.pix_end();
    });
}

} // namespace fb::demos::env
