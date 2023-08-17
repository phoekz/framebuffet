#include "anim.hpp"

namespace fb::demos::anim {

AnimDemo::AnimDemo(GpuDevice& device, const baked::Assets& assets, const baked::Shaders& shaders) :
    _render_targets(device, device.swapchain_size(), CLEAR_COLOR, NAME),
    _debug_draw(device, shaders, NAME) {
    // Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.anim_draw_vs())
        .pixel_shader(shaders.anim_draw_ps())
        .render_target_formats({DXGI_FORMAT_R8G8B8A8_UNORM})
        .depth_stencil_format(DXGI_FORMAT_D32_FLOAT)
        .build(device, _pipeline, dx_name(NAME, "Pipeline"));

    // Constants.
    _constants.create(device, 1, dx_name(NAME, "Constants"));

    // Model.
    const auto mesh = assets.raccoon_animation_mesh();

    // Geometry.
    _vertices.create_with_data(device, mesh.skinning_vertices, dx_name(NAME, "Vertices"));
    _indices.create_with_data(device, mesh.indices, dx_name(NAME, "Indices"));

    // Texture - create.
    const auto texture = assets.raccoon_texture();
    _texture.create(
        device,
        GpuTexture2dDesc {
            .format = texture.format,
            .width = texture.width,
            .height = texture.height,
        },
        dx_name(NAME, "Texture"));

    // Texture - upload.
    device.easy_upload(
        D3D12_SUBRESOURCE_DATA {
            .pData = texture.data.data(),
            .RowPitch = texture.row_pitch,
            .SlicePitch = texture.slice_pitch},
        _texture.resource(),
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    // Animations.
    _joint_inverse_bind_buffer.create_with_data(
        device,
        mesh.joint_inverse_binds,
        dx_name(NAME, "Joint Inverse Binds"));
    _joint_global_transform_buffer.create(
        device,
        mesh.joint_count,
        dx_name(NAME, "Joint Global Transforms"));
    _animation_duration = mesh.duration;
    _node_global_transforms.resize(mesh.node_count);
    _animation_mesh = mesh;
}

static float camera_distance = 4.0f;
static float camera_fov = rad_from_deg(45.0f);
static float camera_latitude = rad_from_deg(30.0f);
static float camera_longitude = rad_from_deg(0.0f);
static float camera_rotation_speed = 0.5f;
static Float2 camera_clip_planes = Float2(0.1f, 300.0f);

auto AnimDemo::gui(const GuiDesc&) -> void {
    ImGui::SliderFloat("Camera Distance", &camera_distance, 1.0f, 200.0f);
    ImGui::SliderAngle("Camera FOV", &camera_fov, 0.0f, 90.0f);
    ImGui::SliderAngle("Camera Latitude", &camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera Longitude", &camera_longitude, 0.0f, 360.0f);
    ImGui::SliderFloat("Camera Rotation Speed", &camera_rotation_speed, 0.0f, 2.0f);
    ImGui::SliderFloat2("Camera Clip Planes", &camera_clip_planes.x, 0.1f, 400.0f);
}

template<typename T, typename F>
auto keyframe_interpolation(
    float time,
    std::span<const float> times,
    std::span<const T> values,
    const T default_value,
    F interpolate) -> T {
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
};

auto AnimDemo::update(const UpdateDesc& desc) -> void {
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
                Float3::Zero,
                [](const Float3& lhs, const Float3& rhs, float t) {
                    return Float3::Lerp(lhs, rhs, t);
                });
            const auto r = keyframe_interpolation(
                _animation_time,
                times_r,
                values_r,
                Quaternion::Identity,
                [](const Quaternion& lhs, const Quaternion& rhs, float t) {
                    return Quaternion::Slerp(lhs, rhs, t);
                });
            const auto s = keyframe_interpolation(
                _animation_time,
                times_s,
                values_s,
                Float3::One,
                [](const Float3& lhs, const Float3& rhs, float t) {
                    return Float3::Lerp(lhs, rhs, t);
                });

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

    // Update Constants.
    Float4x4 camera_transform;
    {
        camera_longitude += camera_rotation_speed * desc.delta_time;
        if (camera_longitude > PI * 2.0f) {
            camera_longitude -= PI * 2.0f;
        }

        auto projection = Float4x4::CreatePerspectiveFieldOfView(
            camera_fov,
            desc.aspect_ratio,
            camera_clip_planes.x,
            camera_clip_planes.y);
        auto eye = camera_distance * dir_from_lonlat(camera_longitude, camera_latitude);
        auto view = Float4x4::CreateLookAt(eye, Float3::Zero, Float3::Up);
        camera_transform = view * projection;

        auto& constants = *_constants.ptr();
        constants.transform = camera_transform;
    }

    // Update debug draw.
    _debug_draw.begin(desc.frame_index);
    _debug_draw.transform(camera_transform);
    _debug_draw.axes();
    _debug_draw.end();
}

auto AnimDemo::render(GpuDevice& device, GpuCommandList& cmd) -> void {
    cmd.set_graphics();
    _render_targets.begin(device, cmd);
    _debug_draw.render(device, cmd);
    cmd.set_graphics_constants({
        _constants.cbv_descriptor().index(),
        _vertices.srv_descriptor().index(),
        _joint_inverse_bind_buffer.srv_descriptor().index(),
        _joint_global_transform_buffer.srv_descriptor().index(),
        _texture.srv_descriptor().index(),
    });
    cmd.set_pipeline(_pipeline);
    cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd.set_index_buffer(_indices.index_buffer_view());
    cmd.draw_indexed_instanced(_indices.element_size(), 1, 0, 0, 0);
    _render_targets.end(device, cmd);
}

}  // namespace fb::demos::anim
