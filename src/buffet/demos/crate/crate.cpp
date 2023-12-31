#include "crate.hpp"

namespace fb::demos::crate {

auto create(Demo& demo, const CreateDesc& desc) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Create", NAME.data());
    DebugScope debug(NAME);

    // Unpack.
    const auto& kitchen_shaders = desc.baked.kitchen.shaders;
    const auto& shaders = desc.baked.buffet.shaders;
    const auto& assets = desc.baked.buffet.assets;
    auto& device = desc.device;

    // Render targets.
    demo.render_targets.create(
        device,
        {
            .size = device.swapchain().size(),
            .color_format = COLOR_FORMAT,
            .color_clear_value = COLOR_CLEAR_VALUE,
            .depth_format = DEPTH_FORMAT,
            .depth_clear_value = DEPTH_CLEAR_VALUE,
            .sample_count = SAMPLE_COUNT,
        }
    );

    // Debug draw.
    demo.debug_draw.create(device, kitchen_shaders, demo.render_targets);

    // Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.crate_draw_vs())
        .pixel_shader(shaders.crate_draw_ps())
        .render_target_formats({demo.render_targets.color_format()})
        .depth_stencil_format(demo.render_targets.depth_format())
        .sample_desc(demo.render_targets.sample_desc())
        .build(device, demo.pipeline, debug.with_name("Pipeline"));

    // Constants.
    demo.constants.create(device, 1, debug.with_name("Constants"));

    // Models.
    for (auto& [model_name, model, mesh, base_color, normal, metallic_roughness] :
         {std::make_tuple(
              "Sci-Fi Crate",
              std::ref(demo.sci_fi_crate),
              assets.sci_fi_case_mesh(),
              assets.sci_fi_case_base_color_texture(),
              assets.sci_fi_case_normal_texture(),
              assets.sci_fi_case_metallic_roughness_texture()
          ),
          std::make_tuple(
              "Metal Plane",
              std::ref(demo.metal_plane),
              assets.metal_plane_mesh(),
              assets.metal_plane_base_color_texture(),
              assets.metal_plane_normal_texture(),
              assets.metal_plane_metallic_roughness_texture()
          )}) {
        DebugScope model_debug(model_name);

        // Geometry.
        model.vertices.create_with_data(device, mesh.vertices, model_debug.with_name("Vertices"));
        model.indices.create_with_data(device, mesh.indices, model_debug.with_name("Indices"));

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
                D3D12_BARRIER_SYNC_PIXEL_SHADING,
                D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
                D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE,
                model_debug.with_name(texture_name)
            );
        }
    }

    // Pbr.
    {
        const auto lut = assets.shanghai_bund_lut();
        const auto irr = assets.shanghai_bund_irr();
        const auto rad = assets.shanghai_bund_rad();
        demo.pbr_lut.create_and_transfer_baked(
            device,
            lut,
            D3D12_BARRIER_SYNC_PIXEL_SHADING,
            D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
            D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE,
            debug.with_name("LUT")
        );
        demo.pbr_irr.create_and_transfer_baked(
            device,
            irr,
            D3D12_BARRIER_SYNC_PIXEL_SHADING,
            D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
            D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE,
            debug.with_name("Irradiance")
        );
        demo.pbr_rad.create_and_transfer_baked(
            device,
            rad,
            D3D12_BARRIER_SYNC_PIXEL_SHADING,
            D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
            D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE,
            debug.with_name("Radiance")
        );
    }
}

