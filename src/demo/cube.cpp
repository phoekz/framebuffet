#include "cube.hpp"
#include "../shaders.hpp"
#include "../gltf.hpp"

namespace fb::cube {

Demo::Demo(Dx& dx) :
    root_signature(dx, Demo::NAME),
    descriptors(dx, Demo::NAME),
    samplers(dx, descriptors) {
    // Descriptors.
    {
        constant_buffer_descriptor = descriptors.cbv_srv_uav().alloc();
        vertex_buffer_descriptor = descriptors.cbv_srv_uav().alloc();
        texture_descriptor = descriptors.cbv_srv_uav().alloc();
        color_target_descriptor = descriptors.rtv().alloc();
        depth_target_descriptor = descriptors.dsv().alloc();
    }

    // Shaders.
    Shader vertex_shader;
    Shader pixel_shader;
    {
        ShaderCompiler sc;
        auto source = read_whole_file("shaders/cube.hlsl");
        vertex_shader = sc.compile(Demo::NAME, ShaderType::Vertex, "vs_main", source);
        pixel_shader = sc.compile(Demo::NAME, ShaderType::Pixel, "ps_main", source);
    }

    // Pipeline state.
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {
            .pRootSignature = root_signature.get(),
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
        };
        FAIL_FAST_IF_FAILED(
            dx.device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipeline_state)));
        dx_set_name(pipeline_state, dx_name(Demo::NAME, "Pipeline State"));
    }

    // Constant buffer.
    {
        constant_buffer.create_cb(
            dx,
            GpuBufferAccessMode::HostWritable,
            dx_name(Demo::NAME, "Constant Buffer"));

        auto cbv_desc = constant_buffer.constant_buffer_view_desc();
        dx.device->CreateConstantBufferView(&cbv_desc, constant_buffer_descriptor.cpu());
    }

    // Model.
    auto model = GltfModel("models/stylized_crate.glb");

    // Geometry.
    {
        vertex_buffer.create_srv(
            dx,
            model.vertex_count(),
            GpuBufferAccessMode::HostWritable,
            dx_name(Demo::NAME, "Vertex Buffer"));
        index_buffer.create_ib(
            dx,
            model.index_count(),
            GpuBufferAccessMode::HostWritable,
            dx_name(Demo::NAME, "Index Buffer"));

        memcpy(vertex_buffer.ptr(), model.vertex_data(), model.vertex_buffer_size());
        memcpy(index_buffer.ptr(), model.index_data(), model.index_buffer_size());

        auto srv_desc = vertex_buffer.shader_resource_view_desc();
        dx.device->CreateShaderResourceView(
            vertex_buffer.resource(),
            &srv_desc,
            vertex_buffer_descriptor.cpu());
    }

    // Texture.
    {
        // Create.
        const auto& image = model.base_color_texture();
        auto texture_desc =
            CD3DX12_RESOURCE_DESC::Tex2D(image.format(), image.width(), image.height(), 1, 1);
        CD3DX12_HEAP_PROPERTIES texture_heap(D3D12_HEAP_TYPE_DEFAULT);
        FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
            &texture_heap,
            D3D12_HEAP_FLAG_NONE,
            &texture_desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&texture)));
        dx_set_name(texture, dx_name(Demo::NAME, "Texture"));

        // Upload.
        D3D12_SUBRESOURCE_DATA subresource_data = {
            .pData = image.data(),
            .RowPitch = image.row_pitch(),
            .SlicePitch = image.slice_pitch(),
        };
        DirectX::ResourceUploadBatch rub(dx.device.get());
        rub.Begin();
        rub.Upload(texture.get(), 0, &subresource_data, 1);
        rub.Transition(
            texture.get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        auto finish = rub.End(dx.command_queue.get());
        finish.wait();

        // Descriptor.
        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {
            .Format = image.format(),
            .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
            .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
            .Texture2D = D3D12_TEX2D_SRV {.MipLevels = 1},
        };
        dx.device->CreateShaderResourceView(texture.get(), &srv_desc, texture_descriptor.cpu());
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
    float aspect_ratio = params.aspect_ratio;
    float elapsed_time = params.elapsed_time;
    Matrix perspective =
        Matrix::CreatePerspectiveFieldOfView(rad_from_deg(45.0f), aspect_ratio, 0.1f, 100.0f);
    Vector3 eye = Vector3(4.0f * std::sin(elapsed_time), 3.0f, 4.0f * std::cos(elapsed_time));
    Matrix view = Matrix::CreateLookAt(eye, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
    constant_buffer.ptr()->transform = view * perspective;
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
    auto ibv = index_buffer.index_buffer_view();
    auto index_count = index_buffer.element_size();

    auto* cmd = dx.command_list.get();
    dx.transition(
        color_target,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
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

    ID3D12DescriptorHeap* heaps[] = {
        descriptors.cbv_srv_uav().heap(),
        descriptors.sampler().heap()};
    cmd->SetDescriptorHeaps(_countof(heaps), heaps);
    cmd->SetGraphicsRootSignature(root_signature.get());
    GpuBindings bindings;
    bindings.push(constant_buffer_descriptor);
    bindings.push(vertex_buffer_descriptor);
    bindings.push(texture_descriptor);
    cmd->SetGraphicsRoot32BitConstants(0, bindings.capacity(), bindings.ptr(), 0);

    cmd->SetPipelineState(pipeline_state.get());
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd->IASetIndexBuffer(&ibv);

    cmd->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
    dx.transition(
        color_target,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

}  // namespace fb::cube
