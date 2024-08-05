#include "fibers.hpp"

namespace fb::demos::fibers {

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

    // Pipelines.
    {
        GpuPipelineBuilder()
            .compute_shader(shaders.fibers_sim_cs())
            .build(device, demo.sim_pipeline, debug.with_name("Sim Pipeline"));

        GpuPipelineBuilder()
            .compute_shader(shaders.fibers_reset_cs())
            .build(device, demo.reset_pipeline, debug.with_name("Reset Pipeline"));

        GpuPipelineBuilder()
            .compute_shader(shaders.fibers_cull_cs())
            .build(device, demo.cull_pipeline, debug.with_name("Cull Pipeline"));

        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(shaders.fibers_light_vs())
            .pixel_shader(shaders.fibers_light_ps())
            .rasterizer(GpuRasterizerDesc {
                .fill_mode = GpuFillMode::Wireframe,
            })
            .render_target_formats({demo.render_targets.color_format()})
            .depth_stencil_format(demo.render_targets.depth_format())
            .build(device, demo.light_pipeline, debug.with_name("Light Pipeline"));

        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(shaders.fibers_plane_vs())
            .pixel_shader(shaders.fibers_plane_ps())
            .render_target_formats({demo.render_targets.color_format()})
            .depth_stencil_format(demo.render_targets.depth_format())
            .build(device, demo.plane_pipeline, debug.with_name("Plane Pipeline"));

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
            .render_target_formats({demo.render_targets.color_format()})
            .build(device, demo.debug_pipeline, debug.with_name("Debug Pipeline"));
    }

    // Constants.
    demo.constants.create(device, 1, debug.with_name("Constants"));

