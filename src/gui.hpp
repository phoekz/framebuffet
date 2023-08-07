#pragma once

#include <gpu/gpu.hpp>
#include "win32.hpp"
#include "maths.hpp"
#include "utils.hpp"

namespace fb::gui {

constexpr uint32_t MAX_VERTEX_COUNT = 1024 * 1024;
constexpr uint32_t MAX_INDEX_COUNT = 1024 * 1024;

struct Constants {
    Matrix transform;
    float pad[48] = {};
};

class Gui {
  public:
    static constexpr std::string_view NAME = "Gui"sv;

    Gui(const Window& window, GpuDevice& device);
    ~Gui();
    auto update() -> void;
    auto render(const GpuDevice& device, GpuCommandList& cmd) -> void;

  private:
    ImGuiContext* _imgui_ctx = nullptr;
    ComPtr<ID3D12PipelineState> _pipeline_state;
    GpuBufferHostCbv<Constants> _constant_buffer;
    struct Geometry {
        GpuBufferHostSrv<ImDrawVert> vertex_buffer;
        GpuBufferHostIndex<ImDrawIdx> index_buffer;
    };
    std::array<Geometry, FRAME_COUNT> _geometries;
    GpuTexture2dSrv _texture;
};

}  // namespace fb::gui
