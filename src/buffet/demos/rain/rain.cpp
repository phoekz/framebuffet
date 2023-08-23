#include "rain.hpp"

namespace fb::demos::rain {

RainDemo::RainDemo(GpuDevice& device, const baked::Assets&, const baked::Shaders& shaders)
    : _render_targets(
        device,
        {
            .size = device.swapchain_size(),
            .color_format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .clear_color = CLEAR_COLOR,
            .sample_count = 1,
        },
        NAME
    )
    , _debug_draw(device, shaders, _render_targets, NAME) {
    // Particles.
    {
        // Buffer.
        _particles.create(device, PARTICLE_COUNT, dx_name(NAME, "Particles"));

        // Data.
        Pcg rand;
        std::vector<Particle> particles(PARTICLE_COUNT);
        for (uint32_t i = 0; i < PARTICLE_COUNT; i++) {
            Particle& particle = particles[i];
            particle.position.x = 2.0f * rand.random_float() - 1.0f;
            particle.position.y = 2.0f * rand.random_float() - 1.0f;
            particle.position.z = 2.0f * rand.random_float() - 1.0f;
        }

        // Transfer.
        device.transfer().resource(
            _particles.resource(),
            D3D12_SUBRESOURCE_DATA {
                .pData = particles.data(),
                .RowPitch = _particles.byte_size(),
                .SlicePitch = _particles.byte_size()},
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
        );
    }

    // Constants.
    _constants.create(device, 1, dx_name(NAME, "Constants"));

    // Compute - Pipeline.
    GpuPipelineBuilder()
        .compute_shader(shaders.rain_sim_cs())
        .build(device, _compute_pipeline, dx_name(NAME, "Sim", "Pipeline"));

    // Draw - Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.rain_draw_vs())
        .pixel_shader(shaders.rain_draw_ps())
        .blend(GPU_PIPELINE_BLEND_ADDITIVE)
        .depth_stencil(GPU_PIPELINE_DEPTH_NONE)
        .render_target_formats({_render_targets.color_format()})
        .depth_stencil_format(_render_targets.depth_format())
        .build(device, _draw_pipeline, dx_name(NAME, "Draw", "Pipeline"));

    // Draw - Geometry.
    {
        const auto vertices = std::to_array<Vertex>({
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
            {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}},
            {{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f}},
            {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
        });
        const auto indices = std::to_array<uint16_t>({0, 1, 2, 0, 2, 3});
        _draw_vertices.create_with_data(device, vertices, dx_name(NAME, "Draw", "Vertices"));
        _draw_indices.create_with_data(device, indices, dx_name(NAME, "Draw", "Indices"));
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
    PIXScopedEvent(PIX_COLOR_DEFAULT, dx_name(NAME, "Update").data());

    auto& p = _parameters;

    p.camera_longitude += p.camera_rotation_speed * desc.delta_time;
    if (p.camera_longitude > PI * 2.0f) {
        p.camera_longitude -= PI * 2.0f;
    }

    auto aspect_ratio = desc.aspect_ratio;
    auto projection =
        Float4x4::CreatePerspectiveFieldOfView(p.camera_fov, aspect_ratio, 0.1f, 100.0f);
    auto eye = p.camera_distance * dir_from_lonlat(p.camera_longitude, p.camera_latitude);
    auto view = Float4x4::CreateLookAt(eye, Float3::Zero, Float3::Up);
    auto from_dir = Float3::UnitZ;
    auto to_dir = Float3(eye.x, 0.0f, eye.z);
    auto rot_quat = Quaternion::FromToRotation(from_dir, to_dir);
    auto rot_Float4x4 = Float4x4::CreateFromQuaternion(rot_quat);
    auto scale = Float4x4::CreateScale(p.particle_width, p.particle_height, 1.0f);
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
        cmd.set_compute();
        cmd.set_compute_constants(Bindings {
            .constants = _constants.cbv_descriptor().index(),
            .particles = _particles.uav_descriptor().index(),
        });
        cmd.set_pipeline(_compute_pipeline);
        cmd.dispatch(DISPATCH_COUNT, 1, 1);
        _particles.uav_barrier(cmd);
        cmd.flush_barriers();
    }

    {
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
    }
}

} // namespace fb::demos::rain
