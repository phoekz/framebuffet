#pragma once

#include "demos.hpp"
#include "gui.hlsli"

class fb::Window;

namespace fb::demos::gui {

inline constexpr uint32_t MAX_VERTEX_COUNT = 1024 * 1024;
inline constexpr uint32_t MAX_INDEX_COUNT = 1024 * 1024;

class Gui {
    FB_NO_COPY_MOVE(Gui);

public:
    static constexpr std::string_view NAME = "Gui"sv;

    Gui() = default;

    auto create(
        const Window& window,
        GpuDevice& device,
        const baked::Assets& assets,
        const baked::Shaders& shaders
    ) -> void;
    auto destroy() -> void;
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

} // namespace fb::demos::gui
