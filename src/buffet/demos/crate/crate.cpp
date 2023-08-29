#include "crate.hpp"
#include <kitchen/gpu/samplers.hlsli>

namespace fb::demos::crate {

auto CrateDemo::create(GpuDevice& device, const Baked& baked) -> void {
    // Render targets.
    _render_targets.create(
        device,
        {
            .size = device.swapchain().size(),
            .color_format = DXGI_FORMAT_R16G16B16A16_FLOAT,
            .clear_color = CLEAR_COLOR,
            .sample_count = 4,
        },
        NAME
    );

    // Debug draw.
    _debug_draw.create(device, baked.kitchen.shaders, _render_targets, NAME);

    // Unpack.
    const auto& shaders = baked.buffet.shaders;
    const auto& assets = baked.buffet.assets;

    // Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.crate_draw_vs())
        .pixel_shader(shaders.crate_draw_ps())
        .render_target_formats({_render_targets.color_format()})
        .depth_stencil_format(_render_targets.depth_format())
        .sample_desc(_render_targets.sample_desc())
        .build(device, _pipeline, dx_name(NAME, "Pipeline"));

    // Constants.
    _constants.create(device, 1, dx_name(NAME, "Constants"));

    // Models.
    for (auto& [model_name, model, mesh, base_color, normal, metallic_roughness] :
         {std::make_tuple(
              "Sci-Fi Crate",
              std::ref(_sci_fi_crate),
              assets.sci_fi_case_mesh(),
              assets.sci_fi_case_base_color_texture(),
              assets.sci_fi_case_normal_texture(),
              assets.sci_fi_case_metallic_roughness_texture()
          ),
          std::make_tuple(
              "Metal Plane",
              std::ref(_metal_plane),
              assets.metal_plane_mesh(),
              assets.metal_plane_base_color_texture(),
              assets.metal_plane_normal_texture(),
              assets.metal_plane_metallic_roughness_texture()
          )}) {
        // Geometry.
        model.vertices
            .create_with_data(device, mesh.vertices, dx_name(NAME, model_name, "Vertices"));
        model.indices.create_with_data(device, mesh.indices, dx_name(NAME, model_name, "Indices"));

        // Textures.
        for (auto& [texture_name, dst_texture, src_texture] : {
                 std::make_tuple("Base Color", std::ref(model.base_color), base_color),
                 std::make_tuple("Normal", std::ref(model.normal), normal),
                 std::make_tuple(
                     "Metallic Roughness",
                     std::ref(model.metallic_roughness),
                     metallic_roughness
                 ),
             }) {
            dst_texture.create_and_transfer_baked(
                device,
                src_texture,
                D3D12_RESOURCE_STATE_COMMON,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                dx_name(NAME, model_name, texture_name)
            );
        }
    }
}

auto CrateDemo::gui(const GuiDesc&) -> void {
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
        "LevelOfDetail\0"
        "DirectionToViewer\0"
        "DirectionToLight\0"
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
    ImGui::SliderFloat("Light Intensity", &p.light_intensity, 0.0f, 8.0f);
}

auto CrateDemo::update(const UpdateDesc& desc) -> void {
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
    _debug_draw.scaled_axes(4.0f);
    {
        // Visualize light direction.
        const auto xyz = light_direction;
        const auto xz = xyz * Float3(1.0f, 0.0f, 1.0f);
        const auto scale = 2.0f;
        _debug_draw.line(Float3::Zero, scale * xyz, COLOR_YELLOW);
        _debug_draw.line(Float3::Zero, scale * xz, COLOR_YELLOW);
        _debug_draw.line(scale * xyz, scale * xz, COLOR_YELLOW);
    }
    _debug_draw.end();

    // Update constants.
    *_constants.ptr() = Constants {
        .transform = camera_transform,
        .light_direction = light_direction,
        .light_ambient = p.light_ambient,
        .light_intensity = p.light_intensity,
        .camera_position = eye,
        .output_mode = p.output_mode,
    };
}

auto CrateDemo::render(GpuDevice& device, GpuCommandList& cmd) -> void {
    cmd.set_graphics();
    _render_targets.set(cmd);
    _debug_draw.render(device, cmd);

    cmd.set_pipeline(_pipeline);
    cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    for (const auto& [model, sampler] :
         {std::make_tuple(std::cref(_sci_fi_crate), GpuSampler::AnisotropicLinearClamp),
          std::make_tuple(std::cref(_metal_plane), GpuSampler::AnisotropicLinearWrap)}) {
        cmd.set_graphics_constants(Bindings {
            .constants = _constants.cbv_descriptor().index(),
            .vertices = model.vertices.srv_descriptor().index(),
            .base_color_texture = model.base_color.srv_descriptor().index(),
            .normal_texture = model.normal.srv_descriptor().index(),
            .metallic_roughness_texture = model.metallic_roughness.srv_descriptor().index(),
            .sampler = (uint32_t)sampler,
        });
        cmd.set_index_buffer(model.indices.index_buffer_view());
        cmd.draw_indexed_instanced(model.indices.element_size(), 1, 0, 0, 0);
    }
}

} // namespace fb::demos::crate
