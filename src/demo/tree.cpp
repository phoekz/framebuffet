#include "tree.hpp"
#include "../shaders.hpp"
#include "../gltf.hpp"
#include <DirectXTK12/ResourceUploadBatch.h>
#include <imgui.h>

#pragma warning(push)
#pragma warning(disable : 4100)

namespace fb::tree {

constexpr float CLEAR_COLOR[4] = {0.32549f, 0.51373f, 0.56078f, 1.0f};

static void init_scene_model(
    Dx& dx,
    const GltfModel& gltf_model,
    const std::string_view model_name,
    Demo::Scene::Model& model) {
    // Geometry.
    {
        auto& vertex_buffer = model.vertex_buffer;
        auto& index_buffer = model.index_buffer;
        vertex_buffer.create_vb(
            dx,
            gltf_model.vertex_count(),
            D3D12_HEAP_TYPE_UPLOAD,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            dx_name(Demo::NAME, model_name, "Vertex Buffer"));
        index_buffer.create_ib(
            dx,
            gltf_model.index_count(),
            D3D12_HEAP_TYPE_UPLOAD,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            dx_name(Demo::NAME, model_name, "Index Buffer"));
        memcpy(vertex_buffer.ptr, gltf_model.vertex_data(), gltf_model.vertex_buffer_size());
        memcpy(index_buffer.ptr, gltf_model.index_data(), gltf_model.index_buffer_size());
    }

    // Texture.
    {
        // Resource.
        constexpr auto TEXTURE_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
        const auto& gltf_texture = gltf_model.base_color_texture;
        auto& texture = model.texture;
        auto texture_desc = CD3DX12_RESOURCE_DESC::Tex2D(
            TEXTURE_FORMAT,
            gltf_texture.width,
            gltf_texture.height,
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
    init_scene_model(dx, GltfModel::load("models/coconut_tree.glb"), "Tree", scene.tree);
    init_scene_model(dx, GltfModel::load("models/sand_plane.glb"), "Plane", scene.plane);
}

static void init_shadow_pass(Dx& dx, Demo::ShadowPass& pass) {
    // Shaders.
    Shader vertex_shader;
    {
        ShaderCompiler sc;
        auto path = "shaders/tree.shadow.hlsl";
        auto source = read_whole_file(path);
        vertex_shader = sc.compile("tree-shadow", ShaderType::Vertex, "vs_main", source);
    }

    // Root signature.
    {
        CD3DX12_ROOT_PARAMETER1 root_parameters[1] = {};
        root_parameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc;
        decltype(desc)::Init_1_2(
            desc,
            _countof(root_parameters),
            root_parameters,
            0,
            nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        FAIL_FAST_IF_FAILED(D3DX12SerializeVersionedRootSignature(
            &desc,
            D3D_ROOT_SIGNATURE_VERSION_1_2,
            &signature,
            &error));
        FAIL_FAST_IF_FAILED(dx.device->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&pass.root_signature)));
        dx_set_name(
            pass.root_signature,
            dx_name(Demo::NAME, Demo::ShadowPass::NAME, "Root Signature"));
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
            .pRootSignature = pass.root_signature.get(),
            .VS = vertex_shader.bytecode(),
            .BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            .SampleMask = UINT_MAX,
            .RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            .DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT),
            .InputLayout = {input_element_descs, _countof(input_element_descs)},
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
    {
        // Resource.
        auto& cb = pass.constants;
        cb.create_cb(
            dx,
            D3D12_HEAP_TYPE_UPLOAD,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            dx_name(Demo::NAME, Demo::ShadowPass::NAME, "Constants"));
        memset(cb.data(), 0, sizeof(ShadowConstants));

        // Descriptor heap.
        auto& cb_dh = pass.constants_descriptor_heap;
        D3D12_DESCRIPTOR_HEAP_DESC desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .NumDescriptors = 1,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
        };
        FAIL_FAST_IF_FAILED(dx.device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&cb_dh)));
        dx_set_name(
            cb_dh,
            dx_name(Demo::NAME, Demo::ShadowPass::NAME, "Constants", "Descriptor Heap"));