    // Geometry.
    {
        const auto mesh = assets.light_bounds_mesh();
        demo.light_mesh.vertices.create_and_transfer(
            device,
            mesh.vertices,
            D3D12_BARRIER_SYNC_VERTEX_SHADING,
            D3D12_BARRIER_ACCESS_VERTEX_BUFFER,
            debug.with_name("Light Vertices")
        );
        demo.light_mesh.indices.create_and_transfer(
            device,
            mesh.indices,
            D3D12_BARRIER_SYNC_INDEX_INPUT,
            D3D12_BARRIER_ACCESS_INDEX_BUFFER,
            debug.with_name("Light Indices")
        );
    }
    {
        const auto vertices = std::to_array<baked::Vertex>({
            {{-PLANE_RADIUS_X, -PLANE_RADIUS_Y, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {}},
            {{+PLANE_RADIUS_X, -PLANE_RADIUS_Y, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {}},
            {{+PLANE_RADIUS_X, +PLANE_RADIUS_Y, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {}},
            {{-PLANE_RADIUS_X, +PLANE_RADIUS_Y, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {}},
        });
        const auto indices = std::to_array<baked::Index>({0, 1, 2, 0, 2, 3});
        demo.plane_mesh.vertices.create_and_transfer(
            device,
            std::span(vertices.data(), vertices.size()),
            D3D12_BARRIER_SYNC_VERTEX_SHADING,
            D3D12_BARRIER_ACCESS_VERTEX_BUFFER,
            debug.with_name("Plane Vertices")
        );
        demo.plane_mesh.indices.create_and_transfer(
            device,
            std::span(indices.data(), indices.size()),
            D3D12_BARRIER_SYNC_INDEX_INPUT,
            D3D12_BARRIER_ACCESS_INDEX_BUFFER,
            debug.with_name("Plane Indices")
        );
    }

    // Lights.
    {
        // Data.
        Pcg rand;
        auto lights = std::vector<Light>(LIGHT_COUNT);
        uint attempts = 0;
        for (uint i = 0; i < LIGHT_COUNT; i++) {
            auto& light = lights[i];
            for (;;) {
                attempts++;
                light.position.x = PLANE_RADIUS_X * (2.0f * rand.random_float() - 1.0f);
                light.position.y = PLANE_RADIUS_Y * (2.0f * rand.random_float() - 1.0f);
                light.position.z = 0.0f;

                float min_distance = FLT_MAX;
                for (uint j = 0; j < i; j++) {
                    const auto& other = lights[j];
                    const auto distance = float3_distance(light.position, other.position);
                    min_distance = std::min(min_distance, distance);
                }
                if (min_distance > 0.4f) {
                    break;
                }
            }

            const auto h = (float)i / (float)LIGHT_COUNT * 360.0f;
            const auto s = 0.75f;
            const auto v = 1.0f;
            light.color = rgb_from_hsv(float3(h, s, v));

            light.speed_variation = 1.0f + 0.25f * rand.random_float();
        }
        FB_LOG_INFO("Light placement attempts: {}", attempts);

        // Buffer.
        demo.lights.create_and_transfer(
            device,
            lights,
            D3D12_BARRIER_SYNC_COMPUTE_SHADING,
            D3D12_BARRIER_ACCESS_UNORDERED_ACCESS,
            debug.with_name("Lights")
        );

        // Compute resources.
        const auto swapchain_size = device.swapchain().size();
        demo.sim_dispatch_count_x = (LIGHT_COUNT + (SIM_DISPATCH_X - 1)) / SIM_DISPATCH_X;
        demo.cull_dispatch_count_x = (swapchain_size.x + (CULL_TILE_SIZE - 1)) / CULL_TILE_SIZE;
        demo.cull_dispatch_count_y = (swapchain_size.y + (CULL_TILE_SIZE - 1)) / CULL_TILE_SIZE;

        demo.light_counts_texture.create(
            device,
            {
                .format = DXGI_FORMAT_R32_UINT,
                .width = demo.cull_dispatch_count_x,
                .height = demo.cull_dispatch_count_y,
            },
            debug.with_name("Light Counts Texture")
        );
        demo.light_offsets_texture.create(
            device,
            {
                .format = DXGI_FORMAT_R32_UINT,
                .width = demo.cull_dispatch_count_x,
                .height = demo.cull_dispatch_count_y,
            },
            debug.with_name("Light Offsets Texture")
        );
        demo.light_indices.create(
            device,
            demo.cull_dispatch_count_x * demo.cull_dispatch_count_y * MAX_LIGHT_PER_TILE,
            debug.with_name("Light Indices")
        );
        demo.light_indices_count.create(device, 1, debug.with_name("Light Indices Count"));
    }

    // Heatmap textures.
    {
        demo.magma_texture.create_and_transfer_baked(
            device,
            assets.heatmap_magma_texture(),
            D3D12_BARRIER_SYNC_PIXEL_SHADING,
            D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
            D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE,
            debug.with_name("Magma Texture")
        );
        demo.viridis_texture.create_and_transfer_baked(
            device,
            assets.heatmap_viridis_texture(),
            D3D12_BARRIER_SYNC_PIXEL_SHADING,
            D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
            D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE,
            debug.with_name("Viridis Texture")
        );
    }
}

auto gui(Demo& demo, const GuiDesc&) -> void {
    ZoneScoped;
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Gui", NAME.data());
    auto& params = demo.parameters;
    ImGui::SliderFloat("Camera Distance", &params.camera_distance, 0.5f, 10.0f);
    ImGui::SliderAngle("Camera FOV", &params.camera_fov, 0.0f, 90.0f);
    ImGui::SliderAngle("Camera Latitude", &params.camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera Longitude", &params.camera_longitude, 0.0f, 360.0f);
    ImGui::SliderFloat2("Camera Clip Planes", &params.camera_clip_planes.x, 0.1f, 400.0f);
    ImGui::Checkbox("Show Light Bounds", &params.show_light_bounds);
    ImGui::SliderFloat("Light Speed", &params.light_speed, 0.0f, 2.0f);
    ImGui::SliderFloat("Light Range", &params.light_range, 0.05f, 0.5f);
    ImGui::SliderInt("Light Intensity Pow2", &params.light_intensity_pow2, 0, 20);
    ImGui::Combo("Heatmap", (int*)&params.heatmap, "Magma\0Viridis\0");
    ImGui::SliderFloat("Heatmap Opacity", &params.heatmap_opacity, 0.0f, 1.0f);
}

auto update(Demo& demo, const UpdateDesc& desc) -> void {
    ZoneScoped;
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Update", NAME.data());
    auto& params = demo.parameters;

    // Update transforms.
    float4x4 clip_from_world;
    float4x4 view_from_clip;
    float4x4 view_from_world;
    {
        auto projection = float4x4_perspective(
            params.camera_fov,
            desc.aspect_ratio,
            params.camera_clip_planes.x,
            params.camera_clip_planes.y
        );
        auto eye = params.camera_distance
            * float3_from_lonlat(params.camera_longitude, params.camera_latitude);
        auto view = float4x4_lookat(eye, FLOAT3_ZERO, FLOAT3_UP);
        clip_from_world = projection * view;
        view_from_clip = float4x4_inverse(projection);
        view_from_world = view;
    }

    // Update debug draw.
    demo.debug_draw.begin(desc.frame_index);
    demo.debug_draw.transform(clip_from_world);
    demo.debug_draw.axes();
    demo.debug_draw.end();

    // Update constants.
    demo.constants.buffer(desc.frame_index).ref() = Constants {
        .clip_from_world = clip_from_world,
        .view_from_clip = view_from_clip,
        .view_from_world = view_from_world,
        .window_size = float2((float)desc.window_size.x, (float)desc.window_size.y),
        .delta_time = desc.delta_time,
        .light_speed = params.light_speed,
        .light_range = params.light_range,
        .light_intensity = 1.0f / (float)(1 << params.light_intensity_pow2),
        .heatmap_opacity = params.heatmap_opacity,
    };
}

auto render(Demo& demo, const RenderDesc& desc) -> void {
    ZoneScoped;
    auto& [cmd, device, frame_index] = desc;
    cmd.pix_begin("%s - Render", NAME.data());

    // Compute.
    cmd.compute_scope([&demo, frame_index](GpuComputeCommandList& cmd) {
        cmd.pix_begin("Compute");

        // Transition to compute.
        demo.lights.explicit_transition(
            cmd,
            D3D12_BARRIER_SYNC_NONE,
            D3D12_BARRIER_SYNC_COMPUTE_SHADING,
            D3D12_BARRIER_ACCESS_NO_ACCESS,
            D3D12_BARRIER_ACCESS_UNORDERED_ACCESS
        );
        demo.light_indices.explicit_transition(
            cmd,
            D3D12_BARRIER_SYNC_NONE,
            D3D12_BARRIER_SYNC_COMPUTE_SHADING,
            D3D12_BARRIER_ACCESS_NO_ACCESS,
            D3D12_BARRIER_ACCESS_UNORDERED_ACCESS
        );
        demo.light_counts_texture.explicit_transition(
            cmd,
            D3D12_BARRIER_SYNC_NONE,
            D3D12_BARRIER_SYNC_COMPUTE_SHADING,
            D3D12_BARRIER_ACCESS_NO_ACCESS,
            D3D12_BARRIER_ACCESS_UNORDERED_ACCESS,
            D3D12_BARRIER_LAYOUT_UNDEFINED,
            D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_UNORDERED_ACCESS
        );
        demo.light_offsets_texture.explicit_transition(
            cmd,
            D3D12_BARRIER_SYNC_NONE,
            D3D12_BARRIER_SYNC_COMPUTE_SHADING,
            D3D12_BARRIER_ACCESS_NO_ACCESS,
            D3D12_BARRIER_ACCESS_UNORDERED_ACCESS,
            D3D12_BARRIER_LAYOUT_UNDEFINED,
            D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_UNORDERED_ACCESS
        );
        cmd.flush_barriers();

        // Sim.
        cmd.set_pipeline(demo.sim_pipeline);
        cmd.set_constants(Bindings {
            .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
            .lights = demo.lights.uav_descriptor().index(),
        });
        cmd.dispatch(demo.sim_dispatch_count_x, 1, 1);
        demo.lights.transition(
            cmd,
            D3D12_BARRIER_SYNC_COMPUTE_SHADING,
            D3D12_BARRIER_ACCESS_UNORDERED_ACCESS
        );
        cmd.flush_barriers();

        // Reset.
        cmd.set_pipeline(demo.reset_pipeline);
        cmd.set_constants(Bindings {
            .light_indices_count = demo.light_indices_count.uav_descriptor().index(),
        });
        cmd.dispatch(1, 1, 1);
        demo.light_indices_count.transition(
            cmd,
            D3D12_BARRIER_SYNC_COMPUTE_SHADING,
            D3D12_BARRIER_ACCESS_UNORDERED_ACCESS
        );
        cmd.flush_barriers();

        // Cull.
        cmd.set_pipeline(demo.cull_pipeline);
        cmd.set_constants(Bindings {
            .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
            .lights = demo.lights.uav_descriptor().index(),
            .light_counts_texture = demo.light_counts_texture.uav_descriptor().index(),
            .light_offsets_texture = demo.light_offsets_texture.uav_descriptor().index(),
            .light_indices = demo.light_indices.uav_descriptor().index(),
            .light_indices_count = demo.light_indices_count.uav_descriptor().index(),
        });
        cmd.dispatch(demo.cull_dispatch_count_x, demo.cull_dispatch_count_y, 1);

        // Transition to graphics.
        demo.lights.transition(
            cmd,
            D3D12_BARRIER_SYNC_PIXEL_SHADING,
            D3D12_BARRIER_ACCESS_SHADER_RESOURCE
        );
        demo.light_indices.transition(
            cmd,
            D3D12_BARRIER_SYNC_PIXEL_SHADING,
            D3D12_BARRIER_ACCESS_SHADER_RESOURCE
        );
        demo.light_counts_texture.transition(
            cmd,
            D3D12_BARRIER_SYNC_PIXEL_SHADING,
            D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
            D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE
        );
        demo.light_offsets_texture.transition(
            cmd,
            D3D12_BARRIER_SYNC_PIXEL_SHADING,
            D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
            D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE
        );
        cmd.flush_barriers();

        cmd.pix_end();
    });

    // Draw.
    cmd.graphics_scope([&demo, frame_index](GpuGraphicsCommandList& cmd) {
        cmd.pix_begin("Draw");

        const auto& params = demo.parameters;

        demo.render_targets.set(cmd);
        demo.debug_draw.render(cmd);

        // Light.
        if (params.show_light_bounds) {
            cmd.set_constants(Bindings {
                .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
                .lights = demo.lights.srv_descriptor().index(),
                .vertices = demo.light_mesh.vertices.srv_descriptor().index(),
            });
            cmd.set_pipeline(demo.light_pipeline);
            cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            cmd.set_index_buffer(demo.light_mesh.indices.index_buffer_view());
            cmd.draw_indexed_instanced(
                demo.light_mesh.indices.element_count(),
                LIGHT_COUNT,
                0,
                0,
                0
            );
        }

        // Plane.
        cmd.set_constants(Bindings {
            .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
            .lights = demo.lights.srv_descriptor().index(),
            .vertices = demo.plane_mesh.vertices.srv_descriptor().index(),
            .light_counts_texture = demo.light_counts_texture.srv_descriptor().index(),
            .light_offsets_texture = demo.light_offsets_texture.srv_descriptor().index(),
            .light_indices = demo.light_indices.srv_descriptor().index(),
        });
        cmd.set_pipeline(demo.plane_pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(demo.plane_mesh.indices.index_buffer_view());
        cmd.draw_indexed_instanced(demo.plane_mesh.indices.element_count(), 1, 0, 0, 0);

        // Debug.
        auto heatmap_index = 0u;
        switch (params.heatmap) {
            case Heatmap::Magma: heatmap_index = demo.magma_texture.srv_descriptor().index(); break;
            case Heatmap::Viridis:
                heatmap_index = demo.viridis_texture.srv_descriptor().index();
                break;
        }
        cmd.set_constants(Bindings {
            .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
            .heatmap_texture = heatmap_index,
            .light_counts_texture = demo.light_counts_texture.srv_descriptor().index(),
        });
        cmd.set_pipeline(demo.debug_pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.draw_instanced(3, 1, 0, 0);
        cmd.pix_end();
    });

    cmd.pix_end();
}

} // namespace fb::demos::fibers
