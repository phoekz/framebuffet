#pragma once

#include <common/common.hpp>
#include "../gpu/gpu.hpp"
#include "../win32/window.hpp"
#include <baked/kitchen/baked.hpp>
#include "gui.hlsli"

namespace fb {

class Window;

inline constexpr uint MAX_VERTEX_COUNT = 1024 * 1024;
inline constexpr uint MAX_INDEX_COUNT = 1024 * 1024;

class KcnGui {
    FB_NO_COPY_MOVE(KcnGui);

public:
    KcnGui() = default;
    ~KcnGui();

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
    GpuBufferHostCbv<fb::kcn::gui::Constants> _constants;
    struct Geometry {
        GpuBufferHostSrv<ImDrawVert> vertices;
        GpuBufferHostIndex<ImDrawIdx> indices;
    };
    std::array<Geometry, FRAME_COUNT> _geometries;
    GpuTextureSrv _texture;
};

} // namespace fb