        // Constant buffer view.
        pass.constants_cpu_descriptor = cb_dh->GetCPUDescriptorHandleForHeapStart();
        pass.constants_gpu_descriptor = cb_dh->GetGPUDescriptorHandleForHeapStart();
        auto cbv_desc = cb.constant_buffer_view_desc();
        dx.device->CreateConstantBufferView(&cbv_desc, pass.constants_cpu_descriptor);
    }

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

        // Descriptor heap.
        auto& dsv_dh = pass.depth_descriptor_heap;
        D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
            .NumDescriptors = 1,
        };
        FAIL_FAST_IF_FAILED(
            dx.device->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(&dsv_dh)));
        dx_set_name(dsv_dh, dx_name(Demo::NAME, Demo::ShadowPass::NAME, "Descriptor Heap"));

        // Depth stencil view.
        pass.depth_cpu_descriptor = dsv_dh->GetCPUDescriptorHandleForHeapStart();
        D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {
            .Format = DXGI_FORMAT_D32_FLOAT,
            .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
            .Flags = D3D12_DSV_FLAG_NONE,
            .Texture2D = D3D12_TEX2D_DSV {.MipSlice = 0},
        };
        dx.device->CreateDepthStencilView(pass.depth.get(), &dsv_desc, pass.depth_cpu_descriptor);
    }
}

static void init_main_pass(Dx& dx, Demo::MainPass& pass) {
    // Root signature.
    {
        CD3DX12_ROOT_PARAMETER1 root_parameters[3] = {};
        root_parameters[0].InitAsConstantBufferView(
            0,
            0,
            D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC,
            D3D12_SHADER_VISIBILITY_ALL);
        CD3DX12_DESCRIPTOR_RANGE1 range1 = {};
        range1.Init(
            D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
            1,
            1,
            0,
            D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        root_parameters[1].InitAsDescriptorTable(1, &range1, D3D12_SHADER_VISIBILITY_PIXEL);
        CD3DX12_DESCRIPTOR_RANGE1 range2 = {};
        range2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
        root_parameters[2].InitAsDescriptorTable(1, &range2, D3D12_SHADER_VISIBILITY_PIXEL);
        D3D12_STATIC_SAMPLER_DESC1 main_sampler = {
            .Filter = D3D12_FILTER_MIN_MAG_MIP_POINT,
            .AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            .AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            .AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            .MipLODBias = 0.0f,
            .MaxAnisotropy = 0,
            .ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
            .BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK,
            .MinLOD = 0.0f,
            .MaxLOD = D3D12_FLOAT32_MAX,
            .ShaderRegister = 3,
            .RegisterSpace = 0,
            .ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL,
            .Flags = D3D12_SAMPLER_FLAG_NONE,
        };
        D3D12_STATIC_SAMPLER_DESC1 shadow_sampler = {
            .Filter = D3D12_FILTER_MIN_MAG_MIP_POINT,
            .AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
            .AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
            .AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
            .MipLODBias = 0.0f,
            .MaxAnisotropy = 0,
            .ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL,
            .BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
            .MinLOD = 0.0f,
            .MaxLOD = D3D12_FLOAT32_MAX,
            .ShaderRegister = 4,
            .RegisterSpace = 0,
            .ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL,
            .Flags = D3D12_SAMPLER_FLAG_NONE,
        };
        D3D12_STATIC_SAMPLER_DESC1 samplers[] = {main_sampler, shadow_sampler};

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc;
        decltype(desc)::Init_1_2(
            desc,
            _countof(root_parameters),
            root_parameters,
            _countof(samplers),
            samplers,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        HRESULT hr = D3DX12SerializeVersionedRootSignature(
            &desc,
            D3D_ROOT_SIGNATURE_VERSION_1_2,
            &signature,
            &error);
        if (FAILED(hr)) {
            if (error) {
                OutputDebugStringA((char*)error->GetBufferPointer());
            }
            FAIL_FAST_IF_FAILED(hr);
        }
        FAIL_FAST_IF_FAILED(hr);
        FAIL_FAST_IF_FAILED(dx.device->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&pass.root_signature)));
        dx_set_name(
            pass.root_signature,
            dx_name(Demo::NAME, Demo::MainPass::NAME, "Root Signature"));
    }

    // Shaders.
    Shader vertex_shader;
    Shader pixel_shader;
    {
        ShaderCompiler sc;
        auto path = "shaders/tree.main.hlsl";
        auto source = read_whole_file(path);
        vertex_shader = sc.compile("tree-main", ShaderType::Vertex, "vs_main", source);
        pixel_shader = sc.compile("tree-main", ShaderType::Pixel, "ps_main", source);
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
            .pRootSignature = pass.root_signature.get(),
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
            pass.root_signature,
            dx_name(Demo::NAME, Demo::MainPass::NAME, "Pipeline State"));
    }

    // Constants.
    {
        pass.constants.create_cb(
            dx,
            D3D12_HEAP_TYPE_UPLOAD,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            dx_name(Demo::NAME, Demo::MainPass::NAME, "Constants"));
        memset(pass.constants.data(), 0, sizeof(MainConstants));
        auto& constants = *pass.constants.data();
        constants.light_direction = Vec3(1.0f, 1.0f, 1.0f);
        constants.light_direction.Normalize();
        constants.ambient_light = 0.25f;
    }
}

