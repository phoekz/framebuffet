#pragma once

#include "dx12.hpp"
#include "win32.hpp"
#include "maths.hpp"
#include "shaders.hpp"
#include "utils.hpp"
#include "buffers.hpp"
#include "descriptors.hpp"
#include "samplers.hpp"
#include "root_signature.hpp"

namespace fb::gui {

constexpr uint32_t MAX_VERTEX_COUNT = 1024 * 1024;
constexpr uint32_t MAX_INDEX_COUNT = 1024 * 1024;

struct Constants {
    Matrix transform;
    float pad[48];
};

struct Gui {
    static constexpr const char* NAME = "Gui";

    Gui(const Window& window, Dx& dx);
    ~Gui();
    void update();
    void render(const Dx& dx);

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
