#include "cube.hpp"

namespace fb::cube {

Demo::Demo(GpuDevice& device) :
    _render_targets(device, device.swapchain_size(), Demo::CLEAR_COLOR, Demo::NAME),
    _debug_draw(device, Demo::NAME) {
    // Shaders.
    GpuShader vertex_shader;
    GpuShader pixel_shader;
    {
        GpuShaderCompiler sc;
        auto source = read_whole_file("shaders/cube.hlsl");
        vertex_shader = sc.compile(Demo::NAME, GpuShaderType::Vertex, "vs_main", source);
        pixel_shader = sc.compile(Demo::NAME, GpuShaderType::Pixel, "ps_main", source);
    }

    // Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(vertex_shader.bytecode())
        .pixel_shader(pixel_shader.bytecode())
        .render_target_formats({DXGI_FORMAT_R8G8B8A8_UNORM})
        .depth_stencil_format(DXGI_FORMAT_D32_FLOAT)
        .build(device, _pipeline, dx_name(Demo::NAME, "Pipeline"));

    // Constant buffer.
    _constant_buffer.create(device, 1, dx_name(Demo::NAME, "Constant Buffer"));

    // Model.
    auto model = GltfModel("models/stylized_crate.glb");

    // Geometry.
    {
        _vertex_buffer.create(device, model.vertex_count(), dx_name(Demo::NAME, "Vertex Buffer"));
        _index_buffer.create(device, model.index_count(), dx_name(Demo::NAME, "Index Buffer"));

        memcpy(_vertex_buffer.ptr(), model.vertex_data(), model.vertex_buffer_size());
        memcpy(_index_buffer.ptr(), model.index_data(), model.index_buffer_size());
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

auto Demo::render(GpuDevice& device, GpuCommandList& cmd) -> void {
    cmd.set_graphics();
    _render_targets.begin(device, cmd);
    _debug_draw.render(device, cmd);
    cmd.set_graphics_constants({
        _constant_buffer.cbv_descriptor().index(),
        _vertex_buffer.srv_descriptor().index(),
        _texture.srv_descriptor().index(),
    });
    cmd.set_pipeline(_pipeline);
    cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd.set_index_buffer(_index_buffer.index_buffer_view());
    cmd.draw_indexed_instanced(_index_buffer.element_size(), 1, 0, 0, 0);
    _render_targets.end(device, cmd);
}

}  // namespace fb::cube
