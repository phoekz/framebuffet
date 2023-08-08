#include "env.hpp"

namespace fb::env {

Demo::Demo(GpuDevice& device) :
    _render_targets(device, device.swapchain_size(), Demo::CLEAR_COLOR, Demo::NAME),
    _debug_draw(device, Demo::NAME) {
    // Shaders.
    GpuShader vertex_shader;
    GpuShader pixel_shader;
    {
        GpuShaderCompiler sc;
        auto source = read_whole_file("shaders/env.hlsl");
        vertex_shader = sc.compile(Demo::NAME, GpuShaderType::Vertex, "vs_main", source);
        pixel_shader = sc.compile(Demo::NAME, GpuShaderType::Pixel, "ps_main", source);
    }

    // Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(vertex_shader.bytecode())
        .pixel_shader(pixel_shader.bytecode())
        .depth_stencil(GPU_PIPELINE_DEPTH_DEFAULT)
        .render_target_formats({DXGI_FORMAT_R8G8B8A8_UNORM})
        .depth_stencil_format(DXGI_FORMAT_D32_FLOAT)
        .build(device, _pipeline, dx_name(Demo::NAME, "Pipeline"));

    // Constants.
    _constant_buffer.create(device, 1, dx_name(Demo::NAME, "Constant Buffer"));

    // Geometry.
    {
        using namespace DirectX::DX12;
        using Vertices = GeometricPrimitive::VertexCollection;
        using Indices = GeometricPrimitive::IndexCollection;
        Vertices vertices;
        Indices indices;
        GeometricPrimitive::CreateBox(vertices, indices, {2.0f, 2.0f, 2.0f}, false, true);
        auto vertex_span =
            std::span(reinterpret_cast<const Vertex*>(vertices.data()), vertices.size());
        auto index_span =
            std::span(reinterpret_cast<const uint16_t*>(indices.data()), indices.size());
        _vertex_buffer.create_with_data(device, vertex_span, dx_name(Demo::NAME, "Vertex Buffer"));
        _index_buffer.create_with_data(device, index_span, dx_name(Demo::NAME, "Index Buffer"));
    }

    // Environment map.
    {
        // Load faces.
        std::vector<ExrImage> cube_faces;
        cube_faces.reserve(6);
        std::tuple<CubeFace, std::string_view> cube_face_paths[] = {
            {CubeFace::PosX, "assets/envmaps/winter_evening_1k_+X.exr"sv},
            {CubeFace::NegX, "assets/envmaps/winter_evening_1k_-X.exr"sv},
            {CubeFace::PosY, "assets/envmaps/winter_evening_1k_+Y.exr"sv},
            {CubeFace::NegY, "assets/envmaps/winter_evening_1k_-Y.exr"sv},
            {CubeFace::PosZ, "assets/envmaps/winter_evening_1k_+Z.exr"sv},
            {CubeFace::NegZ, "assets/envmaps/winter_evening_1k_-Z.exr"sv},
        };
        for (auto& [face, path] : cube_face_paths) {
            cube_faces.push_back(std::move(ExrImage::load(path)));
        }

        // Swap Z faces to match DirectX convention.
        std::swap(cube_faces[4], cube_faces[5]);

        // Validate.
        auto cube_face_size = cube_faces[0].size();
        auto cube_format = cube_faces[0].format();
        FB_ASSERT(std::ranges::all_of(cube_faces, [&](const auto& face) {
            return face.width() == cube_face_size.x && face.height() == cube_face_size.y
                && face.format() == cube_format;
        }));

        // Create texture.
        _env_texture.create(
            device,
            GpuTexture2dDesc {
                .format = cube_format,
                .width = cube_face_size.x,
                .height = cube_face_size.y,
                .depth = 6,
            },
            dx_name(Demo::NAME, "Env Texture"));

        // Upload faces.
        D3D12_SUBRESOURCE_DATA subresources[6];
        for (uint32_t i = 0; i < 6; ++i) {
            const auto& face = cube_faces[i];
            subresources[i] = {
                .pData = face.data(),
                .RowPitch = (int64_t)face.row_pitch(),
                .SlicePitch = (int64_t)face.slice_pitch(),
            };
        }
        device.easy_multi_upload(
            subresources,
            _env_texture.resource(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

static float camera_fov = rad_from_deg(70.0f);
static float camera_distance = 1.25f;
static float camera_longitude = rad_from_deg(45.0f);
static float camera_latitude = rad_from_deg(0.0f);
static float camera_rotation_speed = 0.5f;

auto Demo::gui(const gui::Desc&) -> void {
    ImGui::SliderAngle("Camera FOV", &camera_fov, 1.0f, 90.0f);
    ImGui::SliderFloat("Camera Distance", &camera_distance, 1.0f, 10.0f);
    ImGui::SliderAngle("Camera Longitude", &camera_longitude, -180.0f, 180.0f);
    ImGui::SliderAngle("Camera Latitude", &camera_latitude, -90.0f, 90.0f);
    ImGui::SliderFloat("Camera Rotation Speed", &camera_rotation_speed, 0.0f, 1.0f);
}

auto Demo::update(const demo::UpdateDesc& desc) -> void {
    camera_longitude += camera_rotation_speed * desc.delta_time;
    if (camera_longitude > PI * 2.0f) {
        camera_longitude -= PI * 2.0f;
    }

    float aspect_ratio = desc.aspect_ratio;
    Matrix perspective =
        Matrix::CreatePerspectiveFieldOfView(camera_fov, aspect_ratio, 0.1f, 100.0f);
    Vector3 eye = camera_distance * dir_from_lonlat(camera_longitude, camera_latitude);
    Matrix view = Matrix::CreateLookAt(eye, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
    Matrix env_view = view;
    env_view.m[3][0] = 0.0f;
    env_view.m[3][1] = 0.0f;
    env_view.m[3][2] = 0.0f;
    env_view.m[3][3] = 1.0f;
    Matrix env_transform = env_view * perspective;
    _constant_buffer.ptr()->transform = env_transform;

    Matrix camera_transform = view * perspective;
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
        _env_texture.srv_descriptor().index(),
    });
    cmd.set_pipeline(_pipeline);
    cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd.set_index_buffer(_index_buffer.index_buffer_view());
    cmd.draw_indexed_instanced(_index_buffer.element_size(), 1, 0, 0, 0);
    _render_targets.end(device, cmd);
}

}  // namespace fb::env