static void init_target(Dx& dx, Demo::Target& target) {
    // Color.
    {
        // Resource.
        CD3DX12_HEAP_PROPERTIES color_target_heap(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC color_target_desc = CD3DX12_RESOURCE_DESC::Tex2D(
            DXGI_FORMAT_R8G8B8A8_UNORM,
            dx.swapchain_width,
            dx.swapchain_height,
            1,
            1,
            1,
            0,
            D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
        D3D12_CLEAR_VALUE color_clear_value = {
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .Color = {CLEAR_COLOR[0], CLEAR_COLOR[1], CLEAR_COLOR[2], CLEAR_COLOR[3]},
        };
        FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
            &color_target_heap,
            D3D12_HEAP_FLAG_NONE,
            &color_target_desc,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            &color_clear_value,
            IID_PPV_ARGS(&target.color)));
        dx_set_name(target.color, dx_name(Demo::NAME, "Color Target"));

        // Descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            .NumDescriptors = 1,
        };
        FAIL_FAST_IF_FAILED(dx.device->CreateDescriptorHeap(
            &descriptor_heap_desc,
            IID_PPV_ARGS(&target.color_descriptor_heap)));
        dx_set_name(
            target.color_descriptor_heap,
            dx_name(Demo::NAME, "Color Target", "Descriptor Heap"));

        // Render target view.
        target.color_descriptor =
            target.color_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        dx.device->CreateRenderTargetView(target.color.get(), nullptr, target.color_descriptor);
    }

    // Depth.
    {
        // Resource.
        CD3DX12_HEAP_PROPERTIES depth_target_heap(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC depth_target_desc = CD3DX12_RESOURCE_DESC::Tex2D(
            DXGI_FORMAT_D32_FLOAT,
            dx.swapchain_width,
            dx.swapchain_height,
            1,
            1,
            1,
            0,
            D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
        D3D12_CLEAR_VALUE depth_clear_value = {
            .Format = DXGI_FORMAT_D32_FLOAT,
            .DepthStencil = {.Depth = 1.0f, .Stencil = 0}};
        FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
            &depth_target_heap,
            D3D12_HEAP_FLAG_NONE,
            &depth_target_desc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depth_clear_value,
            IID_PPV_ARGS(&target.depth)));
        dx_set_name(target.depth, dx_name(Demo::NAME, "Depth Target"));

        // Descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
            .NumDescriptors = 1,
        };
        FAIL_FAST_IF_FAILED(dx.device->CreateDescriptorHeap(
            &descriptor_heap_desc,
            IID_PPV_ARGS(&target.depth_descriptor_heap)));
        dx_set_name(
            target.depth_descriptor_heap,
            dx_name(Demo::NAME, "Depth Target", "Descriptor Heap"));

        // Depth stencil view.
        target.depth_descriptor =
            target.depth_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        dx.device->CreateDepthStencilView(target.depth.get(), nullptr, target.depth_descriptor);
    }
}

