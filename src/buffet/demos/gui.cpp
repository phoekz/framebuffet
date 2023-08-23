#include "gui.hpp"

namespace fb::demos::gui {

Gui::Gui(
    const Window& window,
    GpuDevice& device,
    const baked::Assets& assets,
    const baked::Shaders& shaders
) {
    // ImGui.
    {
        IMGUI_CHECKVERSION();
        _imgui_ctx = ImGui::CreateContext();
        auto& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.BackendRendererName = "framebuffet";
        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
        ImGui::StyleColorsDark();
        ImFontConfig font_cfg;
        font_cfg.FontDataOwnedByAtlas = false;
        const auto imgui_font = assets.imgui_font();
        io.Fonts->AddFontFromMemoryTTF(
            (void*)imgui_font.data.data(),
            (int)imgui_font.data.size(),
            16.0f,
            &font_cfg
        );
    }

    // Pipeline.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.gui_draw_vs())
        .pixel_shader(shaders.gui_draw_ps())
        .blend(D3D12_BLEND_DESC {
            .AlphaToCoverageEnable = FALSE,
            .IndependentBlendEnable = FALSE,
            .RenderTarget = {{
                .BlendEnable = TRUE,
                .LogicOpEnable = FALSE,
                .SrcBlend = D3D12_BLEND_SRC_ALPHA,
                .DestBlend = D3D12_BLEND_INV_SRC_ALPHA,
                .BlendOp = D3D12_BLEND_OP_ADD,
                .SrcBlendAlpha = D3D12_BLEND_ONE,
                .DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA,
                .BlendOpAlpha = D3D12_BLEND_OP_ADD,
                .LogicOp = D3D12_LOGIC_OP_NOOP,
                .RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL,
            }}})
        .depth_stencil(GPU_PIPELINE_DEPTH_NONE)
        .render_target_formats({SWAPCHAIN_RTV_FORMAT})
        .rasterizer(GPU_PIPELINE_CULL_NONE)
        .build(device, _pipeline, dx_name(Gui::NAME, "Pipeline"));

    // Constants.
    _constants.create(device, 1, dx_name(Gui::NAME, "Constants"));

    // Font texture.
    {
        // Raw data.
        auto& io = ImGui::GetIO();
        uint8_t* pixels;
        uint32_t width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, (int*)&width, (int*)&height);

        // Texture.
        _texture.create_and_transfer(
            device,
            GpuTextureDesc {
                .format = DXGI_FORMAT_R8G8B8A8_UNORM,
                .width = width,
                .height = height,
            },
            GpuTextureTransferDesc {
                .data = pixels,
                .row_pitch = width * 4,
                .slice_pitch = width * height * 4,
            },
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            dx_name(Gui::NAME, "Font Texture")
        );

        // Font texture ID.
        io.Fonts->SetTexID((ImTextureID)_texture.srv_descriptor().gpu().ptr);
    }

    // Geometry.
    for (uint32_t i = 0; i < FRAME_COUNT; i++) {
        Gui::Geometry& geometry = _geometries[i];
        geometry.vertices.create(device, MAX_VERTEX_COUNT, dx_name(Gui::NAME, "Vertices", i));
        geometry.indices.create(device, MAX_INDEX_COUNT, dx_name(Gui::NAME, "Indices", i));
    }

    // ImGui continued.
    ImGui_ImplWin32_Init(window.hwnd());
}

Gui::~Gui() {
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext(_imgui_ctx);
}

auto Gui::begin_frame() -> void {
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

auto Gui::end_frame() -> void {
    ImGui::Render();
}

auto Gui::render(const GpuDevice& device, GpuCommandList& cmd) -> void {
    auto* draw_data = ImGui::GetDrawData();

    // Avoid rendering when minimized.
    if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f) {
        return;
    }

    // Update geometries.
    auto& geometry = _geometries[device.frame_index()];
    {
        ImDrawVert* vertices = geometry.vertices.ptr();
        ImDrawIdx* indices = geometry.indices.ptr();
        for (int i = 0; i < draw_data->CmdListsCount; i++) {
            auto* cmd_list = draw_data->CmdLists[i];
            memcpy(
                vertices,
                cmd_list->VtxBuffer.Data,
                cmd_list->VtxBuffer.Size * sizeof(ImDrawVert)
            );
            memcpy(indices, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
            vertices += cmd_list->VtxBuffer.Size;
            indices += cmd_list->IdxBuffer.Size;
        }
    }

    // Update transform.
    {
        float l = ImGui::GetDrawData()->DisplayPos.x;
        float r = ImGui::GetDrawData()->DisplayPos.x + ImGui::GetDrawData()->DisplaySize.x;
        float t = ImGui::GetDrawData()->DisplayPos.y;
        float b = ImGui::GetDrawData()->DisplayPos.y + ImGui::GetDrawData()->DisplaySize.y;
        float m[4][4] = {
            {2.0f / (r - l), 0.0f, 0.0f, 0.0f},
            {0.0f, 2.0f / (t - b), 0.0f, 0.0f},
            {0.0f, 0.0f, 0.5f, 0.0f},
            {(r + l) / (l - r), (t + b) / (b - t), 0.5f, 1.0f},
        };
        memcpy(_constants.ptr(), m, sizeof(m));
    }

    // Render.
    {
        cmd.set_graphics();
        cmd.set_viewport(
            0,
            0,
            (uint32_t)draw_data->DisplaySize.x,
            (uint32_t)draw_data->DisplaySize.y
        );
        cmd.set_blend_factor(Float4(0.0f, 0.0f, 0.0f, 0.0f));
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(geometry.indices.index_buffer_view());
        cmd.set_pipeline(_pipeline);

        int global_vtx_offset = 0;
        int global_idx_offset = 0;
        auto clip_off = draw_data->DisplayPos;
        for (int i = 0; i < draw_data->CmdListsCount; i++) {
            auto* cmd_list = draw_data->CmdLists[i];
            for (int j = 0; j < cmd_list->CmdBuffer.Size; j++) {
                auto* pcmd = &cmd_list->CmdBuffer[j];
                if (pcmd->UserCallback != nullptr) {
                    pcmd->UserCallback(cmd_list, pcmd);
                } else {
                    cmd.set_scissor(
                        (uint32_t)(pcmd->ClipRect.x - clip_off.x),
                        (uint32_t)(pcmd->ClipRect.y - clip_off.y),
                        (uint32_t)(pcmd->ClipRect.z - clip_off.x),
                        (uint32_t)(pcmd->ClipRect.w - clip_off.y)
                    );
                    cmd.set_graphics_constants(Bindings {
                        .constants = _constants.cbv_descriptor().index(),
                        .vertices = geometry.vertices.srv_descriptor().index(),
                        .base_vertex = (pcmd->VtxOffset + (uint32_t)global_vtx_offset),
                        .texture = _texture.srv_descriptor().index(),
                    });
                    cmd.draw_indexed_instanced(
                        pcmd->ElemCount,
                        1,
                        pcmd->IdxOffset + global_idx_offset,
                        0,
                        0
                    );
                }
            }
            global_idx_offset += cmd_list->IdxBuffer.Size;
            global_vtx_offset += cmd_list->VtxBuffer.Size;
        }
    }
}

} // namespace fb::demos::gui
