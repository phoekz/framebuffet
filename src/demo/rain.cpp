#include "rain.hpp"

namespace fb::rain {

Demo::Demo(GpuDevice& device) :
    _render_targets(device, device.swapchain_size(), Demo::CLEAR_COLOR, Demo::NAME),
    _debug_draw(device, Demo::NAME) {
    // Particles.
    {
        // Buffer.
        _particle_buffer.create(device, PARTICLE_COUNT, dx_name(Demo::NAME, "Particle Buffer"));

        // Data.
        pcg rand;
        std::vector<Particle> particles(PARTICLE_COUNT);
        for (uint32_t i = 0; i < PARTICLE_COUNT; i++) {
            Particle& particle = particles[i];
            particle.position.x = 2.0f * rand.random_float() - 1.0f;
            particle.position.y = 2.0f * rand.random_float() - 1.0f;
            particle.position.z = 2.0f * rand.random_float() - 1.0f;
        }

        // Upload.
        device.easy_upload(
            D3D12_SUBRESOURCE_DATA {
                .pData = particles.data(),
                .RowPitch = _particle_buffer.byte_size(),
                .SlicePitch = _particle_buffer.byte_size()},
            _particle_buffer.resource(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    }

    // Shaders.
    GpuShader compute_shader;
    GpuShader vertex_shader;
    GpuShader pixel_shader;
    {
        GpuShaderCompiler sc;
        auto compute_name = dx_name(Demo::NAME, Demo::Compute::NAME);
        auto draw_name = dx_name(Demo::NAME, Demo::Draw::NAME);
        auto compute_source = read_whole_file("shaders/rain.compute.hlsl");
        auto draw_source = read_whole_file("shaders/rain.draw.hlsl");
        compute_shader =
            sc.compile(compute_name, GpuShaderType::Compute, "cs_main", compute_source);
        vertex_shader = sc.compile(draw_name, GpuShaderType::Vertex, "vs_main", draw_source);
        pixel_shader = sc.compile(draw_name, GpuShaderType::Pixel, "ps_main", draw_source);
    }

    // Compute - Pipeline.
    GpuPipelineBuilder()
        .compute_shader(compute_shader.bytecode())
        .build(device, _compute.pipeline, dx_name(Demo::NAME, Demo::Compute::NAME, "Pipeline"));

    // Compute - Constant buffer.
    _compute.constant_buffer.create(
        device,
        1,
        dx_name(Demo::NAME, Demo::Compute::NAME, "Constant Buffer"));

    // Draw - Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(vertex_shader.bytecode())
        .pixel_shader(pixel_shader.bytecode())
        .blend(GPU_PIPELINE_BLEND_ADDITIVE)
        .depth_stencil(GPU_PIPELINE_DEPTH_NONE)
        .render_target_formats({DXGI_FORMAT_R8G8B8A8_UNORM})
        .depth_stencil_format(DXGI_FORMAT_D32_FLOAT)
        .build(device, _draw.pipeline, dx_name(Demo::NAME, Demo::Draw::NAME, "Pipeline"));

    // Draw - Constant buffer.
    _draw.constant_buffer.create(
        device,
        1,
        dx_name(Demo::NAME, Demo::Draw::NAME, "Constant Buffer"));

    // Draw - Vertex buffer.
    {
        const auto vertices = std::to_array<Vertex>({
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
            {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}},
            {{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f}},
            {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
        });
        const auto indices = std::to_array<uint16_t>({0, 1, 2, 0, 2, 3});
        _draw.vertex_buffer.create_with_data(
            device,
            vertices,
            dx_name(Demo::NAME, Demo::Draw::NAME, "Vertex Buffer"));
        _draw.index_buffer.create_with_data(
            device,
            indices,
            dx_name(Demo::NAME, Demo::Draw::NAME, "Index Buffer"));
    }
}

static float camera_distance = 1.25f;
static float camera_longitude = rad_from_deg(45.0f);
static float camera_latitude = rad_from_deg(-15.0f);
static float camera_fov = rad_from_deg(45.0f);
static float camera_rotation_speed = 0.05f;
static float particle_width = 0.01f;
static float particle_height = 0.075f;

auto Demo::gui(const gui::Desc&) -> void {
    auto& compute_constants = *_compute.constant_buffer.ptr();
    ImGui::SliderFloat("Rain Speed", &compute_constants.speed, 0.0f, 2.0f);
    ImGui::SliderFloat("Camera Distance", &camera_distance, 1.0f, 10.0f);
    ImGui::SliderAngle("Camera Longitude", &camera_longitude, -180.0f, 180.0f);
    ImGui::SliderAngle("Camera Latitude", &camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera FOV", &camera_fov, 0.0f, 90.0f);
    ImGui::SliderFloat("Camera Rotation Speed", &camera_rotation_speed, 0.0f, 1.0f);
    ImGui::SliderFloat("Particle Width", &particle_width, 0.0f, 1.0f);
    ImGui::SliderFloat("Particle Height", &particle_height, 0.0f, 1.0f);
}

void Demo::update(const demo::UpdateDesc& desc) {
    Float4x4 camera_transform;
    {
        camera_longitude += camera_rotation_speed * desc.delta_time;
        if (camera_longitude > PI * 2.0f) {
            camera_longitude -= PI * 2.0f;
        }

        auto aspect_ratio = desc.aspect_ratio;
        auto projection =
            Float4x4::CreatePerspectiveFieldOfView(camera_fov, aspect_ratio, 0.1f, 100.0f);
        auto eye = camera_distance * dir_from_lonlat(camera_longitude, camera_latitude);
        auto view = Float4x4::CreateLookAt(eye, Float3::Zero, Float3::Up);

        auto from_dir = Float3::UnitZ;
        auto to_dir = Float3(eye.x, 0.0f, eye.z);
        auto rot_quat = Quaternion::FromToRotation(from_dir, to_dir);
        auto rot_Float4x4 = Float4x4::CreateFromQuaternion(rot_quat);
        auto scale = Float4x4::CreateScale(particle_width, particle_height, 1.0f);
        auto particle_transform = scale * rot_Float4x4;

        camera_transform = view * projection;

        auto& compute_constants = *_compute.constant_buffer.ptr();
        auto& draw_constants = *_draw.constant_buffer.ptr();
        compute_constants.delta_time = desc.delta_time;
        draw_constants.transform = camera_transform;
        draw_constants.particle_transform = particle_transform;
    }

    {
        _debug_draw.begin(desc.frame_index);
        _debug_draw.transform(camera_transform);
        _debug_draw.axes();
        _debug_draw.end();
    }
}

void Demo::render(GpuDevice& device, GpuCommandList& cmd) {
    {
        cmd.set_compute();
        _particle_buffer.transition(cmd, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        cmd.set_compute_constants({
            _compute.constant_buffer.cbv_descriptor().index(),
            _particle_buffer.uav_descriptor().index(),
        });
        cmd.set_pipeline(_compute.pipeline);
        cmd.dispatch(DISPATCH_COUNT, 1, 1);
        _particle_buffer.transition(cmd, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    }

    {
        cmd.set_graphics();
        _render_targets.begin(device, cmd);
        cmd.set_graphics_constants({
            _draw.constant_buffer.cbv_descriptor().index(),
            _draw.vertex_buffer.srv_descriptor().index(),
            _particle_buffer.srv_descriptor().index(),
        });
        cmd.set_pipeline(_draw.pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(_draw.index_buffer.index_buffer_view());
        cmd.draw_indexed_instanced(6, PARTICLE_COUNT, 0, 0, 0);
        _debug_draw.render(device, cmd);
        _render_targets.end(device, cmd);
    }
}

}  // namespace fb::rain
