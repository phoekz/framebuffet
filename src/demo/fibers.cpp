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
            .compute_shader(sim_cs)
            .build(device, _sim_pipeline, dx_name(Demo::NAME, "Sim", "Pipeline"));

        const auto reset_cs = sc.compile(Demo::NAME, GpuShaderType::Compute, "reset_cs", source);
        GpuPipelineBuilder()
            .compute_shader(reset_cs)
            .build(device, _reset_pipeline, dx_name(Demo::NAME, "Reset", "Pipeline"));

        const auto cull_cs = sc.compile(Demo::NAME, GpuShaderType::Compute, "cull_cs", source);
        GpuPipelineBuilder()
            .compute_shader(cull_cs)
            .build(device, _cull_pipeline, dx_name(Demo::NAME, "Cull", "Pipeline"));

        const auto light_vs = sc.compile(Demo::NAME, GpuShaderType::Vertex, "light_vs", source);
        const auto light_ps = sc.compile(Demo::NAME, GpuShaderType::Pixel, "light_ps", source);
        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(light_vs)
            .pixel_shader(light_ps)
            .rasterizer(GPU_PIPELINE_WIREFRAME)
            .render_target_formats({DXGI_FORMAT_R8G8B8A8_UNORM})
            .depth_stencil_format(DXGI_FORMAT_D32_FLOAT)
            .build(device, _light_pipeline, dx_name(Demo::NAME, "Light", "Pipeline"));

        const auto plane_vs = sc.compile(Demo::NAME, GpuShaderType::Vertex, "plane_vs", source);
        const auto plane_ps = sc.compile(Demo::NAME, GpuShaderType::Pixel, "plane_ps", source);
        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(plane_vs)
            .pixel_shader(plane_ps)
            .render_target_formats({DXGI_FORMAT_R8G8B8A8_UNORM})
            .depth_stencil_format(DXGI_FORMAT_D32_FLOAT)
            .build(device, _plane_pipeline, dx_name(Demo::NAME, "Plane", "Pipeline"));

        const auto debug_vs = sc.compile(Demo::NAME, GpuShaderType::Vertex, "debug_vs", source);
        const auto debug_ps = sc.compile(Demo::NAME, GpuShaderType::Pixel, "debug_ps", source);
        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(debug_vs)
            .pixel_shader(debug_ps)
            .blend(GPU_PIPELINE_BLEND_ALPHA)
            .depth_stencil(GPU_PIPELINE_DEPTH_NONE)
            .render_target_formats({DXGI_FORMAT_R8G8B8A8_UNORM})
            .build(device, _debug_pipeline, dx_name(Demo::NAME, "Debug", "Pipeline"));
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

    {
        _light_counts_texture.create(
            device,
            {
                .format = DXGI_FORMAT_R32_UINT,
                .width = CULL_DISPATCH_COUNT_X,
                .height = CULL_DISPATCH_COUNT_Y,
            },
            dx_name(Demo::NAME, "Light Counts Texture"));
        _light_offsets_texture.create(
            device,
            {
                .format = DXGI_FORMAT_R32_UINT,
                .width = CULL_DISPATCH_COUNT_X,
                .height = CULL_DISPATCH_COUNT_Y,
            },
            dx_name(Demo::NAME, "Light Offsets Texture"));
        _light_indices_buffer.create(
            device,
            CULL_DISPATCH_COUNT_X * CULL_DISPATCH_COUNT_Y * LIGHT_CAPACITY_PER_TILE,
            dx_name(Demo::NAME, "Light Indices Buffer"));
        _light_indices_count.create(device, 1, dx_name(Demo::NAME, "Light Indices Count"));
    }

    {
        const auto magma_png = read_whole_file("assets/heatmaps/magma.png");
        const auto magma_img = Image::load(magma_png);

        const auto viridis_png = read_whole_file("assets/heatmaps/viridis.png");
        const auto viridis_img = Image::load(viridis_png);

        _magma_texture.create(
            device,
            GpuTexture2dDesc {
                .format = magma_img.format(),
                .width = magma_img.width(),
                .height = magma_img.height(),
            },
            dx_name(Demo::NAME, "Magma Texture"));

        _viridis_texture.create(
            device,
            GpuTexture2dDesc {
                .format = viridis_img.format(),
                .width = viridis_img.width(),
                .height = viridis_img.height(),
            },
            dx_name(Demo::NAME, "Viridis Texture"));

        device.easy_upload(
            D3D12_SUBRESOURCE_DATA {
                .pData = magma_img.data(),
                .RowPitch = magma_img.row_pitch(),
                .SlicePitch = magma_img.slice_pitch()},
            _magma_texture.resource(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        device.easy_upload(
            D3D12_SUBRESOURCE_DATA {
                .pData = viridis_img.data(),
                .RowPitch = viridis_img.row_pitch(),
                .SlicePitch = viridis_img.slice_pitch()},
            _viridis_texture.resource(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

static float camera_distance = 4.0f;
static float camera_fov = rad_from_deg(45.0f);
static float camera_latitude = rad_from_deg(0.0f);
static float camera_longitude = rad_from_deg(90.0f);
static Float2 camera_clip_planes = Float2(0.1f, 100.0f);
static bool show_light_bounds = true;
static int light_intensity_pow2 = 12;
enum class Heatmap : uint32_t { Magma, Viridis };
static Heatmap heatmap = Heatmap::Magma;

auto Demo::gui(const gui::Desc&) -> void {
    auto* cb = _constant_buffer.ptr();
    ImGui::SliderFloat("Camera Distance", &camera_distance, 0.5f, 10.0f);
    ImGui::SliderAngle("Camera FOV", &camera_fov, 0.0f, 90.0f);
    ImGui::SliderAngle("Camera Latitude", &camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera Longitude", &camera_longitude, 0.0f, 360.0f);
    ImGui::SliderFloat2("Camera Clip Planes", &camera_clip_planes.x, 0.1f, 400.0f);
    ImGui::SliderFloat("Light Range", &cb->light_range, 0.05f, 0.5f);
    ImGui::Checkbox("Show Light Bounds", &show_light_bounds);
    ImGui::SliderInt("Light Intensity Pow2", &light_intensity_pow2, 0, 20);
    ImGui::SliderFloat("Debug Opacity", &cb->debug_opacity, 0.0f, 1.0f);
    ImGui::Combo("Heatmap", (int*)&heatmap, "Magma\0Viridis\0");
}

auto Demo::update(const demo::UpdateDesc& desc) -> void {
    // Update transforms.
    Float4x4 clip_from_world;
    Float4x4 view_from_clip;
    Float4x4 view_from_world;
    {
        auto projection = Float4x4::CreatePerspectiveFieldOfView(
            camera_fov,
            desc.aspect_ratio,
            camera_clip_planes.x,
            camera_clip_planes.y);
        auto eye = camera_distance * dir_from_lonlat(camera_longitude, camera_latitude);
        auto view = Float4x4::CreateLookAt(eye, Float3::Zero, Float3::Up);
        clip_from_world = view * projection;
        view_from_clip = projection.Invert();
        view_from_world = view;
    }

    // Update constant buffer.
    auto* cb = _constant_buffer.ptr();
    cb->clip_from_world = clip_from_world;
    cb->view_from_clip = view_from_clip;
    cb->view_from_world = view_from_world;
    cb->window_size = Float2((float)desc.window_size.x, (float)desc.window_size.y);
    cb->delta_time = desc.delta_time;
    cb->light_intensity = 1.0f / (float)(1 << light_intensity_pow2);

    // Update debug draw.
    _debug_draw.begin(desc.frame_index);
    _debug_draw.transform(clip_from_world);
    _debug_draw.axes();
    _debug_draw.end();
}

template<typename F>
static auto compute_constants(GpuCommandList& cmd, F f) -> void {
    Bindings bindings = {};
    f(bindings);
    cmd.set_compute_constants(std::span((uint32_t*)&bindings, sizeof(bindings) / sizeof(uint32_t)));
}

template<typename F>
static auto graphics_constants(GpuCommandList& cmd, F f) -> void {
    Bindings bindings = {};
    f(bindings);
    cmd.set_graphics_constants(
        std::span((uint32_t*)&bindings, sizeof(bindings) / sizeof(uint32_t)));
}

auto Demo::render(GpuDevice& device, GpuCommandList& cmd) -> void {
    // Compute.
    {
        cmd.set_compute();

        // Sim.
        cmd.set_pipeline(_sim_pipeline);
        compute_constants(cmd, [&](Bindings& bindings) {
            bindings.constants = _constant_buffer.cbv_descriptor().index();
            bindings.lights = _light_buffer.uav_descriptor().index();
        });
        cmd.dispatch(SIM_DISPATCH_COUNT, 1, 1);
        _light_buffer.uav_barrier(cmd);

        // Reset.
        cmd.set_pipeline(_reset_pipeline);
        compute_constants(cmd, [&](Bindings& bindings) {
            bindings.light_indices_count = _light_indices_count.uav_descriptor().index();
        });
        cmd.dispatch(1, 1, 1);
        _light_indices_count.uav_barrier(cmd);

        // Cull.
        cmd.set_pipeline(_cull_pipeline);
        compute_constants(cmd, [&](Bindings& bindings) {
            bindings.constants = _constant_buffer.cbv_descriptor().index();
            bindings.lights = _light_buffer.uav_descriptor().index();
            bindings.light_counts_texture = _light_counts_texture.uav_descriptor().index();
            bindings.light_offsets_texture = _light_offsets_texture.uav_descriptor().index();
            bindings.light_indices = _light_indices_buffer.uav_descriptor().index();
            bindings.light_indices_count = _light_indices_count.uav_descriptor().index();
        });
        cmd.dispatch(CULL_DISPATCH_COUNT_X, CULL_DISPATCH_COUNT_Y, 1);
        _light_counts_texture.uav_barrier(cmd);
        _light_offsets_texture.uav_barrier(cmd);
        _light_indices_buffer.uav_barrier(cmd);
        _light_indices_count.uav_barrier(cmd);
    }

    // Graphics.
    {
        cmd.set_graphics();
        _render_targets.begin(device, cmd);
        _debug_draw.render(device, cmd);

        // Light.
        if (show_light_bounds) {
            graphics_constants(cmd, [&](Bindings& bindings) {
                bindings.constants = _constant_buffer.cbv_descriptor().index();
                bindings.lights = _light_buffer.srv_descriptor().index();
                bindings.vertices = _light_mesh.vertex_buffer.srv_descriptor().index();
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
        graphics_constants(cmd, [&](Bindings& bindings) {
            bindings.constants = _constant_buffer.cbv_descriptor().index();
            bindings.lights = _light_buffer.srv_descriptor().index();
            bindings.vertices = _plane_mesh.vertex_buffer.srv_descriptor().index();
            bindings.light_counts_texture = _light_counts_texture.srv_descriptor().index();
            bindings.light_offsets_texture = _light_offsets_texture.srv_descriptor().index();
            bindings.light_indices = _light_indices_buffer.srv_descriptor().index();
        });
        cmd.set_pipeline(_plane_pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(_plane_mesh.index_buffer.index_buffer_view());
        cmd.draw_indexed_instanced(_plane_mesh.index_buffer.element_size(), 1, 0, 0, 0);

        // Debug.
        auto heatmap_index = 0u;
        switch (heatmap) {
            case Heatmap::Magma: heatmap_index = _magma_texture.srv_descriptor().index(); break;
            case Heatmap::Viridis: heatmap_index = _viridis_texture.srv_descriptor().index(); break;
        }
        graphics_constants(cmd, [&](Bindings& bindings) {
            bindings.constants = _constant_buffer.cbv_descriptor().index();
            bindings.light_counts_texture = _light_counts_texture.srv_descriptor().index();
            bindings.heatmap_texture = heatmap_index;
        });
        cmd.set_pipeline(_debug_pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.draw_instanced(3, 1, 0, 0);

        _render_targets.end(device, cmd);
    }
}

}  // namespace fb::fibers
