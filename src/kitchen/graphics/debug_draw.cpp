#include "debug_draw.hpp"

namespace fb::graphics::debug_draw {

auto DebugDraw::create(
    GpuDevice& device,
    const baked::kitchen::Shaders& shaders,
    const render_targets::RenderTargets& render_targets
) -> void {
    DebugScope debug("Debug Draw");

    // Reserve.
    _lines.reserve(MAX_LINE_COUNT);

    // Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE)
        .vertex_shader(shaders.debug_draw_draw_vs())
        .pixel_shader(shaders.debug_draw_draw_ps())
        .blend(GpuBlendDesc {
            .blend_enable = true,
            .rgb_blend_src = GpuBlend::SrcAlpha,
            .rgb_blend_dst = GpuBlend::InvSrcAlpha,
            .rgb_blend_op = GpuBlendOp::Add,
            .alpha_blend_src = GpuBlend::One,
            .alpha_blend_dst = GpuBlend::InvSrcAlpha,
            .alpha_blend_op = GpuBlendOp::Add,
        })
        .rasterizer(GpuRasterizerDesc {
            .cull_mode = GpuCullMode::None,
        })
        .render_target_formats({render_targets.color_format()})
        .depth_stencil_format(render_targets.depth_format())
        .sample_desc(render_targets.sample_desc())
        .build(device, _pipeline, debug.with_name("Pipeline"));

    // Frame resources.
    for (uint i = 0; i < FRAME_COUNT; i++) {
        DebugScope frame_debug(std::format("{}", i));
        auto& frame = _frames[i];
        frame._constants.create(device, 1, frame_debug.with_name("Constants"));
        frame._lines.create(device, MAX_LINE_COUNT, frame_debug.with_name("Lines"));
    }
}

auto DebugDraw::begin(uint frame_index) -> void {
    _frame_index = frame_index;
    _lines.clear();
}

auto DebugDraw::transform(const float4x4& transform) -> void {
    _constants.transform = transform;
}

auto DebugDraw::line(const float3& a, const float3& b, RgbaByte color) -> void {
    FB_ASSERT(_lines.size() < MAX_LINE_COUNT);
    _lines.push_back({a, color});
    _lines.push_back({b, color});
}

auto DebugDraw::axes() -> void {
    scaled_axes(1.0f);
}

auto DebugDraw::scaled_axes(float scale) -> void {
    const auto s = scale;
    line(float3(0.0f, 0.0f, 0.0f), float3(s, 0.0f, 0.0f), COLOR_RED);
    line(float3(0.0f, 0.0f, 0.0f), float3(0.0f, s, 0.0f), COLOR_GREEN);
    line(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, s), COLOR_BLUE);
}

auto DebugDraw::grid(uint size) -> void {
    const auto half_size = (int)(size / 2);
    const auto h = (float)half_size;
    for (int i = -half_size; i <= half_size; i++) {
        const auto v = (float)i;
        line(float3(v, 0.0f, -h), float3(v, 0.0f, h), COLOR_WHITE);
        line(float3(-h, 0.0f, v), float3(h, 0.0f, v), COLOR_WHITE);
    }
}

auto DebugDraw::end() -> void {
    auto& frame = _frames[_frame_index];
    frame._constants.ref() = _constants;
    memcpy(frame._lines.span().data(), _lines.data(), _lines.size() * sizeof(Vertex));
}

auto DebugDraw::render(GpuGraphicsCommandList& cmd) -> void {
    const auto& frame = _frames[_frame_index];
    cmd.begin_pix("Debug Draw");
    cmd.set_constants(Bindings {
        .constants = frame._constants.cbv_descriptor().index(),
        .vertices = frame._lines.srv_descriptor().index(),
    });
    cmd.set_pipeline(_pipeline);
    cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    cmd.draw_instanced((uint)_lines.size(), 1, 0, 0);
    cmd.end_pix();
}

} // namespace fb::graphics::debug_draw
