#include "anim.hpp"

namespace fb::demos::anim {

auto copy_animation_mesh(OwnedAnimationMesh& dst, const baked::AnimationMesh& src) -> void {
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

    // Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.anim_draw_vs())
        .pixel_shader(shaders.anim_draw_ps())
        .render_target_formats({demo.render_targets.color_format()})
        .depth_stencil_format(demo.render_targets.depth_format())
        .sample_desc(demo.render_targets.sample_desc())
        .build(device, demo.pipeline, debug.with_name("Pipeline"));

    // Models.
    using Desc = std::tuple<std::string_view, const fb::baked::AnimationMesh&, Model&>;
    for (auto& [name, src, dst] : {
             Desc {"Female", assets.mixamo_run_female_animation_mesh(), demo.female},
             Desc {"Male", assets.mixamo_run_male_animation_mesh(), demo.male},
         }) {
        DebugScope scope(name);
        dst.constants.create(device, 1, debug.with_name("Constants"));
        dst.skinning_matrices.create(device, src.joint_count, scope.with_name("Skinning Matrices"));
        dst.vertices.create_and_transfer(
            device,
            src.skinning_vertices,
            D3D12_BARRIER_SYNC_VERTEX_SHADING,
            D3D12_BARRIER_ACCESS_VERTEX_BUFFER,
            scope.with_name("Vertices")
        );
        dst.indices.create_and_transfer(
            device,
            src.indices,
            D3D12_BARRIER_SYNC_INDEX_INPUT,
            D3D12_BARRIER_ACCESS_INDEX_BUFFER,
            scope.with_name("Indices")
        );
        dst.animation_duration = src.duration;
        dst.animation_transforms.resize(src.node_count);
        copy_animation_mesh(dst.animation_mesh, src);
    }
}

auto gui(Demo& demo, const GuiDesc&) -> void {
    ZoneScoped;
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Gui", NAME.data());
    auto& params = demo.parameters;
    ImGui::SliderFloat(
        "Female Animation Time",
        &demo.female.animation_time,
        0.0f,
        demo.female.animation_duration
    );
    ImGui::SliderFloat(
        "Male Animation Time",
        &demo.male.animation_time,
        0.0f,
        demo.male.animation_duration
    );
    ImGui::SliderFloat3("Female Position", &params.positions[0].x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Male Position", &params.positions[1].x, -10.0f, 10.0f);
    ImGui::ColorPicker4("Color 0", &params.colors[0].x);
    ImGui::ColorPicker4("Color 1", &params.colors[1].x);
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
    std::span<const float> times,
    std::span<const T> values,
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
    ZoneScoped;
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Update", NAME.data());
    auto& params = demo.parameters;

    // Update animation transforms.
    using Desc = std::tuple<Model&>;
    for (auto& [model] : {Desc(demo.female), Desc(demo.male)}) {
        // Unpack.
        auto* node_transforms = model.animation_transforms.data();

        // Timing.
        model.animation_time += desc.delta_time;
        while (model.animation_time > model.animation_duration) {
            model.animation_time -= model.animation_duration;
        }

        // Helpers.
        const auto span_from = [](const auto& container, size_t offset, size_t count) {
            return std::span(container.data() + offset, count);
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
                float3::Zero,
                [](const float3& lhs, const float3& rhs, float t) {
                    return float3::Lerp(lhs, rhs, t);
                }
            );
            const auto r = keyframe_interpolation(
                model.animation_time,
                times_r,
                values_r,
                Quaternion::Identity,
                [](const Quaternion& lhs, const Quaternion& rhs, float t) {
                    return Quaternion::Slerp(lhs, rhs, t);
                }
            );
            const auto s = keyframe_interpolation(
                model.animation_time,
                times_s,
                values_s,
                float3::One,
                [](const float3& lhs, const float3& rhs, float t) {
                    return float3::Lerp(lhs, rhs, t);
                }
            );

            const auto transform = float4x4_from_trs(t, r, s);
            const auto parent_index = node_parents[node_index];

            if (parent_index == ~0u) {
                node_transforms[node_index] = transform;
            } else {
                node_transforms[node_index] = transform * node_transforms[parent_index];
            }
        }

        // Write skinning matrices.
        auto sms = model.skinning_matrices.buffer(desc.frame_index).span();
        for (uint joint_index = 0; joint_index < sms.size(); joint_index++) {
            const auto node_index = mesh.joint_nodes[joint_index];
            sms[joint_index] = mesh.joint_inverse_binds[joint_index] * node_transforms[node_index];
        }
    }

    // Update transforms.
    float4x4 camera_transform;
    {
        params.camera_longitude += params.camera_rotation_speed * desc.delta_time;
        if (params.camera_longitude > PI * 2.0f) {
            params.camera_longitude -= PI * 2.0f;
        }

        auto projection = float4x4::CreatePerspectiveFieldOfView(
            params.camera_fov,
            desc.aspect_ratio,
            params.camera_clip_planes.x,
            params.camera_clip_planes.y
        );
        auto eye = params.camera_distance
            * dir_from_lonlat(params.camera_longitude, params.camera_latitude);
        auto height_offset = float3(0.0f, params.camera_height_offset, 0.0f);
        auto view = float4x4::CreateLookAt(eye + height_offset, height_offset, float3::Up);
        camera_transform = view * projection;
    }

    // Update constants.
    const float4x4 fbx_scale = float4x4::CreateScale(0.01f); // Todo: Bake this into the model.
    const float4x4 female_translation = float4x4::CreateTranslation(demo.parameters.positions[0]);
    const float4x4 male_translation = float4x4::CreateTranslation(demo.parameters.positions[1]);
    const float4x4 female_transform = fbx_scale * female_translation * camera_transform;
    const float4x4 male_transform = fbx_scale * male_translation * camera_transform;
    demo.female.constants.buffer(desc.frame_index).ref() = Constants {
        .transform = female_transform,
    };
    demo.male.constants.buffer(desc.frame_index).ref() = Constants {
        .transform = male_transform,
    };

    // Update debug draw.
    demo.debug_draw.begin(desc.frame_index);
    demo.debug_draw.transform(camera_transform);
    demo.debug_draw.grid(50);
    demo.debug_draw.axes();
    demo.debug_draw.end();
}

auto render(Demo& demo, const RenderDesc& desc) -> void {
    ZoneScoped;
    auto& [cmd, device, frame_index] = desc;
    cmd.graphics_scope([&demo, frame_index](GpuGraphicsCommandList& cmd) {
        cmd.pix_begin("%s - Render", NAME.data());

        demo.render_targets.set(cmd);
        demo.debug_draw.render(cmd);

        cmd.pix_begin("Animation");
        cmd.set_pipeline(demo.pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        using Desc = std::tuple<uint, Model&>;
        for (const auto& [model_index, model] : {Desc(0, demo.female), Desc(1, demo.male)}) {
            const auto& mesh = model.animation_mesh;
            cmd.set_index_buffer(model.indices.index_buffer_view());
            for (uint i = 0; i < mesh.submeshes.size(); i++) {
                const auto& submesh = mesh.submeshes[i];
                const uint r = (uint)(demo.parameters.colors[i].x * 255.0f);
                const uint g = (uint)(demo.parameters.colors[i].y * 255.0f);
                const uint b = (uint)(demo.parameters.colors[i].z * 255.0f);
                const uint a = (uint)(demo.parameters.colors[i].w * 255.0f);
                const uint color = (a << 24) | (b << 16) | (g << 8) | r;
                cmd.set_constants(Bindings {
                    .constants = model.constants.buffer(frame_index).cbv_descriptor().index(),
                    .vertices = model.vertices.srv_descriptor().index(),
                    .skinning_matrices =
                        model.skinning_matrices.buffer(frame_index).srv_descriptor().index(),
                    .color = color,
                });
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

        cmd.pix_end();
    });
}

} // namespace fb::demos::anim
