#include "tree.hpp"

#pragma warning(push)
#pragma warning(disable : 4100)

namespace fb::tree {

static void init_scene_model(
    Dx& dx,
    const GltfModel& gltf_model,
    std::string_view model_name,
    Demo::Scene::Model& model) {
    // Geometry.
    {
        auto& vertex_buffer = model.vertex_buffer;
        auto& index_buffer = model.index_buffer;
        vertex_buffer.create(
            dx,
            gltf_model.vertex_count(),
            dx_name(Demo::NAME, model_name, "Vertex Buffer"));
        index_buffer.create(
            dx,
            gltf_model.index_count(),
            dx_name(Demo::NAME, model_name, "Index Buffer"));
        memcpy(vertex_buffer.ptr(), gltf_model.vertex_data(), gltf_model.vertex_buffer_size());
        memcpy(index_buffer.ptr(), gltf_model.index_data(), gltf_model.index_buffer_size());
    }

    // Texture.
    {
        // Resource.
        const auto& gltf_texture = gltf_model.base_color_texture();
        auto& texture = model.texture;
        auto texture_desc = CD3DX12_RESOURCE_DESC::Tex2D(
            gltf_texture.format(),
            gltf_texture.width(),
            gltf_texture.height(),
            1,
            1);
        CD3DX12_HEAP_PROPERTIES texture_heap(D3D12_HEAP_TYPE_DEFAULT);
        FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
            &texture_heap,
            D3D12_HEAP_FLAG_NONE,
            &texture_desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&texture)));
        dx_set_name(texture, dx_name(Demo::NAME, model_name, "Texture"));

        // Upload.
        D3D12_SUBRESOURCE_DATA subresource_data = {
            .pData = gltf_texture.data(),
            .RowPitch = gltf_texture.row_pitch(),
            .SlicePitch = gltf_texture.slice_pitch(),
        };
        DirectX::ResourceUploadBatch rub(dx.device.get());
        rub.Begin();
        rub.Upload(texture.get(), 0, &subresource_data, 1);
        rub.Transition(
            texture.get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        auto finish = rub.End(dx.command_queue.get());
        finish.wait();
    }
}

static void init_scene(Dx& dx, Demo::Scene& scene) {
    init_scene_model(dx, GltfModel("models/coconut_tree.glb"), "Tree", scene.tree);
    init_scene_model(dx, GltfModel("models/sand_plane.glb"), "Plane", scene.plane);
}

static void init_shadow_pass(Dx& dx, Demo& demo, Demo::ShadowPass& pass) {
    // Shaders.
    Shader vertex_shader;
    {
        ShaderCompiler sc;
        auto path = "shaders/tree.shadow.hlsl";
        auto source = read_whole_file(path);
        vertex_shader = sc.compile(
            dx_name(Demo::NAME, Demo::ShadowPass::NAME),
            ShaderType::Vertex,
            "vs_main",
            source);
    }

    // Pipeline state.
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {
            .pRootSignature = demo.root_signature.get(),
            .VS = vertex_shader.bytecode(),
            .BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            .SampleMask = UINT_MAX,
            .RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            .DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT),
            .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            .DSVFormat = DXGI_FORMAT_D32_FLOAT,
            .SampleDesc = {.Count = 1, .Quality = 0},
        };
        FAIL_FAST_IF_FAILED(
            dx.device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pass.pipeline_state)));
        dx_set_name(
            pass.pipeline_state,
            dx_name(Demo::NAME, Demo::ShadowPass::NAME, "Pipeline State"));
    }

    // Constants.
    pass.constants.create(dx, 1, dx_name(Demo::NAME, Demo::ShadowPass::NAME, "Constants"));

    // Depth.
    {
        // Resource.
        CD3DX12_HEAP_PROPERTIES heap_properties(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC resource_desc = CD3DX12_RESOURCE_DESC::Tex2D(
            DXGI_FORMAT_R32_TYPELESS,
            SHADOW_MAP_SIZE,
            SHADOW_MAP_SIZE,
            1,
            1,
            1,
            0,
            D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
        D3D12_CLEAR_VALUE clear_value = {
            .Format = DXGI_FORMAT_D32_FLOAT,
            .DepthStencil = {1.0f, 0},
        };
        FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
            &heap_properties,
            D3D12_HEAP_FLAG_NONE,
            &resource_desc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &clear_value,
            IID_PPV_ARGS(&pass.depth)));
        dx_set_name(pass.depth, dx_name(Demo::NAME, Demo::ShadowPass::NAME, "Depth"));
    }
}

