#include "rain.hpp"

namespace fb::rain {

Demo::Demo(GpuDevice& device) :
    descriptors(device, Demo::NAME),
    render_targets(device, descriptors, device.swapchain_size(), Demo::CLEAR_COLOR, Demo::NAME),
    debug_draw(device, Demo::NAME) {
    // Descriptors.
    {
        particle_buffer_srv_descriptor = descriptors.cbv_srv_uav().alloc();
        particle_buffer_uav_descriptor = descriptors.cbv_srv_uav().alloc();
        compute.constant_buffer_descriptor = descriptors.cbv_srv_uav().alloc();
        draw.constant_buffer_descriptor = descriptors.cbv_srv_uav().alloc();
        draw.vertex_buffer_descriptor = descriptors.cbv_srv_uav().alloc();
    }

    // Particles.
    {
        // Buffer.
        particle_buffer.create(device, PARTICLE_COUNT, dx_name(Demo::NAME, "Particle Buffer"));

        // Descriptors.
        device.create_shader_resource_view(
            particle_buffer.resource(),
            particle_buffer.srv_desc(),
            particle_buffer_srv_descriptor.cpu());
        device.create_unordered_access_view(
            particle_buffer.resource(),
            std::nullopt,
            particle_buffer.uav_desc(),
            particle_buffer_uav_descriptor.cpu());

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
                .RowPitch = particle_buffer.byte_size(),
                .SlicePitch = particle_buffer.byte_size()},
            particle_buffer.resource(),
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
    compute.pipeline_state = device.create_compute_pipeline_state(
        D3D12_COMPUTE_PIPELINE_STATE_DESC {
            .pRootSignature = device.root_signature(),
            .CS = compute_shader.bytecode()},
        dx_name(Demo::NAME, Demo::Compute::NAME, "Pipeline State"));

    // Compute - Constant buffer.
    {
        compute.constant_buffer.create(
            device,
            1,
            dx_name(Demo::NAME, Demo::Compute::NAME, "Constant Buffer"));
        device.create_constant_buffer_view(
            compute.constant_buffer.cbv_desc(),
            compute.constant_buffer_descriptor.cpu());
    }

    // Draw - Pipeline state.
    using CommonStates = DirectX::DX12::CommonStates;
    draw.pipeline_state = device.create_graphics_pipeline_state(
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
    {
        draw.constant_buffer.create(
            device,
            1,
            dx_name(Demo::NAME, Demo::Draw::NAME, "Constant Buffer"));
        device.create_constant_buffer_view(
            draw.constant_buffer.cbv_desc(),
            draw.constant_buffer_descriptor.cpu());
    }

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

        draw.vertex_buffer.create(
            device,
            vertex_count,
            dx_name(Demo::NAME, Demo::Draw::NAME, "Vertex Buffer"));
        draw.index_buffer.create(
            device,
            index_count,
            dx_name(Demo::NAME, Demo::Draw::NAME, "Index Buffer"));

        device.create_shader_resource_view(
            draw.vertex_buffer.resource(),
            draw.vertex_buffer.srv_desc(),
            draw.vertex_buffer_descriptor.cpu());

        memcpy(draw.vertex_buffer.ptr(), vertices, sizeof(vertices));
        memcpy(draw.index_buffer.ptr(), indices, sizeof(indices));
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
        auto& compute_constants = *compute.constant_buffer.ptr();
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
        auto& draw_constants = *draw.constant_buffer.ptr();

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
        debug_draw.begin(desc.frame_index);
        debug_draw.transform(camera_transform);
        debug_draw.axes();
        debug_draw.end();
    }
}

void Demo::render(GpuDevice& device) {
    auto* cmd = device.command_list();

    {
        // Transition particles.
        device.transition(
            particle_buffer.resource(),
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

        // Pipeline state.
        cmd->SetDescriptorHeaps(1, descriptors.cbv_srv_uav().heap_ptr());
        cmd->SetComputeRootSignature(device.root_signature());
        GpuBindings bindings;
        bindings.push(compute.constant_buffer_descriptor);
        bindings.push(particle_buffer_uav_descriptor);
        cmd->SetComputeRoot32BitConstants(0, bindings.capacity(), bindings.ptr(), 0);
        cmd->SetPipelineState(compute.pipeline_state.get());

        // Dispatch.
        cmd->Dispatch(DISPATCH_COUNT, 1, 1);

        // Transition particles.
        device.transition(
            particle_buffer.resource(),
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    }

    {
        // Begin.
        render_targets.begin(device);

        // Pipeline state.
        cmd->SetDescriptorHeaps(1, descriptors.cbv_srv_uav().heap_ptr());
        cmd->SetGraphicsRootSignature(device.root_signature());
        GpuBindings bindings;
        bindings.push(draw.constant_buffer_descriptor);
        bindings.push(draw.vertex_buffer_descriptor);
        bindings.push(particle_buffer_srv_descriptor);
        cmd->SetGraphicsRoot32BitConstants(0, bindings.capacity(), bindings.ptr(), 0);
        cmd->SetPipelineState(draw.pipeline_state.get());

        // Input assembler.
        cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        auto ibv = draw.index_buffer.index_buffer_view();
        cmd->IASetIndexBuffer(&ibv);

        // Draw.
        cmd->DrawIndexedInstanced(6, PARTICLE_COUNT, 0, 0, 0);

        // Debug.
        debug_draw.render(device);

        // End.
        render_targets.end(device);
    }
}

}  // namespace fb::rain
