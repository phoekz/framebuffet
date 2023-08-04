#pragma once

#include "device.hpp"
#include "utils.hpp"
#include "maths.hpp"
#include "buffers.hpp"
#include "descriptors.hpp"
#include "root_signature.hpp"
#include <pch.hpp>

namespace fb {

static constexpr ColorRgba8 COLOR_RED = {(uint8_t)255, 0, 0, 255};
static constexpr ColorRgba8 COLOR_GREEN = {(uint8_t)0, 255, 0, 255};
static constexpr ColorRgba8 COLOR_BLUE = {(uint8_t)0, 0, 255, 255};
static constexpr ColorRgba8 COLOR_YELLOW = {(uint8_t)255, 255, 0, 255};
static constexpr ColorRgba8 COLOR_MAGENTA = {(uint8_t)255, 0, 255, 255};
static constexpr ColorRgba8 COLOR_CYAN = {(uint8_t)0, 255, 255, 255};

class GpuDebugDraw {
    FB_NO_COPY_MOVE(GpuDebugDraw);

  public:
    static constexpr std::string_view NAME = "Debug Draw"sv;
    static constexpr size_t MAX_LINE_COUNT = 1024;

    GpuDebugDraw(GpuDevice& device, std::string_view name);

    auto begin(uint32_t frame_index) -> void;
    auto transform(const Matrix& transform) -> void;
    auto line(const Vector3& a, const Vector3& b, ColorRgba8 color) -> void;
    auto axes() -> void;
    auto end() -> void;
    auto render(GpuDevice& device) -> void;

  private:
    GpuRootSignature _root_signature;
    GpuDescriptors _descriptors;
    ComPtr<ID3D12PipelineState> _pipeline_state;

    uint32_t _frame_index = 0;

    struct Constants {
        Matrix transform;
        float pad[48];
    };
    Constants _constants;

    struct Vertex {
        Vector3 position;
        ColorRgba8 color;
    };
    std::vector<Vertex> _lines;

    struct Frame {
        GpuBufferHostCbv<Constants> _constant_buffer;
        GpuDescriptorHandle _constant_buffer_descriptor;
        GpuBufferHostSrv<Vertex> _lines_buffer;
        GpuDescriptorHandle _lines_buffer_descriptor;
    };
    std::array<Frame, FRAME_COUNT> _frames;
};

}  // namespace fb
