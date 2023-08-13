#include "fibers.hpp"

namespace fb::fibers {

Demo::Demo(GpuDevice& device) :
    _render_targets(device, device.swapchain_size(), Demo::CLEAR_COLOR, Demo::NAME),
    _debug_draw(device, Demo::NAME) {
    // Pipeline.
    {
        GpuShaderCompiler sc;
        const auto source = read_whole_file("shaders/fibers.hlsl");

        const auto sim_cs = sc.compile(Demo::NAME, GpuShaderType::Compute, "sim_cs", source);
        GpuPipelineBuilder()
            .compute_shader(sim_cs.bytecode())
            .build(device, _sim_pipeline, dx_name(Demo::NAME, "Sim", "Pipeline"));

        const auto light_vs = sc.compile(Demo::NAME, GpuShaderType::Vertex, "light_vs", source);
        const auto light_ps = sc.compile(Demo::NAME, GpuShaderType::Pixel, "light_ps", source);
        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(light_vs.bytecode())
            .pixel_shader(light_ps.bytecode())
            .rasterizer(GPU_PIPELINE_WIREFRAME)
            .render_target_formats({DXGI_FORMAT_R8G8B8A8_UNORM})
            .depth_stencil_format(DXGI_FORMAT_D32_FLOAT)
            .build(device, _light_pipeline, dx_name(Demo::NAME, "Light", "Pipeline"));

        const auto plane_vs = sc.compile(Demo::NAME, GpuShaderType::Vertex, "plane_vs", source);
        const auto plane_ps = sc.compile(Demo::NAME, GpuShaderType::Pixel, "plane_ps", source);
        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(plane_vs.bytecode())
            .pixel_shader(plane_ps.bytecode())
            .render_target_formats({DXGI_FORMAT_R8G8B8A8_UNORM})
            .depth_stencil_format(DXGI_FORMAT_D32_FLOAT)
            .build(device, _plane_pipeline, dx_name(Demo::NAME, "Plane", "Pipeline"));
    }

    // Constants.
    _constant_buffer.create(device, 1, dx_name(Demo::NAME, "Constant Buffer"));

    // Geometry.
    {
        using namespace DirectX::DX12;
        using Vertices = GeometricPrimitive::VertexCollection;
        using Indices = GeometricPrimitive::IndexCollection;
        Vertices vertices;
        Indices indices;
        GeometricPrimitive::CreateBox(vertices, indices, {2.0f, 2.0f, 2.0f});
        _light_mesh.vertex_buffer.create_with_data(
            device,
            std::span(reinterpret_cast<const Vertex*>(vertices.data()), vertices.size()),
            dx_name(Demo::NAME, "Light", "Vertex Buffer"));
        _light_mesh.index_buffer.create_with_data(
            device,
            std::span(reinterpret_cast<const Index*>(indices.data()), indices.size()),
            dx_name(Demo::NAME, "Light", "Index Buffer"));
    }
    {
        const auto vertices = std::to_array<Vertex>({
            {{-PLANE_RADIUS_X, -PLANE_RADIUS_Y, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{-PLANE_RADIUS_X, +PLANE_RADIUS_Y, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
            {{+PLANE_RADIUS_X, +PLANE_RADIUS_Y, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{+PLANE_RADIUS_X, -PLANE_RADIUS_Y, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        });
        const auto indices = std::to_array<Index>({0, 1, 2, 0, 2, 3});
        _plane_mesh.vertex_buffer.create_with_data(
            device,
            std::span(vertices.data(), vertices.size()),
            dx_name(Demo::NAME, "Plane", "Vertex Buffer"));
        _plane_mesh.index_buffer.create_with_data(
            device,
            std::span(indices.data(), indices.size()),
            dx_name(Demo::NAME, "Plane", "Index Buffer"));
    }

    // Lights.
    {
        // Buffer.
        _light_buffer.create(device, LIGHT_COUNT, dx_name(Demo::NAME, "Light Buffer"));

        // Data.
        pcg rand;
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
                if (min_distance > 0.4f)
                    break;
            }

            const auto h = (float)i / (float)LIGHT_COUNT * 360.0f;
            const auto s = 0.75f;
            const auto v = 1.0f;
            light.color = rgb_from_hsv(Float3(h, s, v));

            light.speed_variation = 1.0f + 0.25f * rand.random_float();
        }
        FB_LOG_INFO("Light placement attempts: {}", attempts);

        // Upload.
        device.easy_upload(
            D3D12_SUBRESOURCE_DATA {
                .pData = lights.data(),
                .RowPitch = _light_buffer.byte_size(),
                .SlicePitch = _light_buffer.byte_size()},
            _light_buffer.resource(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    }
}

static float camera_distance = 3.0f;
static float camera_fov = rad_from_deg(45.0f);
static float camera_latitude = rad_from_deg(0.0f);
static float camera_longitude = rad_from_deg(90.0f);
static Float2 camera_clip_planes = Float2(0.1f, 100.0f);
static bool show_light_bounds = false;
static int light_intensity_pow2 = 12;

auto Demo::gui(const gui::Desc&) -> void {
    auto* cb = _constant_buffer.ptr();
    ImGui::SliderFloat("Camera Distance", &camera_distance, 0.5f, 5.0f);
    ImGui::SliderAngle("Camera FOV", &camera_fov, 0.0f, 90.0f);
    ImGui::SliderAngle("Camera Latitude", &camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera Longitude", &camera_longitude, 0.0f, 360.0f);
    ImGui::SliderFloat2("Camera Clip Planes", &camera_clip_planes.x, 0.1f, 400.0f);
    ImGui::SliderFloat("Light Range", &cb->light_range, 0.05f, 0.5f);
    ImGui::Checkbox("Show Light Bounds", &show_light_bounds);
    ImGui::SliderInt("Light Intensity Pow2", &light_intensity_pow2, 0, 20);
}

auto Demo::update(const demo::UpdateDesc& desc) -> void {
    // Update camera transform.
    Float4x4 camera_transform;
    {
        auto projection = Float4x4::CreatePerspectiveFieldOfView(
            camera_fov,
            desc.aspect_ratio,
            camera_clip_planes.x,
            camera_clip_planes.y);
        auto eye = camera_distance * dir_from_lonlat(camera_longitude, camera_latitude);
        auto view = Float4x4::CreateLookAt(eye, Float3::Zero, Float3::Up);
        camera_transform = view * projection;
    }

    // Update constant buffer.
    auto* cb = _constant_buffer.ptr();
    cb->transform = camera_transform;
    cb->delta_time = desc.delta_time;
    cb->light_intensity = 1.0f / (float)(1 << light_intensity_pow2);

    // Update debug draw.
    _debug_draw.begin(desc.frame_index);
    _debug_draw.transform(camera_transform);
    _debug_draw.axes();
    _debug_draw.end();
}

auto Demo::render(GpuDevice& device, GpuCommandList& cmd) -> void {
    // Compute.
    {
        // Sim.
        cmd.set_compute();
        cmd.set_pipeline(_sim_pipeline);
        cmd.set_compute_constants({
            _constant_buffer.cbv_descriptor().index(),
            _light_buffer.uav_descriptor().index(),
        });
        cmd.dispatch(DISPATCH_COUNT, 1, 1);
        _light_buffer.uav_barrier(cmd);
    }

    // Graphics.
    {
        cmd.set_graphics();
        _render_targets.begin(device, cmd);
        _debug_draw.render(device, cmd);

        // Light.
        if (show_light_bounds) {
            cmd.set_graphics_constants({
                _constant_buffer.cbv_descriptor().index(),
                _light_buffer.srv_descriptor().index(),
                _light_mesh.vertex_buffer.srv_descriptor().index(),
            });
            cmd.set_pipeline(_light_pipeline);
            cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            cmd.set_index_buffer(_light_mesh.index_buffer.index_buffer_view());
            cmd.draw_indexed_instanced(
                _light_mesh.index_buffer.element_size(),
                LIGHT_COUNT,
                0,
                0,
                0);
        }

        // Plane.
        cmd.set_graphics_constants({
            _constant_buffer.cbv_descriptor().index(),
            _light_buffer.srv_descriptor().index(),
            _plane_mesh.vertex_buffer.srv_descriptor().index(),
        });
        cmd.set_pipeline(_plane_pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(_plane_mesh.index_buffer.index_buffer_view());
        cmd.draw_indexed_instanced(_plane_mesh.index_buffer.element_size(), 1, 0, 0, 0);

        _render_targets.end(device, cmd);
    }
}

}  // namespace fb::fibers
