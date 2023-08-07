#include "rain.hpp"

namespace fb::rain {

Demo::Demo(GpuDevice& device) :
    _render_targets(device, device.swapchain_size(), Demo::CLEAR_COLOR, Demo::NAME),
    _debug_draw(device, Demo::NAME) {
    // Particles.
    {
        // Buffer.
        _particle_buffer.create(device, PARTICLE_COUNT, dx_name(Demo::NAME, "Particle Buffer"));

        // Data.
        pcg rand;
        std::vector<Particle> particles(PARTICLE_COUNT);
        for (uint32_t i = 0; i < PARTICLE_COUNT; i++) {
            Particle& particle = particles[i];
            particle.position.x = 2.0f * rand.random_float() - 1.0f;
            particle.position.y = 2.0f * rand.random_float() - 1.0f;
            particle.position.z = 2.0f * rand.random_float() - 1.0f;
        }

        // Upload.
        device.easy_upload(
            D3D12_SUBRESOURCE_DATA {
                .pData = particles.data(),
                .RowPitch = _particle_buffer.byte_size(),
                .SlicePitch = _particle_buffer.byte_size()},
            _particle_buffer.resource(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    }

    // Shaders.
    GpuShader compute_shader;
    GpuShader vertex_shader;
    GpuShader pixel_shader;
    {
        GpuShaderCompiler sc;
        auto compute_name = dx_name(Demo::NAME, Demo::Compute::NAME);
        auto draw_name = dx_name(Demo::NAME, Demo::Draw::NAME);
        auto compute_source = read_whole_file("shaders/rain.compute.hlsl");
        auto draw_source = read_whole_file("shaders/rain.draw.hlsl");
        compute_shader =
            sc.compile(compute_name, GpuShaderType::Compute, "cs_main", compute_source);
        vertex_shader = sc.compile(draw_name, GpuShaderType::Vertex, "vs_main", draw_source);
        pixel_shader = sc.compile(draw_name, GpuShaderType::Pixel, "ps_main", draw_source);
    }

    // Compute - Pipeline state.
    _compute.pipeline_state = device.create_compute_pipeline_state(
        D3D12_COMPUTE_PIPELINE_STATE_DESC {
            .pRootSignature = device.root_signature(),
            .CS = compute_shader.bytecode()},
        dx_name(Demo::NAME, Demo::Compute::NAME, "Pipeline State"));

    // Compute - Constant buffer.
    _compute.constant_buffer.create(
        device,
        1,
        dx_name(Demo::NAME, Demo::Compute::NAME, "Constant Buffer"));

    // Draw - Pipeline state.
    using CommonStates = DirectX::DX12::CommonStates;
    _draw.pipeline_state = device.create_graphics_pipeline_state(
        {
            .pRootSignature = device.root_signature(),
            .VS = vertex_shader.bytecode(),
            .PS = pixel_shader.bytecode(),
            .BlendState = CommonStates::Additive,
            .SampleMask = UINT_MAX,
            .RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            .DepthStencilState = CommonStates::DepthNone,
            .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            .NumRenderTargets = 1,
            .RTVFormats = {DXGI_FORMAT_R8G8B8A8_UNORM},
            .DSVFormat = DXGI_FORMAT_D32_FLOAT,
            .SampleDesc = {.Count = 1, .Quality = 0},
        },
        dx_name(Demo::NAME, Demo::Draw::NAME, "Pipeline State"));

    // Draw - Constant buffer.
    _draw.constant_buffer.create(
        device,
        1,
        dx_name(Demo::NAME, Demo::Draw::NAME, "Constant Buffer"));

    // Draw - Vertex buffer.
    {
        Vertex vertices[] = {
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
            {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}},
            {{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f}},
            {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
        };

        uint16_t indices[] = {0, 1, 2, 0, 2, 3};
        uint32_t vertex_count = (uint32_t)_countof(vertices);
        uint32_t index_count = (uint32_t)_countof(indices);

        _draw.vertex_buffer.create(
            device,
            vertex_count,
            dx_name(Demo::NAME, Demo::Draw::NAME, "Vertex Buffer"));
        _draw.index_buffer.create(
            device,
            index_count,
            dx_name(Demo::NAME, Demo::Draw::NAME, "Index Buffer"));

        memcpy(_draw.vertex_buffer.ptr(), vertices, sizeof(vertices));
        memcpy(_draw.index_buffer.ptr(), indices, sizeof(indices));
    }
}

void Demo::update(const demo::UpdateDesc& desc) {
    static float camera_distance = 1.25f;
    static float camera_lon = rad_from_deg(45.0f);
    static float camera_lat = rad_from_deg(-15.0f);
    static float camera_rotation_speed = 0.05f;
    static float particle_width = 0.01f;
    static float particle_height = 0.075f;

    // Update light angle.
    {
        camera_lon += camera_rotation_speed * desc.delta_time;
        if (camera_lon > PI * 2.0f) {
            camera_lon -= PI * 2.0f;
        }
    }

    // ImGui.
    {
        auto& compute_constants = *_compute.constant_buffer.ptr();
        compute_constants.delta_time = desc.delta_time;

        if (ImGui::Begin(Demo::NAME.data())) {
            ImGui::SliderFloat("Speed", &compute_constants.speed, 0.0f, 2.0f);
            ImGui::SliderFloat("Camera Distance", &camera_distance, 1.0f, 10.0f);
            ImGui::SliderAngle("Camera Longitude", &camera_lon, -180.0f, 180.0f);
            ImGui::SliderAngle("Camera Latitude", &camera_lat, -90.0f, 90.0f);
            ImGui::SliderFloat("Camera Rotation Speed", &camera_rotation_speed, 0.0f, 1.0f);
            ImGui::SliderFloat("Particle Width", &particle_width, 0.0f, 1.0f);
            ImGui::SliderFloat("Particle Height", &particle_height, 0.0f, 1.0f);
        }
        ImGui::End();
    }

    Matrix camera_transform;
    {
        auto& draw_constants = *_draw.constant_buffer.ptr();

        constexpr auto FOV = rad_from_deg(45.0f);
        auto aspect_ratio = desc.aspect_ratio;
        auto projection = Matrix::CreatePerspectiveFieldOfView(FOV, aspect_ratio, 0.1f, 100.0f);

        Vector3 eye;
        eye.x = camera_distance * cos(camera_lat) * cos(camera_lon);
        eye.y = camera_distance * sin(camera_lat);
        eye.z = camera_distance * cos(camera_lat) * sin(camera_lon);

        auto target = Vector3(0.0f, 0.0f, 0.0f);
        auto up = Vector3(0.0f, 1.0f, 0.0f);
        auto view = Matrix::CreateLookAt(eye, target, up);

        auto from_dir = Vector3::UnitZ;
        auto to_dir = Vector3(eye.x, 0.0f, eye.z);
        auto rot_quat = Quaternion::FromToRotation(from_dir, to_dir);
        auto rot_matrix = Matrix::CreateFromQuaternion(rot_quat);
        auto scale = Matrix::CreateScale(particle_width, particle_height, 1.0f);
        auto particle_transform = scale * rot_matrix;

        camera_transform = view * projection;
        draw_constants.transform = camera_transform;
        draw_constants.particle_transform = particle_transform;
    }

    {
        _debug_draw.begin(desc.frame_index);
        _debug_draw.transform(camera_transform);
        _debug_draw.axes();
        _debug_draw.end();
    }
}

void Demo::render(GpuDevice& device) {
    auto* cmd = device.command_list();

    {
        // Transition particles.
        device.transition(
            _particle_buffer.resource(),
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

        // Pipeline state.
        GpuBindings bindings;
        bindings.push(_compute.constant_buffer.cbv_descriptor());
        bindings.push(_particle_buffer.uav_descriptor());
        device.cmd_set_compute();
        cmd->SetComputeRoot32BitConstants(0, bindings.capacity(), bindings.ptr(), 0);
        cmd->SetPipelineState(_compute.pipeline_state.get());

        // Dispatch.
        cmd->Dispatch(DISPATCH_COUNT, 1, 1);

        // Transition particles.
        device.transition(
            _particle_buffer.resource(),
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    }

    {
        // Begin.
        _render_targets.begin(device);

        // Pipeline state.
        GpuBindings bindings;
        bindings.push(_draw.constant_buffer.cbv_descriptor());
        bindings.push(_draw.vertex_buffer.srv_descriptor());
        bindings.push(_particle_buffer.srv_descriptor());
        device.cmd_set_graphics();
        cmd->SetGraphicsRoot32BitConstants(0, bindings.capacity(), bindings.ptr(), 0);
        cmd->SetPipelineState(_draw.pipeline_state.get());

        // Input assembler.
        cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        auto ibv = _draw.index_buffer.index_buffer_view();
        cmd->IASetIndexBuffer(&ibv);

        // Draw.
        cmd->DrawIndexedInstanced(6, PARTICLE_COUNT, 0, 0, 0);

        // Debug.
        _debug_draw.render(device);

        // End.
        _render_targets.end(device);
    }
}

}  // namespace fb::rain
