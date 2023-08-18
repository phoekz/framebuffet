#include "debug_draw.hpp"

namespace fb::demos {

DebugDraw::DebugDraw(GpuDevice& device, const baked::Shaders& shaders, std::string_view name) {
    // Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE)
        .vertex_shader(shaders.debug_draw_draw_vs())
        .pixel_shader(shaders.debug_draw_draw_ps())
        .blend(GPU_PIPELINE_BLEND_ALPHA)
        .depth_stencil(GPU_PIPELINE_DEPTH_DEFAULT)
        .rasterizer(GPU_PIPELINE_CULL_NONE)
        .render_target_formats({DXGI_FORMAT_R8G8B8A8_UNORM})
        .depth_stencil_format(DXGI_FORMAT_D32_FLOAT)
        .build(device, _pipeline, dx_name(NAME, "Pipeline"));

    // Frame resources.
    for (uint32_t i = 0; i < FRAME_COUNT; i++) {
        auto& frame = _frames[i];
        frame._constants.create(device, 1, dx_name(name, NAME, "Constants", i));
        frame._lines.create(device, MAX_LINE_COUNT, dx_name(name, NAME, "Lines", i));
    }
}

auto DebugDraw::begin(uint32_t frame_index) -> void {
    _frame_index = frame_index;
    _lines.clear();
}

auto DebugDraw::transform(const Float4x4& transform) -> void {
    _constants.transform = transform;
}

auto DebugDraw::line(const Float3& a, const Float3& b, RgbaByte color) -> void {
    FB_ASSERT(_lines.size() < MAX_LINE_COUNT);
    _lines.push_back({a, color});
    _lines.push_back({b, color});
}

auto DebugDraw::axes() -> void {
    line(Float3(0.0f, 0.0f, 0.0f), Float3(1.0f, 0.0f, 0.0f), COLOR_RED);
    line(Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 1.0f, 0.0f), COLOR_GREEN);
    line(Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 0.0f, 1.0f), COLOR_BLUE);
}

auto DebugDraw::end() -> void {
    auto& frame = _frames[_frame_index];
    memcpy(frame._constants.ptr(), &_constants, sizeof(_constants));
    memcpy(frame._lines.ptr(), _lines.data(), _lines.size() * sizeof(Vertex));
}

auto DebugDraw::render(GpuDevice&, const GpuCommandList& cmd) -> void {
    const auto& frame = _frames[_frame_index];
    cmd.set_graphics_constants({
        frame._constants.cbv_descriptor().index(),
        frame._lines.srv_descriptor().index(),
    });
    cmd.set_pipeline(_pipeline);
    cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    cmd.draw_instanced((uint32_t)_lines.size(), 1, 0, 0);
}

} // namespace fb::demos
