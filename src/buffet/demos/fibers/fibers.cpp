#include "fibers.hpp"

namespace fb::demos::fibers {

auto FibersDemo::create(
    GpuDevice& device,
    const baked::Assets& assets,
    const baked::Shaders& shaders
) -> void {
    // Render targets.
    _render_targets.create(
        device,
        {
            .size = device.swapchain().size(),
            .color_format = DXGI_FORMAT_R16G16B16A16_FLOAT,
            .clear_color = CLEAR_COLOR,
            .sample_count = 1,
        },
        NAME
    );

    // Debug draw.
    _debug_draw.create(device, shaders, _render_targets, NAME);

    // Pipelines.
    {
        GpuPipelineBuilder()
            .compute_shader(shaders.fibers_sim_cs())
            .build(device, _sim_pipeline, dx_name(NAME, "Sim", "Pipeline"));

        GpuPipelineBuilder()
            .compute_shader(shaders.fibers_reset_cs())
            .build(device, _reset_pipeline, dx_name(NAME, "Reset", "Pipeline"));

        GpuPipelineBuilder()
            .compute_shader(shaders.fibers_cull_cs())
            .build(device, _cull_pipeline, dx_name(NAME, "Cull", "Pipeline"));

        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(shaders.fibers_light_vs())
            .pixel_shader(shaders.fibers_light_ps())
            .rasterizer(GpuRasterizerDesc {
                .fill_mode = GpuFillMode::Wireframe,
            })
            .render_target_formats({_render_targets.color_format()})
            .depth_stencil_format(_render_targets.depth_format())
            .build(device, _light_pipeline, dx_name(NAME, "Light", "Pipeline"));

        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(shaders.fibers_plane_vs())
            .pixel_shader(shaders.fibers_plane_ps())
            .render_target_formats({_render_targets.color_format()})
            .depth_stencil_format(_render_targets.depth_format())
            .build(device, _plane_pipeline, dx_name(NAME, "Plane", "Pipeline"));

        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(shaders.fibers_debug_vs())
            .pixel_shader(shaders.fibers_debug_ps())
            .blend(GpuBlendDesc {
                .blend_enable = true,
                .rgb_blend_src = GpuBlend::SrcAlpha,
                .rgb_blend_dst = GpuBlend::InvSrcAlpha,
                .rgb_blend_op = GpuBlendOp::Add,
                .alpha_blend_src = GpuBlend::One,
                .alpha_blend_dst = GpuBlend::InvSrcAlpha,
                .alpha_blend_op = GpuBlendOp::Add,
            })
            .depth_stencil(GpuDepthStencilDesc {
                .depth_read = false,
                .depth_write = false,
            })
            .render_target_formats({_render_targets.color_format()})
            .build(device, _debug_pipeline, dx_name(NAME, "Debug", "Pipeline"));
    }

    // Constants.
    _constants.create(device, 1, dx_name(NAME, "Constants"));

    // Geometry.
    {
        const auto mesh = assets.light_bounds_mesh();
        _light_mesh.vertices
            .create_with_data(device, mesh.vertices, dx_name(NAME, "Light", "Vertices"));
        _light_mesh.indices
            .create_with_data(device, mesh.indices, dx_name(NAME, "Light", "Indices"));
    }
    {
        const auto vertices = std::to_array<baked::Vertex>({
            {{-PLANE_RADIUS_X, -PLANE_RADIUS_Y, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{+PLANE_RADIUS_X, -PLANE_RADIUS_Y, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            {{+PLANE_RADIUS_X, +PLANE_RADIUS_Y, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-PLANE_RADIUS_X, +PLANE_RADIUS_Y, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        });
        const auto indices = std::to_array<baked::Index>({0, 1, 2, 0, 2, 3});
        _plane_mesh.vertices.create_with_data(
            device,
            std::span(vertices.data(), vertices.size()),
            dx_name(NAME, "Plane", "Vertices")
        );
        _plane_mesh.indices.create_with_data(
            device,
            std::span(indices.data(), indices.size()),
            dx_name(NAME, "Plane", "Indices")
        );
    }

    // Lights.
    {
        // Data.
        Pcg rand;
        auto lights = std::vector<Light>(LIGHT_COUNT);
        uint32_t attempts = 0;
        for (uint32_t i = 0; i < LIGHT_COUNT; i++) {
            auto& light = lights[i];
            for (;;) {
                attempts++;
                light.position.x = PLANE_RADIUS_X * (2.0f * rand.random_float() - 1.0f);
                light.position.y = PLANE_RADIUS_Y * (2.0f * rand.random_float() - 1.0f);
                light.position.z = 0.0f;

                float min_distance = FLT_MAX;
                for (uint32_t j = 0; j < i; j++) {
                    const auto& other = lights[j];
                    const auto distance = Float3::Distance(light.position, other.position);
                    min_distance = std::min(min_distance, distance);
                }
                if (min_distance > 0.4f) {
                    break;
                }
            }

            const auto h = (float)i / (float)LIGHT_COUNT * 360.0f;
            const auto s = 0.75f;
            const auto v = 1.0f;
            light.color = rgb_from_hsv(Float3(h, s, v));

            light.speed_variation = 1.0f + 0.25f * rand.random_float();
        }
        FB_LOG_INFO("Light placement attempts: {}", attempts);

        // Buffer.
        _lights.create_and_transfer(
            device,
            lights,
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
            dx_name(NAME, "Lights")
        );
    }

    const auto swapchain_size = device.swapchain().size();
    _cull_dispatch_count_x = (swapchain_size.x + (CULL_DISPATCH_SIZE - 1)) / CULL_DISPATCH_SIZE;
    _cull_dispatch_count_y = (swapchain_size.y + (CULL_DISPATCH_SIZE - 1)) / CULL_DISPATCH_SIZE;

    {
        _light_counts_texture.create(
            device,
            {
                .format = DXGI_FORMAT_R32_UINT,
                .width = _cull_dispatch_count_x,
                .height = _cull_dispatch_count_y,
            },
            dx_name(NAME, "Light Counts Texture")
        );
        _light_offsets_texture.create(
            device,
            {
                .format = DXGI_FORMAT_R32_UINT,
                .width = _cull_dispatch_count_x,
                .height = _cull_dispatch_count_y,
            },
            dx_name(NAME, "Light Offsets Texture")
        );
        _light_indices.create(
            device,
            _cull_dispatch_count_x * _cull_dispatch_count_y * MAX_LIGHT_PER_TILE,
            dx_name(NAME, "Light Indices")
        );
        _light_indices_count.create(device, 1, dx_name(NAME, "Light Indices Count"));
    }

    {
        _magma_texture.create_and_transfer_baked(
            device,
            assets.heatmap_magma_texture(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            dx_name(NAME, "Magma Texture")
        );
        _viridis_texture.create_and_transfer_baked(
            device,
            assets.heatmap_viridis_texture(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            dx_name(NAME, "Viridis Texture")
        );
    }
}

auto FibersDemo::gui(const GuiDesc&) -> void {
    auto& p = _parameters;
    ImGui::SliderFloat("Camera Distance", &p.camera_distance, 0.5f, 10.0f);
    ImGui::SliderAngle("Camera FOV", &p.camera_fov, 0.0f, 90.0f);
    ImGui::SliderAngle("Camera Latitude", &p.camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera Longitude", &p.camera_longitude, 0.0f, 360.0f);
    ImGui::SliderFloat2("Camera Clip Planes", &p.camera_clip_planes.x, 0.1f, 400.0f);
    ImGui::Checkbox("Show Light Bounds", &p.show_light_bounds);
    ImGui::SliderFloat("Light Speed", &p.light_speed, 0.0f, 2.0f);
    ImGui::SliderFloat("Light Range", &p.light_range, 0.05f, 0.5f);
    ImGui::SliderInt("Light Intensity Pow2", &p.light_intensity_pow2, 0, 20);
    ImGui::Combo("Heatmap", (int*)&p.heatmap, "Magma\0Viridis\0");
    ImGui::SliderFloat("Heatmap Opacity", &p.heatmap_opacity, 0.0f, 1.0f);
}

auto FibersDemo::update(const UpdateDesc& desc) -> void {
    const auto& p = _parameters;

    // Update transforms.
    Float4x4 clip_from_world;
    Float4x4 view_from_clip;
    Float4x4 view_from_world;
    {
        auto projection = Float4x4::CreatePerspectiveFieldOfView(
            p.camera_fov,
            desc.aspect_ratio,
            p.camera_clip_planes.x,
            p.camera_clip_planes.y
        );
        auto eye = p.camera_distance * dir_from_lonlat(p.camera_longitude, p.camera_latitude);
        auto view = Float4x4::CreateLookAt(eye, Float3::Zero, Float3::Up);
        clip_from_world = view * projection;
        view_from_clip = projection.Invert();
        view_from_world = view;
    }

    // Update debug draw.
    _debug_draw.begin(desc.frame_index);
    _debug_draw.transform(clip_from_world);
    _debug_draw.axes();
    _debug_draw.end();

    // Update constants.
    *_constants.ptr() = Constants {
        .clip_from_world = clip_from_world,
        .view_from_clip = view_from_clip,
        .view_from_world = view_from_world,
        .window_size = Float2((float)desc.window_size.x, (float)desc.window_size.y),
        .delta_time = desc.delta_time,
        .light_speed = p.light_speed,
        .light_range = p.light_range,
        .light_intensity = 1.0f / (float)(1 << p.light_intensity_pow2),
        .heatmap_opacity = p.heatmap_opacity,
    };
}

auto FibersDemo::render(GpuDevice& device, GpuCommandList& cmd) -> void {
    const auto& p = _parameters;

    // Compute.
    {
        cmd.begin_pix("Compute");
        cmd.set_compute();

        // Sim.
        cmd.set_pipeline(_sim_pipeline);
        cmd.set_compute_constants(Bindings {
            .constants = _constants.cbv_descriptor().index(),
            .lights = _lights.uav_descriptor().index(),
        });
        cmd.dispatch(SIM_DISPATCH_COUNT, 1, 1);
        _lights.uav_barrier(cmd);
        cmd.flush_barriers();

        // Reset.
        cmd.set_pipeline(_reset_pipeline);
        cmd.set_compute_constants(Bindings {
            .light_indices_count = _light_indices_count.uav_descriptor().index(),
        });
        cmd.dispatch(1, 1, 1);
        _light_indices_count.uav_barrier(cmd);
        cmd.flush_barriers();

        // Cull.
        cmd.set_pipeline(_cull_pipeline);
        cmd.set_compute_constants(Bindings {
            .constants = _constants.cbv_descriptor().index(),
            .lights = _lights.uav_descriptor().index(),
            .light_counts_texture = _light_counts_texture.uav_descriptor().index(),
            .light_offsets_texture = _light_offsets_texture.uav_descriptor().index(),
            .light_indices = _light_indices.uav_descriptor().index(),
            .light_indices_count = _light_indices_count.uav_descriptor().index(),
        });
        cmd.dispatch(_cull_dispatch_count_x, _cull_dispatch_count_y, 1);
        _light_counts_texture.uav_barrier(cmd);
        _light_offsets_texture.uav_barrier(cmd);
        _light_indices.uav_barrier(cmd);
        _light_indices_count.uav_barrier(cmd);
        cmd.flush_barriers();
        cmd.end_pix();
    }

    // Draw.
    {
        cmd.begin_pix("Draw");
        cmd.set_graphics();
        _render_targets.set(cmd);
        _debug_draw.render(device, cmd);

        // Light.
        if (p.show_light_bounds) {
            cmd.set_graphics_constants(Bindings {
                .constants = _constants.cbv_descriptor().index(),
                .lights = _lights.srv_descriptor().index(),
                .vertices = _light_mesh.vertices.srv_descriptor().index(),
            });
            cmd.set_pipeline(_light_pipeline);
            cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            cmd.set_index_buffer(_light_mesh.indices.index_buffer_view());
            cmd.draw_indexed_instanced(_light_mesh.indices.element_size(), LIGHT_COUNT, 0, 0, 0);
        }

        // Plane.
        cmd.set_graphics_constants(Bindings {
            .constants = _constants.cbv_descriptor().index(),
            .lights = _lights.srv_descriptor().index(),
            .vertices = _plane_mesh.vertices.srv_descriptor().index(),
            .light_counts_texture = _light_counts_texture.srv_descriptor().index(),
            .light_offsets_texture = _light_offsets_texture.srv_descriptor().index(),
            .light_indices = _light_indices.srv_descriptor().index(),
        });
        cmd.set_pipeline(_plane_pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(_plane_mesh.indices.index_buffer_view());
        cmd.draw_indexed_instanced(_plane_mesh.indices.element_size(), 1, 0, 0, 0);

        // Debug.
        auto heatmap_index = 0u;
        switch (p.heatmap) {
            case Heatmap::Magma: heatmap_index = _magma_texture.srv_descriptor().index(); break;
            case Heatmap::Viridis: heatmap_index = _viridis_texture.srv_descriptor().index(); break;
        }
        cmd.set_graphics_constants(Bindings {
            .constants = _constants.cbv_descriptor().index(),
            .heatmap_texture = heatmap_index,
            .light_counts_texture = _light_counts_texture.srv_descriptor().index(),
        });
        cmd.set_pipeline(_debug_pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.draw_instanced(3, 1, 0, 0);
        cmd.end_pix();
    }
}

} // namespace fb::demos::fibers
