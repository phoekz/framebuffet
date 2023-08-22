#include "cube.hpp"

namespace fb::demos::cube {

CubeDemo::CubeDemo(GpuDevice& device, const baked::Assets& assets, const baked::Shaders& shaders)
    : _render_targets(
        device,
        {
            .size = device.swapchain_size(),
            .color_format = DXGI_FORMAT_R16G16B16A16_FLOAT,
            .clear_color = CLEAR_COLOR,
            .sample_count = 4,
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
        .sample_desc(_render_targets.sample_desc())
        .build(device, _pipeline, dx_name(NAME, "Pipeline"));

    // Constants.
    _constants.create(device, 1, dx_name(NAME, "Constants"));

    // Geometry.
    const auto mesh = assets.sci_fi_case_mesh();
    _vertices.create_with_data(device, mesh.vertices, dx_name(NAME, "Vertices"));
    _indices.create_with_data(device, mesh.indices, dx_name(NAME, "Indices"));

    // Textures.
    {
        for (auto& [texture, gpu_texture, name] : std::to_array({
                 std::make_tuple(
                     assets.sci_fi_case_base_color_texture(),
                     &_base_color_texture,
                     "Base Color"
                 ),
                 std::make_tuple(assets.sci_fi_case_normal_texture(), &_normal_texture, "Normal"),
                 std::make_tuple(
                     assets.sci_fi_case_metallic_roughness_texture(),
                     &_metallic_roughness_texture,
                     "Metallic Roughness"
                 ),
             })) {
            gpu_texture->create(
                device,
                GpuTextureDesc {
                    .format = texture.format,
                    .width = texture.width,
                    .height = texture.height,
                },
                dx_name(NAME, name)
            );
            device.transfer().resource(
                gpu_texture->resource(),
                D3D12_SUBRESOURCE_DATA {
                    .pData = texture.data.data(),
                    .RowPitch = texture.row_pitch,
                    .SlicePitch = texture.slice_pitch},
                D3D12_RESOURCE_STATE_COMMON,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
            );
        }
    }
}

auto CubeDemo::gui(const GuiDesc&) -> void {
    auto& p = _parameters;
    ImGui::Combo(
        "Output Mode",
        (int*)&p.output_mode,
        "Shaded\0"
        "ShadingNormal\0"
        "Lighting\0"
        "VertexLighting\0"
        "BaseColorTexture\0"
        "NormalTexture\0"
        "Metallic\0"
        "Roughness\0"
        "VertexTexCoord\0"
        "VertexNormal\0"
        "VertexTangent\0"
        "VertexBitangent\0"
    );

    ImGui::SliderFloat("Camera Distance", &p.camera_distance, 0.0f, 10.0f);
    ImGui::SliderAngle("Camera FOV", &p.camera_fov, 0.0f, 90.0f);
    ImGui::SliderAngle("Camera Latitude", &p.camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera Longitude", &p.camera_longitude, 0.0f, 360.0f);
    ImGui::SliderFloat("Camera Rotation Speed", &p.camera_rotation_speed, 0.0f, 2.0f);

    ImGui::SliderAngle("Light Latitude", &p.light_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Light Longitude", &p.light_longitude, 0.0f, 360.0f);
    ImGui::SliderFloat("Light Rotation Speed", &p.light_rotation_speed, 0.0f, 2.0f);
    ImGui::SliderFloat("Light Ambient", &p.light_ambient, 0.0f, 1.0f);
}

auto CubeDemo::update(const UpdateDesc& desc) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, dx_name(NAME, "Update").data());

    auto& p = _parameters;

    // Update camera.
    p.camera_longitude += p.camera_rotation_speed * desc.delta_time;
    if (p.camera_longitude > PI * 2.0f) {
        p.camera_longitude -= PI * 2.0f;
    }
    const auto projection =
        Float4x4::CreatePerspectiveFieldOfView(p.camera_fov, desc.aspect_ratio, 0.1f, 100.0f);
    const auto eye = p.camera_distance * dir_from_lonlat(p.camera_longitude, p.camera_latitude);
    const auto view = Float4x4::CreateLookAt(eye, Float3::Zero, Float3::Up);
    const auto camera_transform = view * projection;

    // Update light.
    p.light_longitude += p.light_rotation_speed * desc.delta_time;
    if (p.light_longitude > PI * 2.0f) {
        p.light_longitude -= PI * 2.0f;
    }
    const auto light_direction = dir_from_lonlat(p.light_longitude, p.light_latitude);

    // Update debug draw.
    _debug_draw.begin(desc.frame_index);
    _debug_draw.transform(camera_transform);
    _debug_draw.axes();
    _debug_draw.line(Float3::Zero, light_direction, COLOR_YELLOW);
    _debug_draw.end();

    // Update constants.
    *_constants.ptr() = Constants {
        .transform = camera_transform,
        .light_direction = light_direction,
        .light_ambient = p.light_ambient,
        .output_mode = p.output_mode,
    };
}

auto CubeDemo::render(GpuDevice& device, GpuCommandList& cmd) -> void {
    cmd.set_graphics();
    _render_targets.begin(device, cmd);
    _debug_draw.render(device, cmd);
    cmd.set_graphics_constants(Bindings {
        .constants = _constants.cbv_descriptor().index(),
        .vertices = _vertices.srv_descriptor().index(),
        .base_color_texture = _base_color_texture.srv_descriptor().index(),
        .normal_texture = _normal_texture.srv_descriptor().index(),
        .metallic_roughness_texture = _metallic_roughness_texture.srv_descriptor().index(),
    });
    cmd.set_pipeline(_pipeline);
    cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd.set_index_buffer(_indices.index_buffer_view());
    cmd.draw_indexed_instanced(_indices.element_size(), 1, 0, 0, 0);
    _render_targets.end(device, cmd);
}

} // namespace fb::demos::cube
