#include "cube.hpp"

namespace fb::demos::cube {

CubeDemo::CubeDemo(GpuDevice& device, const baked::Assets& assets, const baked::Shaders& shaders)
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
    // Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.cube_draw_vs())
        .pixel_shader(shaders.cube_draw_ps())
        .render_target_formats({_render_targets.color_format()})
        .depth_stencil_format(_render_targets.depth_format())
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
            dx_name(NAME, "Texture")
        );

        // Transfer.
        device.transfer().resource(
            _texture.resource(),
            D3D12_SUBRESOURCE_DATA {
                .pData = texture.data.data(),
                .RowPitch = texture.row_pitch,
                .SlicePitch = texture.slice_pitch},
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );
    }
}

auto CubeDemo::gui(const GuiDesc&) -> void {
    auto& p = _parameters;
    ImGui::SliderFloat("Camera Distance", &p.camera_distance, 0.0f, 10.0f);
    ImGui::SliderAngle("Camera FOV", &p.camera_fov, 0.0f, 90.0f);
    ImGui::SliderAngle("Camera Latitude", &p.camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera Longitude", &p.camera_longitude, 0.0f, 360.0f);
    ImGui::SliderFloat("Camera Rotation Speed", &p.camera_rotation_speed, 0.0f, 2.0f);
}

auto CubeDemo::update(const UpdateDesc& desc) -> void {
    auto& p = _parameters;

    p.camera_longitude += p.camera_rotation_speed * desc.delta_time;
    if (p.camera_longitude > PI * 2.0f) {
        p.camera_longitude -= PI * 2.0f;
    }

    auto projection =
        Float4x4::CreatePerspectiveFieldOfView(p.camera_fov, desc.aspect_ratio, 0.1f, 100.0f);
    auto eye = p.camera_distance * dir_from_lonlat(p.camera_longitude, p.camera_latitude);
    auto view = Float4x4::CreateLookAt(eye, Float3::Zero, Float3::Up);
    auto camera_transform = view * projection;

    _debug_draw.begin(desc.frame_index);
    _debug_draw.transform(camera_transform);
    _debug_draw.axes();
    _debug_draw.end();

    *_constants.ptr() = Constants {
        .transform = camera_transform,
    };
}

auto CubeDemo::render(GpuDevice& device, GpuCommandList& cmd) -> void {
    cmd.set_graphics();
    _render_targets.begin(device, cmd);
    _debug_draw.render(device, cmd);
    cmd.set_graphics_constants(Bindings {
        .constants = _constants.cbv_descriptor().index(),
        .vertices = _vertices.srv_descriptor().index(),
        .texture = _texture.srv_descriptor().index(),
    });
    cmd.set_pipeline(_pipeline);
    cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd.set_index_buffer(_indices.index_buffer_view());
    cmd.draw_indexed_instanced(_indices.element_size(), 1, 0, 0, 0);
    _render_targets.end(device, cmd);
}

} // namespace fb::demos::cube
