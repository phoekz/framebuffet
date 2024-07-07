#include "text.hpp"

namespace fb::demos::text {

auto create(Demo& demo, const CreateDesc& desc) -> void {
    ZoneScoped;
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

    // Pbr.
    {
        const auto irr = assets.industrial_sunset_02_puresky_irr();
        demo.pbr.irr.create_and_transfer_baked(
            device,
            irr,
            D3D12_BARRIER_SYNC_PIXEL_SHADING,
            D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
            D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE,
            debug.with_name("Irradiance")
        );
    }

    // Glyph.
    {
        DebugScope pass_debug("Glyph");
        auto& pass = demo.glyph;

        // Pipeline.
        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(shaders.text_glyph_vs())
            .pixel_shader(shaders.text_glyph_ps())
            .render_target_formats({demo.render_targets.color_format()})
            .depth_stencil_format(demo.render_targets.depth_format())
            .sample_desc(demo.render_targets.sample_desc())
            .build(device, pass.pipeline, pass_debug.with_name("Pipeline"));

        // Constants.
        pass.constants.create(device, 1, pass_debug.with_name("Constants"));

        // Geometry.
        const auto mesh_array = assets.roboto_medium_mesh_array();
        pass.submeshes
            .insert(pass.submeshes.end(), mesh_array.submeshes.begin(), mesh_array.submeshes.end());
        pass.vertices
            .create_with_data(device, mesh_array.vertices, pass_debug.with_name("Vertices"));
        pass.indices.create_with_data(device, mesh_array.indices, pass_debug.with_name("Indices"));

        // Glyphs.
        const auto font = assets.roboto_medium_font();
        pass.instances.create(device, MAX_GLYPH_COUNT, pass_debug.with_name("Instances"));
        pass.glyphs.insert(pass.glyphs.end(), font.glyphs.begin(), font.glyphs.end());
        pass.first_character = (char)pass.glyphs[0].character;
        pass.last_character = (char)pass.glyphs[pass.glyphs.size() - 1].character;
        pass.ascender = font.ascender;
        pass.space_advance = font.space_advance;

        // Measure text width.
        auto text_width = 0.0f;
        for (const auto c : TEXT) {
            const auto glyph_index = c - pass.first_character;
            text_width += pass.glyphs[glyph_index].advance;
        }
        auto text_height = (3.0f - 1.5f) * font.ascender;
        demo.parameters.text_offset = float3(-text_width / 2.0f, text_height / 2.0f, 0.0f);

        // Indirect commands.
        pass.indirect_command_signature = device.create_command_signature(
            dword_count<GlyphBindings>(),
            D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED,
            pass_debug.with_name("Indirect Command Signature")
        );
        pass.indirect_commands
            .create(device, MAX_GLYPH_COUNT, pass_debug.with_name("Indirect Commands"));
        pass.indirect_counts.create(device, 1, pass_debug.with_name("Indirect Counts"));
    }

    // Background.
    {
        DebugScope pass_debug("Background");

        demo.bg.constants.create(device, 1, pass_debug.with_name("Constants"));

        const auto mesh = assets.skybox_mesh();
        demo.bg.vertices.create_with_data(device, mesh.vertices, pass_debug.with_name("Vertices"));
        demo.bg.indices.create_with_data(device, mesh.indices, pass_debug.with_name("Indices"));

        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(shaders.text_background_vs())
            .pixel_shader(shaders.text_background_ps())
            .depth_stencil(GpuDepthStencilDesc {
                .depth_read = true,
                .depth_write = true,
                .depth_func = GpuComparisonFunc::LessEqual,
            })
            .render_target_formats({demo.render_targets.color_format()})
            .depth_stencil_format(demo.render_targets.depth_format())
            .sample_desc(demo.render_targets.sample_desc())
            .build(device, demo.bg.pipeline, pass_debug.with_name("Pipeline"));
    }
}

auto gui(Demo& demo, const GuiDesc&) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Gui", NAME.data());
    auto& params = demo.parameters;

    ImGui::SliderFloat("Camera Distance", &params.camera_distance, 0.0f, 10.0f);
    ImGui::SliderAngle("Camera FOV", &params.camera_fov, 0.0f, 90.0f);
    ImGui::SliderAngle("Camera Latitude", &params.camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera Longitude", &params.camera_longitude, 0.0f, 360.0f);
    ImGui::SliderFloat("Scene Rotation Speed", &params.scene_rotation_speed, 0.0f, 2.0f);
    ImGui::ColorPicker4("Glyph Color", (float*)&params.glyph_color);
}

auto update(Demo& demo, const UpdateDesc& desc) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Update", NAME.data());
    auto& params = demo.parameters;