static void init_main_pass(Dx& dx, Demo& demo, Demo::MainPass& pass) {
    // Shaders.
    Shader vertex_shader;
    Shader pixel_shader;
    {
        ShaderCompiler sc;
        auto path = "shaders/tree.main.hlsl";
        auto source = read_whole_file(path);
        auto name = dx_name(Demo::NAME, Demo::MainPass::NAME);
        vertex_shader = sc.compile(name, ShaderType::Vertex, "vs_main", source);
        pixel_shader = sc.compile(name, ShaderType::Pixel, "ps_main", source);
    }

    // Pipeline state.
    {
        D3D12_INPUT_ELEMENT_DESC input_element_descs[] = {
            // clang-format off
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            // clang-format on
        };
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {
            .pRootSignature = demo.root_signature.get(),
            .VS = vertex_shader.bytecode(),
            .PS = pixel_shader.bytecode(),
            .BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            .SampleMask = UINT_MAX,
            .RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            .DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT),
            .InputLayout = {input_element_descs, _countof(input_element_descs)},
            .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            .NumRenderTargets = 1,
            .RTVFormats = {DXGI_FORMAT_R8G8B8A8_UNORM},
            .DSVFormat = DXGI_FORMAT_D32_FLOAT,
            .SampleDesc = {.Count = 1, .Quality = 0},
        };
        FAIL_FAST_IF_FAILED(
            dx.device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pass.pipeline_state)));
        dx_set_name(
            pass.pipeline_state,
            dx_name(Demo::NAME, Demo::MainPass::NAME, "Pipeline State"));
    }

    // Constants.
    pass.constants.create(dx, 1, dx_name(Demo::NAME, Demo::MainPass::NAME, "Constants"));
}

static void init_descriptors(
    Dx& dx,
    Demo& demo,
    Demo::Scene& scene,
    Demo::ShadowPass& shadow_pass,
    Demo::MainPass& main_pass) {
    // Allocate descriptors.
    {
        scene.tree.vertex_buffer_descriptor = demo.descriptors.cbv_srv_uav().alloc();
        scene.tree.texture_descriptor = demo.descriptors.cbv_srv_uav().alloc();
        scene.plane.vertex_buffer_descriptor = demo.descriptors.cbv_srv_uav().alloc();
        scene.plane.texture_descriptor = demo.descriptors.cbv_srv_uav().alloc();
        shadow_pass.constants_descriptor = demo.descriptors.cbv_srv_uav().alloc();
        shadow_pass.depth_srv_descriptor = demo.descriptors.cbv_srv_uav().alloc();
        shadow_pass.depth_dsv_descriptor = demo.descriptors.dsv().alloc();
        main_pass.constants_descriptor = demo.descriptors.cbv_srv_uav().alloc();
    }

    // Resource views.
    {
        // Constants.
        {
            auto cbv_desc = shadow_pass.constants.cbv_desc();
            dx.device->CreateConstantBufferView(&cbv_desc, shadow_pass.constants_descriptor.cpu());
        }
        {
            auto cbv_desc = main_pass.constants.cbv_desc();
            dx.device->CreateConstantBufferView(&cbv_desc, main_pass.constants_descriptor.cpu());
        }

        // Scene.
        {
            auto& model = scene.tree;
            auto srv_desc = model.vertex_buffer.srv_desc();
            dx.device->CreateShaderResourceView(
                model.vertex_buffer.resource(),
                &srv_desc,
                model.vertex_buffer_descriptor.cpu());
        }
        {
            auto& model = scene.plane;
            auto srv_desc = model.vertex_buffer.srv_desc();
            dx.device->CreateShaderResourceView(
                model.vertex_buffer.resource(),
                &srv_desc,
                model.vertex_buffer_descriptor.cpu());
        }
        {
            D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
                .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                .Texture2D = D3D12_TEX2D_SRV {.MipLevels = 1},
            };
            dx.device->CreateShaderResourceView(
                scene.tree.texture.get(),
                &srv_desc,
                scene.tree.texture_descriptor.cpu());
            dx.device->CreateShaderResourceView(
                scene.plane.texture.get(),
                &srv_desc,
                scene.plane.texture_descriptor.cpu());
        }

        // Shadow map.
        {
            D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {
                .Format = DXGI_FORMAT_D32_FLOAT,
                .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
                .Flags = D3D12_DSV_FLAG_NONE,
                .Texture2D = D3D12_TEX2D_DSV {.MipSlice = 0},
            };
            dx.device->CreateDepthStencilView(
                shadow_pass.depth.get(),
                &dsv_desc,
                shadow_pass.depth_dsv_descriptor.cpu());
        }
        {
            D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {
                .Format = DXGI_FORMAT_R32_FLOAT,
                .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
                .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                .Texture2D = D3D12_TEX2D_SRV {.MipLevels = 1},
            };
            dx.device->CreateShaderResourceView(
                shadow_pass.depth.get(),
                &srv_desc,
                shadow_pass.depth_srv_descriptor.cpu());
        }
    }
}

