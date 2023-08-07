#include "cube.hpp"

namespace fb::cube {

Demo::Demo(GpuDevice& device) :
    _descriptors(device, Demo::NAME),
    _samplers(device, _descriptors),
    _render_targets(device, _descriptors, device.swapchain_size(), Demo::CLEAR_COLOR, Demo::NAME),
    _debug_draw(device, Demo::NAME) {
    // Descriptors.
    {
        _constant_buffer_descriptor = _descriptors.cbv_srv_uav().alloc();
        _vertex_buffer_descriptor = _descriptors.cbv_srv_uav().alloc();
        _texture_descriptor = _descriptors.cbv_srv_uav().alloc();
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

    // Constant buffer.
    {
        _constant_buffer.create(device, 1, dx_name(Demo::NAME, "Constant Buffer"));
        device.create_constant_buffer_view(
            _constant_buffer.cbv_desc(),
            _constant_buffer_descriptor.cpu());
    }

    // Model.
    auto model = GltfModel("models/stylized_crate.glb");

    // Geometry.
    {
        _vertex_buffer.create(device, model.vertex_count(), dx_name(Demo::NAME, "Vertex Buffer"));
        _index_buffer.create(device, model.index_count(), dx_name(Demo::NAME, "Index Buffer"));

        memcpy(_vertex_buffer.ptr(), model.vertex_data(), model.vertex_buffer_size());
        memcpy(_index_buffer.ptr(), model.index_data(), model.index_buffer_size());

        device.create_shader_resource_view(
            _vertex_buffer.resource(),
            _vertex_buffer.srv_desc(),
            _vertex_buffer_descriptor.cpu());
    }

    // Texture.
    {
        // Create.
        const auto& image = model.base_color_texture();
        _texture.create(
            device,
            GpuTexture2dDesc {
                .format = image.format(),
                .width = image.width(),
                .height = image.height(),
            },
            dx_name(Demo::NAME, "Texture"));

        // Descriptor.
        device.create_shader_resource_view(
            _texture.resource(),
            _texture.srv_desc(),
            _texture_descriptor.cpu());

        // Upload.
        device.easy_upload(
            D3D12_SUBRESOURCE_DATA {
                .pData = image.data(),
                .RowPitch = image.row_pitch(),
                .SlicePitch = image.slice_pitch()},
            _texture.resource(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

auto Demo::update(const demo::UpdateDesc& desc) -> void {
    float aspect_ratio = desc.aspect_ratio;
    float elapsed_time = desc.elapsed_time;
    Matrix perspective =
        Matrix::CreatePerspectiveFieldOfView(rad_from_deg(45.0f), aspect_ratio, 0.1f, 100.0f);
    Vector3 eye = Vector3(4.0f * std::sin(elapsed_time), 3.0f, 4.0f * std::cos(elapsed_time));
    Matrix view = Matrix::CreateLookAt(eye, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
    Matrix transform = view * perspective;
    _constant_buffer.ptr()->transform = transform;

    _debug_draw.begin(desc.frame_index);
    _debug_draw.transform(transform);
    _debug_draw.axes();
    _debug_draw.end();
}

auto Demo::render(GpuDevice& device) -> void {
    auto* cmd = device.command_list();
    _render_targets.begin(device);

    // Debug pass.
    _debug_draw.render(device);

    // Main pass.
    ID3D12DescriptorHeap* heaps[] = {
        _descriptors.cbv_srv_uav().heap(),
        _descriptors.sampler().heap()};
    cmd->SetDescriptorHeaps(_countof(heaps), heaps);
    cmd->SetGraphicsRootSignature(device.root_signature());
    GpuBindings bindings;
    bindings.push(_constant_buffer_descriptor);
    bindings.push(_vertex_buffer_descriptor);
    bindings.push(_texture_descriptor);
    cmd->SetGraphicsRoot32BitConstants(0, bindings.capacity(), bindings.ptr(), 0);

    cmd->SetPipelineState(_pipeline_state.get());
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    auto ibv = _index_buffer.index_buffer_view();
    cmd->IASetIndexBuffer(&ibv);

    auto index_count = _index_buffer.element_size();
    cmd->DrawIndexedInstanced(index_count, 1, 0, 0, 0);

    _render_targets.end(device);
}

}  // namespace fb::cube
