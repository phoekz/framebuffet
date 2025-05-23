#include "rain.hpp"

namespace fb::demos::rain {

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

    // Particles.
    {
        // Data.
        Pcg rand;
        std::vector<Particle> particles(PARTICLE_COUNT);
        for (uint i = 0; i < PARTICLE_COUNT; i++) {
            Particle& particle = particles[i];
            particle.position.x = 2.0f * rand.random_float() - 1.0f;
            particle.position.y = 2.0f * rand.random_float() - 1.0f;
            particle.position.z = 2.0f * rand.random_float() - 1.0f;
        }

        // Buffer.
        demo.particles.create_and_transfer(
            device,
            particles,
            D3D12_BARRIER_SYNC_ALL_SHADING,
            D3D12_BARRIER_ACCESS_UNORDERED_ACCESS,
            debug.with_name("Particles")
        );
    }

    // Constants.
    demo.constants.create(device, 1, debug.with_name("Constants"));

    // Compute - Pipeline.
    GpuPipelineBuilder()
        .compute_shader(shaders.rain_sim_cs())
        .build(device, demo.compute_pipeline, debug.with_name("Sim Pipeline"));

    // Draw - Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.rain_draw_vs())
        .pixel_shader(shaders.rain_draw_ps())
        .blend(GpuBlendDesc {
            .blend_enable = true,
            .rgb_blend_src = GpuBlend::SrcAlpha,
            .rgb_blend_dst = GpuBlend::One,
            .rgb_blend_op = GpuBlendOp::Add,
            .alpha_blend_src = GpuBlend::SrcAlpha,
            .alpha_blend_dst = GpuBlend::One,
            .alpha_blend_op = GpuBlendOp::Add,
        })
        .depth_stencil(GpuDepthStencilDesc {
            .depth_read = false,
            .depth_write = false,
        })
        .render_target_formats({demo.render_target.color_format(0)})
        .depth_stencil_format(demo.render_target.depth_format())
        .build(device, demo.draw_pipeline, debug.with_name("Draw Pipeline"));

    // Draw - Geometry.
    {
        const auto vertices = std::to_array<Vertex>({
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}},
        });
        const auto indices = std::to_array<uint16_t>({0, 1, 2, 0, 2, 3});
        demo.draw_vertices.create_and_transfer(
            device,
            vertices,
            D3D12_BARRIER_SYNC_VERTEX_SHADING,
            D3D12_BARRIER_ACCESS_VERTEX_BUFFER,
            debug.with_name("Draw Vertices")
        );
        demo.draw_indices.create_and_transfer(
            device,
            indices,
            D3D12_BARRIER_SYNC_INDEX_INPUT,
            D3D12_BARRIER_ACCESS_INDEX_BUFFER,
            debug.with_name("Draw Indices")
        );
    }
}

auto gui(Demo& demo, const GuiDesc&) -> void {
    FB_PERF_FUNC();
    auto& params = demo.parameters;
    ImGui::SliderFloat("Rain Speed", &params.speed, 0.0f, 2.0f);
    ImGui::SliderFloat("Camera Distance", &params.camera_distance, 1.0f, 10.0f);
    ImGui::SliderAngle("Camera Longitude", &params.camera_longitude, -180.0f, 180.0f);
    ImGui::SliderAngle("Camera Latitude", &params.camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera FOV", &params.camera_fov, 0.0f, 90.0f);
    ImGui::SliderFloat("Camera Rotation Speed", &params.camera_rotation_speed, 0.0f, 1.0f);
    ImGui::SliderFloat("Particle Width", &params.particle_width, 0.0f, 1.0f);
    ImGui::SliderFloat("Particle Height", &params.particle_height, 0.0f, 1.0f);
}

auto update(Demo& demo, const UpdateDesc& desc) -> void {
    FB_PERF_FUNC();
    auto& params = demo.parameters;

    // Update camera.
    params.camera_longitude += params.camera_rotation_speed * desc.delta_time;
    if (params.camera_longitude > FLOAT_PI * 2.0f) {
        params.camera_longitude -= FLOAT_PI * 2.0f;
    }
    auto aspect_ratio = desc.aspect_ratio;
    auto projection = float4x4_perspective(params.camera_fov, aspect_ratio, 0.1f, 100.0f);
    auto eye = params.camera_distance
        * float3_from_lonlat(params.camera_longitude, params.camera_latitude);
    auto view = float4x4_lookat(eye, FLOAT3_ZERO, FLOAT3_UP);
    auto from_dir = FLOAT3_Z;
    auto to_dir = float3(eye.x, 0.0f, eye.z);
    auto rot_quat = float_quat(from_dir, to_dir);
    auto rotation = (float4x4)rot_quat;
    auto scale = float4x4_scaling(params.particle_width, params.particle_height, 1.0f);
    auto particle_transform = rotation * scale;
    auto camera_transform = projection * view;

    // Update debug draw.
    demo.debug_draw.begin(desc.frame_index);
    demo.debug_draw.transform(camera_transform);
    demo.debug_draw.axes();
    demo.debug_draw.end();

    // Update constants.
    demo.constants.buffer(desc.frame_index).ref() = Constants {
        .transform = camera_transform,
        .particle_transform = particle_transform,
        .delta_time = desc.delta_time,
        .speed = params.speed,
    };
}

auto render(Demo& demo, const RenderDesc& desc) -> void {
    FB_PERF_FUNC();
    auto& [cmd, device, frame_index] = desc;
    cmd.pix_begin("%s - Render", NAME.data());

    cmd.compute_scope([&demo, frame_index](GpuComputeCommandList& cmd) {
        cmd.pix_begin("Compute");
        cmd.set_constants(Bindings {
            .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
            .particles = demo.particles.uav_descriptor().index(),
        });
        cmd.set_pipeline(demo.compute_pipeline);
        cmd.dispatch(DISPATCH_COUNT, 1, 1);
        cmd.pix_end();
    });

    cmd.graphics_scope([&demo, frame_index](GpuGraphicsCommandList& cmd) {
        cmd.pix_begin("Draw");
        demo.render_target_view.set_graphics(cmd);
        demo.debug_draw.render(cmd);
        cmd.set_constants(Bindings {
            .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
            .particles = demo.particles.srv_descriptor().index(),
            .vertices = demo.draw_vertices.srv_descriptor().index(),
        });
        cmd.set_pipeline(demo.draw_pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(demo.draw_indices.index_buffer_view());
        cmd.draw_indexed_instanced(6, PARTICLE_COUNT, 0, 0, 0);
        cmd.pix_end();
    });

    cmd.pix_end();
}

} // namespace fb::demos::rain