    // Update text.
    {
        auto& pass = demo.glyph;

        // Reset state.
        pass.glyph_submesh_count = 0;
        memset(pass.glyph_submeshes, 0, sizeof(pass.glyph_submeshes));
        memset(pass.text_buffer, 0, sizeof(pass.text_buffer));

        // Format text.
        const auto str_length = (uint)snprintf(
            pass.text_buffer,
            MAX_GLYPH_COUNT - 1,
            "%s\nlat: %.2f\nlon: %.2f\n",
            TEXT.data(),
            params.camera_latitude,
            params.scene_rotation_angle
        );
        FB_ASSERT(str_length < MAX_GLYPH_COUNT);

        // Update instances.
        auto x_offset = 0.0f;
        auto y_offset = 0.0f;
        auto dst_glyph_instances = pass.instances.buffer(desc.frame_index).span();
        auto dst_indirect_commands = pass.indirect_commands.buffer(desc.frame_index).span();
        for (uint i = 0; i < str_length; i++) {
            const auto glyph_char = pass.text_buffer[i];
            if (glyph_char == ' ') {
                x_offset += pass.space_advance;
            } else if (glyph_char == '\n') {
                x_offset = 0.0f;
                y_offset -= pass.ascender;
            } else {
                FB_ASSERT(pass.first_character <= glyph_char && glyph_char <= pass.last_character);
                const auto glyph_index = glyph_char - pass.first_character;
                pass.glyph_submeshes[pass.glyph_submesh_count] = (uint)glyph_index;
                dst_glyph_instances[pass.glyph_submesh_count] = GlyphInstance {
                    .position = float3(x_offset, y_offset, 0.0f) + params.text_offset,
                };
                dst_indirect_commands[pass.glyph_submesh_count] = DrawGlyphCommand {
                    .bindings =
                        GlyphBindings {
                            .constants =
                                pass.constants.buffer(desc.frame_index).cbv_descriptor().index(),
                            .vertices = pass.vertices.srv_descriptor().index(),
                            .instances =
                                pass.instances.buffer(desc.frame_index).srv_descriptor().index(),
                            .irr_texture = demo.pbr.irr.srv_descriptor().index(),
                            .sampler = (uint)GpuSampler::LinearClamp,
                            .base_vertex = pass.submeshes[glyph_index].base_vertex,
                            .instance_id = pass.glyph_submesh_count,
                        },
                    .draw_indexed =
                        D3D12_DRAW_INDEXED_ARGUMENTS {
                            .IndexCountPerInstance = pass.submeshes[glyph_index].index_count,
                            .InstanceCount = 1,
                            .StartIndexLocation = pass.submeshes[glyph_index].start_index,
                            .BaseVertexLocation = 0,
                            .StartInstanceLocation = 0,
                        },
                };
                pass.glyph_submesh_count++;
                x_offset += pass.glyphs[glyph_index].advance;
            }
        }
        pass.indirect_counts.buffer(desc.frame_index).ref() = pass.glyph_submesh_count;
    }

    // Update camera.
    params.scene_rotation_angle += params.scene_rotation_speed * desc.delta_time;
    if (params.scene_rotation_angle > PI * 2.0f) {
        params.scene_rotation_angle -= PI * 2.0f;
    }
    const auto projection =
        float4x4::CreatePerspectiveFieldOfView(params.camera_fov, desc.aspect_ratio, 0.1f, 100.0f);
    const auto eye =
        params.camera_distance * dir_from_lonlat(params.camera_longitude, params.camera_latitude);
    const auto view = float4x4::CreateLookAt(eye, float3::Zero, float3::Up);
    const auto scene_transform = float4x4::CreateRotationY(params.scene_rotation_angle);
    const auto camera_transform = view * projection;

    auto env_view = view;
    env_view.m[3][0] = 0.0f;
    env_view.m[3][1] = 0.0f;
    env_view.m[3][2] = 0.0f;
    env_view.m[3][3] = 1.0f;
    const auto env_transform = scene_transform * env_view * projection;

    // Update debug draw.
    demo.debug_draw.begin(desc.frame_index);
    demo.debug_draw.transform(camera_transform);
    demo.debug_draw.scaled_axes(2.0f);
    demo.debug_draw.end();

    // Update constants.
    demo.bg.constants.buffer(desc.frame_index).ref() = BackgroundConstants {
        .transform = env_transform,
    };
    demo.glyph.constants.buffer(desc.frame_index).ref() = GlyphConstants {
        .transform = camera_transform,
        .scene_transform = scene_transform,
        .color = params.glyph_color,
    };
}

auto render(Demo& demo, const RenderDesc& desc) -> void {
    auto& [cmd, device, frame_index] = desc;
    cmd.graphics_scope([&demo, frame_index](GpuGraphicsCommandList& cmd) {
        cmd.pix_begin("%s - Render", NAME.data());
        demo.render_targets.set(cmd);
        demo.debug_draw.render(cmd);

        {
            cmd.pix_begin("Background");
            const auto& pass = demo.bg;
            cmd.set_constants(BackgroundBindings {
                .constants = pass.constants.buffer(frame_index).cbv_descriptor().index(),
                .vertices = pass.vertices.srv_descriptor().index(),
                .irr_texture = demo.pbr.irr.srv_descriptor().index(),
            });
            cmd.set_pipeline(pass.pipeline);
            cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            cmd.set_index_buffer(pass.indices.index_buffer_view());
            cmd.draw_indexed_instanced(pass.indices.element_count(), 1, 0, 0, 0);
            cmd.pix_end();
        }

        {
            cmd.pix_begin("Glyphs");
            const auto& pass = demo.glyph;
            cmd.set_pipeline(pass.pipeline);
            cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            cmd.set_index_buffer(pass.indices.index_buffer_view());
            cmd.execute_indirect(
                pass.indirect_command_signature,
                MAX_GLYPH_COUNT,
                pass.indirect_commands.buffer(frame_index).resource(),
                std::cref(pass.indirect_counts.buffer(frame_index).resource())
            );
            cmd.pix_end();
        }

        cmd.pix_end();
    });
}

} // namespace fb::demos::text