static void init_descriptors(
    Dx& dx,
    Demo::Scene& scene,
    Demo::ShadowPass& shadow_pass,
    Demo::MainPass& main_pass) {
    // Descriptor heap.
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .NumDescriptors = 4,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            .NodeMask = 0,
        };
        FAIL_FAST_IF_FAILED(
            dx.device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&main_pass.descriptor_heap)));
        dx_set_name(
            main_pass.descriptor_heap,
            dx_name(Demo::NAME, Demo::MainPass::NAME, "Descriptor Heap"));

        auto increment_size = dx.device->GetDescriptorHandleIncrementSize(desc.Type);
        auto cpu_heap_start = main_pass.descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        auto gpu_heap_start = main_pass.descriptor_heap->GetGPUDescriptorHandleForHeapStart();

        main_pass.constants_cpu_descriptor = cpu_heap_start;
        main_pass.constants_gpu_descriptor = gpu_heap_start;
        cpu_heap_start.ptr += increment_size;
        gpu_heap_start.ptr += increment_size;

        scene.tree.texture_cpu_descriptor = cpu_heap_start;
        scene.tree.texture_gpu_descriptor = gpu_heap_start;
        cpu_heap_start.ptr += increment_size;
        gpu_heap_start.ptr += increment_size;

        scene.plane.texture_cpu_descriptor = cpu_heap_start;
        scene.plane.texture_gpu_descriptor = gpu_heap_start;
        cpu_heap_start.ptr += increment_size;
        gpu_heap_start.ptr += increment_size;

        shadow_pass.depth_srv_cpu_descriptor = cpu_heap_start;
        shadow_pass.depth_srv_gpu_descriptor = gpu_heap_start;
        cpu_heap_start.ptr += increment_size;
        gpu_heap_start.ptr += increment_size;
    }

    // Resource views.
    {
        // Constants.
        {
            auto cbv_desc = main_pass.constants.constant_buffer_view_desc();
            dx.device->CreateConstantBufferView(&cbv_desc, main_pass.constants_cpu_descriptor);
        }

        // Scene.
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
                scene.tree.texture_cpu_descriptor);
            dx.device->CreateShaderResourceView(
                scene.plane.texture.get(),
                &srv_desc,
                scene.plane.texture_cpu_descriptor);
        }

        // Shadow map.
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
                shadow_pass.depth_srv_cpu_descriptor);
        }
    }
}

Demo::Demo(Dx& dx) {
    init_scene(dx, scene);
    init_shadow_pass(dx, shadow_pass);
    init_main_pass(dx, main_pass);
    init_target(dx, target);
    init_descriptors(dx, scene, shadow_pass, main_pass);
}

