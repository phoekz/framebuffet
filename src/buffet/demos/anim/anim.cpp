#include "anim.hpp"

namespace fb::demos::anim {

auto copy_animation_mesh(OwnedAnimationMesh& dst, const baked::AnimationMesh& src) -> void {
    dst.transform = src.transform;
    dst.node_count = src.node_count;
    dst.joint_count = src.joint_count;
    dst.duration = src.duration;
    dst.skinning_vertices.assign(src.skinning_vertices.begin(), src.skinning_vertices.end());
    dst.indices.assign(src.indices.begin(), src.indices.end());
    dst.submeshes.assign(src.submeshes.begin(), src.submeshes.end());
    dst.joint_nodes.assign(src.joint_nodes.begin(), src.joint_nodes.end());
    dst.joint_inverse_binds.assign(src.joint_inverse_binds.begin(), src.joint_inverse_binds.end());
    dst.node_parents.assign(src.node_parents.begin(), src.node_parents.end());
    dst.node_channels.assign(src.node_channels.begin(), src.node_channels.end());
    dst.node_channels_times_t.assign(
        src.node_channels_times_t.begin(),
        src.node_channels_times_t.end()
    );
    dst.node_channels_times_r.assign(
        src.node_channels_times_r.begin(),
        src.node_channels_times_r.end()
    );
    dst.node_channels_times_s.assign(
        src.node_channels_times_s.begin(),
        src.node_channels_times_s.end()
    );
    dst.node_channels_values_t.assign(
        src.node_channels_values_t.begin(),
        src.node_channels_values_t.end()
    );
    dst.node_channels_values_r.assign(
        src.node_channels_values_r.begin(),
        src.node_channels_values_r.end()
    );
    dst.node_channels_values_s.assign(
        src.node_channels_values_s.begin(),
        src.node_channels_values_s.end()
    );
}

auto create(Demo& demo, const CreateDesc& desc) -> void {
    FB_PERF_FUNC();
    DebugScope debug(NAME);

    // Unpack.
    const auto& kitchen_shaders = desc.baked.kitchen.shaders;
    const auto& shaders = desc.baked.buffet.shaders;
    const auto& assets = desc.baked.buffet.assets;
    auto& device = desc.device;

    // Render targets.
    demo.render_target.create(
        device,
        {
            .size = device.swapchain().size(),
            .sample_count = SAMPLE_COUNT,
            .colors = COLOR_ATTACHMENTS,
            .depth_stencil = DEPTH_STENCIL_ATTACHMENT,
        }
    );
    demo.render_target_view.create(demo.render_target.view_desc());

    // Debug draw.
    demo.debug_draw.create(device, kitchen_shaders, demo.render_target_view);

    // Shadow.
    {
        auto& shadow = demo.shadow;

        // Depth.
        constexpr DXGI_FORMAT DEPTH_RAW_FORMAT = DXGI_FORMAT_R32_TYPELESS;
        constexpr DXGI_FORMAT DEPTH_SRV_FORMAT = DXGI_FORMAT_R32_FLOAT;
        constexpr DXGI_FORMAT DEPTH_DSV_FORMAT = DXGI_FORMAT_D32_FLOAT;
        shadow.texture.create(
            device,
            GpuTextureDesc {
                .format = DEPTH_RAW_FORMAT,
                .width = SHADOW_MAP_SIZE,
                .height = SHADOW_MAP_SIZE,
                .clear_value =
                    D3D12_CLEAR_VALUE {
                        .Format = DEPTH_DSV_FORMAT,
                        .DepthStencil = {1.0f, 0},
                    },
                .srv_format = DEPTH_SRV_FORMAT,
                .dsv_format = DEPTH_DSV_FORMAT,
            },
            debug.with_name("Shadow Depth")
        );

        // Pipeline.
        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(shaders.anim_shadow_vs())
            .depth_stencil_format(DEPTH_DSV_FORMAT)
            .build(device, shadow.pipeline, debug.with_name("Shadow Pipeline"));
    }

    // Anim.
    {
        DebugScope scope("Anim");
        auto& anim = demo.anim;

        // Models.
        using Desc = std::tuple<std::string_view, const fb::baked::AnimationMesh&, Model&>;
        for (auto& [name, src, dst] : {
                 Desc {"Female", assets.mixamo_run_female_animation_mesh(), anim.female},
                 Desc {"Male", assets.mixamo_run_male_animation_mesh(), anim.male},
             }) {
            DebugScope model_scope(name);
            dst.constants.create(device, 1, debug.with_name("Constants"));
            dst.skinning_matrices
                .create(device, src.joint_count, model_scope.with_name("Skinning Matrices"));
            dst.vertices.create_and_transfer(
                device,
                src.skinning_vertices,
                D3D12_BARRIER_SYNC_VERTEX_SHADING,
                D3D12_BARRIER_ACCESS_VERTEX_BUFFER,
                model_scope.with_name("Vertices")
            );
            dst.indices.create_and_transfer(
                device,
                src.indices,
                D3D12_BARRIER_SYNC_INDEX_INPUT,
                D3D12_BARRIER_ACCESS_INDEX_BUFFER,
                model_scope.with_name("Indices")
            );
            dst.animation_duration = src.duration;
            dst.animation_global_transforms.resize(src.node_count);
            copy_animation_mesh(dst.animation_mesh, src);
        }

        // Pipeline.
        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(shaders.anim_anim_vs())
            .pixel_shader(shaders.anim_anim_ps())
            .render_target_formats({demo.render_target.color_format(0)})
            .depth_stencil_format(demo.render_target.depth_format())
            .sample_desc(demo.render_target.sample_desc())
            .build(device, anim.pipeline, scope.with_name("Pipeline"));
    }

    // Ground.
    {
        DebugScope scope("Ground");
        auto& ground = demo.ground;

        // Constants.
        ground.constants.create(device, 1, debug.with_name("Constants"));

        // Geometry.
        const auto scale = 32.0f;
        const auto vertices = std::to_array<baked::Vertex>({
            {{-scale, 0.0f, -scale}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {}},
            {{-scale, 0.0f, scale}, {0.0f, 1.0f, 0.0f}, {0.0f, scale}, {}},
            {{scale, 0.0f, scale}, {0.0f, 1.0f, 0.0f}, {scale, scale}, {}},
            {{scale, 0.0f, -scale}, {0.0f, 1.0f, 0.0f}, {scale, 0.0f}, {}},
        });
        const auto indices = std::to_array<baked::Index>({0, 1, 2, 0, 2, 3});
        ground.vertices.create_and_transfer(
            device,
            Span(vertices.data(), vertices.size()),
            D3D12_BARRIER_SYNC_VERTEX_SHADING,
            D3D12_BARRIER_ACCESS_VERTEX_BUFFER,
            scope.with_name("Vertices")
        );
        ground.indices.create_and_transfer(
            device,
            Span(indices.data(), indices.size()),
            D3D12_BARRIER_SYNC_INDEX_INPUT,
            D3D12_BARRIER_ACCESS_INDEX_BUFFER,
            scope.with_name("Indices")
        );

        // Texture.
        {
            const auto width = 4;
            const auto height = 4;
            auto pixels = std::vector<uint8_t>(width * height * 4, 0);
            for (uint y = 0; y < height; y++) {
                for (uint x = 0; x < width; x++) {
                    const auto i = y * width + x;
                    const auto color = (x + y) % 2 == 0 ? (uint8_t)210 : (uint8_t)180;
                    pixels[i * 4 + 0] = color;
                    pixels[i * 4 + 1] = color;
                    pixels[i * 4 + 2] = color;
                    pixels[i * 4 + 3] = 255;
                }
            }

            GpuTextureTransferDesc transfer_desc = {
                .row_pitch = width * 4,
                .slice_pitch = width * height * 4,
                .data = pixels.data(),
            };

            ground.texture.create_and_transfer(
                device,
                GpuTextureDesc {
                    .format = DXGI_FORMAT_R8G8B8A8_UNORM,
                    .width = width,
                    .height = height,
                    .depth = 1,
                    .mip_count = 1,
                    .sample_count = 1,
                },
                Span(&transfer_desc, 1),
                D3D12_BARRIER_SYNC_PIXEL_SHADING,
                D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
                D3D12_BARRIER_LAYOUT_SHADER_RESOURCE,
                scope.with_name("Texture")
            );
        }

        // Pipeline.
        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(shaders.anim_ground_vs())
            .pixel_shader(shaders.anim_ground_ps())
            .render_target_formats({demo.render_target.color_format(0)})
            .depth_stencil_format(demo.render_target.depth_format())
            .sample_desc(demo.render_target.sample_desc())
            .build(device, ground.pipeline, debug.with_name("Pipeline"));
    }
}

auto gui(Demo& demo, const GuiDesc&) -> void {
    FB_PERF_FUNC();
    auto& params = demo.parameters;
    auto& anim = demo.anim;
    if (ImGui::Button("Defaults")) {
        params = {};
    }
    ImGui::SliderFloat("Light Projection Size", &params.light_projection_size, 1.0f, 5.0f);
    ImGui::SliderAngle("Light Longitude", &params.light_longitude, 0.0f, 360.0f);
    ImGui::SliderAngle("Light Latitude", &params.light_latitude, 0.0f, 180.0f);
    ImGui::SliderFloat("Light Distance", &params.light_distance, 1.0f, 200.0f);
    ImGui::SliderFloat("Shadow Near Plane", &params.shadow_near_plane, 0.0f, 10.0f);
    ImGui::SliderFloat("Shadow Far Plane", &params.shadow_far_plane, 1.0f, 100.0f);
    ImGui::SliderFloat("Time Scale", &params.time_scale, 0.0f, 1.0f);
    ImGui::SliderFloat("Scroll Speed", &params.texture_scroll_speed, 0.0f, 4.0f);
    ImGui::SliderFloat(
        "Female Animation Time",
        &anim.female.animation_time,
        0.0f,
        anim.female.animation_duration
    );
    ImGui::SliderFloat(
        "Male Animation Time",
        &anim.male.animation_time,
        0.0f,
        anim.male.animation_duration
    );
    ImGui::SliderFloat3("Female Position", &params.positions[0].x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Male Position", &params.positions[1].x, -10.0f, 10.0f);
    ImGui::ColorEdit4("Color 0", &params.colors[0].x);
    ImGui::ColorEdit4("Color 1", &params.colors[1].x);
    ImGui::SliderFloat("Camera Distance", &params.camera_distance, 1.0f, 10.0f);
    ImGui::SliderFloat("Camera Height Offset", &params.camera_height_offset, 0.0f, 2.0f);
    ImGui::SliderAngle("Camera FOV", &params.camera_fov, 0.0f, 90.0f);
    ImGui::SliderAngle("Camera Latitude", &params.camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera Longitude", &params.camera_longitude, 0.0f, 360.0f);
    ImGui::SliderFloat("Camera Rotation Speed", &params.camera_rotation_speed, 0.0f, 2.0f);
    ImGui::SliderFloat2("Camera Clip Planes", &params.camera_clip_planes.x, 0.1f, 400.0f);
}

template<typename T, typename F>
static auto keyframe_interpolation(
    float time,
    Span<const float> times,
    Span<const T> values,
    const T default_value,
    F interpolate
) -> T {
    FB_ASSERT(time >= 0.0f);
    FB_ASSERT(times.size() == values.size());

    if (times.empty()) {
        return default_value;
    }

    if (time >= times.back()) {
        return values.back();
    }

    if (times.size() == 1) {
        return values[0];
    }

    T return_value = default_value;
    for (size_t i = 0; i < times.size() - 1; ++i) {
        if (time >= times[i] && time < times[i + 1]) {
            const auto t = (time - times[i]) / (times[i + 1] - times[i]);
            return_value = interpolate(values[i], values[i + 1], t);
            break;
        }
    }
    return return_value;
}

auto update(Demo& demo, const UpdateDesc& desc) -> void {
    FB_PERF_FUNC();
    auto& params = demo.parameters;
    auto& ground = demo.ground;
    auto& anim = demo.anim;

    // Time scales.
    const float female_time_scale =
        params.time_scale * anim.male.animation_duration / anim.female.animation_duration;
    const float male_time_scale = params.time_scale;

    // Update animation transforms.
    using TransformDesc = std::tuple<float, Model&>;
    for (auto& [time_scale, model] : {
             TransformDesc(female_time_scale, anim.female),
             TransformDesc(male_time_scale, anim.male),
         }) {
        FB_PERF_SCOPE("Update Transforms");

        // Unpack.
        auto* global_transforms = model.animation_global_transforms.data();

        // Timing.
        model.animation_time += time_scale * desc.delta_time;
        while (model.animation_time > model.animation_duration) {
            model.animation_time -= model.animation_duration;
        }

        // Helpers.
        const auto span_from = [](const auto& container, size_t offset, size_t count) {
            return Span(container.data() + offset, count);
        };

        // Interpolation & transform hierarchy.
        const auto& mesh = model.animation_mesh;
        const auto& node_parents = mesh.node_parents;
        for (size_t node_index = 0; node_index < mesh.node_count; ++node_index) {
            const auto& channel = mesh.node_channels[node_index];
            const auto times_t =
                span_from(mesh.node_channels_times_t, channel.t_offset, channel.t_count);
            const auto times_r =
                span_from(mesh.node_channels_times_r, channel.r_offset, channel.r_count);
            const auto times_s =
                span_from(mesh.node_channels_times_s, channel.s_offset, channel.s_count);
            const auto values_t =
                span_from(mesh.node_channels_values_t, channel.t_offset, channel.t_count);
            const auto values_r =
                span_from(mesh.node_channels_values_r, channel.r_offset, channel.r_count);
            const auto values_s =
                span_from(mesh.node_channels_values_s, channel.s_offset, channel.s_count);

            const auto t = keyframe_interpolation(
                model.animation_time,
                times_t,
                values_t,
                FLOAT3_ZERO,
                [](const float3& lhs, const float3& rhs, float t) {
                    return float3_lerp(lhs, rhs, t);
                }
            );
            const auto r = keyframe_interpolation(
                model.animation_time,
                times_r,
                values_r,
                FLOAT_QUAT_IDENTITY,
                [](const float_quat& lhs, const float_quat& rhs, float t) {
                    return float_quat_slerp(lhs, rhs, t);
                }
            );
            const auto s = keyframe_interpolation(
                model.animation_time,
                times_s,
                values_s,
                FLOAT3_ONE,
                [](const float3& lhs, const float3& rhs, float t) {
                    return float3_lerp(lhs, rhs, t);
                }
            );

            const auto local_transform = float4x4_from_trs(t, r, s);
            const auto parent_index = node_parents[node_index];
            if (parent_index == ~0u) {
                global_transforms[node_index] = local_transform;
            } else {
                global_transforms[node_index] = global_transforms[parent_index] * local_transform;
            }
        }

        // Write skinning matrices.
        auto sms = model.skinning_matrices.buffer(desc.frame_index).span();
        for (uint joint_index = 0; joint_index < sms.size(); joint_index++) {
            const auto node_index = mesh.joint_nodes[joint_index];
            sms[joint_index] =
                global_transforms[node_index] * mesh.joint_inverse_binds[joint_index];
        }
    }

    // Update transforms.
    float4x4 camera_transform;
    {
        params.camera_longitude += params.camera_rotation_speed * desc.delta_time;
        if (params.camera_longitude > FLOAT_PI * 2.0f) {
            params.camera_longitude -= FLOAT_PI * 2.0f;
        }

        const auto projection = float4x4_perspective(
            params.camera_fov,
            desc.aspect_ratio,
            params.camera_clip_planes.x,
            params.camera_clip_planes.y
        );
        const auto eye = params.camera_distance
            * float3_from_lonlat(params.camera_longitude, params.camera_latitude);
        const auto height_offset = float3(0.0f, params.camera_height_offset, 0.0f);
        const auto view = float4x4_lookat(eye + height_offset, height_offset, FLOAT3_UP);
        camera_transform = projection * view;
    }

    // Update light.
    float4x4 light_transform;
    {
        const auto light_direction =
            float3_from_lonlat(params.light_longitude, params.light_latitude);
        const auto projection = float4x4_orthographic(
            -0.5f * params.light_projection_size,
            0.5f * params.light_projection_size,
            -0.5f * params.light_projection_size,
            0.5f * params.light_projection_size,
            params.shadow_near_plane,
            params.shadow_far_plane
        );
        const auto eye = params.light_distance * light_direction;
        const auto view = float4x4_lookat(eye, FLOAT3_ZERO, FLOAT3_UP);

        light_transform = projection * view;
    }

    // Update ground.
    {
        ground.texture_scroll += params.texture_scroll_speed * desc.delta_time;
        if (ground.texture_scroll > 1.0f) {
            ground.texture_scroll -= 1.0f;
        }
    }

    // Update constants.
    {
        ground.constants.buffer(desc.frame_index).ref() = Constants {
            .transform = camera_transform,
            .light_transform = light_transform,
        };
    }
    {
        using ConstantDesc = std::tuple<uint, Model&>;
        for (auto& [model_index, model] : {
                 ConstantDesc(0, anim.female),
                 ConstantDesc(1, anim.male),
             }) {
            const auto& root_transform = model.animation_mesh.transform;
            const auto translation = float4x4_translation(demo.parameters.positions[model_index]);
            model.constants.buffer(desc.frame_index).ref() = Constants {
                .transform = camera_transform * translation * root_transform,
                .light_transform = light_transform * translation * root_transform,
            };
        }
    }

    // Update debug draw.
    demo.debug_draw.begin(desc.frame_index);
    demo.debug_draw.transform(camera_transform);
    demo.debug_draw.axes();
    demo.debug_draw.end();
}

auto render(Demo& demo, const RenderDesc& desc) -> void {
    FB_PERF_FUNC();
    auto& [cmd, device, frame_index] = desc;
    cmd.graphics_scope([&demo, frame_index](GpuGraphicsCommandList& cmd) {
        using ModelDesc = std::tuple<uint, Model&>;

        auto& shadow = demo.shadow;
        auto& ground = demo.ground;
        auto& anim = demo.anim;

        cmd.pix_begin("%s - Render", NAME.data());

        {
            cmd.pix_begin("Shadow");
            shadow.texture.transition(
                cmd,
                D3D12_BARRIER_SYNC_DEPTH_STENCIL,
                D3D12_BARRIER_ACCESS_DEPTH_STENCIL_WRITE,
                D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_WRITE
            );
            cmd.flush_barriers();
            cmd.set_viewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
            cmd.set_scissor(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
            cmd.set_rtvs_dsv({}, shadow.texture.dsv_descriptor());
            cmd.clear_dsv(shadow.texture.dsv_descriptor(), 1.0f, 0);
            cmd.set_pipeline(shadow.pipeline);
            cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            for (const auto& [model_index, model] : {
                     ModelDesc(0, anim.female),
                     ModelDesc(1, anim.male),
                 }) {
                const auto& mesh = model.animation_mesh;
                cmd.set_index_buffer(model.indices.index_buffer_view());
                for (uint i = 0; i < mesh.submeshes.size(); i++) {
                    const auto& submesh = mesh.submeshes[i];
                    cmd.set_constants(
                        Bindings {
                            .constants =
                                model.constants.buffer(frame_index).cbv_descriptor().index(),
                            .vertices = model.vertices.srv_descriptor().index(),
                            .skinning_matrices = model.skinning_matrices.buffer(frame_index)
                                                     .srv_descriptor()
                                                     .index(),
                        }
                    );
                    cmd.draw_indexed_instanced(
                        submesh.index_count,
                        1,
                        submesh.start_index,
                        submesh.base_vertex,
                        0
                    );
                }
            }
            shadow.texture.transition(
                cmd,
                D3D12_BARRIER_SYNC_PIXEL_SHADING,
                D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
                D3D12_BARRIER_LAYOUT_SHADER_RESOURCE
            );
            cmd.flush_barriers();
            cmd.pix_end();
        }

        demo.render_target_view.set_graphics(cmd);
        demo.debug_draw.render(cmd);

        {
            cmd.pix_begin("Ground");
            cmd.set_pipeline(ground.pipeline);
            cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            cmd.set_index_buffer(ground.indices.index_buffer_view());
            cmd.set_constants(
                Bindings {
                    .constants = ground.constants.buffer(frame_index).cbv_descriptor().index(),
                    .vertices = ground.vertices.srv_descriptor().index(),
                    .texture = ground.texture.srv_descriptor().index(),
                    .texture_scroll = ground.texture_scroll,
                    .shadow_texture = shadow.texture.srv_descriptor().index(),
                }
            );
            cmd.draw_indexed_instanced(6, 1, 0, 0, 0);
            cmd.pix_end();
        }

        {
            cmd.pix_begin("Animation");
            cmd.set_pipeline(anim.pipeline);
            cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            for (const auto& [model_index, model] : {
                     ModelDesc(0, anim.female),
                     ModelDesc(1, anim.male),
                 }) {
                const auto& mesh = model.animation_mesh;
                cmd.set_index_buffer(model.indices.index_buffer_view());
                for (uint i = 0; i < mesh.submeshes.size(); i++) {
                    const auto& submesh = mesh.submeshes[i];
                    const uint r = (uint)(demo.parameters.colors[i].x * 255.0f);
                    const uint g = (uint)(demo.parameters.colors[i].y * 255.0f);
                    const uint b = (uint)(demo.parameters.colors[i].z * 255.0f);
                    const uint a = (uint)(demo.parameters.colors[i].w * 255.0f);
                    const uint color = (a << 24) | (b << 16) | (g << 8) | r;
                    cmd.set_constants(
                        Bindings {
                            .constants =
                                model.constants.buffer(frame_index).cbv_descriptor().index(),
                            .vertices = model.vertices.srv_descriptor().index(),
                            .skinning_matrices = model.skinning_matrices.buffer(frame_index)
                                                     .srv_descriptor()
                                                     .index(),
                            .color = color,
                            .shadow_texture = shadow.texture.srv_descriptor().index(),
                        }
                    );
                    cmd.draw_indexed_instanced(
                        submesh.index_count,
                        1,
                        submesh.start_index,
                        submesh.base_vertex,
                        0
                    );
                }
            }
            cmd.pix_end();
        }

        cmd.pix_end();
    });
}

} // namespace fb::demos::anim