auto gui(Demo& demo, const GuiDesc&) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Gui", NAME.data());
    auto& params = demo.parameters;
    ImGui::Combo(
        "Output Mode",
        (int*)&params.output_mode,
        "Shaded\0"
        "EnvDiffuse\0"
        "EnvSpecular\0"
        "EnvLut\0"
        "EnvIrradiance\0"
        "EnvRadiance\0"
        "DirectLighting\0"
        "DirectBrdf\0"
        "VertexLighting\0"
        "ShadingNormal\0"
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

    ImGui::SliderFloat("Camera Distance", &params.camera_distance, 0.0f, 10.0f);
    ImGui::SliderAngle("Camera FOV", &params.camera_fov, 0.0f, 90.0f);
    ImGui::SliderAngle("Camera Latitude", &params.camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera Longitude", &params.camera_longitude, 0.0f, 360.0f);
    ImGui::SliderFloat("Camera Rotation Speed", &params.camera_rotation_speed, 0.0f, 2.0f);

    ImGui::SliderAngle("Light Latitude", &params.light_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Light Longitude", &params.light_longitude, 0.0f, 360.0f);
    ImGui::SliderFloat("Light Rotation Speed", &params.light_rotation_speed, 0.0f, 2.0f);
    ImGui::SliderFloat("Light Ambient", &params.light_ambient, 0.0f, 1.0f);
    ImGui::SliderFloat("Light Intensity", &params.light_intensity, 0.0f, 8.0f);
}

auto update(Demo& demo, const UpdateDesc& desc) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Update", NAME.data());
    auto& params = demo.parameters;

    // Update camera.
    params.camera_longitude += params.camera_rotation_speed * desc.delta_time;
    if (params.camera_longitude > PI * 2.0f) {
        params.camera_longitude -= PI * 2.0f;
    }
    const auto projection =
        float4x4::CreatePerspectiveFieldOfView(params.camera_fov, desc.aspect_ratio, 0.1f, 100.0f);
    const auto eye =
        params.camera_distance * dir_from_lonlat(params.camera_longitude, params.camera_latitude);
    const auto view = float4x4::CreateLookAt(eye, float3::Zero, float3::Up);
    const auto camera_transform = view * projection;

    // Update light.
    params.light_longitude += params.light_rotation_speed * desc.delta_time;
    if (params.light_longitude > PI * 2.0f) {
        params.light_longitude -= PI * 2.0f;
    }
    const auto light_direction = dir_from_lonlat(params.light_longitude, params.light_latitude);

    // Update debug draw.
    demo.debug_draw.begin(desc.frame_index);
    demo.debug_draw.transform(camera_transform);
    demo.debug_draw.scaled_axes(4.0f);
    {
        // Visualize light direction.
        const auto xyz = light_direction;
        const auto xz = xyz * float3(1.0f, 0.0f, 1.0f);
        const auto scale = 2.0f;
        demo.debug_draw.line(float3::Zero, scale * xyz, COLOR_YELLOW);
        demo.debug_draw.line(float3::Zero, scale * xz, COLOR_YELLOW);
        demo.debug_draw.line(scale * xyz, scale * xz, COLOR_YELLOW);
    }
    demo.debug_draw.end();

    // Update constants.
    demo.constants.buffer(desc.frame_index).ref() = Constants {
        .transform = camera_transform,
        .light_direction = light_direction,
        .light_ambient = params.light_ambient,
        .light_intensity = params.light_intensity,
        .camera_position = eye,
        .output_mode = params.output_mode,
        .rad_texture_mip_count = demo.pbr_rad.mip_count(),
    };
}

auto render(Demo& demo, const RenderDesc& desc) -> void {
    auto& [cmd, device, frame_index] = desc;
    cmd.graphics_scope([&demo, frame_index](GpuGraphicsCommandList& cmd) {
        cmd.begin_pix("%s - Render", NAME.data());

        demo.render_targets.set(cmd);
        demo.debug_draw.render(cmd);

        cmd.begin_pix("Models");
        cmd.set_pipeline(demo.pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        for (const auto& [model, sampler] :
             {std::make_tuple(std::cref(demo.sci_fi_crate), GpuSampler::AnisotropicLinearClamp),
              std::make_tuple(std::cref(demo.metal_plane), GpuSampler::AnisotropicLinearWrap)}) {
            cmd.set_constants(Bindings {
                .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
                .vertices = model.vertices.srv_descriptor().index(),
                .base_color_texture = model.base_color.srv_descriptor().index(),
                .normal_texture = model.normal.srv_descriptor().index(),
                .metallic_roughness_texture = model.metallic_roughness.srv_descriptor().index(),
                .sampler = (uint)sampler,
                .lut_texture = demo.pbr_lut.srv_descriptor().index(),
                .irr_texture = demo.pbr_irr.srv_descriptor().index(),
                .rad_texture = demo.pbr_rad.srv_descriptor().index(),
            });
            cmd.set_index_buffer(model.indices.index_buffer_view());
            cmd.draw_indexed_instanced(model.indices.element_count(), 1, 0, 0, 0);
        }
        cmd.end_pix();

        cmd.end_pix();
    });
}

} // namespace fb::demos::crate
