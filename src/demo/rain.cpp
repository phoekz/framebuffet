#include "rain.hpp"

namespace fb::rain {

Demo::Demo(Dx& dx) :
    root_signature(dx, Demo::NAME),
    descriptors(dx, Demo::NAME),
    render_targets(
        dx,
        descriptors,
        dx.swapchain_width,
        dx.swapchain_height,
        Demo::CLEAR_COLOR,
        Demo::NAME),
    debug_draw(dx, Demo::NAME) {
    // Descriptors.
    {
        particle_buffer_srv_descriptor = descriptors.cbv_srv_uav().alloc();
        particle_buffer_uav_descriptor = descriptors.cbv_srv_uav().alloc();
        compute.constant_buffer_descriptor = descriptors.cbv_srv_uav().alloc();
        draw.constant_buffer_descriptor = descriptors.cbv_srv_uav().alloc();
    }

    // Particles.
    {
        // Buffer.
        particle_buffer.create(dx, PARTICLE_COUNT, dx_name(Demo::NAME, "Particle Buffer"));

        // Descriptors.
        auto srv_desc = particle_buffer.srv_desc();
        auto uav_desc = particle_buffer.uav_desc();
        dx.device->CreateShaderResourceView(
            particle_buffer.resource(),
            &srv_desc,
            particle_buffer_srv_descriptor.cpu());
        dx.device->CreateUnorderedAccessView(
            particle_buffer.resource(),
            nullptr,
            &uav_desc,
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
        auto buffer = particle_buffer.resource();
        D3D12_SUBRESOURCE_DATA subresource_data = {
            .pData = particles.data(),
            .RowPitch = particle_buffer.byte_size(),
            .SlicePitch = particle_buffer.byte_size(),
        };
        DirectX::ResourceUploadBatch rub(dx.device.get());
        rub.Begin();
        rub.Transition(buffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
        rub.Upload(buffer, 0, &subresource_data, 1);
        rub.Transition(
            buffer,
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        auto finish = rub.End(dx.command_queue.get());
        finish.wait();
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
        compute_shader = sc.compile(compute_name, GpuShaderType::Compute, "cs_main", compute_source);
        vertex_shader = sc.compile(draw_name, GpuShaderType::Vertex, "vs_main", draw_source);
        pixel_shader = sc.compile(draw_name, GpuShaderType::Pixel, "ps_main", draw_source);
    }

    // Compute - Pipeline state.
    {
        D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {
            .pRootSignature = root_signature.get(),
            .CS = compute_shader.bytecode(),
        };
        FAIL_FAST_IF_FAILED(
            dx.device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&compute.pipeline_state)));
        dx_set_name(
            compute.pipeline_state,
            dx_name(Demo::NAME, Demo::Compute::NAME, "Pipeline State"));
    }

    // Compute - Constant buffer.
    {
        compute.constant_buffer.create(
            dx,
            1,
            dx_name(Demo::NAME, Demo::Compute::NAME, "Constant Buffer"));

        auto cbv_desc = compute.constant_buffer.cbv_desc();
        dx.device->CreateConstantBufferView(&cbv_desc, compute.constant_buffer_descriptor.cpu());
    }

    // Draw - Pipeline state.
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {
            .pRootSignature = root_signature.get(),
            .VS = vertex_shader.bytecode(),
            .PS = pixel_shader.bytecode(),
            .BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            .SampleMask = UINT_MAX,
            .RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            .DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT),
            .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
            .NumRenderTargets = 1,
            .RTVFormats = {DXGI_FORMAT_R8G8B8A8_UNORM},
            .DSVFormat = DXGI_FORMAT_D32_FLOAT,
            .SampleDesc = {.Count = 1, .Quality = 0},
        };
        FAIL_FAST_IF_FAILED(
            dx.device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&draw.pipeline_state)));
        dx_set_name(draw.pipeline_state, dx_name(Demo::NAME, Demo::Draw::NAME, "Pipeline State"));
    }

    // Draw - Constant buffer.
    {
        draw.constant_buffer.create(
            dx,
            1,
            dx_name(Demo::NAME, Demo::Draw::NAME, "Constant Buffer"));

        auto cbv_desc = draw.constant_buffer.cbv_desc();
        dx.device->CreateConstantBufferView(&cbv_desc, draw.constant_buffer_descriptor.cpu());
    }
}

void Demo::update(const demo::UpdateDesc& desc) {
    {
        auto& constants = *compute.constant_buffer.ptr();
        constants.delta_time = desc.delta_time;

        if (ImGui::Begin(Demo::NAME.data())) {
            ImGui::SliderFloat("Speed", &constants.speed, 0.0f, 2.0f);
        }
        ImGui::End();
    }

    Matrix transform;
    {
        auto aspect_ratio = desc.aspect_ratio;
        auto fov = rad_from_deg(45.0f);
        auto perspective = Matrix::CreatePerspectiveFieldOfView(fov, aspect_ratio, 0.1f, 100.0f);
        auto eye = Vector3(1.0f, 0.5f, 1.0f);
        auto target = Vector3(0.0f, 0.0f, 0.0f);
        auto up = Vector3(0.0f, 1.0f, 0.0f);
        auto view = Matrix::CreateLookAt(eye, target, up);
        transform = view * perspective;
        auto& constants = *draw.constant_buffer.ptr();
        constants.transform = transform;
    }

    {
        debug_draw.begin(desc.frame_index);
        debug_draw.transform(transform);
        debug_draw.axes();
        debug_draw.end();
    }
}

void Demo::render(Dx& dx) {
    auto* cmd = dx.command_list.get();

    {
        // Transition particles.
        dx.transition(
            particle_buffer.resource(),
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

        // Pipeline state.
        cmd->SetDescriptorHeaps(1, descriptors.cbv_srv_uav().heap_ptr());
        cmd->SetComputeRootSignature(root_signature.get());
        GpuBindings bindings;
        bindings.push(compute.constant_buffer_descriptor);
        bindings.push(particle_buffer_uav_descriptor);
        cmd->SetComputeRoot32BitConstants(0, bindings.capacity(), bindings.ptr(), 0);
        cmd->SetPipelineState(compute.pipeline_state.get());

        // Dispatch.
        cmd->Dispatch(DISPATCH_COUNT, 1, 1);

        // Transition particles.
        dx.transition(
            particle_buffer.resource(),
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    }

    {
        // Begin.
        render_targets.begin(dx);

        // Pipeline state.
        cmd->SetDescriptorHeaps(1, descriptors.cbv_srv_uav().heap_ptr());
        cmd->SetGraphicsRootSignature(root_signature.get());
        GpuBindings bindings;
        bindings.push(draw.constant_buffer_descriptor);
        bindings.push(particle_buffer_srv_descriptor);
        cmd->SetGraphicsRoot32BitConstants(0, bindings.capacity(), bindings.ptr(), 0);
        cmd->SetPipelineState(draw.pipeline_state.get());

        // Input assembler.
        cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

        // Draw.
        cmd->DrawInstanced(PARTICLE_COUNT, 1, 0, 0);

        // Debug.
        debug_draw.render(dx);

        // End.
        render_targets.end(dx);
    }
}

}  // namespace fb::rain