Demo::Demo(Dx& dx) :
    root_signature(dx, Demo::NAME),
    descriptors(dx, Demo::NAME),
    samplers(dx, descriptors),
    render_targets(
        dx,
        descriptors,
        dx.swapchain_width,
        dx.swapchain_height,
        Demo::CLEAR_COLOR,
        Demo::NAME),
    debug_draw(dx, Demo::NAME) {
    init_scene(dx, scene);
    init_shadow_pass(dx, *this, shadow_pass);
    init_main_pass(dx, *this, main_pass);
    init_descriptors(dx, *this, scene, shadow_pass, main_pass);
}

void Demo::update(const demo::UpdateDesc& desc) {
    static float light_projection_size = 15.0f;
    static float light_distance = 15.0f;
    static float shadow_near_plane = 0.1f;
    static float shadow_far_plane = 100.0f;
    static float camera_angle = PI;

    auto& shadow_constants = *shadow_pass.constants.ptr();
    auto& main_constants = *main_pass.constants.ptr();

    // ImGui.
    if (ImGui::Begin(Demo::NAME.data())) {
        ImGui::SliderFloat("Ambient", &main_constants.ambient_light, 0.0f, 1.0f);
        ImGui::SliderFloat("Light Projection Size", &light_projection_size, 1.0f, 200.0f);
        ImGui::SliderFloat("Light Distance", &light_distance, 1.0f, 200.0f);
        ImGui::SliderFloat("Shadow Near Plane", &shadow_near_plane, 0.0f, 10.0f);
        ImGui::SliderFloat("Shadow Far Plane", &shadow_far_plane, 1.0f, 100.0f);
        ImGui::SliderFloat("Camera Angle", &camera_angle, 0.0f, 2.0f * PI);
    }
    ImGui::End();

    // Shadow pass - constants.
    {
        auto projection = Matrix::CreateOrthographic(
            light_projection_size,
            light_projection_size,
            shadow_near_plane,
            shadow_far_plane);
        auto eye = light_distance * main_constants.light_direction;
        auto at = Vector3(0.0f, 0.0f, 0.0f);
        auto up = Vector3(0.0f, 1.0f, 0.0f);
        auto view = Matrix::CreateLookAt(eye, at, up);

        shadow_constants.transform = view * projection;
    }

    // Main pass - constants.
    Matrix transform;
    {
        auto fov = rad_from_deg(45.0f);
        auto aspect_ratio = desc.aspect_ratio;
        auto projection = Matrix::CreatePerspectiveFieldOfView(fov, aspect_ratio, 0.1f, 100.0f);
        auto eye = Vector3(7.0f * std::sin(camera_angle), 8.0f, 7.0f * std::cos(camera_angle));
        auto at = Vector3(0.0f, 3.0f, 0.0f);
        auto up = Vector3(0.0f, 1.0f, 0.0f);
        auto view = Matrix::CreateLookAt(eye, at, up);
        transform = view * projection;

        main_constants.transform = transform;
        main_constants.light_transform = shadow_constants.transform;
    }

    // Debug.
    {
        debug_draw.begin(desc.frame_index);
        debug_draw.transform(transform);
        debug_draw.axes();
        debug_draw.line(Vector3(), 16.0f * main_constants.light_direction, COLOR_YELLOW);
        debug_draw.end();
    }
}

