#include "cube.hpp"

namespace fb::cube {

Demo::Demo(GpuDevice& device) :
    _render_targets(device, device.swapchain_size(), Demo::CLEAR_COLOR, Demo::NAME),
    _debug_draw(device, Demo::NAME) {
    // Shaders.
    GpuShaderBytecode vertex_shader;
    GpuShaderBytecode pixel_shader;
    {
        GpuShaderCompiler sc;
        auto source = read_whole_file("shaders/cube.hlsl");
        vertex_shader = sc.compile(Demo::NAME, GpuShaderType::Vertex, "vs_main", source);
        pixel_shader = sc.compile(Demo::NAME, GpuShaderType::Pixel, "ps_main", source);
    }

    // Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(vertex_shader)
        .pixel_shader(pixel_shader)
        .render_target_formats({DXGI_FORMAT_R8G8B8A8_UNORM})
        .depth_stencil_format(DXGI_FORMAT_D32_FLOAT)
        .build(device, _pipeline, dx_name(Demo::NAME, "Pipeline"));

    // Constant buffer.
    _constant_buffer.create(device, 1, dx_name(Demo::NAME, "Constant Buffer"));

    // Model.
    auto model = GltfModel("assets/models/stylized_crate.glb");

    // Geometry.
    _vertex_buffer.create_with_data(device, model.vertices(), dx_name(Demo::NAME, "Vertex Buffer"));
    _index_buffer.create_with_data(device, model.indices(), dx_name(Demo::NAME, "Index Buffer"));

    // Texture.
    {
        // Create.
        const auto& image = model.base_color_texture();
        _texture.create(
            device,
            GpuTexture2dDesc {
                .format = image.format(),
                .width = image.width(),
                .height = image.height(),
            },
            dx_name(Demo::NAME, "Texture"));

        // Upload.
        device.easy_upload(
            D3D12_SUBRESOURCE_DATA {
                .pData = image.data(),
                .RowPitch = image.row_pitch(),
                .SlicePitch = image.slice_pitch()},
            _texture.resource(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

static float camera_distance = 5.0f;
static float camera_fov = rad_from_deg(45.0f);
static float camera_latitude = rad_from_deg(30.0f);
static float camera_longitude = rad_from_deg(0.0f);
static float camera_rotation_speed = 0.5f;

auto Demo::gui(const gui::Desc&) -> void {
    ImGui::SliderFloat("Camera Distance", &camera_distance, 0.0f, 10.0f);
    ImGui::SliderAngle("Camera FOV", &camera_fov, 0.0f, 90.0f);
    ImGui::SliderAngle("Camera Latitude", &camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera Longitude", &camera_longitude, 0.0f, 360.0f);
    ImGui::SliderFloat("Camera Rotation Speed", &camera_rotation_speed, 0.0f, 2.0f);
}

auto Demo::update(const demo::UpdateDesc& desc) -> void {
    Float4x4 camera_transform;
    {
        camera_longitude += camera_rotation_speed * desc.delta_time;
        if (camera_longitude > PI * 2.0f) {
            camera_longitude -= PI * 2.0f;
        }

        auto projection =
            Float4x4::CreatePerspectiveFieldOfView(camera_fov, desc.aspect_ratio, 0.1f, 100.0f);
        auto eye = camera_distance * dir_from_lonlat(camera_longitude, camera_latitude);
        auto view = Float4x4::CreateLookAt(eye, Float3::Zero, Float3::Up);
        camera_transform = view * projection;

        auto& constants = *_constant_buffer.ptr();
        constants.transform = camera_transform;
    }

    {
        _debug_draw.begin(desc.frame_index);
        _debug_draw.transform(camera_transform);
        _debug_draw.axes();
        _debug_draw.end();
    }
}

auto Demo::render(GpuDevice& device, GpuCommandList& cmd) -> void {
    cmd.set_graphics();
    _render_targets.begin(device, cmd);
    _debug_draw.render(device, cmd);
    cmd.set_graphics_constants({
        _constant_buffer.cbv_descriptor().index(),
        _vertex_buffer.srv_descriptor().index(),
        _texture.srv_descriptor().index(),
    });
    cmd.set_pipeline(_pipeline);
    cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd.set_index_buffer(_index_buffer.index_buffer_view());
    cmd.draw_indexed_instanced(_index_buffer.element_size(), 1, 0, 0, 0);
    _render_targets.end(device, cmd);
}

}  // namespace fb::cube
