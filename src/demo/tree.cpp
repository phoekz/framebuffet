#include "tree.hpp"

#pragma warning(push)
#pragma warning(disable : 4100)

namespace fb::tree {

Demo::Scene::Model::Model(
    GpuDevice& device,
    const GltfModel& gltf_model,
    std::string_view model_name) {
    // Geometry.
    {
        vertex_buffer.create(
            device,
            gltf_model.vertex_count(),
            dx_name(Demo::NAME, model_name, "Vertex Buffer"));
        index_buffer.create(
            device,
            gltf_model.index_count(),
            dx_name(Demo::NAME, model_name, "Index Buffer"));
        memcpy(vertex_buffer.ptr(), gltf_model.vertex_data(), gltf_model.vertex_buffer_size());
        memcpy(index_buffer.ptr(), gltf_model.index_data(), gltf_model.index_buffer_size());
    }

    // Texture.
    {
        // Resource.
        const auto& gltf_texture = gltf_model.base_color_texture();
        texture.create(
            device,
            GpuTexture2dDesc {
                .format = gltf_texture.format(),
                .width = gltf_texture.width(),
                .height = gltf_texture.height(),
            },
            dx_name(Demo::NAME, model_name, "Texture"));

        // Upload.
        device.easy_upload(
            D3D12_SUBRESOURCE_DATA {
                .pData = gltf_texture.data(),
                .RowPitch = gltf_texture.row_pitch(),
                .SlicePitch = gltf_texture.slice_pitch()},
            texture.resource(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

Demo::Scene::Scene(GpuDevice& device) :
    tree(device, GltfModel("models/coconut_tree.glb"), "Tree"),
    plane(device, GltfModel("models/sand_plane.glb"), "Plane") {}

Demo::ShadowPass::ShadowPass(GpuDevice& device) {
    // Shaders.
    GpuShader vertex_shader;
    {
        GpuShaderCompiler sc;
        auto path = "shaders/tree.shadow.hlsl";
        auto source = read_whole_file(path);
        vertex_shader = sc.compile(
            dx_name(Demo::NAME, Demo::ShadowPass::NAME),
            GpuShaderType::Vertex,
            "vs_main",
            source);
    }

    // Pipeline state.
    pipeline_state = device.create_graphics_pipeline_state(
        D3D12_GRAPHICS_PIPELINE_STATE_DESC {
            .pRootSignature = device.root_signature(),
            .VS = vertex_shader.bytecode(),
            .BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            .SampleMask = UINT_MAX,
            .RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            .DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT),
            .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            .DSVFormat = DXGI_FORMAT_D32_FLOAT,
            .SampleDesc = {.Count = 1, .Quality = 0},
        },
        dx_name(Demo::NAME, Demo::ShadowPass::NAME, "Pipeline State"));

    // Constants.
    constants.create(device, 1, dx_name(Demo::NAME, Demo::ShadowPass::NAME, "Constants"));

    // Depth.
    depth.create(
        device,
        GpuTexture2dDesc {
            .format = DXGI_FORMAT_R32_TYPELESS,
            .width = SHADOW_MAP_SIZE,
            .height = SHADOW_MAP_SIZE,
            .clear_value =
                D3D12_CLEAR_VALUE {
                    .Format = DXGI_FORMAT_D32_FLOAT,
                    .DepthStencil = {1.0f, 0},
                },
            .srv_format = DXGI_FORMAT_R32_FLOAT,
            .dsv_format = DXGI_FORMAT_D32_FLOAT,
        },
        dx_name(Demo::NAME, Demo::ShadowPass::NAME, "Depth"));
}

Demo::MainPass::MainPass(GpuDevice& device) {
    // Shaders.
    GpuShader vertex_shader;
    GpuShader pixel_shader;
    {
        GpuShaderCompiler sc;
        auto path = "shaders/tree.main.hlsl";
        auto source = read_whole_file(path);
        auto name = dx_name(Demo::NAME, Demo::MainPass::NAME);
        vertex_shader = sc.compile(name, GpuShaderType::Vertex, "vs_main", source);
        pixel_shader = sc.compile(name, GpuShaderType::Pixel, "ps_main", source);
    }

    // Pipeline state.
    pipeline_state = device.create_graphics_pipeline_state(
        D3D12_GRAPHICS_PIPELINE_STATE_DESC {
            .pRootSignature = device.root_signature(),
            .VS = vertex_shader.bytecode(),
            .PS = pixel_shader.bytecode(),
            .BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            .SampleMask = UINT_MAX,
            .RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            .DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT),
            .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            .NumRenderTargets = 1,
            .RTVFormats = {DXGI_FORMAT_R8G8B8A8_UNORM},
            .DSVFormat = DXGI_FORMAT_D32_FLOAT,
            .SampleDesc = {.Count = 1, .Quality = 0}},
        dx_name(Demo::NAME, Demo::MainPass::NAME, "Pipeline State"));

    // Constants.
    constants.create(device, 1, dx_name(Demo::NAME, Demo::MainPass::NAME, "Constants"));
}

Demo::Demo(GpuDevice& device) :
    _render_targets(device, device.swapchain_size(), Demo::CLEAR_COLOR, Demo::NAME),
    _debug_draw(device, Demo::NAME),
    _scene(device),
    _shadow_pass(device),
    _main_pass(device) {}

auto Demo::update(const demo::UpdateDesc& desc) -> void {
    static float light_projection_size = 15.0f;
    static float light_lon = 0.0f;
    static float light_lat = rad_from_deg(30.0f);
    static float light_distance = 15.0f;
    static float shadow_near_plane = 0.1f;
    static float shadow_far_plane = 100.0f;
    static float camera_angle = PI;

    auto& shadow_constants = *_shadow_pass.constants.ptr();
    auto& main_constants = *_main_pass.constants.ptr();

    // Update light angle.
    {
        light_lon += desc.delta_time;
        if (light_lon > PI * 2.0f) {
            light_lon -= PI * 2.0f;
        }
    }

    // ImGui.
    if (ImGui::Begin(Demo::NAME.data())) {
        ImGui::SliderFloat("Ambient", &main_constants.ambient_light, 0.0f, 1.0f);
        ImGui::SliderFloat("Light Projection Size", &light_projection_size, 1.0f, 200.0f);
        ImGui::SliderAngle("Light Lon", &light_lon, 0.0f, 360.0f);
        ImGui::SliderAngle("Light Lat", &light_lat, 0.0f, 180.0f);
        ImGui::SliderFloat("Light Distance", &light_distance, 1.0f, 200.0f);
        ImGui::SliderFloat("Shadow Near Plane", &shadow_near_plane, 0.0f, 10.0f);
        ImGui::SliderFloat("Shadow Far Plane", &shadow_far_plane, 1.0f, 100.0f);
        ImGui::SliderFloat("Camera Angle", &camera_angle, 0.0f, 2.0f * PI);
    }
    ImGui::End();

    // Light direction.
    {
        auto x = std::cos(light_lon) * std::cos(light_lat);
        auto y = std::sin(light_lat);
        auto z = std::sin(light_lon) * std::cos(light_lat);
        main_constants.light_direction = Vector3(x, y, z);
    }

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
        constexpr auto FOV = rad_from_deg(45.0f);
        auto aspect_ratio = desc.aspect_ratio;
        auto projection = Matrix::CreatePerspectiveFieldOfView(FOV, aspect_ratio, 0.1f, 100.0f);
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
        _debug_draw.begin(desc.frame_index);
        _debug_draw.transform(transform);
        _debug_draw.axes();
        _debug_draw.line(Vector3(), 16.0f * main_constants.light_direction, COLOR_YELLOW);
        _debug_draw.end();
    }
}

auto Demo::render(GpuDevice& device, GpuCommandList& cmd) -> void {
    // Shadow pass.
    {
        cmd.set_graphics();
        _shadow_pass.depth.transition(cmd, D3D12_RESOURCE_STATE_DEPTH_WRITE);
        cmd.set_viewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
        cmd.set_scissor(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
        cmd.set_rtv_dsv(std::nullopt, _shadow_pass.depth.dsv_descriptor());
        cmd.clear_dsv(_shadow_pass.depth.dsv_descriptor(), 1.0f);
        cmd.set_graphics_constants({
            _shadow_pass.constants.cbv_descriptor().index(),
            _scene.tree.vertex_buffer.srv_descriptor().index(),
        });
        cmd.set_pipeline(_shadow_pass.pipeline_state);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(_scene.tree.index_buffer.index_buffer_view());
        cmd.draw_indexed_instanced(_scene.tree.index_buffer.element_size(), 1, 0, 0, 0);
        _shadow_pass.depth.transition(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }

    // Main pass.
    {
        cmd.set_graphics();
        _render_targets.begin(device, cmd);
        _debug_draw.render(device, cmd);
        cmd.set_pipeline(_main_pass.pipeline_state);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        cmd.set_graphics_constants({
            _main_pass.constants.cbv_descriptor().index(),
            _scene.tree.vertex_buffer.srv_descriptor().index(),
            _scene.tree.texture.srv_descriptor().index(),
            _shadow_pass.depth.srv_descriptor().index(),
        });
        cmd.set_index_buffer(_scene.tree.index_buffer.index_buffer_view());
        cmd.draw_indexed_instanced(_scene.tree.index_buffer.element_size(), 1, 0, 0, 0);

        cmd.set_graphics_constants({
            _main_pass.constants.cbv_descriptor().index(),
            _scene.plane.vertex_buffer.srv_descriptor().index(),
            _scene.plane.texture.srv_descriptor().index(),
            _shadow_pass.depth.srv_descriptor().index(),
        });
        cmd.set_index_buffer(_scene.plane.index_buffer.index_buffer_view());
        cmd.draw_indexed_instanced(_scene.plane.index_buffer.element_size(), 1, 0, 0, 0);

        _render_targets.end(device, cmd);
    }
}

}  // namespace fb::tree
