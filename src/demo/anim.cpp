#include "anim.hpp"

namespace fb::anim {

Demo::Demo(GpuDevice& device) :
    _render_targets(device, device.swapchain_size(), Demo::CLEAR_COLOR, Demo::NAME),
    _debug_draw(device, Demo::NAME),
    _model("assets/models/low-poly_racoon_run_animation.glb") {
    // Shaders.
    const auto source = read_whole_file("shaders/anim.hlsl");
    GpuShaderCompiler sc;
    const auto vertex_shader = sc.compile(Demo::NAME, GpuShaderType::Vertex, "vs_main", source);
    const auto pixel_shader = sc.compile(Demo::NAME, GpuShaderType::Pixel, "ps_main", source);

    // Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(vertex_shader.bytecode())
        .pixel_shader(pixel_shader.bytecode())
        .render_target_formats({DXGI_FORMAT_R8G8B8A8_UNORM})
        .depth_stencil_format(DXGI_FORMAT_D32_FLOAT)
        .build(device, _pipeline, dx_name(Demo::NAME, "Pipeline"));

    // Constant buffer.
    _constant_buffer.create(device, 1, dx_name(Demo::NAME, "Constant Buffer"));

    // Geometry.
    _vertex_buffer.create_with_data(
        device,
        _model.skinning_vertices(),
        dx_name(Demo::NAME, "Vertex Buffer"));
    _index_buffer.create_with_data(device, _model.indices(), dx_name(Demo::NAME, "Index Buffer"));

    // Texture - create.
    const auto& image = _model.base_color_texture();
    _texture.create(
        device,
        GpuTexture2dDesc {
            .format = image.format(),
            .width = image.width(),
            .height = image.height(),
        },
        dx_name(Demo::NAME, "Texture"));

    // Texture - upload.
    device.easy_upload(
        D3D12_SUBRESOURCE_DATA {
            .pData = image.data(),
            .RowPitch = image.row_pitch(),
            .SlicePitch = image.slice_pitch()},
        _texture.resource(),
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    // Animations.
    _joint_inverse_bind_buffer.create_with_data(
        device,
        _model.joint_inverse_binds(),
        dx_name(Demo::NAME, "Joint Inverse Binds Buffer"));
    _joint_global_transform_buffer.create(
        device,
        _model.joint_count(),
        dx_name(Demo::NAME, "Joint Global Transform Buffer"));
    _animation_duration = _model.animation_duration();
    _node_global_transforms.resize(_model.node_parents().size());
}

static float camera_distance = 4.0f;
static float camera_fov = rad_from_deg(45.0f);
static float camera_latitude = rad_from_deg(30.0f);
static float camera_longitude = rad_from_deg(0.0f);
static float camera_rotation_speed = 0.5f;
static Float2 camera_clip_planes = Float2(0.1f, 300.0f);

auto Demo::gui(const gui::Desc&) -> void {
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

auto Demo::update(const demo::UpdateDesc& desc) -> void {
    // Update animation.
    {
        _animation_time += desc.delta_time;
        while (_animation_time > _animation_duration) {
            _animation_time -= _animation_duration;
        }

        auto node_parents = _model.node_parents();
        for (size_t node_index = 0; node_index < _model.node_count(); ++node_index) {
            const auto& channel = _model.node_channels()[node_index];
            const auto times_t = _model.node_channel_times_t(channel);
            const auto times_r = _model.node_channel_times_r(channel);
            const auto times_s = _model.node_channel_times_s(channel);
            const auto values_t = _model.node_channel_values_t(channel);
            const auto values_r = _model.node_channel_values_r(channel);
            const auto values_s = _model.node_channel_values_s(channel);

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

            if (node_parents[node_index] == GLTF_NULL_NODE) {
                _node_global_transforms[node_index] = transform;
            } else {
                _node_global_transforms[node_index] =
                    transform * _node_global_transforms[node_parents[node_index]];
            }
        }

        auto* jgt = _joint_global_transform_buffer.ptr();
        for (size_t joint_index = 0; joint_index < _model.joint_count(); joint_index++) {
            const auto node_index = _model.joint_nodes()[joint_index];
            jgt[joint_index] = _node_global_transforms[node_index];
        }
    }

    // Update constant buffer.
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

        auto& constants = *_constant_buffer.ptr();
        constants.transform = camera_transform;
    }

    // Update debug draw.
    _debug_draw.begin(desc.frame_index);
    _debug_draw.transform(camera_transform);
    _debug_draw.axes();
    _debug_draw.end();
}

auto Demo::render(GpuDevice& device, GpuCommandList& cmd) -> void {
    cmd.set_graphics();
    _render_targets.begin(device, cmd);
    _debug_draw.render(device, cmd);
    cmd.set_graphics_constants({
        _constant_buffer.cbv_descriptor().index(),
        _vertex_buffer.srv_descriptor().index(),
        _joint_inverse_bind_buffer.srv_descriptor().index(),
        _joint_global_transform_buffer.srv_descriptor().index(),
        _texture.srv_descriptor().index(),
    });
    cmd.set_pipeline(_pipeline);
    cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd.set_index_buffer(_index_buffer.index_buffer_view());
    cmd.draw_indexed_instanced(_index_buffer.element_size(), 1, 0, 0, 0);
    _render_targets.end(device, cmd);
}

}  // namespace fb::anim
