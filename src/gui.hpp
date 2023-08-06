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

struct Gui {
    static constexpr std::string_view NAME = "Gui"sv;

    Gui(const Window& window, GpuDevice& device);
    ~Gui();
    void update();
    void render(const GpuDevice& device);

    ImGuiContext* imgui_ctx = nullptr;

    GpuRootSignature root_signature;
    GpuDescriptors descriptors;
    GpuSamplers samplers;

    ComPtr<ID3D12PipelineState> pipeline_state;

    GpuBufferHostCbv<Constants> constant_buffer;
    GpuDescriptorHandle constant_buffer_descriptor;

    struct Geometry {
        GpuBufferHostSrv<ImDrawVert> vertex_buffer;
        GpuDescriptorHandle vertex_buffer_descriptor;
        GpuBufferHostIndex<ImDrawIdx> index_buffer;
    };
    std::array<Geometry, FRAME_COUNT> geometries;

    ComPtr<ID3D12Resource> texture;
    GpuDescriptorHandle texture_descriptor;
};

}  // namespace fb::gui
