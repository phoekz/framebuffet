#include "env.hpp"

namespace fb::demos::env {

auto EnvDemo::create(GpuDevice& device, const baked::Assets& assets, const baked::Shaders& shaders)
    -> void {
    // Render targets.
    _render_targets.create(
        device,
        {
            .size = device.swapchain().size(),
            .color_format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .clear_color = CLEAR_COLOR,
            .sample_count = 1,
        },
        NAME
    );

    // Debug draw.
    _debug_draw.create(device, shaders, _render_targets, NAME);

    // Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.env_draw_vs())
        .pixel_shader(shaders.env_draw_ps())
        .depth_stencil(GpuDepthStencilDesc {
            .depth_read = true,
            .depth_write = true,
            .depth_func = GpuComparisonFunc::LessEqual,
        })
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
        std::array<GpuTextureTransferDesc, 6> transfers;
        for (uint32_t i = 0; i < 6; ++i) {
            transfers[i] = {
                .row_pitch = env_texture.row_pitch,
                .slice_pitch = env_texture.slice_pitch,
                .data = env_texture.datas[i].data(),
            };
        }
        _env_texture.create_and_transfer(
            device,
            GpuTextureDesc {
                .format = env_texture.format,
                .width = env_texture.width,
                .height = env_texture.height,
                .depth = 6,
            },
            transfers,
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            dx_name(NAME, "Env Texture")
        );
    }
}

auto EnvDemo::gui(const GuiDesc&) -> void {
    auto& p = _parameters;
    ImGui::SliderAngle("Camera FOV", &p.camera_fov, 1.0f, 90.0f);
    ImGui::SliderFloat("Camera Distance", &p.camera_distance, 1.0f, 10.0f);
    ImGui::SliderAngle("Camera Longitude", &p.camera_longitude, -180.0f, 180.0f);
    ImGui::SliderAngle("Camera Latitude", &p.camera_latitude, -90.0f, 90.0f);
    ImGui::SliderFloat("Camera Rotation Speed", &p.camera_rotation_speed, 0.0f, 1.0f);
}

auto EnvDemo::update(const UpdateDesc& desc) -> void {
    auto& p = _parameters;

    p.camera_longitude += p.camera_rotation_speed * desc.delta_time;
    if (p.camera_longitude > PI * 2.0f) {
        p.camera_longitude -= PI * 2.0f;
    }

    float aspect_ratio = desc.aspect_ratio;
    Float4x4 perspective =
        Float4x4::CreatePerspectiveFieldOfView(p.camera_fov, aspect_ratio, 0.1f, 100.0f);
    Float3 eye = p.camera_distance * dir_from_lonlat(p.camera_longitude, p.camera_latitude);
    Float4x4 view = Float4x4::CreateLookAt(eye, Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 1.0f, 0.0f));
    Float4x4 env_view = view;
    env_view.m[3][0] = 0.0f;
    env_view.m[3][1] = 0.0f;
    env_view.m[3][2] = 0.0f;
    env_view.m[3][3] = 1.0f;
    Float4x4 env_transform = env_view * perspective;

    Float4x4 camera_transform = view * perspective;
    _debug_draw.begin(desc.frame_index);
    _debug_draw.transform(camera_transform);
    _debug_draw.axes();
    _debug_draw.end();

    *_constants.ptr() = Constants {
        .transform = env_transform,
    };
}

auto EnvDemo::render(GpuDevice& device, GpuCommandList& cmd) -> void {
    cmd.set_graphics();
    _render_targets.set(cmd);
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
}

} // namespace fb::demos::env
