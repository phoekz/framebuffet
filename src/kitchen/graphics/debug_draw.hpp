#pragma once

#include "../pch.hpp"
#include "../gpu/gpu.hpp"
#include <baked/kitchen/baked.hpp>
#include "render_targets.hpp"
#include "debug_draw.hlsli"

namespace fb::graphics::debug_draw {

class DebugDraw {
    FB_NO_COPY_MOVE(DebugDraw);

public:
    static constexpr size_t MAX_LINE_COUNT = 1 << 16;

    DebugDraw() = default;

    auto create(
        GpuDevice& device,
        const baked::kitchen::Shaders& shaders,
        const render_targets::RenderTargets& render_targets
    ) -> void;
    auto begin(uint frame_index) -> void;
    auto transform(const float4x4& transform) -> void;
    auto line(const float3& a, const float3& b, RgbaByte color) -> void;
    auto axes() -> void;
    auto scaled_axes(float scale) -> void;
    auto grid(uint size) -> void;
    auto end() -> void;
    auto render(GpuDevice& device, const GpuCommandList& cmd) -> void;

private:
    GpuPipeline _pipeline;

    uint _frame_index = 0;

    struct Constants {
        float4x4 transform;
        float pad[48] = {};
    };
    Constants _constants;

    struct Vertex {
        float3 position;
        RgbaByte color = {};
    };
    std::vector<Vertex> _lines;

    struct Frame {
        GpuBufferHostCbv<Constants> _constants;
        GpuBufferHostSrv<Vertex> _lines;
    };
    std::array<Frame, FRAME_COUNT> _frames;
};

} // namespace fb::graphics::debug_draw
