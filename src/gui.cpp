#include "gui.hpp"
#include "utils.hpp"
#include "win32.hpp"
#include "maths.hpp"

#include <backends/imgui_impl_win32.h>

namespace fb::gui {

Gui::Gui(const Window& window, GpuDevice& device) :
    root_signature(device, Gui::NAME),
    descriptors(device, Gui::NAME),
    samplers(device, descriptors) {
    // ImGui.
    {
        IMGUI_CHECKVERSION();
        imgui_ctx = ImGui::CreateContext();
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

    // Descriptors.
    {
        constant_buffer_descriptor = descriptors.cbv_srv_uav().alloc();
        for (auto& geometry : geometries) {
            geometry.vertex_buffer_descriptor = descriptors.cbv_srv_uav().alloc();
        }
        texture_descriptor = descriptors.cbv_srv_uav().alloc();
    }

    // Pipeline state.
    pipeline_state = device.create_graphics_pipeline_state(
        D3D12_GRAPHICS_PIPELINE_STATE_DESC {
            .pRootSignature = root_signature.get(),
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
    {
        constant_buffer.create(device, 1, dx_name(Gui::NAME, "Constant Buffer"));
        device.create_constant_buffer_view(
            constant_buffer.cbv_desc(),
            constant_buffer_descriptor.cpu());
    }

    // Font texture.
    {
        // Raw data.
        auto& io = ImGui::GetIO();
        uint8_t* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        // Texture.
        texture = device.create_committed_resource(
            CD3DX12_HEAP_PROPERTIES {D3D12_HEAP_TYPE_DEFAULT},
            CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1),
            D3D12_RESOURCE_STATE_COMMON,
            std::nullopt,
            dx_name(Gui::NAME, "Font Texture"));

        // Upload.
        device.easy_upload(
            D3D12_SUBRESOURCE_DATA {
                .pData = pixels,
                .RowPitch = width * 4,
                .SlicePitch = width * height * 4},
            texture,
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        // SRV.
        device.create_shader_resource_view(
            texture,
            D3D12_SHADER_RESOURCE_VIEW_DESC {
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
                .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                .Texture2D = {.MipLevels = 1}},
            texture_descriptor.cpu());
        io.Fonts->SetTexID((ImTextureID)texture_descriptor.gpu().ptr);
    }

    // Geometry.
    for (uint32_t i = 0; i < FRAME_COUNT; i++) {
        Gui::Geometry& geometry = geometries[i];
        geometry.vertex_buffer.create(
            device,
            MAX_VERTEX_COUNT,
            dx_name(Gui::NAME, "Vertex Buffer", i));
        geometry.index_buffer.create(
            device,
            MAX_INDEX_COUNT,
            dx_name(Gui::NAME, "Index Buffer", i));
        device.create_shader_resource_view(
            geometry.vertex_buffer.resource(),
            geometry.vertex_buffer.srv_desc(),
            geometry.vertex_buffer_descriptor.cpu());
    }

    // ImGui continued.
    ImGui_ImplWin32_Init(window.hwnd());
}

Gui::~Gui() {
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext(imgui_ctx);
}

void Gui::update() {
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
}

void Gui::render(const GpuDevice& device) {
    ImGui::Render();
    auto* draw_data = ImGui::GetDrawData();

    // Avoid rendering when minimized.
    if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f) {
        return;
    }

    // Update geometries.
    auto& geometry = geometries[device.frame_index()];
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
        memcpy(constant_buffer.ptr(), m, sizeof(m));
    }

    // Render.
    {
        auto* cmd = device.command_list();

        CD3DX12_VIEWPORT viewport(0.0f, 0.0f, draw_data->DisplaySize.x, draw_data->DisplaySize.y);
        auto ibv = geometry.index_buffer.index_buffer_view();
        const float blend_factor[4] = {0.f, 0.f, 0.f, 0.f};

        cmd->RSSetViewports(1, &viewport);
        cmd->OMSetBlendFactor(blend_factor);

        ID3D12DescriptorHeap* heaps[] = {
            descriptors.cbv_srv_uav().heap(),
            descriptors.sampler().heap()};
        cmd->SetDescriptorHeaps(_countof(heaps), heaps);
        cmd->SetGraphicsRootSignature(root_signature.get());

        cmd->SetPipelineState(pipeline_state.get());
        cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd->IASetIndexBuffer(&ibv);

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
                    D3D12_RECT scissor_rect = {
                        (LONG)(pcmd->ClipRect.x - clip_off.x),
                        (LONG)(pcmd->ClipRect.y - clip_off.y),
                        (LONG)(pcmd->ClipRect.z - clip_off.x),
                        (LONG)(pcmd->ClipRect.w - clip_off.y),
                    };
                    GpuBindings bindings;
                    bindings.push(constant_buffer_descriptor);
                    bindings.push(geometry.vertex_buffer_descriptor);
                    bindings.push((pcmd->VtxOffset + (uint32_t)global_vtx_offset));
                    bindings.push(texture_descriptor);
                    cmd->SetGraphicsRoot32BitConstants(0, bindings.capacity(), bindings.ptr(), 0);

                    cmd->RSSetScissorRects(1, &scissor_rect);
                    cmd->DrawIndexedInstanced(
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
