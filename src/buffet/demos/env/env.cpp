#include "env.hpp"

namespace fb::demos::env {

EnvDemo::EnvDemo(GpuDevice& device, const baked::Assets& assets, const baked::Shaders& shaders) :
    _render_targets(
        device,
        {
            .size = device.swapchain_size(),
            .color_format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .clear_color = CLEAR_COLOR,
            .sample_count = 1,
        },
        NAME),
    _debug_draw(device, shaders, _render_targets, NAME) {
    // Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.env_draw_vs())
        .pixel_shader(shaders.env_draw_ps())
        .depth_stencil(GPU_PIPELINE_DEPTH_DEFAULT)
        .render_target_formats({_render_targets.color_format()})
        .depth_stencil_format(_render_targets.depth_format())
        .build(device, _pipeline, dx_name(NAME, "Pipeline"));

    // Constants.
    _constants.create(device, 1, dx_name(NAME, "Constants"));

    // Geometry.
    {
        const auto mesh = assets.skybox_mesh();
        _vertices.create_with_data(device, mesh.vertices, dx_name(NAME, "Vertices"));
        _indices.create_with_data(device, mesh.indices, dx_name(NAME, "Indices"));
    }

    // Environment map.
    {
        // Create texture.
        const auto env_texture = assets.winter_evening_cube_texture();
        _env_texture.create(
            device,
            GpuTextureDesc {
                .format = env_texture.format,
                .width = env_texture.width,
                .height = env_texture.height,
                .depth = 6,
            },
            dx_name(NAME, "Env Texture"));

        // Transfer.
        std::array<D3D12_SUBRESOURCE_DATA, 6> subresources;
        for (uint32_t i = 0; i < 6; ++i) {
            subresources[i] = {
                .pData = env_texture.datas[i].data(),
                .RowPitch = (int64_t)env_texture.row_pitch,
                .SlicePitch = (int64_t)env_texture.slice_pitch,
            };
        }
        device.transfer().resource(
            _env_texture.resource(),
            subresources,
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

static float camera_fov = rad_from_deg(70.0f);
static float camera_distance = 1.25f;
static float camera_longitude = rad_from_deg(45.0f);
static float camera_latitude = rad_from_deg(0.0f);
static float camera_rotation_speed = 0.5f;

auto EnvDemo::gui(const GuiDesc&) -> void {
    ImGui::SliderAngle("Camera FOV", &camera_fov, 1.0f, 90.0f);
    ImGui::SliderFloat("Camera Distance", &camera_distance, 1.0f, 10.0f);
    ImGui::SliderAngle("Camera Longitude", &camera_longitude, -180.0f, 180.0f);
    ImGui::SliderAngle("Camera Latitude", &camera_latitude, -90.0f, 90.0f);
    ImGui::SliderFloat("Camera Rotation Speed", &camera_rotation_speed, 0.0f, 1.0f);
}

auto EnvDemo::update(const UpdateDesc& desc) -> void {
    camera_longitude += camera_rotation_speed * desc.delta_time;
    if (camera_longitude > PI * 2.0f) {
        camera_longitude -= PI * 2.0f;
    }

    float aspect_ratio = desc.aspect_ratio;
    Float4x4 perspective =
        Float4x4::CreatePerspectiveFieldOfView(camera_fov, aspect_ratio, 0.1f, 100.0f);
    Float3 eye = camera_distance * dir_from_lonlat(camera_longitude, camera_latitude);
    Float4x4 view = Float4x4::CreateLookAt(eye, Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 1.0f, 0.0f));
    Float4x4 env_view = view;
    env_view.m[3][0] = 0.0f;
    env_view.m[3][1] = 0.0f;
    env_view.m[3][2] = 0.0f;
    env_view.m[3][3] = 1.0f;
    Float4x4 env_transform = env_view * perspective;
    _constants.ptr()->transform = env_transform;

    Float4x4 camera_transform = view * perspective;
    _debug_draw.begin(desc.frame_index);
    _debug_draw.transform(camera_transform);
    _debug_draw.axes();
    _debug_draw.end();
}

auto EnvDemo::render(GpuDevice& device, GpuCommandList& cmd) -> void {
    cmd.set_graphics();
    _render_targets.begin(device, cmd);
    _debug_draw.render(device, cmd);
    cmd.set_graphics_constants(Bindings {
        .constants = _constants.cbv_descriptor().index(),
        .vertices = _vertices.srv_descriptor().index(),
        .texture = _env_texture.srv_descriptor().index(),
    });
    cmd.set_pipeline(_pipeline);
    cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd.set_index_buffer(_indices.index_buffer_view());
    cmd.draw_indexed_instanced(_indices.element_size(), 1, 0, 0, 0);
    _render_targets.end(device, cmd);
}

} // namespace fb::demos::env
