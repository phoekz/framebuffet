#pragma once

#include "../pch.hpp"
#include "../gpu/gpu.hpp"
#include "../win32/window.hpp"
#include <baked/kitchen/baked.hpp>
#include "gui.hlsli"

class fb::Window;

namespace fb::graphics::gui {

inline constexpr uint MAX_VERTEX_COUNT = 1024 * 1024;
inline constexpr uint MAX_INDEX_COUNT = 1024 * 1024;

class Gui {
    FB_NO_COPY_MOVE(Gui);

public:
    Gui() = default;
    ~Gui();

    auto create(
        const Window& window,
        GpuDevice& device,
        const baked::kitchen::Assets& assets,
        const baked::kitchen::Shaders& shaders
    ) -> void;
    auto begin_frame() -> void;
    auto end_frame() -> void;
    auto render(const GpuDevice& device, GpuCommandList& cmd) -> void;

private:
    ImGuiContext* _imgui_ctx = nullptr;
    GpuPipeline _pipeline;
    GpuBufferHostCbv<Constants> _constants;
    struct Geometry {
        GpuBufferHostSrv<ImDrawVert> vertices;
        GpuBufferHostIndex<ImDrawIdx> indices;
    };
    std::array<Geometry, FRAME_COUNT> _geometries;
    GpuTextureSrv _texture;
};

} // namespace fb::graphics::gui