void Demo::render(Dx& dx) {
    // Shadow pass.
    {
        D3D12_VIEWPORT viewport = {
            .TopLeftX = 0.0f,
            .TopLeftY = 0.0f,
            .Width = (float)SHADOW_MAP_SIZE,
            .Height = (float)SHADOW_MAP_SIZE,
            .MinDepth = 0.0f,
            .MaxDepth = 1.0f,
        };
        D3D12_RECT scissor = {
            .left = 0,
            .top = 0,
            .right = (LONG)SHADOW_MAP_SIZE,
            .bottom = (LONG)SHADOW_MAP_SIZE,
        };

        auto* cmd = dx.command_list.get();
        dx.transition(
            shadow_pass.depth,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_DEPTH_WRITE);
        cmd->RSSetViewports(1, &viewport);
        cmd->RSSetScissorRects(1, &scissor);
        cmd->OMSetRenderTargets(0, nullptr, FALSE, shadow_pass.depth_dsv_descriptor.cpu_ptr());
        cmd->ClearDepthStencilView(
            shadow_pass.depth_dsv_descriptor.cpu(),
            D3D12_CLEAR_FLAG_DEPTH,
            1.0f,
            0,
            0,
            nullptr);

        ID3D12DescriptorHeap* heaps[] = {
            descriptors.cbv_srv_uav().heap(),
            descriptors.sampler().heap()};
        cmd->SetDescriptorHeaps(_countof(heaps), heaps);
        cmd->SetGraphicsRootSignature(root_signature.get());
        GpuBindings bindings;
        bindings.push(shadow_pass.constants_descriptor);
        bindings.push(scene.tree.vertex_buffer_descriptor);
        cmd->SetGraphicsRoot32BitConstants(0, bindings.capacity(), bindings.ptr(), 0);
        cmd->SetPipelineState(shadow_pass.pipeline_state.get());
        cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        {
            auto ibv = scene.tree.index_buffer.index_buffer_view();
            auto index_count = scene.tree.index_buffer.element_size();
            cmd->IASetIndexBuffer(&ibv);
            cmd->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
        }

        dx.transition(
            shadow_pass.depth,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }

    // Main pass.
    {
        auto* cmd = dx.command_list.get();

        render_targets.begin(dx);

        debug_draw.render(dx);

        ID3D12DescriptorHeap* heaps[] = {
            descriptors.cbv_srv_uav().heap(),
            descriptors.sampler().heap()};
        cmd->SetDescriptorHeaps(_countof(heaps), heaps);
        cmd->SetGraphicsRootSignature(root_signature.get());
        cmd->SetPipelineState(main_pass.pipeline_state.get());
        cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        {
            auto ibv = scene.tree.index_buffer.index_buffer_view();
            auto index_count = scene.tree.index_buffer.element_size();
            cmd->IASetIndexBuffer(&ibv);
            GpuBindings bindings;
            bindings.push(main_pass.constants_descriptor);
            bindings.push(scene.tree.vertex_buffer_descriptor);
            bindings.push(scene.tree.texture_descriptor);
            bindings.push(shadow_pass.depth_srv_descriptor);
            cmd->SetGraphicsRoot32BitConstants(0, bindings.capacity(), bindings.ptr(), 0);
            cmd->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
        }

        {
            auto ibv = scene.plane.index_buffer.index_buffer_view();
            auto index_count = scene.plane.index_buffer.element_size();
            cmd->IASetIndexBuffer(&ibv);
            GpuBindings bindings;
            bindings.push(main_pass.constants_descriptor);
            bindings.push(scene.plane.vertex_buffer_descriptor);
            bindings.push(scene.plane.texture_descriptor);
            bindings.push(shadow_pass.depth_srv_descriptor);
            cmd->SetGraphicsRoot32BitConstants(0, bindings.capacity(), bindings.ptr(), 0);
            cmd->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
        }

        render_targets.end(dx);
    }
}

}  // namespace fb::tree
