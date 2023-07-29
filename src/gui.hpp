#pragma once

#include "dx12.hpp"
#include "win32.hpp"
#include "shaders.hpp"
#include "utils.hpp"
#include <imgui.h>

namespace fb {

struct Gui {
    Gui(Window* window, Dx& dx);
    ~Gui();
    void update();
    void render(const Dx& dx);

    ImGuiContext* imgui_ctx = nullptr;
    fb::Shader vertex_shader;
    fb::Shader pixel_shader;
    ComPtr<ID3D12RootSignature> root_signature;
    ComPtr<ID3D12PipelineState> pipeline_state;
    ComPtr<ID3D12Resource> font_texture_resource;
    ComPtr<ID3D12DescriptorHeap> descriptor_heap;
    struct Geometry {
        ComPtr<ID3D12Resource> vertex_buffer;
        ComPtr<ID3D12Resource> index_buffer;
        size_t vertex_buffer_size = sizeof(ImDrawVert) * 1024 * 1024;
        size_t index_buffer_size = sizeof(ImDrawIdx) * 1024 * 1024;
    };
    std::array<Geometry, fb::FRAME_COUNT> geometries;
};

}  // namespace fb
