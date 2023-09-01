#include "env.hpp"

namespace fb::demos::env {

auto EnvDemo::create(GpuDevice& device, const Baked& baked) -> void {
    DebugScope debug(NAME);

    // Render targets.
    _render_targets.create(
        device,
        {
            .size = device.swapchain().size(),
            .color_format = DXGI_FORMAT_R16G16B16A16_FLOAT,
            .clear_color = CLEAR_COLOR,
            .sample_count = 4,
        }
    );

    // Debug draw.
    _debug_draw.create(device, baked.kitchen.shaders, _render_targets);

    // Unpack.
    const auto& shaders = baked.buffet.shaders;
    const auto& assets = baked.buffet.assets;

    // Pbr.
    {
        const auto lut = assets.winter_evening_lut();
        const auto irr = assets.winter_evening_irr();
        const auto rad = assets.winter_evening_rad();

        _pbr.lut.create_and_transfer_baked(
            device,
            lut,
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            debug.with_name("LUT")
        );
        _pbr.irr.create_and_transfer_baked(
            device,
            irr,
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            debug.with_name("Irradiance")
        );
        _pbr.rad.create_and_transfer_baked(
            device,
            rad,
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            debug.with_name("Radiance")
        );
    }

    // Background.
    {
        DebugScope pass_debug("Background");
        auto& pass = _background;

        // Constants.
        pass.constants.create(device, 1, pass_debug.with_name("Constants"));

        // Geometry.
        const auto mesh = assets.skybox_mesh();
        pass.vertices.create_with_data(device, mesh.vertices, pass_debug.with_name("Vertices"));
        pass.indices.create_with_data(device, mesh.indices, pass_debug.with_name("Indices"));

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
            .render_target_formats({_render_targets.color_format()})
            .depth_stencil_format(_render_targets.depth_format())
            .sample_desc(_render_targets.sample_desc())
            .build(device, pass.pipeline, pass_debug.with_name("Pipeline"));
    }

    // Model.
    {
        DebugScope pass_debug("Model");
        auto& pass = _model;

        // Constants.
        pass.constants.create(device, 1, pass_debug.with_name("Constants"));

        // Geometry.
        const auto mesh = assets.sphere_mesh();
        pass.vertices.create_with_data(device, mesh.vertices, pass_debug.with_name("Vertices"));
        pass.indices.create_with_data(device, mesh.indices, pass_debug.with_name("Indices"));

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
            .render_target_formats({_render_targets.color_format()})
            .depth_stencil_format(_render_targets.depth_format())
            .sample_desc(_render_targets.sample_desc())
            .build(device, pass.pipeline, pass_debug.with_name("Pipeline"));
    }
}

auto EnvDemo::gui(const GuiDesc&) -> void {
    auto& p = _parameters;
    ImGui::SliderFloat("Model Metallic", &p.model_metallic, 0.0f, 1.0f);
    ImGui::SliderFloat("Model Roughness", &p.model_roughness, 0.0f, 1.0f);
    ImGui::Checkbox("Tonemap", (bool*)&p.tonemap);
    ImGui::SliderFloat("Exposure", &p.exposure, -10.0f, 10.0f);
    ImGui::SliderFloat("BG Roughness", &p.background_roughness, 0.0f, 1.0f);
    ImGui::SliderAngle("Camera FOV", &p.camera_fov, 1.0f, 90.0f);
    ImGui::SliderFloat("Camera Distance", &p.camera_distance, 1.0f, 10.0f);
    ImGui::SliderAngle("Camera Longitude", &p.camera_longitude, -360.0f, 360.0f);
    ImGui::SliderAngle("Camera Latitude", &p.camera_latitude, -90.0f + 1.0f, 90.0f - 1.0f);
    ImGui::SliderFloat("Camera Rotation Speed", &p.camera_rotation_speed, 0.0f, 1.0f);
}

auto EnvDemo::update(const UpdateDesc& desc) -> void {
    // Update camera rotation.
    {
        auto& p = _parameters;
        p.camera_longitude += p.camera_rotation_speed * desc.delta_time;
        if (p.camera_longitude > PI * 2.0f) {
            p.camera_longitude -= PI * 2.0f;
        }
    }

    // Update camera transforms.
    const auto& p = _parameters;
    const auto aspect_ratio = desc.aspect_ratio;
    const auto perspective =
        Float4x4::CreatePerspectiveFieldOfView(p.camera_fov, aspect_ratio, 0.1f, 100.0f);
    const auto eye = p.camera_distance * dir_from_lonlat(p.camera_longitude, p.camera_latitude);
    const auto view = Float4x4::CreateLookAt(eye, Float3::Zero, Float3::Up);
    const auto camera_transform = view * perspective;

    // Update exposure.
    const auto exposure = std::pow(2.0f, -p.exposure);

    // Update debug.
    {
        _debug_draw.begin(desc.frame_index);
        _debug_draw.transform(camera_transform);
        _debug_draw.scaled_axes(2.0f);
        _debug_draw.end();
    }

    // Update background constants.
    {
        auto env_view = view;
        env_view.m[3][0] = 0.0f;
        env_view.m[3][1] = 0.0f;
        env_view.m[3][2] = 0.0f;
        env_view.m[3][3] = 1.0f;
        const auto env_transform = env_view * perspective;

        *_background.constants.ptr() = BackgroundConstants {
            .transform = env_transform,
            .tonemap = p.tonemap,
            .exposure = exposure,
            .roughness = p.background_roughness,
            .mip_count = _pbr.rad.mip_count(),
        };
    }

    // Update model constants.
    {
        *_model.constants.ptr() = ModelConstants {
            .transform = camera_transform,
            .camera_position = eye,
            .tonemap = p.tonemap,
            .metallic = p.model_metallic,
            .roughness = p.model_roughness,
            .mip_count = _pbr.rad.mip_count(),
        };
    }
}

auto EnvDemo::render(GpuDevice& device, GpuCommandList& cmd) -> void {
    // Graphics passes.
    cmd.set_graphics();
    _render_targets.set(cmd);

    // Debug.
    _debug_draw.render(device, cmd);

    // Background.
    {
        const auto& pbr = _pbr;
        const auto& pass = _background;
        cmd.set_graphics_constants(BackgroundBindings {
            .constants = pass.constants.cbv_descriptor().index(),
            .vertices = pass.vertices.srv_descriptor().index(),
            .texture = pbr.rad.srv_descriptor().index(),
        });
        cmd.set_pipeline(pass.pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(pass.indices.index_buffer_view());
        cmd.draw_indexed_instanced(pass.indices.element_size(), 1, 0, 0, 0);
    }

    // Model.
    {
        const auto& pbr = _pbr;
        const auto& pass = _model;
        cmd.set_graphics_constants(ModelBindings {
            .constants = pass.constants.cbv_descriptor().index(),
            .vertices = pass.vertices.srv_descriptor().index(),
            .lut_texture = pbr.lut.srv_descriptor().index(),
            .irr_texture = pbr.irr.srv_descriptor().index(),
            .rad_texture = pbr.rad.srv_descriptor().index(),
        });
        cmd.set_pipeline(pass.pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(pass.indices.index_buffer_view());
        cmd.draw_indexed_instanced(pass.indices.element_size(), 1, 0, 0, 0);
    }
}

} // namespace fb::demos::env
