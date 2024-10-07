#include "gui.hpp"

namespace fb {

KcnGui::~KcnGui() {
    if (_imgui_ctx) {
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext(_imgui_ctx);
    }
}

auto KcnGui::create(
    const Window& window,
    GpuDevice& device,
    const baked::kitchen::Assets& assets,
    const baked::kitchen::Shaders& shaders
) -> void {
    FB_PERF_FUNC();
    DebugScope debug("Gui");

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
        .blend(GpuBlendDesc {
            .blend_enable = true,
            .rgb_blend_src = GpuBlend::SrcAlpha,
            .rgb_blend_dst = GpuBlend::InvSrcAlpha,
            .rgb_blend_op = GpuBlendOp::Add,
            .alpha_blend_src = GpuBlend::One,
            .alpha_blend_dst = GpuBlend::InvSrcAlpha,
            .alpha_blend_op = GpuBlendOp::Add,
        })
        .depth_stencil(GpuDepthStencilDesc {
            .depth_read = false,
            .depth_write = false,
        })
        .render_target_formats({device.swapchain().format()})
        .rasterizer(GpuRasterizerDesc {
            .cull_mode = GpuCullMode::None,
        })
        .build(device, _pipeline, debug.with_name("Pipeline"));

    // Constants.
    _constants.create(device, 1, debug.with_name("Constants"));

    // Font texture.
    {
        // Raw data.
        auto& io = ImGui::GetIO();
        uint8_t* pixels;
        uint width, height;
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
                .row_pitch = width * 4,
                .slice_pitch = width * height * 4,
                .data = pixels,
            },
            D3D12_BARRIER_SYNC_PIXEL_SHADING,
            D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
            D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE,
            debug.with_name("Font Texture")
        );

        // Font texture ID.
        io.Fonts->SetTexID((ImTextureID)_texture.srv_descriptor().gpu().ptr);
    }

    // Geometry.
    for (uint i = 0; i < FRAME_COUNT; i++) {
        DebugScope frame_debug(std::format("{}", i));
        Geometry& geometry = _geometries[i];
        geometry.vertices.create(device, MAX_VERTEX_COUNT, frame_debug.with_name("Vertices"));
        geometry.indices.create(device, MAX_INDEX_COUNT, frame_debug.with_name("Indices"));
    }

    // ImGui continued.
    ImGui_ImplWin32_Init(window.hwnd());
}

auto KcnGui::begin_frame() -> void {
    FB_PERF_FUNC();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

auto KcnGui::end_frame() -> void {
    FB_PERF_FUNC();
    ImGui::Render();
}

auto KcnGui::render(const GpuDevice& device, GpuCommandList& cmd) -> void {
    FB_PERF_FUNC();
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
        // clang-format off
        auto m = std::to_array({
            2.0f / (r - l), 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / (t - b), 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.0f,
            (r + l) / (l - r), (t + b) / (b - t), 0.5f, 1.0f,
        });
        // clang-format on
        memcpy(_constants.span().data(), m.data(), sizeof(m));
    }

    // Render.
    cmd.graphics_scope([&](GpuGraphicsCommandList& gcmd) {
        gcmd.pix_begin("Gui");
        gcmd.set_viewport(0, 0, (uint)draw_data->DisplaySize.x, (uint)draw_data->DisplaySize.y);
        gcmd.set_blend_factor(float4(0.0f, 0.0f, 0.0f, 0.0f));
        gcmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        gcmd.set_index_buffer(geometry.indices.index_buffer_view());
        gcmd.set_pipeline(_pipeline);

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
                    gcmd.set_scissor(
                        (uint)(pcmd->ClipRect.x - clip_off.x),
                        (uint)(pcmd->ClipRect.y - clip_off.y),
                        (uint)(pcmd->ClipRect.z - clip_off.x),
                        (uint)(pcmd->ClipRect.w - clip_off.y)
                    );
                    gcmd.set_constants(fb::kcn::gui::Bindings {
                        .constants = _constants.cbv_descriptor().index(),
                        .vertices = geometry.vertices.srv_descriptor().index(),
                        .base_vertex = (pcmd->VtxOffset + (uint)global_vtx_offset),
                        .texture = _texture.srv_descriptor().index(),
                    });
                    gcmd.draw_indexed_instanced(
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
        gcmd.pix_end();
    });
}

} // namespace fb
