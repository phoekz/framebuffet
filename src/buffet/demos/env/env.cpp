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
        const auto texture = assets.winter_evening_hdr_texture();
        pass.rect_texture.create_and_transfer_baked(
            device,
            texture,
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            dx_name(NAME, PASS_NAME, "Rect Texture")
        );
        pass.rect_texture_size = Uint2(texture.width, texture.height);

        // Cube texture.
        pass.cube_texture_size = Uint2(1024, 1024);
        pass.cube_texture.create(
            device,
            GpuTextureDesc {
                .format = DXGI_FORMAT_R16G16B16A16_FLOAT,
                .width = pass.cube_texture_size.x,
                .height = pass.cube_texture_size.y,
                .depth = 6,
            },
            dx_name(NAME, PASS_NAME, "Cube Texture")
        );

        // LUT texture.
        pass.lut_texture_size = Uint2(512, 512);
        pass.lut_texture.create(
            device,
            GpuTextureDesc {
                .format = DXGI_FORMAT_R16G16_FLOAT,
                .width = pass.lut_texture_size.x,
                .height = pass.lut_texture_size.y,
                .depth = 1,
            },
            dx_name(NAME, PASS_NAME, "LUT Texture")
        );
        pass.lut_sample_count = 1024;

        // Irradiance cube texture.
        pass.irr_texture_size = Uint2(64, 64);
        pass.irr_texture.create(
            device,
            GpuTextureDesc {
                .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
                .width = pass.irr_texture_size.x,
                .height = pass.irr_texture_size.y,
                .depth = 6,
            },
            dx_name(NAME, PASS_NAME, "Irradiance Cube Texture")
        );
        pass.irr_sample_count = 1 << 17;
        pass.irr_dispatch_sample_count = 256;

        // Radiance cube texture.
        pass.rad_texture_size = Uint2(1024, 1024);
        pass.rad_texture_mip_count = mip_count_from_size(pass.rad_texture_size);
        pass.rad_texture.create(
            device,
            GpuTextureDesc {
                .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
                .width = pass.rad_texture_size.x,
                .height = pass.rad_texture_size.y,
                .depth = 6,
                .mip_count = pass.rad_texture_mip_count,
            },
            dx_name(NAME, PASS_NAME, "Radiance Cube Texture")
        );
        pass.rad_sample_count = 1 << 17;
        pass.rad_dispatch_sample_count = 256;

        // Pipelines.
        GpuPipelineBuilder()
            .compute_shader(shaders.env_cfr_cs())
            .build(device, pass.cfr_pipeline, dx_name(NAME, PASS_NAME, "CFR Pipeline"));
        GpuPipelineBuilder()
            .compute_shader(shaders.env_lut_cs())
            .build(device, pass.lut_pipeline, dx_name(NAME, PASS_NAME, "LUT Pipeline"));
        GpuPipelineBuilder()
            .compute_shader(shaders.env_irr_cs())
            .build(device, pass.irr_pipeline, dx_name(NAME, PASS_NAME, "Irradiance Pipeline"));
        GpuPipelineBuilder()
            .compute_shader(shaders.env_rad_cs())
            .build(device, pass.rad_pipeline, dx_name(NAME, PASS_NAME, "Radiance Pipeline"));
    }

    // Screen.
    {
        static constexpr auto PASS_NAME = "Screen"sv;
        auto& pass = _screen;

        // Rect layout.
        pass.rect_offset = Float2(0.0f, 0.0f);
        pass.rect_scale = Float2(400.0f, 200.0f);

        // Cube face layout (left-handed).
        //    +y
        // -x +z +x -z
        //    -y
        pass.cube_offsets = {
            Float2(200.0f, 200.0f), // +x
            Float2(0.0f, 200.0f),   // -x
            Float2(100.0f, 100.0f), // +y
            Float2(100.0f, 300.0f), // -y
            Float2(100.0f, 200.0f), // +z
            Float2(300.0f, 200.0f), // -z
        };
        for (auto& offset : pass.cube_offsets) {
            offset += Float2(0.0f, 100.0f);
        }
        pass.cube_scale = Float2(100.0f, 100.0f);

        // LUT layout.
        pass.lut_offset = Float2(400.0f, 000.0f);
        pass.lut_scale = Float2(200.0f, 200.0f);

        // Irradiance face layout (left-handed).
        //    +y
        // -x +z +x -z
        //    -y
        pass.irr_offsets = {
            Float2(200.0f, 200.0f), // +x
            Float2(0.0f, 200.0f),   // -x
            Float2(100.0f, 100.0f), // +y
            Float2(100.0f, 300.0f), // -y
            Float2(100.0f, 200.0f), // +z
            Float2(300.0f, 200.0f), // -z
        };
        for (auto& offset : pass.irr_offsets) {
            offset += Float2(0.0f, 400.0f);
        }
        pass.irr_scale = Float2(100.0f, 100.0f);

        // Radiance face layout.
        FB_ASSERT(_compute.rad_texture_mip_count == 11);
        pass.rad_scale = Float2(50.0f, 50.0f);
        for (uint32_t mip_id = 0; mip_id < 11; mip_id++) {
            for (uint32_t face_id = 0; face_id < 6; face_id++) {
                auto& offset = pass.rad_offsets[mip_id * 6 + face_id];
                offset = Float2(face_id * pass.rad_scale.x, mip_id * pass.rad_scale.y);
                offset += Float2((float)device.swapchain().size().x, 0.0f);
                offset -= Float2(6 * pass.rad_scale.x, 0.0f);
            }
        }

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

auto EnvDemo::gui(const GuiDesc&) -> void {
    auto& p = _parameters;
    if (ImGui::Button("Start")) {
        _compute.irr_started = true;
        _compute.rad_started = true;
    }
    ImGui::ProgressBar(
        _compute.irr_dispatch_index
            / (float)((_compute.irr_sample_count / _compute.irr_dispatch_sample_count)),
        ImVec2()
    );
    ImGui::SameLine();
    ImGui::Text("Irradiance");
    ImGui::ProgressBar(
        _compute.rad_dispatch_index
            / (float)((_compute.rad_sample_count / _compute.rad_dispatch_sample_count)),
        ImVec2()
    );
    ImGui::SameLine();
    ImGui::Text("Radiance");
    ImGui::Checkbox("Tonemap", (bool*)&p.tonemap);
    ImGui::SliderFloat("Exposure", &p.exposure, -10.0f, 10.0f);
    ImGui::SliderFloat("BG Roughness", &p.background_roughness, 0.0f, 1.0f);
    ImGui::SliderAngle("Camera FOV", &p.camera_fov, 1.0f, 90.0f);
    ImGui::SliderFloat("Camera Distance", &p.camera_distance, 1.0f, 10.0f);
    ImGui::SliderAngle("Camera Longitude", &p.camera_longitude, -180.0f, 180.0f);
    ImGui::SliderAngle("Camera Latitude", &p.camera_latitude, -90.0f, 90.0f);
    ImGui::SliderFloat("Camera Rotation Speed", &p.camera_rotation_speed, 0.0f, 1.0f);
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

    // Update exposure.
    const auto exposure = std::pow(2.0f, -p.exposure);

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
            .exposure = exposure,
            .roughness = p.background_roughness,
            .mip_count = _compute.rad_texture_mip_count,
        };
    }

    // Update compute constants.
    {
        *_compute.constants.ptr() = ComputeConstants {
            .rect_texture_size = _compute.rect_texture_size,
            .cube_texture_size = _compute.cube_texture_size,
            .lut_texture_size = _compute.lut_texture_size,
            .lut_sample_count = _compute.lut_sample_count,
            .irr_texture_size = _compute.irr_texture_size,
            .irr_sample_count = _compute.irr_sample_count,
            .irr_dispatch_sample_count = _compute.irr_dispatch_sample_count,
            .rad_texture_size = _compute.rad_texture_size,
            .rad_texture_mip_count = _compute.rad_texture_mip_count,
            .rad_sample_count = _compute.rad_sample_count,
            .rad_dispatch_sample_count = _compute.rad_dispatch_sample_count,
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
            .exposure = exposure,
        };
    }
}

