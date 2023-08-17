#include "debug_draw.hpp"
#include "shaders.hpp"

namespace fb {

GpuDebugDraw::GpuDebugDraw(GpuDevice& device, std::string_view name) {
    // Shaders.
    GpuShaderBytecode vertex_shader;
    GpuShaderBytecode pixel_shader;
    {
        GpuShaderCompiler sc;
        auto source = read_whole_file("shaders/debug_draw.hlsl");
        vertex_shader = sc.compile(NAME, GpuShaderType::Vertex, "vs_main", source);
        pixel_shader = sc.compile(NAME, GpuShaderType::Pixel, "ps_main", source);
    }

    // Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE)
        .vertex_shader(vertex_shader)
        .pixel_shader(pixel_shader)
        .blend(GPU_PIPELINE_BLEND_ALPHA)
        .depth_stencil(GPU_PIPELINE_DEPTH_DEFAULT)
        .rasterizer(GPU_PIPELINE_CULL_NONE)
        .render_target_formats({DXGI_FORMAT_R8G8B8A8_UNORM})
        .depth_stencil_format(DXGI_FORMAT_D32_FLOAT)
        .build(device, _pipeline, dx_name(NAME, "Pipeline"));

    // Frame resources.
    for (uint32_t i = 0; i < FRAME_COUNT; i++) {
        auto& frame = _frames[i];
        frame._constant_buffer.create(device, 1, dx_name(name, NAME, "Constant Buffer", i));
        frame._lines_buffer.create(device, MAX_LINE_COUNT, dx_name(name, NAME, "Lines Buffer", i));
    }
}

auto GpuDebugDraw::begin(uint32_t frame_index) -> void {
    _frame_index = frame_index;
    _lines.clear();
}

auto GpuDebugDraw::transform(const Float4x4& transform) -> void {
    _constants.transform = transform;
}

auto GpuDebugDraw::line(const Float3& a, const Float3& b, RgbaByte color) -> void {
    FB_ASSERT(_lines.size() < MAX_LINE_COUNT);
    _lines.push_back({a, color});
    _lines.push_back({b, color});
}

auto GpuDebugDraw::axes() -> void {
    line(Float3(0.0f, 0.0f, 0.0f), Float3(1.0f, 0.0f, 0.0f), COLOR_RED);
    line(Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 1.0f, 0.0f), COLOR_GREEN);
    line(Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 0.0f, 1.0f), COLOR_BLUE);
}

auto GpuDebugDraw::end() -> void {
    auto& frame = _frames[_frame_index];
    memcpy(frame._constant_buffer.ptr(), &_constants, sizeof(_constants));
    memcpy(frame._lines_buffer.ptr(), _lines.data(), _lines.size() * sizeof(Vertex));
}

auto GpuDebugDraw::render(GpuDevice&, const GpuCommandList& cmd) -> void {
    const auto& frame = _frames[_frame_index];
    cmd.set_graphics_constants({
        frame._constant_buffer.cbv_descriptor().index(),
        frame._lines_buffer.srv_descriptor().index(),
    });
    cmd.set_pipeline(_pipeline);
    cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    cmd.draw_instanced((uint32_t)_lines.size(), 1, 0, 0);
}

}  // namespace fb
