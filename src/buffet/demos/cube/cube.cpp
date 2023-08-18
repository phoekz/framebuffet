#include "cube.hpp"

namespace fb::demos::cube {

CubeDemo::CubeDemo(GpuDevice& device, const baked::Assets& assets, const baked::Shaders& shaders) :
    _render_targets(device, device.swapchain_size(), CLEAR_COLOR, NAME),
    _debug_draw(device, shaders, NAME) {
    // Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.cube_draw_vs())
        .pixel_shader(shaders.cube_draw_ps())
        .render_target_formats({DXGI_FORMAT_R8G8B8A8_UNORM})
        .depth_stencil_format(DXGI_FORMAT_D32_FLOAT)
        .build(device, _pipeline, dx_name(NAME, "Pipeline"));

    // Constants.
    _constants.create(device, 1, dx_name(NAME, "Constants"));

    // Geometry.
    const auto mesh = assets.stylized_crate_mesh();
    _vertices.create_with_data(device, mesh.vertices, dx_name(NAME, "Vertices"));
    _indices.create_with_data(device, mesh.indices, dx_name(NAME, "Indices"));

    // Texture.
    {
        // Create.
        const auto texture = assets.stylized_crate_texture();
        _texture.create(
            device,
            GpuTextureDesc {
                .format = texture.format,
                .width = texture.width,
                .height = texture.height,
            },
            dx_name(NAME, "Texture"));

        // Upload.
        device.easy_upload(
            D3D12_SUBRESOURCE_DATA {
                .pData = texture.data.data(),
                .RowPitch = texture.row_pitch,
                .SlicePitch = texture.slice_pitch},
            _texture.resource(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

static float camera_distance = 5.0f;
static float camera_fov = rad_from_deg(45.0f);
static float camera_latitude = rad_from_deg(30.0f);
static float camera_longitude = rad_from_deg(0.0f);
static float camera_rotation_speed = 0.5f;

auto CubeDemo::gui(const GuiDesc&) -> void {
    ImGui::SliderFloat("Camera Distance", &camera_distance, 0.0f, 10.0f);
    ImGui::SliderAngle("Camera FOV", &camera_fov, 0.0f, 90.0f);
    ImGui::SliderAngle("Camera Latitude", &camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera Longitude", &camera_longitude, 0.0f, 360.0f);
    ImGui::SliderFloat("Camera Rotation Speed", &camera_rotation_speed, 0.0f, 2.0f);
}

auto CubeDemo::update(const UpdateDesc& desc) -> void {
    Float4x4 camera_transform;
    {
        camera_longitude += camera_rotation_speed * desc.delta_time;
        if (camera_longitude > PI * 2.0f) {
            camera_longitude -= PI * 2.0f;
        }

        auto projection =
            Float4x4::CreatePerspectiveFieldOfView(camera_fov, desc.aspect_ratio, 0.1f, 100.0f);
        auto eye = camera_distance * dir_from_lonlat(camera_longitude, camera_latitude);
        auto view = Float4x4::CreateLookAt(eye, Float3::Zero, Float3::Up);
        camera_transform = view * projection;

        auto& constants = *_constants.ptr();
        constants.transform = camera_transform;
    }

    {
        _debug_draw.begin(desc.frame_index);
        _debug_draw.transform(camera_transform);
        _debug_draw.axes();
        _debug_draw.end();
    }
}

auto CubeDemo::render(GpuDevice& device, GpuCommandList& cmd) -> void {
    cmd.set_graphics();
    _render_targets.begin(device, cmd);
    _debug_draw.render(device, cmd);
    cmd.set_graphics_constants({
        _constants.cbv_descriptor().index(),
        _vertices.srv_descriptor().index(),
        _texture.srv_descriptor().index(),
    });
    cmd.set_pipeline(_pipeline);
    cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd.set_index_buffer(_indices.index_buffer_view());
    cmd.draw_indexed_instanced(_indices.element_size(), 1, 0, 0, 0);
    _render_targets.end(device, cmd);
}

} // namespace fb::demos::cube
