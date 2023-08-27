#include "env.hpp"

namespace fb::demos::env {

auto EnvDemo::create(GpuDevice& device, const baked::Assets& assets, const baked::Shaders& shaders)
    -> void {
    // Render targets.
    _render_targets.create(
        device,
        {
            .size = device.swapchain().size(),
            .color_format = DXGI_FORMAT_R16G16B16A16_FLOAT,
            .clear_color = CLEAR_COLOR,
            .sample_count = 1,
        },
        NAME
    );

    // Debug draw.
    _debug_draw.create(device, shaders, _render_targets, NAME);

    // Background.
    {
        static constexpr auto PASS_NAME = "Background"sv;
        auto& pass = _background;

        // Constants.
        pass.constants.create(device, 1, dx_name(NAME, PASS_NAME, "Constants"));

        // Geometry.
        const auto mesh = assets.skybox_mesh();
        pass.vertices.create_with_data(device, mesh.vertices, dx_name(NAME, PASS_NAME, "Vertices"));
        pass.indices.create_with_data(device, mesh.indices, dx_name(NAME, PASS_NAME, "Indices"));

        // Texture.
        const auto texture = assets.winter_evening_cube_texture();
        std::array<GpuTextureTransferDesc, 6> transfers;
        for (uint32_t i = 0; i < 6; ++i) {
            transfers[i] = {
                .row_pitch = texture.row_pitch,
                .slice_pitch = texture.slice_pitch,
                .data = texture.datas[i].data(),
            };
        }
        pass.texture.create_and_transfer(
            device,
            GpuTextureDesc {
                .format = texture.format,
                .width = texture.width,
                .height = texture.height,
                .depth = 6,
            },
            transfers,
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            dx_name(NAME, PASS_NAME, "Texture")
        );

        // Pipeline.
        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(shaders.env_background_vs())
            .pixel_shader(shaders.env_background_ps())
            .depth_stencil(GpuDepthStencilDesc {
                .depth_read = true,
                .depth_write = true,
                .depth_func = GpuComparisonFunc::LessEqual,
            })
            .render_target_formats({_render_targets.color_format()})
            .depth_stencil_format(_render_targets.depth_format())
            .build(device, pass.pipeline, dx_name(NAME, PASS_NAME, "Pipeline"));
    }

    // Compute.
    {
        static constexpr auto PASS_NAME = "Compute"sv;
        auto& pass = _compute;

        // Constants.
        pass.constants.create(device, 1, dx_name(NAME, PASS_NAME, "Constants"));

        // Rect texture.
        const auto texture = assets.farm_field_hdr_texture();
        pass.rect_texture.create_and_transfer_baked(
            device,
            texture,
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            dx_name(NAME, PASS_NAME, "Rect Texture")
        );
        pass.rect_texture_size = Uint2(texture.width, texture.height);

        // Cube texture.
        const auto cube_texture_size = Uint2(1024, 1024);
        pass.cube_texture.create(
            device,
            GpuTextureDesc {
                .format = DXGI_FORMAT_R16G16B16A16_FLOAT,
                .width = cube_texture_size.x,
                .height = cube_texture_size.y,
                .depth = 6,
            },
            dx_name(NAME, PASS_NAME, "Cube Texture")
        );
        pass.cube_texture_size = cube_texture_size;

        // Pipeline.
        GpuPipelineBuilder()
            .compute_shader(shaders.env_cube_from_rect_cs())
            .build(device, pass.pipeline, dx_name(NAME, PASS_NAME, "Pipeline"));
    }

    // Screen.
    {
        static constexpr auto PASS_NAME = "Screen"sv;
        auto& pass = _screen;

        // Input rect layout.
        pass.input_offset = Float2(0.0f, 0.0f);
        pass.input_scale = Float2(400.0f, 200.0f);

        // Cube face layout (left-handed).
        //    +y
        // -x +z +x -z
        //    -y
        pass.output_offsets = {
            Float2(200.0f, 200.0f), // +x
            Float2(0.0f, 200.0f),   // -x
            Float2(100.0f, 100.0f), // +y
            Float2(100.0f, 300.0f), // -y
            Float2(100.0f, 200.0f), // +z
            Float2(300.0f, 200.0f), // -z
        };
        for (auto& offset : pass.output_offsets) {
            offset += Float2(0.0f, 100.0f);
        }
        pass.output_scale = Float2(100.0f, 100.0f);

        // Constants.
        pass.constants.create(device, 1, dx_name(NAME, PASS_NAME, "Constants"));

        // Geometry.
        const auto vertices = std::to_array({
            ScreenVertex {Float2(0.0f, 0.0f), Float2(0.0f, 0.0f)},
            ScreenVertex {Float2(0.0f, 1.0f), Float2(0.0f, 1.0f)},
            ScreenVertex {Float2(1.0f, 1.0f), Float2(1.0f, 1.0f)},
            ScreenVertex {Float2(1.0f, 0.0f), Float2(1.0f, 0.0f)},
        });
        const auto indices = std::to_array({0u, 1u, 2u, 0u, 2u, 3u});
        pass.vertices.create_with_data(device, vertices, dx_name(NAME, PASS_NAME, "Vertices"));
        pass.indices.create_with_data(device, indices, dx_name(NAME, PASS_NAME, "Indices"));

        // Pipeline.
        GpuPipelineBuilder()
            .vertex_shader(shaders.env_screen_vs())
            .pixel_shader(shaders.env_screen_ps())
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .depth_stencil(GpuDepthStencilDesc {
                .depth_read = false,
                .depth_write = false,
            })
            .rasterizer(GpuRasterizerDesc {
                .cull_mode = GpuCullMode::None,
            })
            .render_target_formats({_render_targets.color_format()})
            .build(device, pass.pipeline, dx_name(NAME, PASS_NAME, "Pipeline"));
    }
}

auto EnvDemo::gui(const GuiDesc& desc) -> void {
    auto& p = _parameters;
    ImGui::Checkbox("Tonemap", (bool*)&p.tonemap);
    ImGui::SliderAngle("Camera FOV", &p.camera_fov, 1.0f, 90.0f);
    ImGui::SliderFloat("Camera Distance", &p.camera_distance, 1.0f, 10.0f);
    ImGui::SliderAngle("Camera Longitude", &p.camera_longitude, -180.0f, 180.0f);
    ImGui::SliderAngle("Camera Latitude", &p.camera_latitude, -90.0f, 90.0f);
    ImGui::SliderFloat("Camera Rotation Speed", &p.camera_rotation_speed, 0.0f, 1.0f);

    {
        auto& pass = _screen;
        const auto w = (float)desc.window_size.x;
        const auto h = (float)desc.window_size.y;
        const auto s = std::max(w, h);
        ImGui::SliderFloat2("Input Offset", &pass.input_offset.x, 0.0f, s);
        ImGui::SliderFloat2("Input Scale", &pass.input_scale.x, 0.0f, s);
    }
}

auto EnvDemo::update(const UpdateDesc& desc) -> void {
    // Update camera rotation.
    {
        auto& p = _parameters;
        p.camera_longitude += p.camera_rotation_speed * desc.delta_time;
        if (p.camera_longitude > PI * 2.0f) {
            p.camera_longitude -= PI * 2.0f;
        }
    }

    // Update camera transforms.
    const auto& p = _parameters;
    const auto aspect_ratio = desc.aspect_ratio;
    const auto perspective =
        Float4x4::CreatePerspectiveFieldOfView(p.camera_fov, aspect_ratio, 0.1f, 100.0f);
    const auto eye = p.camera_distance * dir_from_lonlat(p.camera_longitude, p.camera_latitude);
    const auto view = Float4x4::CreateLookAt(eye, Float3::Zero, Float3::Up);

    // Update debug.
    {
        const auto camera_transform = view * perspective;
        _debug_draw.begin(desc.frame_index);
        _debug_draw.transform(camera_transform);
        _debug_draw.axes();
        _debug_draw.end();
    }

    // Update background constants.
    {
        auto env_view = view;
        env_view.m[3][0] = 0.0f;
        env_view.m[3][1] = 0.0f;
        env_view.m[3][2] = 0.0f;
        env_view.m[3][3] = 1.0f;
        const auto env_transform = env_view * perspective;

        *_background.constants.ptr() = BackgroundConstants {
            .transform = env_transform,
            .tonemap = p.tonemap,
        };
    }

    // Update compute constants.
    {
        *_compute.constants.ptr() = ComputeConstants {
            .rect_texture_size =
                Float2((float)_compute.rect_texture_size.x, (float)_compute.rect_texture_size.y),
            .cube_texture_size =
                Float2((float)_compute.cube_texture_size.x, (float)_compute.cube_texture_size.y),
        };
    }

    // Update screen constants.
    {
        const auto w = (float)desc.window_size.x;
        const auto h = (float)desc.window_size.y;
        Float4x4 screen_transform = Float4x4(
            Float4(2.0f / w, 0.0f, 0.0f, 0.0f),
            Float4(0.0f, -2.0f / h, 0.0f, 0.0f),
            Float4(0.0f, 0.0f, 1.0f, 0.0f),
            Float4(-1.0f, 1.0f, 0.0f, 1.0f)
        );
        *_screen.constants.ptr() = ScreenConstants {
            .transform = screen_transform,
            .tonemap = p.tonemap,
        };
    }
}

auto EnvDemo::render(GpuDevice& device, GpuCommandList& cmd) -> void {
    // Compute pass.
    {
        auto& pass = _compute;

        pass.cube_texture.transition(cmd, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        cmd.flush_barriers();

        cmd.set_compute();
        cmd.set_compute_constants(ComputeBindings {
            .constants = pass.constants.cbv_descriptor().index(),
            .rect_texture = pass.rect_texture.srv_descriptor().index(),
            .cube_texture = pass.cube_texture.uav_descriptor().index(),
        });
        cmd.set_pipeline(pass.pipeline);
        cmd.dispatch(
            pass.cube_texture_size.x / CUBE_FROM_RECT_DISPATCH_X,
            pass.cube_texture_size.y / CUBE_FROM_RECT_DISPATCH_Y,
            6
        );

        pass.cube_texture.transition(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        cmd.flush_barriers();
    }

    // Graphics passes.
    cmd.set_graphics();
    _render_targets.set(cmd);

    // Debug.
    _debug_draw.render(device, cmd);

    // Background.
    {
        const auto& background_pass = _background;
        const auto& compute_pass = _compute;
        cmd.set_graphics_constants(BackgroundBindings {
            .constants = background_pass.constants.cbv_descriptor().index(),
            .vertices = background_pass.vertices.srv_descriptor().index(),
            .texture = compute_pass.cube_texture.srv_descriptor().index(),
        });
        cmd.set_pipeline(background_pass.pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(background_pass.indices.index_buffer_view());
        cmd.draw_indexed_instanced(background_pass.indices.element_size(), 1, 0, 0, 0);
    }

    // Screen.
    {
        const auto& compute_pass = _compute;
        const auto& screen_pass = _screen;
        cmd.set_pipeline(screen_pass.pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(screen_pass.indices.index_buffer_view());

        cmd.set_graphics_constants(ScreenBindings {
            .constants = screen_pass.constants.cbv_descriptor().index(),
            .vertices = screen_pass.vertices.srv_descriptor().index(),
            .texture = compute_pass.rect_texture.srv_descriptor().index(),
            .texture_slice = 0,
            .screen_offset = screen_pass.input_offset,
            .screen_scale = screen_pass.input_scale,
        });
        cmd.draw_indexed_instanced(screen_pass.indices.element_size(), 1, 0, 0, 0);

        for (uint32_t i = 0; i < 6; ++i) {
            cmd.set_graphics_constants(ScreenBindings {
                .constants = screen_pass.constants.cbv_descriptor().index(),
                .vertices = screen_pass.vertices.srv_descriptor().index(),
                .texture = compute_pass.cube_texture.alt_srv_descriptor().index(),
                .texture_slice = i,
                .screen_offset = screen_pass.output_offsets[i],
                .screen_scale = screen_pass.output_scale,
            });
            cmd.draw_indexed_instanced(screen_pass.indices.element_size(), 1, 0, 0, 0);
        }
    }
}

} // namespace fb::demos::env
