#include "env.hpp"

namespace fb::env {

Demo::Demo(GpuDevice& device) :
    _descriptors(device, Demo::NAME),
    _samplers(device, _descriptors),
    _render_targets(device, _descriptors, device.swapchain_size(), Demo::CLEAR_COLOR, Demo::NAME),
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
            .DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT),
            .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            .NumRenderTargets = 1,
            .RTVFormats = {DXGI_FORMAT_R8G8B8A8_UNORM},
            .DSVFormat = DXGI_FORMAT_D32_FLOAT,
            .SampleDesc = {.Count = 1, .Quality = 0},
        },
        dx_name(Demo::NAME, "Pipeline State"));

    // Descriptors.
    {
        _constant_buffer_descriptor = _descriptors.cbv_srv_uav().alloc();
        _vertex_buffer_descriptor = _descriptors.cbv_srv_uav().alloc();
        _env_texture_descriptor = _descriptors.cbv_srv_uav().alloc();
    }

    // Constants.
    {
        _constant_buffer.create(device, 1, dx_name(Demo::NAME, "Constant Buffer"));
        device.create_constant_buffer_view(
            _constant_buffer.cbv_desc(),
            _constant_buffer_descriptor.cpu());
    }

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

        device.create_shader_resource_view(
            _vertex_buffer.resource(),
            _vertex_buffer.srv_desc(),
            _vertex_buffer_descriptor.cpu());

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
        _env_texture = device.create_committed_resource(
            CD3DX12_HEAP_PROPERTIES {D3D12_HEAP_TYPE_DEFAULT},
            CD3DX12_RESOURCE_DESC::Tex2D(cube_format, cube_face_size.x, cube_face_size.y, 6, 1),
            D3D12_RESOURCE_STATE_COMMON,
            std::nullopt,
            dx_name(Demo::NAME, "Env Texture"));

        // Create SRV.
        device.create_shader_resource_view(
            _env_texture,
            D3D12_SHADER_RESOURCE_VIEW_DESC {
                .Format = cube_format,
                .ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE,
                .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                .TextureCube =
                    D3D12_TEXCUBE_SRV {
                        .MostDetailedMip = 0,
                        .MipLevels = 1,
                        .ResourceMinLODClamp = 0.0f,
                    },
            },
            _env_texture_descriptor.cpu());

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
            _env_texture,
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

auto Demo::render(GpuDevice& device) -> void {
    _render_targets.begin(device);
    _debug_draw.render(device);
    {
        auto* cmd = device.command_list();
        ID3D12DescriptorHeap* heaps[] = {
            _descriptors.cbv_srv_uav().heap(),
            _descriptors.sampler().heap()};
        cmd->SetDescriptorHeaps(_countof(heaps), heaps);
        cmd->SetGraphicsRootSignature(device.root_signature());
        GpuBindings bindings;
        bindings.push(_constant_buffer_descriptor);
        bindings.push(_vertex_buffer_descriptor);
        bindings.push(_env_texture_descriptor);
        cmd->SetGraphicsRoot32BitConstants(0, bindings.capacity(), bindings.ptr(), 0);
        cmd->SetPipelineState(_pipeline_state.get());

        cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        auto ibv = _index_buffer.index_buffer_view();
        cmd->IASetIndexBuffer(&ibv);

        auto index_count = _index_buffer.element_size();
        cmd->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
    }
    _render_targets.end(device);
}

}  // namespace fb::env
