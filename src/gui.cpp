#include "gui.hpp"
#include "utils.hpp"
#include "win32.hpp"
#include "dx12.hpp"
#include "maths.hpp"

#include <backends/imgui_impl_win32.h>

#include <DirectXTK12/ResourceUploadBatch.h>

namespace fb {

struct Gui {
    ImGuiContext* ctx = nullptr;
    fb::DxShader vertex_shader;
    fb::DxShader pixel_shader;
    ID3D12RootSignature* root_signature = nullptr;
    ID3D12PipelineState* pipeline_state = nullptr;
    ID3D12Resource* font_texture_resource = nullptr;
    ID3D12DescriptorHeap* cbv_srv_heap = nullptr;
    struct Geometry {
        ID3D12Resource* vertex_buffer = nullptr;
        ID3D12Resource* index_buffer = nullptr;
        size_t vertex_buffer_size = sizeof(ImDrawVert) * 1024 * 1024;
        size_t index_buffer_size = sizeof(ImDrawIdx) * 1024 * 1024;
    };
    std::array<Geometry, fb::FRAME_COUNT> geometries;
};

Gui* gui_create(Window* window, Dx* dx) {
    // ImGui.
    ImGuiContext* imgui_ctx = nullptr;
    {
        IMGUI_CHECKVERSION();
        imgui_ctx = ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.BackendRendererName = "framebuffet";
        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
        ImGui::StyleColorsDark();
    }

    // Shaders.
    fb::DxShader vertex_shader;
    fb::DxShader pixel_shader;
    {
        fb::Dxc dxc;
        fb::dxc_create(&dxc);
        auto source = fb::read_whole_file("shaders/gui.hlsl");
        fb::dxc_shader_compile(
            &dxc,
            {
                .name = "gui",
                .type = fb::DxShaderType::Vertex,
                .entry_point = "vertex_shader",
                .source = source,
            },
            &vertex_shader);
        fb::dxc_shader_compile(
            &dxc,
            {
                .name = "gui",
                .type = fb::DxShaderType::Pixel,
                .entry_point = "pixel_shader",
                .source = source,
            },
            &pixel_shader);
        fb::dxc_destroy(&dxc);
    }

    // Root signature.
    ID3D12RootSignature* root_signature = nullptr;
    {
        CD3DX12_DESCRIPTOR_RANGE1 srv_range = {};
        srv_range.Init(
            D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
            1,
            0,
            0,
            D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

        CD3DX12_ROOT_PARAMETER1 root_parameters[2] = {};
        root_parameters[0].InitAsConstants(16, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
        root_parameters[1].InitAsDescriptorTable(1, &srv_range, D3D12_SHADER_VISIBILITY_PIXEL);

        CD3DX12_STATIC_SAMPLER_DESC1 sampler;
        sampler.Init(
            0,
            D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            0.0f,
            0,
            D3D12_COMPARISON_FUNC_ALWAYS,
            D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK,
            0.0f,
            0.0f,
            D3D12_SHADER_VISIBILITY_PIXEL,
            0,
            D3D12_SAMPLER_FLAG_NONE);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc;
        decltype(desc)::Init_1_2(
            desc,
            _countof(root_parameters),
            root_parameters,
            1,
            &sampler,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        fb::ComPtr<ID3DBlob> signature;
        fb::ComPtr<ID3DBlob> error;
        FAIL_FAST_IF_FAILED(D3DX12SerializeVersionedRootSignature(
            &desc,
            D3D_ROOT_SIGNATURE_VERSION_1_2,
            &signature,
            &error));
        FAIL_FAST_IF_FAILED(dx->device->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&root_signature)));
        fb::dx_set_name(root_signature, "Gui Root Signature");
    }

    // Pipeline state.
    ID3D12PipelineState* pipeline_state = nullptr;
    {
        D3D12_INPUT_ELEMENT_DESC input_element_descs[] = {
            // clang-format off
            {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, (UINT)IM_OFFSETOF(ImDrawVert, pos), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, (UINT)IM_OFFSETOF(ImDrawVert, uv), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"COLOR", 0,DXGI_FORMAT_R8G8B8A8_UNORM, 0, (UINT)IM_OFFSETOF(ImDrawVert, col), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            // clang-format on
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {
            .pRootSignature = root_signature,
            .VS = fb::dx_shader_bytecode(&vertex_shader),
            .PS = fb::dx_shader_bytecode(&pixel_shader),
            .BlendState =
                {
                    .AlphaToCoverageEnable = false,
                    .IndependentBlendEnable = false,
                    .RenderTarget =
                        {
                            {
                                .BlendEnable = true,
                                .LogicOpEnable = false,
                                .SrcBlend = D3D12_BLEND_SRC_ALPHA,
                                .DestBlend = D3D12_BLEND_INV_SRC_ALPHA,
                                .BlendOp = D3D12_BLEND_OP_ADD,
                                .SrcBlendAlpha = D3D12_BLEND_ONE,
                                .DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA,
                                .BlendOpAlpha = D3D12_BLEND_OP_ADD,
                                .LogicOp = D3D12_LOGIC_OP_NOOP,
                                .RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL,
                            },
                        },
                },
            .SampleMask = UINT_MAX,
            .RasterizerState =
                {
                    .FillMode = D3D12_FILL_MODE_SOLID,
                    .CullMode = D3D12_CULL_MODE_NONE,
                    .FrontCounterClockwise = FALSE,
                    .DepthBias = D3D12_DEFAULT_DEPTH_BIAS,
                    .DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
                    .SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
                    .DepthClipEnable = true,
                    .MultisampleEnable = FALSE,
                    .AntialiasedLineEnable = FALSE,
                    .ForcedSampleCount = 0,
                    .ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF,
                },
            .DepthStencilState =
                {
                    .DepthEnable = false,
                    .DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,
                    .DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS,
                    .StencilEnable = false,
                    .FrontFace =
                        {
                            .StencilFailOp = D3D12_STENCIL_OP_KEEP,
                            .StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
                            .StencilPassOp = D3D12_STENCIL_OP_KEEP,
                            .StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS,
                        },
                    .BackFace =
                        {
                            .StencilFailOp = D3D12_STENCIL_OP_KEEP,
                            .StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
                            .StencilPassOp = D3D12_STENCIL_OP_KEEP,
                            .StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS,
                        },
                },
            .InputLayout =
                {
                    input_element_descs,
                    _countof(input_element_descs),
                },
            .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            .NumRenderTargets = 1,
            .RTVFormats =
                {
                    DXGI_FORMAT_R8G8B8A8_UNORM,
                },
            .SampleDesc =
                {
                    .Count = 1,
                    .Quality = 0,
                },
        };
        FAIL_FAST_IF_FAILED(
            dx->device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipeline_state)));
        fb::dx_set_name(pipeline_state, "Gui Pipeline State");
    }

    // Font texture.
    ID3D12Resource* font_texture_resource = nullptr;
    {
        // Raw data.
        ImGuiIO& io = ImGui::GetIO();
        uint8_t* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        // Texture.
        auto texture_desc =
            CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1);
        CD3DX12_HEAP_PROPERTIES texture_heap(D3D12_HEAP_TYPE_DEFAULT);
        FAIL_FAST_IF_FAILED(dx->device->CreateCommittedResource(
            &texture_heap,
            D3D12_HEAP_FLAG_NONE,
            &texture_desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&font_texture_resource)));
        fb::dx_set_name(font_texture_resource, "Gui Font Texture");

        // Upload.
        D3D12_SUBRESOURCE_DATA subresource_data = {
            .pData = pixels,
            .RowPitch = width * 4,
            .SlicePitch = width * height * 4,
        };
        DirectX::ResourceUploadBatch rub(dx->device);
        rub.Begin();
        rub.Upload(font_texture_resource, 0, &subresource_data, 1);
        rub.Transition(
            font_texture_resource,
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        auto finish = rub.End(dx->command_queue);
        finish.wait();
    }

    // Descriptors.
    ID3D12DescriptorHeap* cbv_srv_heap = nullptr;
    {
        D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .NumDescriptors = 1,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            .NodeMask = 0,
        };
        FAIL_FAST_IF_FAILED(
            dx->device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&cbv_srv_heap)));
        dx_set_name(cbv_srv_heap, "Gui Heap");

        D3D12_CPU_DESCRIPTOR_HANDLE cpu_desc = cbv_srv_heap->GetCPUDescriptorHandleForHeapStart();
        D3D12_GPU_DESCRIPTOR_HANDLE gpu_desc = cbv_srv_heap->GetGPUDescriptorHandleForHeapStart();

        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
            .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
            .Texture2D =
                {
                    .MipLevels = 1,
                },
        };
        dx->device->CreateShaderResourceView(font_texture_resource, &srv_desc, cpu_desc);
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->SetTexID((ImTextureID)gpu_desc.ptr);
    }

    // Geometry.
    std::array<Gui::Geometry, fb::FRAME_COUNT> geometries;
    for (uint32_t i = 0; i < fb::FRAME_COUNT; i++) {
        Gui::Geometry& geometry = geometries[i];
        CD3DX12_HEAP_PROPERTIES geometry_buffer_heap(D3D12_HEAP_TYPE_UPLOAD);
        CD3DX12_RESOURCE_DESC vertex_buffer_desc =
            CD3DX12_RESOURCE_DESC::Buffer(geometry.vertex_buffer_size, D3D12_RESOURCE_FLAG_NONE);
        CD3DX12_RESOURCE_DESC index_buffer_desc =
            CD3DX12_RESOURCE_DESC::Buffer(geometry.index_buffer_size, D3D12_RESOURCE_FLAG_NONE);
        FAIL_FAST_IF_FAILED(dx->device->CreateCommittedResource(
            &geometry_buffer_heap,
            D3D12_HEAP_FLAG_NONE,
            &vertex_buffer_desc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&geometry.vertex_buffer)));
        FAIL_FAST_IF_FAILED(dx->device->CreateCommittedResource(
            &geometry_buffer_heap,
            D3D12_HEAP_FLAG_NONE,
            &index_buffer_desc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&geometry.index_buffer)));
        fb::dx_set_indexed_name(geometry.vertex_buffer, "Gui Vertex Buffer", i);
        fb::dx_set_indexed_name(geometry.index_buffer, "Gui Index Buffer", i);
    }

    // ImGui continued.
    ImGui_ImplWin32_Init((HWND)window_handle(window));

    // Result.
    Gui* gui = new Gui();
    gui->ctx = imgui_ctx;
    gui->vertex_shader = vertex_shader;
    gui->pixel_shader = pixel_shader;
    gui->root_signature = root_signature;
    gui->pipeline_state = pipeline_state;
    gui->font_texture_resource = font_texture_resource;
    gui->cbv_srv_heap = cbv_srv_heap;
    gui->geometries = geometries;
    return gui;
}

