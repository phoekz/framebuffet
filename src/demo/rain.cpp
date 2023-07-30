#include "rain.hpp"
#include "../shaders.hpp"
#include "../pcg.hpp"
#include <directxtk12/CommonStates.h>
#include <DirectXTK12/ResourceUploadBatch.h>
#include <imgui.h>

namespace fb::rain {

Demo::Demo(Dx& dx) {
    // Particles.
    {
        // Buffer.
        particle_buffer.create_uav(
            dx,
            PARTICLE_COUNT,
            D3D12_HEAP_TYPE_DEFAULT,
            D3D12_RESOURCE_STATE_COMMON,
            "Rain",
            "Particle Buffer");
        draw.vertex_buffer_view = particle_buffer.vertex_buffer_view();
        auto buffer = particle_buffer.resource.get();

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
        D3D12_SUBRESOURCE_DATA subresource_data = {
            .pData = particles.data(),
            .RowPitch = particle_buffer.byte_size,
            .SlicePitch = particle_buffer.byte_size,
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
    fb::Shader compute_shader;
    fb::Shader vertex_shader;
    fb::Shader pixel_shader;
    {
        fb::ShaderCompiler sc;
        auto compute_name = "rain-compute";
        auto draw_name = "rain-draw";
        auto compute_source = fb::read_whole_file("shaders/rain.compute.hlsl");
        auto draw_source = fb::read_whole_file("shaders/rain.draw.hlsl");
        compute_shader =
            sc.compile(compute_name, fb::ShaderType::Compute, "cs_main", compute_source);
        vertex_shader = sc.compile(draw_name, fb::ShaderType::Vertex, "vs_main", draw_source);
        pixel_shader = sc.compile(draw_name, fb::ShaderType::Pixel, "ps_main", draw_source);
    }

    // Compute - Root signature.
    {
        CD3DX12_ROOT_PARAMETER1 root_parameters[2];
        root_parameters[0].InitAsConstants(2, 0, 0, D3D12_SHADER_VISIBILITY_ALL);
        root_parameters[1].InitAsUnorderedAccessView(
            0,
            0,
            D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc;
        decltype(desc)::Init_1_2(
            desc,
            _countof(root_parameters),
            root_parameters,
            0,
            nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_NONE);

        fb::ComPtr<ID3DBlob> signature;
        fb::ComPtr<ID3DBlob> error;
        FAIL_FAST_IF_FAILED(D3DX12SerializeVersionedRootSignature(
            &desc,
            D3D_ROOT_SIGNATURE_VERSION_1_2,
            &signature,
            &error));
        FAIL_FAST_IF_FAILED(dx.device->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&compute.root_signature)));
        fb::dx_set_name(compute.root_signature, "Rain - Compute - Root Signature");
    }

    // Compute - Pipeline state.
    {
        D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {
            .pRootSignature = compute.root_signature.get(),
            .CS = compute_shader.bytecode(),
        };
        FAIL_FAST_IF_FAILED(
            dx.device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&compute.pipeline_state)));
        fb::dx_set_name(compute.pipeline_state, "Rain - Compute - Pipeline State");
    }

