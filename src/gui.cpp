#include "gui.hpp"
#include "utils.hpp"
#include "win32.hpp"
#include "maths.hpp"

#include <backends/imgui_impl_win32.h>

namespace fb::gui {

Gui::Gui(const Window& window, GpuDevice& device) {
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
    }

    // Shaders.
    GpuShader vertex_shader;
    GpuShader pixel_shader;
    {
        GpuShaderCompiler sc;
        auto source = read_whole_file("shaders/gui.hlsl");
        vertex_shader = sc.compile(Gui::NAME, GpuShaderType::Vertex, "vs_main", source);
        pixel_shader = sc.compile(Gui::NAME, GpuShaderType::Pixel, "ps_main", source);
    }

    // Pipeline state.
    _pipeline_state = device.create_graphics_pipeline_state(
        D3D12_GRAPHICS_PIPELINE_STATE_DESC {
            .pRootSignature = device.root_signature(),
            .VS = vertex_shader.bytecode(),
            .PS = pixel_shader.bytecode(),
            .BlendState =
                {.AlphaToCoverageEnable = false,
                 .IndependentBlendEnable = false,
                 .RenderTarget =
                     {{.BlendEnable = true,
                       .LogicOpEnable = false,
                       .SrcBlend = D3D12_BLEND_SRC_ALPHA,
                       .DestBlend = D3D12_BLEND_INV_SRC_ALPHA,
                       .BlendOp = D3D12_BLEND_OP_ADD,
                       .SrcBlendAlpha = D3D12_BLEND_ONE,
                       .DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA,
                       .BlendOpAlpha = D3D12_BLEND_OP_ADD,
                       .LogicOp = D3D12_LOGIC_OP_NOOP,
                       .RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL}}},
            .SampleMask = UINT_MAX,
            .RasterizerState =
                {.FillMode = D3D12_FILL_MODE_SOLID,
                 .CullMode = D3D12_CULL_MODE_NONE,
                 .FrontCounterClockwise = FALSE,
                 .DepthBias = D3D12_DEFAULT_DEPTH_BIAS,
                 .DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
                 .SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
                 .DepthClipEnable = true,
                 .MultisampleEnable = FALSE,
                 .AntialiasedLineEnable = FALSE,
                 .ForcedSampleCount = 0,
                 .ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF},
            .DepthStencilState = DirectX::DX12::CommonStates::DepthNone,
            .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            .NumRenderTargets = 1,
            .RTVFormats = {DXGI_FORMAT_R8G8B8A8_UNORM},
            .SampleDesc = {.Count = 1, .Quality = 0},
        },
        dx_name(Gui::NAME, "Pipeline State"));

    // Constants.
    _constant_buffer.create(device, 1, dx_name(Gui::NAME, "Constant Buffer"));

    // Font texture.
    {
        // Raw data.
        auto& io = ImGui::GetIO();
        uint8_t* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        // Texture.
        _texture.create(
            device,
            GpuTexture2dDesc {
                .format = DXGI_FORMAT_R8G8B8A8_UNORM,
                .width = (uint32_t)width,
                .height = (uint32_t)height,
            },
            dx_name(Gui::NAME, "Font Texture"));

        // ImGui Font texture ID.
        io.Fonts->SetTexID((ImTextureID)_texture.srv_descriptor().gpu().ptr);

        // Upload.
        device.easy_upload(
            D3D12_SUBRESOURCE_DATA {
                .pData = pixels,
                .RowPitch = width * 4,
                .SlicePitch = width * height * 4},
            _texture.resource(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }

    // Geometry.
    for (uint32_t i = 0; i < FRAME_COUNT; i++) {
        Gui::Geometry& geometry = _geometries[i];
        geometry.vertex_buffer.create(
            device,
            MAX_VERTEX_COUNT,
            dx_name(Gui::NAME, "Vertex Buffer", i));
        geometry.index_buffer.create(
            device,
            MAX_INDEX_COUNT,
            dx_name(Gui::NAME, "Index Buffer", i));
    }

    // ImGui continued.
    ImGui_ImplWin32_Init(window.hwnd());
}

Gui::~Gui() {
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext(_imgui_ctx);
}

auto Gui::update() -> void {
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
}

auto Gui::render(const GpuDevice& device, GpuCommandList& cmd) -> void {
    ImGui::Render();
    auto* draw_data = ImGui::GetDrawData();

    // Avoid rendering when minimized.
    if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f) {
        return;
    }

    // Update geometries.
    auto& geometry = _geometries[device.frame_index()];
    {
        ImDrawVert* vertices = geometry.vertex_buffer.ptr();
        ImDrawIdx* indices = geometry.index_buffer.ptr();
        for (int i = 0; i < draw_data->CmdListsCount; i++) {
            auto* cmd_list = draw_data->CmdLists[i];
            memcpy(
                vertices,
                cmd_list->VtxBuffer.Data,
                cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
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
        memcpy(_constant_buffer.ptr(), m, sizeof(m));
    }

    // Render.
    {
        cmd.set_graphics();
        cmd.set_viewport(
            0,
            0,
            (uint32_t)draw_data->DisplaySize.x,
            (uint32_t)draw_data->DisplaySize.y);
        cmd.set_blend_factor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(geometry.index_buffer.index_buffer_view());
        cmd.set_pipeline(_pipeline_state);

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
                        (uint32_t)(pcmd->ClipRect.w - clip_off.y));
                    cmd.set_graphics_constants({
                        _constant_buffer.cbv_descriptor().index(),
                        geometry.vertex_buffer.srv_descriptor().index(),
                        (pcmd->VtxOffset + (uint32_t)global_vtx_offset),
                        _texture.srv_descriptor().index(),
                    });
                    cmd.draw_indexed_instanced(
                        pcmd->ElemCount,
                        1,
                        pcmd->IdxOffset + global_idx_offset,
                        0,
                        0);
                }
            }
            global_idx_offset += cmd_list->IdxBuffer.Size;
            global_vtx_offset += cmd_list->VtxBuffer.Size;
        }
    }
}

}  // namespace fb::gui