void Demo::update(const UpdateParams& params) {
    static float light_projection_size = 15.0f;
    static float light_distance = 15.0f;
    static float shadow_near_plane = 0.1f;
    static float shadow_far_plane = 100.0f;
    static float camera_angle = PI;

    auto& shadow_constants = *shadow_pass.constants.data();
    auto& main_constants = *main_pass.constants.data();

    // ImGui.
    if (ImGui::Begin("Shadows")) {
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
        auto projection = Mat4x4::CreateOrthographic(
            light_projection_size,
            light_projection_size,
            shadow_near_plane,
            shadow_far_plane);
        auto eye = light_distance * main_constants.light_direction;
        auto at = Vec3(0.0f, 0.0f, 0.0f);
        auto up = Vec3(0.0f, 1.0f, 0.0f);
        auto view = Mat4x4::CreateLookAt(eye, at, up);

        shadow_constants.transform = view * projection;
    }

    // Main pass - constants.
    {
        auto fov = rad_from_deg(45.0f);
        auto aspect_ratio = params.aspect_ratio;
        auto projection = Mat4x4::CreatePerspectiveFieldOfView(fov, aspect_ratio, 0.1f, 100.0f);
        auto eye = Vec3(7.0f * std::sin(camera_angle), 8.0f, 7.0f * std::cos(camera_angle));
        auto at = Vec3(0.0f, 3.0f, 0.0f);
        auto up = Vec3(0.0f, 1.0f, 0.0f);
        auto view = Mat4x4::CreateLookAt(eye, at, up);

        main_constants.transform = view * projection;
        main_constants.light_transform = shadow_constants.transform;
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
        cmd->OMSetRenderTargets(0, nullptr, FALSE, &shadow_pass.depth_cpu_descriptor);
        cmd->ClearDepthStencilView(
            shadow_pass.depth_cpu_descriptor,
            D3D12_CLEAR_FLAG_DEPTH,
            1.0f,
            0,
            0,
            nullptr);
        cmd->SetGraphicsRootSignature(shadow_pass.root_signature.get());
        cmd->SetDescriptorHeaps(1, shadow_pass.constants_descriptor_heap.addressof());
        cmd->SetGraphicsRootConstantBufferView(
            0,
            shadow_pass.constants.resource->GetGPUVirtualAddress());
        cmd->SetPipelineState(shadow_pass.pipeline_state.get());
        cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        {
            auto vbv = scene.tree.vertex_buffer.vertex_buffer_view();
            auto ibv = scene.tree.index_buffer.index_buffer_view();
            auto index_count = scene.tree.index_buffer.element_size;
            cmd->IASetVertexBuffers(0, 1, &vbv);
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
        D3D12_VIEWPORT viewport = {
            .TopLeftX = 0.0f,
            .TopLeftY = 0.0f,
            .Width = (float)dx.swapchain_width,
            .Height = (float)dx.swapchain_height,
            .MinDepth = 0.0f,
            .MaxDepth = 1.0f,
        };
        D3D12_RECT scissor = {
            .left = 0,
            .top = 0,
            .right = (LONG)dx.swapchain_width,
            .bottom = (LONG)dx.swapchain_height,
        };

        auto* cmd = dx.command_list.get();
        dx.transition(
            target.color,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_RENDER_TARGET);
        cmd->RSSetViewports(1, &viewport);
        cmd->RSSetScissorRects(1, &scissor);
        cmd->OMSetRenderTargets(1, &target.color_descriptor, FALSE, &target.depth_descriptor);
        cmd->ClearRenderTargetView(target.color_descriptor, CLEAR_COLOR, 0, nullptr);
        cmd->ClearDepthStencilView(
            target.depth_descriptor,
            D3D12_CLEAR_FLAG_DEPTH,
            1.0f,
            0,
            0,
            nullptr);
        cmd->SetGraphicsRootSignature(main_pass.root_signature.get());
        cmd->SetDescriptorHeaps(1, main_pass.descriptor_heap.addressof());
        cmd->SetGraphicsRootConstantBufferView(
            0,
            main_pass.constants.resource->GetGPUVirtualAddress());
        cmd->SetGraphicsRootDescriptorTable(2, shadow_pass.depth_srv_gpu_descriptor);
        cmd->SetPipelineState(main_pass.pipeline_state.get());
        cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        {
            auto vbv = scene.tree.vertex_buffer.vertex_buffer_view();
            auto ibv = scene.tree.index_buffer.index_buffer_view();
            auto index_count = scene.tree.index_buffer.element_size;
            cmd->IASetVertexBuffers(0, 1, &vbv);
            cmd->IASetIndexBuffer(&ibv);
            cmd->SetGraphicsRootDescriptorTable(1, scene.tree.texture_gpu_descriptor);
            cmd->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
        }

        {
            auto vbv = scene.plane.vertex_buffer.vertex_buffer_view();
            auto ibv = scene.plane.index_buffer.index_buffer_view();
            auto index_count = scene.plane.index_buffer.element_size;
            cmd->IASetVertexBuffers(0, 1, &vbv);
            cmd->IASetIndexBuffer(&ibv);
            cmd->SetGraphicsRootDescriptorTable(1, scene.plane.texture_gpu_descriptor);
            cmd->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
        }

        dx.transition(
            target.color,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

}  // namespace fb::tree