    // Compute - Descriptor heap.
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .NumDescriptors = 1,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
        };
        FAIL_FAST_IF_FAILED(
            dx.device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&compute.descriptor_heap)));
        fb::dx_set_name(compute.descriptor_heap, "Rain - Compute - Descriptor Heap");
    }

    // Draw - Root signature.
    {
        CD3DX12_ROOT_PARAMETER1 root_parameters[1];
        root_parameters[0].InitAsConstants(16, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc;
        decltype(desc)::Init_1_2(
            desc,
            _countof(root_parameters),
            root_parameters,
            0,
            nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        fb::ComPtr<ID3DBlob> signature;
        fb::ComPtr<ID3DBlob> error;
        FAIL_FAST_IF_FAILED(D3DX12SerializeVersionedRootSignature(
            &desc,
            D3D_ROOT_SIGNATURE_VERSION_1_2,
            &signature,
            &error));
        FAIL_FAST_IF_FAILED(dx.device->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&draw.root_signature)));
        fb::dx_set_name(draw.root_signature, "Rain - Draw - Root Signature");
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
            .pRootSignature = draw.root_signature.get(),
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
        fb::dx_set_name(draw.pipeline_state, "Rain - Draw - Pipeline State");
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
        fb::dx_set_name(color_target, "Rain - Color Target");

        D3D12_DESCRIPTOR_HEAP_DESC descriptors_desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            .NumDescriptors = 1,
        };
        FAIL_FAST_IF_FAILED(dx.device->CreateDescriptorHeap(
            &descriptors_desc,
            IID_PPV_ARGS(&color_target_descriptor_heap)));
        fb::dx_set_name(color_target_descriptor_heap, "Rain - Color Target Descriptor Heap");
        color_target_descriptor =
            color_target_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        dx.device->CreateRenderTargetView(color_target.get(), nullptr, color_target_descriptor);
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
        fb::dx_set_name(depth_target, "Rain - Depth Target");

        D3D12_DESCRIPTOR_HEAP_DESC descriptors_desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
            .NumDescriptors = 1,
        };
        FAIL_FAST_IF_FAILED(dx.device->CreateDescriptorHeap(
            &descriptors_desc,
            IID_PPV_ARGS(&depth_target_descriptor_heap)));
        fb::dx_set_name(depth_target_descriptor_heap, "Rain - Depth Target Descriptor Heap");
        depth_target_descriptor =
            depth_target_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        dx.device->CreateDepthStencilView(depth_target.get(), nullptr, depth_target_descriptor);
    }
}

void Demo::update(const UpdateParams& params) {
    compute.constants.delta_time = params.delta_time;

    if (ImGui::Begin("Rain")) {
        ImGui::SliderFloat("Speed", &compute.constants.speed, 0.0f, 2.0f);
        ImGui::End();
    }

    float aspect_ratio = params.aspect_ratio;
    fb::Mat4x4 perspective = fb::Mat4x4::CreatePerspectiveFieldOfView(
        fb::rad_from_deg(45.0f),
        aspect_ratio,
        0.1f,
        100.0f);
    fb::Vec3 eye = fb::Vec3(1.0f, 0.5f, 1.0f);
    fb::Mat4x4 view =
        fb::Mat4x4::CreateLookAt(eye, fb::Vec3(0.0f, 0.0f, 0.0f), fb::Vec3(0.0f, 1.0f, 0.0f));
    draw.transform = view * perspective;
}

void Demo::render(Dx& dx) {
    constexpr float CLEAR_COLOR[4] = {0.0f, 0.0f, 0.0f, 1.0f};
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

    auto* cmd = dx.command_list.get();
    dx.transition(
        particle_buffer.resource,
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    cmd->SetComputeRootSignature(compute.root_signature.get());
    cmd->SetComputeRoot32BitConstants(0, 2, &compute.constants, 0);
    cmd->SetComputeRootUnorderedAccessView(1, particle_buffer.resource->GetGPUVirtualAddress());
    cmd->SetPipelineState(compute.pipeline_state.get());
    cmd->Dispatch(DISPATCH_COUNT, 1, 1);
    dx.transition(
        particle_buffer.resource,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

    dx.transition(
        color_target,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
    cmd->RSSetViewports(1, &viewport);
    cmd->RSSetScissorRects(1, &scissor);
    cmd->OMSetRenderTargets(1, &color_target_descriptor, FALSE, &depth_target_descriptor);
    cmd->ClearRenderTargetView(color_target_descriptor, CLEAR_COLOR, 0, nullptr);
    cmd->ClearDepthStencilView(
        depth_target_descriptor,
        D3D12_CLEAR_FLAG_DEPTH,
        1.0f,
        0,
        0,
        nullptr);
    cmd->SetGraphicsRootSignature(draw.root_signature.get());
    cmd->SetGraphicsRoot32BitConstants(0, 16, &draw.transform, 0);
    cmd->SetPipelineState(draw.pipeline_state.get());
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
    cmd->IASetVertexBuffers(0, 1, &draw.vertex_buffer_view);
    cmd->DrawInstanced(PARTICLE_COUNT, 1, 0, 0);
    dx.transition(
        color_target,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

}  // namespace fb::rain
