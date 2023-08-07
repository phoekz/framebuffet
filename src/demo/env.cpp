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

    // Pipeline state.
    _pipeline_state = device.create_graphics_pipeline_state(
        D3D12_GRAPHICS_PIPELINE_STATE_DESC {
            .pRootSignature = device.root_signature(),
            .VS = vertex_shader.bytecode(),
            .PS = pixel_shader.bytecode(),
            .BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            .SampleMask = UINT_MAX,
            .RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            .DepthStencilState =
                D3D12_DEPTH_STENCIL_DESC {
                    .DepthEnable = true,
                    .DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,
                    .DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL,
                },
            .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            .NumRenderTargets = 1,
            .RTVFormats = {DXGI_FORMAT_R8G8B8A8_UNORM},
            .DSVFormat = DXGI_FORMAT_D32_FLOAT,
            .SampleDesc = {.Count = 1, .Quality = 0},
        },
        dx_name(Demo::NAME, "Pipeline State"));

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
        _vertex_buffer.create(
            device,
            (uint32_t)vertices.size(),
            dx_name(Demo::NAME, "Vertex Buffer"));
        _index_buffer.create(device, (uint32_t)indices.size(), dx_name(Demo::NAME, "Index Buffer"));

        memcpy(_vertex_buffer.ptr(), vertices.data(), vertices.size() * sizeof(Vertex));
        memcpy(_index_buffer.ptr(), indices.data(), indices.size() * sizeof(Index));
    }

    // Environment map.
    {
        // Load faces.
        std::vector<ExrImage> cube_faces;
        cube_faces.reserve(6);
        std::tuple<CubeFace, std::string_view> cube_face_paths[] = {
            {CubeFace::PosX, "envmaps/winter_evening_1k_+X.exr"sv},
            {CubeFace::NegX, "envmaps/winter_evening_1k_-X.exr"sv},
            {CubeFace::PosY, "envmaps/winter_evening_1k_+Y.exr"sv},
            {CubeFace::NegY, "envmaps/winter_evening_1k_-Y.exr"sv},
            {CubeFace::PosZ, "envmaps/winter_evening_1k_+Z.exr"sv},
            {CubeFace::NegZ, "envmaps/winter_evening_1k_-Z.exr"sv},
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

auto Demo::update(const demo::UpdateDesc& desc) -> void {
    static float camera_fov = rad_from_deg(70.0f);
    static float camera_distance = 1.25f;
    static float camera_lon = rad_from_deg(45.0f);
    static float camera_lat = rad_from_deg(0.0f);
    static float camera_rotation_speed = 0.5f;

    // Update light angle.
    {
        camera_lon += camera_rotation_speed * desc.delta_time;
        if (camera_lon > PI * 2.0f) {
            camera_lon -= PI * 2.0f;
        }
    }

    // ImGui.
    if (ImGui::Begin(Demo::NAME.data())) {
        ImGui::SliderAngle("Camera FOV", &camera_fov, 1.0f, 90.0f);
        ImGui::SliderFloat("Camera Distance", &camera_distance, 1.0f, 10.0f);
        ImGui::SliderAngle("Camera Longitude", &camera_lon, -180.0f, 180.0f);
        ImGui::SliderAngle("Camera Latitude", &camera_lat, -90.0f, 90.0f);
        ImGui::SliderFloat("Camera Rotation Speed", &camera_rotation_speed, 0.0f, 1.0f);
    }
    ImGui::End();

    float aspect_ratio = desc.aspect_ratio;
    Matrix perspective =
        Matrix::CreatePerspectiveFieldOfView(camera_fov, aspect_ratio, 0.1f, 100.0f);
    Vector3 eye;
    eye.x = camera_distance * cos(camera_lat) * cos(camera_lon);
    eye.y = camera_distance * sin(camera_lat);
    eye.z = camera_distance * cos(camera_lat) * sin(camera_lon);
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
    cmd.set_pipeline(_pipeline_state);
    cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd.set_index_buffer(_index_buffer.index_buffer_view());
    cmd.draw_indexed_instanced(_index_buffer.element_size(), 1, 0, 0, 0);
    _render_targets.end(device, cmd);
}

}  // namespace fb::env
