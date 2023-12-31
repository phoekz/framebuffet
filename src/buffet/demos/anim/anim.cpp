#include "anim.hpp"

namespace fb::demos::anim {

auto copy_animation_mesh(OwnedAnimationMesh& dst, const baked::AnimationMesh& src) -> void {
    dst.node_count = src.node_count;
    dst.joint_count = src.joint_count;
    dst.duration = src.duration;
    dst.skinning_vertices.assign(src.skinning_vertices.begin(), src.skinning_vertices.end());
    dst.indices.assign(src.indices.begin(), src.indices.end());
    dst.joint_nodes.assign(src.joint_nodes.begin(), src.joint_nodes.end());
    dst.joint_inverse_binds.assign(src.joint_inverse_binds.begin(), src.joint_inverse_binds.end());
    dst.node_parents.assign(src.node_parents.begin(), src.node_parents.end());
    dst.node_transforms.assign(src.node_transforms.begin(), src.node_transforms.end());
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

    // Constants.
    demo.constants.create(device, 1, debug.with_name("Constants"));

    // Model.
    const auto mesh = assets.raccoon_animation_mesh();

    // Geometry.
    demo.vertices.create_with_data(device, mesh.skinning_vertices, debug.with_name("Vertices"));
    demo.indices.create_with_data(device, mesh.indices, debug.with_name("Indices"));

    // Texture.
    const auto texture = assets.raccoon_base_color_texture();
    demo.texture.create_and_transfer_baked(
        device,
        texture,
        D3D12_BARRIER_SYNC_PIXEL_SHADING,
        D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
        D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE,
        debug.with_name("Texture")
    );

    // Animations.
    demo.joint_inverse_bind_buffer
        .create_with_data(device, mesh.joint_inverse_binds, debug.with_name("Joint Inverse Binds"));
    demo.joint_global_transform_buffer
        .create(device, mesh.joint_count, debug.with_name("Joint Global Transforms"));
    demo.animation_duration = mesh.duration;
    demo.node_global_transforms.resize(mesh.node_count);
    copy_animation_mesh(demo.animation_mesh, mesh);
}

auto gui(Demo& demo, const GuiDesc&) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Gui", NAME.data());
    auto& params = demo.parameters;
    ImGui::SliderFloat("Camera Distance", &params.camera_distance, 1.0f, 10.0f);
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
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Update", NAME.data());
    auto& params = demo.parameters;

    // Update animation.
    {
        demo.animation_time += desc.delta_time;
        while (demo.animation_time > demo.animation_duration) {
            demo.animation_time -= demo.animation_duration;
        }

        const auto& mesh = demo.animation_mesh;

        const auto span_from = [](const auto& container, size_t offset, size_t count) {
            return std::span(container.data() + offset, count);
        };

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
                demo.animation_time,
                times_t,
                values_t,
                float3::Zero,
                [](const float3& lhs, const float3& rhs, float t) {
                    return float3::Lerp(lhs, rhs, t);
                }
            );
            const auto r = keyframe_interpolation(
                demo.animation_time,
                times_r,
                values_r,
                Quaternion::Identity,
                [](const Quaternion& lhs, const Quaternion& rhs, float t) {
                    return Quaternion::Slerp(lhs, rhs, t);
                }
            );
            const auto s = keyframe_interpolation(
                demo.animation_time,
                times_s,
                values_s,
                float3::One,
                [](const float3& lhs, const float3& rhs, float t) {
                    return float3::Lerp(lhs, rhs, t);
                }
            );

            const auto transform = float4x4_from_trs(t, r, s);

            if (node_parents[node_index] == ~0u) {
                demo.node_global_transforms[node_index] = transform;
            } else {
                demo.node_global_transforms[node_index] =
                    transform * demo.node_global_transforms[node_parents[node_index]];
            }
        }

        auto jgt = demo.joint_global_transform_buffer.span();
        for (uint joint_index = 0; joint_index < jgt.size(); joint_index++) {
            const auto node_index = mesh.joint_nodes[joint_index];
            jgt[joint_index] = demo.node_global_transforms[node_index];
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
        auto view = float4x4::CreateLookAt(eye, float3::Zero, float3::Up);
        camera_transform = view * projection;
    }

    // Update debug draw.
    demo.debug_draw.begin(desc.frame_index);
    demo.debug_draw.transform(camera_transform);
    demo.debug_draw.grid(50);
    demo.debug_draw.axes();
    demo.debug_draw.end();

    // Update constants.
    demo.constants.buffer(desc.frame_index).ref() = Constants {
        .transform = camera_transform,
    };
}

auto render(Demo& demo, const RenderDesc& desc) -> void {
    auto& [cmd, device, frame_index] = desc;
    cmd.graphics_scope([&demo, frame_index](GpuGraphicsCommandList& cmd) {
        cmd.begin_pix("%s - Render", NAME.data());

        demo.render_targets.set(cmd);
        demo.debug_draw.render(cmd);

        cmd.begin_pix("Animation");
        cmd.set_constants(Bindings {
            .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
            .vertices = demo.vertices.srv_descriptor().index(),
            .joints_inverse_binds = demo.joint_inverse_bind_buffer.srv_descriptor().index(),
            .joints_global_transforms = demo.joint_global_transform_buffer.srv_descriptor().index(),
            .texture = demo.texture.srv_descriptor().index(),
        });
        cmd.set_pipeline(demo.pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(demo.indices.index_buffer_view());
        cmd.draw_indexed_instanced(demo.indices.element_count(), 1, 0, 0, 0);
        cmd.end_pix();

        cmd.end_pix();
    });
}

} // namespace fb::demos::anim
