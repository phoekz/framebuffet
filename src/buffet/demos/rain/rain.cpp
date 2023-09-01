#include "rain.hpp"

namespace fb::demos::rain {

auto RainDemo::create(GpuDevice& device, const Baked& baked) -> void {
    DebugScope debug(NAME);

    // Render targets.
    _render_targets.create(
        device,
        {
            .size = device.swapchain().size(),
            .color_format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .clear_color = CLEAR_COLOR,
            .sample_count = 1,
        }
    );

    // Debug draw.
    _debug_draw.create(device, baked.kitchen.shaders, _render_targets);

    // Unpack.
    const auto& shaders = baked.buffet.shaders;

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
        _particles.create_and_transfer(
            device,
            particles,
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
            debug.with_name("Particles")
        );
    }

    // Constants.
    _constants.create(device, 1, debug.with_name("Constants"));

    // Compute - Pipeline.
    GpuPipelineBuilder()
        .compute_shader(shaders.rain_sim_cs())
        .build(device, _compute_pipeline, debug.with_name("Sim Pipeline"));

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
        .render_target_formats({_render_targets.color_format()})
        .depth_stencil_format(_render_targets.depth_format())
        .build(device, _draw_pipeline, debug.with_name("Draw Pipeline"));

    // Draw - Geometry.
    {
        const auto vertices = std::to_array<Vertex>({
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}},
        });
        const auto indices = std::to_array<uint16_t>({0, 1, 2, 0, 2, 3});
        _draw_vertices.create_with_data(device, vertices, debug.with_name("Draw Vertices"));
        _draw_indices.create_with_data(device, indices, debug.with_name("Draw Indices"));
    }
}

auto RainDemo::gui(const GuiDesc&) -> void {
    auto& p = _parameters;
    ImGui::SliderFloat("Rain Speed", &p.speed, 0.0f, 2.0f);
    ImGui::SliderFloat("Camera Distance", &p.camera_distance, 1.0f, 10.0f);
    ImGui::SliderAngle("Camera Longitude", &p.camera_longitude, -180.0f, 180.0f);
    ImGui::SliderAngle("Camera Latitude", &p.camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera FOV", &p.camera_fov, 0.0f, 90.0f);
    ImGui::SliderFloat("Camera Rotation Speed", &p.camera_rotation_speed, 0.0f, 1.0f);
    ImGui::SliderFloat("Particle Width", &p.particle_width, 0.0f, 1.0f);
    ImGui::SliderFloat("Particle Height", &p.particle_height, 0.0f, 1.0f);
}

void RainDemo::update(const UpdateDesc& desc) {
    auto& p = _parameters;

    p.camera_longitude += p.camera_rotation_speed * desc.delta_time;
    if (p.camera_longitude > PI * 2.0f) {
        p.camera_longitude -= PI * 2.0f;
    }

    auto aspect_ratio = desc.aspect_ratio;
    auto projection =
        float4x4::CreatePerspectiveFieldOfView(p.camera_fov, aspect_ratio, 0.1f, 100.0f);
    auto eye = p.camera_distance * dir_from_lonlat(p.camera_longitude, p.camera_latitude);
    auto view = float4x4::CreateLookAt(eye, float3::Zero, float3::Up);
    auto from_dir = float3::UnitZ;
    auto to_dir = float3(eye.x, 0.0f, eye.z);
    auto rot_quat = Quaternion::FromToRotation(from_dir, to_dir);
    auto rot_Float4x4 = float4x4::CreateFromQuaternion(rot_quat);
    auto scale = float4x4::CreateScale(p.particle_width, p.particle_height, 1.0f);
    auto particle_transform = scale * rot_Float4x4;
    auto camera_transform = view * projection;

    _debug_draw.begin(desc.frame_index);
    _debug_draw.transform(camera_transform);
    _debug_draw.axes();
    _debug_draw.end();

    *_constants.ptr() = Constants {
        .transform = camera_transform,
        .particle_transform = particle_transform,
        .delta_time = desc.delta_time,
        .speed = p.speed,
    };
}

void RainDemo::render(GpuDevice& device, GpuCommandList& cmd) {
    {
        cmd.begin_pix("Compute");
        cmd.set_compute();
        cmd.set_compute_constants(Bindings {
            .constants = _constants.cbv_descriptor().index(),
            .particles = _particles.uav_descriptor().index(),
        });
        cmd.set_pipeline(_compute_pipeline);
        cmd.dispatch(DISPATCH_COUNT, 1, 1);
        _particles.uav_barrier(cmd);
        cmd.flush_barriers();
        cmd.end_pix();
    }

    {
        cmd.begin_pix("Draw");
        cmd.set_graphics();
        _render_targets.set(cmd);
        cmd.set_graphics_constants(Bindings {
            .constants = _constants.cbv_descriptor().index(),
            .particles = _particles.srv_descriptor().index(),
            .vertices = _draw_vertices.srv_descriptor().index(),
        });
        cmd.set_pipeline(_draw_pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(_draw_indices.index_buffer_view());
        cmd.draw_indexed_instanced(6, PARTICLE_COUNT, 0, 0, 0);
        _debug_draw.render(device, cmd);
        cmd.end_pix();
    }
}

} // namespace fb::demos::rain
