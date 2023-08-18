#pragma once

#include "demos.hpp"
#include "gui.hlsli"

namespace fb::demos::gui {

inline constexpr uint32_t MAX_VERTEX_COUNT = 1024 * 1024;
inline constexpr uint32_t MAX_INDEX_COUNT = 1024 * 1024;

struct Constants {
    Float4x4 transform;
    float pad[48] = {};
};

class Gui {
  public:
    static constexpr std::string_view NAME = "Gui"sv;

    Gui(const Window& window,
        GpuDevice& device,
        const baked::Assets& assets,
        const baked::Shaders& shaders);
    ~Gui();
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
