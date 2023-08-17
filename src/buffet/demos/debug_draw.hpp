#pragma once

#include "demos.hpp"

namespace fb::demos {

inline constexpr RgbaByte COLOR_RED = {(uint8_t)255, 0, 0, 255};
inline constexpr RgbaByte COLOR_GREEN = {(uint8_t)0, 255, 0, 255};
inline constexpr RgbaByte COLOR_BLUE = {(uint8_t)0, 0, 255, 255};
inline constexpr RgbaByte COLOR_YELLOW = {(uint8_t)255, 255, 0, 255};
inline constexpr RgbaByte COLOR_MAGENTA = {(uint8_t)255, 0, 255, 255};
inline constexpr RgbaByte COLOR_CYAN = {(uint8_t)0, 255, 255, 255};
inline constexpr RgbaByte COLOR_WHITE = {(uint8_t)255, 255, 255, 255};
inline constexpr RgbaByte COLOR_BLACK = {(uint8_t)0, 0, 0, 255};

class DebugDraw {
    FB_NO_COPY_MOVE(DebugDraw);

  public:
    static constexpr std::string_view NAME = "Debug Draw"sv;
    static constexpr size_t MAX_LINE_COUNT = 1 << 16;

    DebugDraw(GpuDevice& device, const baked::Shaders& shaders, std::string_view name);

    auto begin(uint32_t frame_index) -> void;
    auto transform(const Float4x4& transform) -> void;
    auto line(const Float3& a, const Float3& b, RgbaByte color) -> void;
    auto axes() -> void;
    auto end() -> void;
    auto render(GpuDevice& device, const GpuCommandList& cmd) -> void;

  private:
    GpuPipeline _pipeline;

    uint32_t _frame_index = 0;

    struct Constants {
        Float4x4 transform;
        float pad[48] = {};
    };
    Constants _constants;

    struct Vertex {
        Float3 position;
        RgbaByte color = {};
    };
    std::vector<Vertex> _lines;

    struct Frame {
        GpuBufferHostCbv<Constants> _constants;
        GpuBufferHostSrv<Vertex> _lines;
    };
    std::array<Frame, FRAME_COUNT> _frames;
};

}  // namespace fb::demos