void gui_destroy(Gui* gui) {
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext(gui->ctx);

    for (const auto& geometry : gui->geometries) {
        geometry.vertex_buffer->Release();
        geometry.index_buffer->Release();
    }
    gui->cbv_srv_heap->Release();
    gui->font_texture_resource->Release();
    gui->pipeline_state->Release();
    gui->root_signature->Release();
    dx_shader_destroy(&gui->pixel_shader);
    dx_shader_destroy(&gui->vertex_shader);

    delete gui;
}

void gui_update(Gui*) {
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
    ImGui::Render();
}

void gui_render(Gui* gui, Dx* dx) {
    ImDrawData* draw_data = ImGui::GetDrawData();

    // Avoid rendering when minimized.
    if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f) {
        return;
    }

    // Update geometries.
    Gui::Geometry& geometry = gui->geometries[dx->frame_index];
    {
        ImDrawVert* vertices = nullptr;
        ImDrawIdx* indices = nullptr;
        D3D12_RANGE whole_range = {0, 0};
        FAIL_FAST_IF_FAILED(geometry.vertex_buffer->Map(0, &whole_range, (void**)&vertices));
        FAIL_FAST_IF_FAILED(geometry.index_buffer->Map(0, &whole_range, (void**)&indices));
        for (int i = 0; i < draw_data->CmdListsCount; i++) {
            ImDrawList* cmd_list = draw_data->CmdLists[i];
            memcpy(
                vertices,
                cmd_list->VtxBuffer.Data,
                cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
            memcpy(indices, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
            vertices += cmd_list->VtxBuffer.Size;
            indices += cmd_list->IdxBuffer.Size;
        }
        geometry.vertex_buffer->Unmap(0, nullptr);
        geometry.index_buffer->Unmap(0, nullptr);
    }

    // Update transform.
    fb::Mat4x4 transform;
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
        memcpy(&transform, m, sizeof(m));
    }

    // Render.
    {
        ID3D12GraphicsCommandList9* cmd = dx->command_list;

        CD3DX12_VIEWPORT viewport(0.0f, 0.0f, draw_data->DisplaySize.x, draw_data->DisplaySize.y);
        D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view = {
            .BufferLocation = geometry.vertex_buffer->GetGPUVirtualAddress(),
            .SizeInBytes = (UINT)geometry.vertex_buffer->GetDesc().Width,
            .StrideInBytes = (UINT)sizeof(ImDrawVert),
        };
        D3D12_INDEX_BUFFER_VIEW index_buffer_view = {
            .BufferLocation = geometry.index_buffer->GetGPUVirtualAddress(),
            .SizeInBytes = (UINT)geometry.index_buffer->GetDesc().Width,
            .Format = sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT,
        };
        const float blend_factor[4] = {0.f, 0.f, 0.f, 0.f};

        cmd->RSSetViewports(1, &viewport);
        cmd->IASetVertexBuffers(0, 1, &vertex_buffer_view);
        cmd->IASetIndexBuffer(&index_buffer_view);
        cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd->SetPipelineState(gui->pipeline_state);
        cmd->SetGraphicsRootSignature(gui->root_signature);
        cmd->SetGraphicsRoot32BitConstants(0, 16, &transform, 0);
        cmd->SetDescriptorHeaps(1, &gui->cbv_srv_heap);
        cmd->OMSetBlendFactor(blend_factor);

        int global_vtx_offset = 0;
        int global_idx_offset = 0;
        ImVec2 clip_off = draw_data->DisplayPos;
        for (int i = 0; i < draw_data->CmdListsCount; i++) {
            ImDrawList* cmd_list = draw_data->CmdLists[i];
            for (int j = 0; j < cmd_list->CmdBuffer.Size; j++) {
                ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];
                if (pcmd->UserCallback != nullptr) {
                    pcmd->UserCallback(cmd_list, pcmd);
                } else {
                    D3D12_RECT scissor_rect = {
                        (LONG)(pcmd->ClipRect.x - clip_off.x),
                        (LONG)(pcmd->ClipRect.y - clip_off.y),
                        (LONG)(pcmd->ClipRect.z - clip_off.x),
                        (LONG)(pcmd->ClipRect.w - clip_off.y),
                    };
                    cmd->SetGraphicsRootDescriptorTable(
                        1,
                        gui->cbv_srv_heap->GetGPUDescriptorHandleForHeapStart());
                    cmd->RSSetScissorRects(1, &scissor_rect);
                    cmd->DrawIndexedInstanced(
                        pcmd->ElemCount,
                        1,
                        pcmd->IdxOffset + global_idx_offset,
                        pcmd->VtxOffset + global_vtx_offset,
                        0);
                }
            }
            global_idx_offset += cmd_list->IdxBuffer.Size;
            global_vtx_offset += cmd_list->VtxBuffer.Size;
        }
    }
}

}  // namespace fb
