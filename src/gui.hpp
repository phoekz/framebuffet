#pragma once

#include "dx12.hpp"
#include "win32.hpp"
#include "shaders.hpp"
#include "utils.hpp"
#include "buffers.hpp"

namespace fb {

constexpr uint32_t MAX_VERTEX_COUNT = 1024 * 1024;
constexpr uint32_t MAX_INDEX_COUNT = 1024 * 1024;

struct Gui {
    Gui(const Window& window, Dx& dx);
    ~Gui();
    void update();
    void render(const Dx& dx);

    ImGuiContext* imgui_ctx = nullptr;
    Shader vertex_shader;
    Shader pixel_shader;
    ComPtr<ID3D12RootSignature> root_signature;
    ComPtr<ID3D12PipelineState> pipeline_state;
    ComPtr<ID3D12Resource> font_texture_resource;
    ComPtr<ID3D12DescriptorHeap> descriptor_heap;
    struct Geometry {
        GpuBuffer<ImDrawVert> vertex_buffer;
        GpuBuffer<ImDrawIdx> index_buffer;
    };
    std::array<Geometry, FRAME_COUNT> geometries;
};

}  // namespace fb
