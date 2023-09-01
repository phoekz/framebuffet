#include "anim.hpp"

namespace fb::demos::anim {

auto AnimDemo::create(GpuDevice& device, const Baked& baked) -> void {
    DebugScope debug(NAME);

    // Render targets.
    _render_targets.create(
        device,
        {
            .size = device.swapchain().size(),
            .color_format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .clear_color = CLEAR_COLOR,
            .sample_count = 4,
        }
    );

    // Debug draw.
    _debug_draw.create(device, baked.kitchen.shaders, _render_targets);

    // Unpack.
    const auto& shaders = baked.buffet.shaders;
    const auto& assets = baked.buffet.assets;

    // Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.anim_draw_vs())
        .pixel_shader(shaders.anim_draw_ps())
        .render_target_formats({_render_targets.color_format()})
        .depth_stencil_format(_render_targets.depth_format())
        .sample_desc(_render_targets.sample_desc())
        .build(device, _pipeline, debug.with_name("Pipeline"));

    // Constants.
    _constants.create(device, 1, debug.with_name("Constants"));

    // Model.
    const auto mesh = assets.raccoon_animation_mesh();

    // Geometry.
    _vertices.create_with_data(device, mesh.skinning_vertices, debug.with_name("Vertices"));
    _indices.create_with_data(device, mesh.indices, debug.with_name("Indices"));

    // Texture.
    const auto texture = assets.raccoon_base_color_texture();
    _texture.create_and_transfer_baked(
        device,
        texture,
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        debug.with_name("Texture")
    );

    // Animations.
    _joint_inverse_bind_buffer
        .create_with_data(device, mesh.joint_inverse_binds, debug.with_name("Joint Inverse Binds"));
    _joint_global_transform_buffer
        .create(device, mesh.joint_count, debug.with_name("Joint Global Transforms"));
    _animation_duration = mesh.duration;
    _node_global_transforms.resize(mesh.node_count);
    _animation_mesh = mesh;
}

auto AnimDemo::gui(const GuiDesc&) -> void {
    auto& p = _parameters;
    ImGui::SliderFloat("Camera Distance", &p.camera_distance, 1.0f, 200.0f);
    ImGui::SliderAngle("Camera FOV", &p.camera_fov, 0.0f, 90.0f);
    ImGui::SliderAngle("Camera Latitude", &p.camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera Longitude", &p.camera_longitude, 0.0f, 360.0f);
    ImGui::SliderFloat("Camera Rotation Speed", &p.camera_rotation_speed, 0.0f, 2.0f);
    ImGui::SliderFloat2("Camera Clip Planes", &p.camera_clip_planes.x, 0.1f, 400.0f);
}

template<typename T, typename F>
auto keyframe_interpolation(
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

auto AnimDemo::update(const UpdateDesc& desc) -> void {
    auto& p = _parameters;

    // Update animation.
    {
        _animation_time += desc.delta_time;
        while (_animation_time > _animation_duration) {
            _animation_time -= _animation_duration;
        }

        const auto node_parents = _animation_mesh.node_parents;
        for (size_t node_index = 0; node_index < _animation_mesh.node_count; ++node_index) {
            const auto& channel = _animation_mesh.node_channels[node_index];
            const auto times_t =
                _animation_mesh.node_channels_times_t.subspan(channel.t_offset, channel.t_count);
            const auto times_r =
                _animation_mesh.node_channels_times_r.subspan(channel.r_offset, channel.r_count);
            const auto times_s =
                _animation_mesh.node_channels_times_s.subspan(channel.s_offset, channel.s_count);
            const auto values_t =
                _animation_mesh.node_channels_values_t.subspan(channel.t_offset, channel.t_count);
            const auto values_r =
                _animation_mesh.node_channels_values_r.subspan(channel.r_offset, channel.r_count);
            const auto values_s =
                _animation_mesh.node_channels_values_s.subspan(channel.s_offset, channel.s_count);

            const auto t = keyframe_interpolation(
                _animation_time,
                times_t,
                values_t,
                float3::Zero,
                [](const float3& lhs, const float3& rhs, float t) {
                    return float3::Lerp(lhs, rhs, t);
                }
            );
            const auto r = keyframe_interpolation(
                _animation_time,
                times_r,
                values_r,
                Quaternion::Identity,
                [](const Quaternion& lhs, const Quaternion& rhs, float t) {
                    return Quaternion::Slerp(lhs, rhs, t);
                }
            );
            const auto s = keyframe_interpolation(
                _animation_time,
                times_s,
                values_s,
                float3::One,
                [](const float3& lhs, const float3& rhs, float t) {
                    return float3::Lerp(lhs, rhs, t);
                }
            );

            const auto transform = float4x4_from_trs(t, r, s);

            if (node_parents[node_index] == ~0u) {
                _node_global_transforms[node_index] = transform;
            } else {
                _node_global_transforms[node_index] =
                    transform * _node_global_transforms[node_parents[node_index]];
            }
        }

        auto* jgt = _joint_global_transform_buffer.ptr();
        for (size_t joint_index = 0; joint_index < _animation_mesh.joint_count; joint_index++) {
            const auto node_index = _animation_mesh.joint_nodes[joint_index];
            jgt[joint_index] = _node_global_transforms[node_index];
        }
    }

    // Update transforms.
    float4x4 camera_transform;
    {
        p.camera_longitude += p.camera_rotation_speed * desc.delta_time;
        if (p.camera_longitude > PI * 2.0f) {
            p.camera_longitude -= PI * 2.0f;
        }

        auto projection = float4x4::CreatePerspectiveFieldOfView(
            p.camera_fov,
            desc.aspect_ratio,
            p.camera_clip_planes.x,
            p.camera_clip_planes.y
        );
        auto eye = p.camera_distance * dir_from_lonlat(p.camera_longitude, p.camera_latitude);
        auto view = float4x4::CreateLookAt(eye, float3::Zero, float3::Up);
        camera_transform = view * projection;
    }

    // Update debug draw.
    _debug_draw.begin(desc.frame_index);
    _debug_draw.transform(camera_transform);
    _debug_draw.grid(50);
    _debug_draw.axes();
    _debug_draw.end();

    // Update constants.
    *_constants.ptr() = Constants {
        .transform = camera_transform,
    };
}

auto AnimDemo::render(GpuDevice& device, GpuCommandList& cmd) -> void {
    cmd.set_graphics();
    _render_targets.set(cmd);
    _debug_draw.render(device, cmd);
    cmd.set_graphics_constants(Bindings {
        .constants = _constants.cbv_descriptor().index(),
        .vertices = _vertices.srv_descriptor().index(),
        .joints_inverse_binds = _joint_inverse_bind_buffer.srv_descriptor().index(),
        .joints_global_transforms = _joint_global_transform_buffer.srv_descriptor().index(),
        .texture = _texture.srv_descriptor().index(),
    });
    cmd.set_pipeline(_pipeline);
    cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd.set_index_buffer(_indices.index_buffer_view());
    cmd.draw_indexed_instanced(_indices.element_count(), 1, 0, 0, 0);
}

} // namespace fb::demos::anim
