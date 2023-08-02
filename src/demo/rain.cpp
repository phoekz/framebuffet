#include "rain.hpp"
#include "../shaders.hpp"
#include "../pcg.hpp"

namespace fb::rain {

Demo::Demo(Dx& dx) : root_signature(dx, Demo::NAME), descriptors(dx, Demo::NAME) {
    // Descriptors.
    {
        particle_buffer_descriptor = descriptors.cbv_srv_uav().alloc();
        compute.constant_buffer_descriptor = descriptors.cbv_srv_uav().alloc();
        draw.constant_buffer_descriptor = descriptors.cbv_srv_uav().alloc();
        color_target_descriptor = descriptors.rtv().alloc();
        depth_target_descriptor = descriptors.dsv().alloc();
    }

    // Particles.
    {
        // Buffer.
        particle_buffer.create_uav(
            dx,
            PARTICLE_COUNT,
            GpuBufferAccessMode::GpuExclusive,
            dx_name(Demo::NAME, "Particle Buffer"));

        // Descriptor.
        auto uav_desc = particle_buffer.unordered_access_view_desc();
        dx.device->CreateUnorderedAccessView(
            particle_buffer.resource(),
            nullptr,
            &uav_desc,
            particle_buffer_descriptor.cpu());

        // Vertex buffer view.
        draw.vertex_buffer_view = particle_buffer.vertex_buffer_view();

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
    Shader compute_shader;
    Shader vertex_shader;
    Shader pixel_shader;
    {
        ShaderCompiler sc;
        auto compute_name = dx_name(Demo::NAME, Demo::Compute::NAME);
        auto draw_name = dx_name(Demo::NAME, Demo::Draw::NAME);
        auto compute_source = read_whole_file("shaders/rain.compute.hlsl");
        auto draw_source = read_whole_file("shaders/rain.draw.hlsl");
        compute_shader = sc.compile(compute_name, ShaderType::Compute, "cs_main", compute_source);
        vertex_shader = sc.compile(draw_name, ShaderType::Vertex, "vs_main", draw_source);
        pixel_shader = sc.compile(draw_name, ShaderType::Pixel, "ps_main", draw_source);
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
        compute.constant_buffer.create_cb(
            dx,
            GpuBufferAccessMode::HostWritable,
            dx_name(Demo::NAME, Demo::Compute::NAME, "Constant Buffer"));

        auto cbv_desc = compute.constant_buffer.constant_buffer_view_desc();
        dx.device->CreateConstantBufferView(&cbv_desc, compute.constant_buffer_descriptor.cpu());
    }

    // Draw - Pipeline state.
    {
        D3D12_INPUT_ELEMENT_DESC input_element_descs[] = {{
            .SemanticName = "POSITION",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = 0,
            .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0,
        }};
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {
            .pRootSignature = root_signature.get(),
            .VS = vertex_shader.bytecode(),
            .PS = pixel_shader.bytecode(),
            .BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            .SampleMask = UINT_MAX,
            .RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            .DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT),
            .InputLayout = {input_element_descs, _countof(input_element_descs)},
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
        draw.constant_buffer.create_cb(
            dx,
            GpuBufferAccessMode::HostWritable,
            dx_name(Demo::NAME, Demo::Draw::NAME, "Constant Buffer"));

        auto cbv_desc = draw.constant_buffer.constant_buffer_view_desc();
        dx.device->CreateConstantBufferView(&cbv_desc, draw.constant_buffer_descriptor.cpu());
    }

    // Color target.
    {
        CD3DX12_HEAP_PROPERTIES color_target_heap(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC color_target_desc = CD3DX12_RESOURCE_DESC::Tex2D(
            DXGI_FORMAT_R8G8B8A8_UNORM,
            dx.swapchain_width,
            dx.swapchain_height,
            1,
            1,
            1,
            0,
            D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
        D3D12_CLEAR_VALUE color_clear_value = {
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .Color = {0.0f, 0.0f, 0.0f, 1.0f},
        };
        FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
            &color_target_heap,
            D3D12_HEAP_FLAG_NONE,
            &color_target_desc,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            &color_clear_value,
            IID_PPV_ARGS(&color_target)));
        dx_set_name(color_target, dx_name(Demo::NAME, "Color Target"));

        dx.device->CreateRenderTargetView(
            color_target.get(),
            nullptr,
            color_target_descriptor.cpu());
    }

    // Depth target.
    {
        CD3DX12_HEAP_PROPERTIES depth_target_heap(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC depth_target_desc = CD3DX12_RESOURCE_DESC::Tex2D(
            DXGI_FORMAT_D32_FLOAT,
            dx.swapchain_width,
            dx.swapchain_height,
            1,
            1,
            1,
            0,
            D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
        D3D12_CLEAR_VALUE depth_clear_value = {
            .Format = DXGI_FORMAT_D32_FLOAT,
            .DepthStencil = {.Depth = 1.0f, .Stencil = 0}};
        FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
            &depth_target_heap,
            D3D12_HEAP_FLAG_NONE,
            &depth_target_desc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depth_clear_value,
            IID_PPV_ARGS(&depth_target)));
        dx_set_name(depth_target, dx_name(Demo::NAME, "Depth Target"));

        dx.device->CreateDepthStencilView(
            depth_target.get(),
            nullptr,
            depth_target_descriptor.cpu());
    }
}

void Demo::update(const UpdateParams& params) {
    {
        auto& constants = *compute.constant_buffer.ptr();
        constants.delta_time = params.delta_time;

        if (ImGui::Begin(Demo::NAME)) {
            ImGui::SliderFloat("Speed", &constants.speed, 0.0f, 2.0f);
        }
        ImGui::End();
    }

    {
        auto aspect_ratio = params.aspect_ratio;
        auto fov = rad_from_deg(45.0f);
        auto perspective = Matrix::CreatePerspectiveFieldOfView(fov, aspect_ratio, 0.1f, 100.0f);
        auto eye = Vector3(1.0f, 0.5f, 1.0f);
        auto target = Vector3(0.0f, 0.0f, 0.0f);
        auto up = Vector3(0.0f, 1.0f, 0.0f);
        auto view = Matrix::CreateLookAt(eye, target, up);
        auto& constants = *draw.constant_buffer.ptr();
        constants.transform = view * perspective;
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
        bindings.push(particle_buffer_descriptor);
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
        constexpr float CLEAR_COLOR[4] = {0.0f, 0.0f, 0.0f, 1.0f};

        // Transition render targets.
        dx.transition(
            color_target,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_RENDER_TARGET);

        // Render targets.
        D3D12_VIEWPORT viewport = {
            .TopLeftX = 0.0f,
            .TopLeftY = 0.0f,
            .Width = (float)dx.swapchain_width,
            .Height = (float)dx.swapchain_height,
            .MinDepth = 0.0f,
            .MaxDepth = 1.0f,
        };
        D3D12_RECT scissor = {
            .left = 0,
            .top = 0,
            .right = (LONG)dx.swapchain_width,
            .bottom = (LONG)dx.swapchain_height,
        };
        cmd->RSSetViewports(1, &viewport);
        cmd->RSSetScissorRects(1, &scissor);
        cmd->OMSetRenderTargets(
            1,
            color_target_descriptor.cpu_ptr(),
            FALSE,
            depth_target_descriptor.cpu_ptr());
        cmd->ClearRenderTargetView(color_target_descriptor.cpu(), CLEAR_COLOR, 0, nullptr);
        cmd->ClearDepthStencilView(
            depth_target_descriptor.cpu(),
            D3D12_CLEAR_FLAG_DEPTH,
            1.0f,
            0,
            0,
            nullptr);

        // Pipeline state.
        cmd->SetDescriptorHeaps(1, descriptors.cbv_srv_uav().heap_ptr());
        cmd->SetGraphicsRootSignature(root_signature.get());
        GpuBindings bindings;
        bindings.push(draw.constant_buffer_descriptor);
        cmd->SetGraphicsRoot32BitConstants(0, bindings.capacity(), bindings.ptr(), 0);
        cmd->SetPipelineState(draw.pipeline_state.get());

        // Input assembler.
        cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
        cmd->IASetVertexBuffers(0, 1, &draw.vertex_buffer_view);

        // Draw.
        cmd->DrawInstanced(PARTICLE_COUNT, 1, 0, 0);

        // Transition render targets.
        dx.transition(
            color_target,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

}  // namespace fb::rain