auto EnvDemo::render(GpuDevice& device, GpuCommandList& cmd) -> void {
    // Compute pass.
    {
        auto& pass = _compute;
        cmd.set_compute();

        pass.cube_texture.transition(cmd, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        pass.lut_texture.transition(cmd, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        pass.irr_texture.transition(cmd, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        pass.rad_texture.transition(cmd, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        cmd.flush_barriers();

        if (!pass.cfr_completed) {
            cmd.set_pipeline(pass.cfr_pipeline);
            cmd.set_compute_constants(ComputeBindings {
                .constants = pass.constants.cbv_descriptor().index(),
                .rect_texture = pass.rect_texture.srv_descriptor().index(),
                .cube_texture = pass.cube_texture.uav_descriptor().index(),
            });
            cmd.dispatch(
                pass.cube_texture_size.x / CFR_DISPATCH_X,
                pass.cube_texture_size.y / CFR_DISPATCH_Y,
                6
            );
            pass.cfr_completed = true;
        }

        if (!pass.lut_completed) {
            cmd.set_pipeline(pass.lut_pipeline);
            cmd.set_compute_constants(ComputeBindings {
                .constants = pass.constants.cbv_descriptor().index(),
                .lut_texture = pass.lut_texture.uav_descriptor().index(),
            });
            cmd.dispatch(
                pass.lut_texture_size.x / LUT_DISPATCH_X,
                pass.lut_texture_size.y / LUT_DISPATCH_Y,
                1
            );
            pass.lut_completed = true;
        }

        if (!pass.irr_completed && pass.irr_started) {
            cmd.set_pipeline(pass.irr_pipeline);
            cmd.set_compute_constants(ComputeBindings {
                .constants = pass.constants.cbv_descriptor().index(),
                .cube_texture = pass.cube_texture.srv_descriptor().index(),
                .irr_texture = pass.irr_texture.uav_descriptor().index(),
                .irr_dispatch_index = pass.irr_dispatch_index,
            });
            cmd.dispatch(
                pass.irr_texture_size.x / IRR_DISPATCH_X,
                pass.irr_texture_size.y / IRR_DISPATCH_Y,
                6
            );
            pass.irr_dispatch_index++;
            if (pass.irr_dispatch_index
                == (pass.irr_sample_count / pass.irr_dispatch_sample_count)) {
                pass.irr_completed = true;
            }
        }

        if (!pass.rad_completed && pass.rad_started) {
            cmd.set_pipeline(pass.rad_pipeline);
            for (uint32_t mip = 0; mip < pass.rad_texture_mip_count; mip++) {
                cmd.set_compute_constants(ComputeBindings {
                    .constants = pass.constants.cbv_descriptor().index(),
                    .cube_texture = pass.cube_texture.srv_descriptor().index(),
                    .rad_texture = pass.rad_texture.uav_descriptor().index(),
                    .rad_mip_id = mip,
                    .rad_dispatch_index = pass.rad_dispatch_index,
                });
                const uint32_t mip_width = std::max(1u, pass.rad_texture_size.x >> mip);
                const uint32_t mip_height = std::max(1u, pass.rad_texture_size.y >> mip);
                cmd.dispatch(
                    (mip_width + (RAD_DISPATCH_X - 1)) / RAD_DISPATCH_X,
                    (mip_height + (RAD_DISPATCH_Y - 1)) / RAD_DISPATCH_Y,
                    6
                );
            }
            pass.rad_dispatch_index++;
            if (pass.rad_dispatch_index
                == (pass.rad_sample_count / pass.rad_dispatch_sample_count)) {
                pass.rad_completed = true;
            }
        }

        pass.cube_texture.transition(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        pass.lut_texture.transition(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        pass.irr_texture.transition(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        pass.rad_texture.transition(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
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
            .texture = compute_pass.rad_texture.srv_descriptor().index(),
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

        // Rect texture.
        cmd.set_graphics_constants(ScreenBindings {
            .constants = screen_pass.constants.cbv_descriptor().index(),
            .vertices = screen_pass.vertices.srv_descriptor().index(),
            .texture = compute_pass.rect_texture.srv_descriptor().index(),
            .texture_face_id = 0,
            .texture_mip_id = 0,
            .screen_offset = screen_pass.rect_offset,
            .screen_scale = screen_pass.rect_scale,
        });
        cmd.draw_indexed_instanced(screen_pass.indices.element_size(), 1, 0, 0, 0);

        // Cube textures.
        for (uint32_t i = 0; i < 6; ++i) {
            cmd.set_graphics_constants(ScreenBindings {
                .constants = screen_pass.constants.cbv_descriptor().index(),
                .vertices = screen_pass.vertices.srv_descriptor().index(),
                .texture = compute_pass.cube_texture.alt_srv_descriptor().index(),
                .texture_face_id = i,
                .texture_mip_id = 0,
                .screen_offset = screen_pass.cube_offsets[i],
                .screen_scale = screen_pass.cube_scale,
            });
            cmd.draw_indexed_instanced(screen_pass.indices.element_size(), 1, 0, 0, 0);
        }

        // LUT texture.
        cmd.set_graphics_constants(ScreenBindings {
            .constants = screen_pass.constants.cbv_descriptor().index(),
            .vertices = screen_pass.vertices.srv_descriptor().index(),
            .texture = compute_pass.lut_texture.srv_descriptor().index(),
            .texture_face_id = 0,
            .texture_mip_id = 0,
            .screen_offset = screen_pass.lut_offset,
            .screen_scale = screen_pass.lut_scale,
        });
        cmd.draw_indexed_instanced(screen_pass.indices.element_size(), 1, 0, 0, 0);

        // Irradiance cube textures.
        for (uint32_t i = 0; i < 6; ++i) {
            cmd.set_graphics_constants(ScreenBindings {
                .constants = screen_pass.constants.cbv_descriptor().index(),
                .vertices = screen_pass.vertices.srv_descriptor().index(),
                .texture = compute_pass.irr_texture.alt_srv_descriptor().index(),
                .texture_face_id = i,
                .texture_mip_id = 0,
                .screen_offset = screen_pass.irr_offsets[i],
                .screen_scale = screen_pass.irr_scale,
            });
            cmd.draw_indexed_instanced(screen_pass.indices.element_size(), 1, 0, 0, 0);
        }

        // Radiance cube textures.
        for (uint32_t mip_id = 0; mip_id < 11; mip_id++) {
            for (uint32_t face_id = 0; face_id < 6; face_id++) {
                cmd.set_graphics_constants(ScreenBindings {
                    .constants = screen_pass.constants.cbv_descriptor().index(),
                    .vertices = screen_pass.vertices.srv_descriptor().index(),
                    .texture = compute_pass.rad_texture.alt_srv_descriptor().index(),
                    .texture_face_id = face_id,
                    .texture_mip_id = mip_id,
                    .screen_offset = screen_pass.rad_offsets[mip_id * 6 + face_id],
                    .screen_scale = screen_pass.rad_scale,
                });
                cmd.draw_indexed_instanced(screen_pass.indices.element_size(), 1, 0, 0, 0);
            }
        }
    }
}

} // namespace fb::demos::env
