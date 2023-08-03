#include "cube.hpp"

namespace fb::cube {

Demo::Demo(Dx& dx) :
    root_signature(dx, Demo::NAME),
    descriptors(dx, Demo::NAME),
    samplers(dx, descriptors),
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
        constant_buffer_descriptor = descriptors.cbv_srv_uav().alloc();
        vertex_buffer_descriptor = descriptors.cbv_srv_uav().alloc();
        texture_descriptor = descriptors.cbv_srv_uav().alloc();
    }

    // Shaders.
    GpuShader vertex_shader;
    GpuShader pixel_shader;
    {
        GpuShaderCompiler sc;
        auto source = read_whole_file("shaders/cube.hlsl");
        vertex_shader = sc.compile(Demo::NAME, GpuShaderType::Vertex, "vs_main", source);
        pixel_shader = sc.compile(Demo::NAME, GpuShaderType::Pixel, "ps_main", source);
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
        constant_buffer.create(dx, 1, dx_name(Demo::NAME, "Constant Buffer"));

        auto cbv_desc = constant_buffer.cbv_desc();
        dx.device->CreateConstantBufferView(&cbv_desc, constant_buffer_descriptor.cpu());
    }

    // Model.
    auto model = GltfModel("models/stylized_crate.glb");

    // Geometry.
    {
        vertex_buffer.create(dx, model.vertex_count(), dx_name(Demo::NAME, "Vertex Buffer"));
        index_buffer.create(dx, model.index_count(), dx_name(Demo::NAME, "Index Buffer"));

        memcpy(vertex_buffer.ptr(), model.vertex_data(), model.vertex_buffer_size());
        memcpy(index_buffer.ptr(), model.index_data(), model.index_buffer_size());

        auto srv_desc = vertex_buffer.srv_desc();
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
}

void Demo::update(const demo::UpdateDesc& desc) {
    float aspect_ratio = desc.aspect_ratio;
    float elapsed_time = desc.elapsed_time;
    Matrix perspective =
        Matrix::CreatePerspectiveFieldOfView(rad_from_deg(45.0f), aspect_ratio, 0.1f, 100.0f);
    Vector3 eye = Vector3(4.0f * std::sin(elapsed_time), 3.0f, 4.0f * std::cos(elapsed_time));
    Matrix view = Matrix::CreateLookAt(eye, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
    Matrix transform = view * perspective;
    constant_buffer.ptr()->transform = transform;

    debug_draw.begin(desc.frame_index);
    debug_draw.transform(transform);
    debug_draw.axes();
    debug_draw.end();
}

void Demo::render(Dx& dx) {
    auto* cmd = dx.command_list.get();
    render_targets.begin(dx);

    // Debug pass.
    debug_draw.render(dx);

    // Main pass.
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
    auto ibv = index_buffer.index_buffer_view();
    cmd->IASetIndexBuffer(&ibv);

    auto index_count = index_buffer.element_size();
    cmd->DrawIndexedInstanced(index_count, 1, 0, 0, 0);

    render_targets.end(dx);
}

}  // namespace fb::cube
