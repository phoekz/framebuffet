#include "tree.hpp"

namespace fb::demos::tree {

auto create(Demo& demo, const CreateDesc& desc) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Create", NAME.data());
    DebugScope debug(NAME);

    // Unpack.
    const auto& kitchen_shaders = desc.baked.kitchen.shaders;
    const auto& shaders = desc.baked.buffet.shaders;
    const auto& assets = desc.baked.buffet.assets;
    auto& device = desc.device;

    // Render targets.
    demo.render_targets.create(
        device,
        {
            .size = device.swapchain().size(),
            .color_format = COLOR_FORMAT,
            .clear_color = CLEAR_COLOR,
            .sample_count = SAMPLE_COUNT,
        }
    );

    // Debug draw.
    demo.debug_draw.create(device, kitchen_shaders, demo.render_targets);

    // Constants.
    demo.constants.create(device, 1, debug.with_name("Constants"));

    // Geometry.
    const auto tree = assets.coconut_tree_mesh();
    const auto plane = assets.sand_plane_mesh();
    demo.tree_vertices.create_with_data(device, tree.vertices, debug.with_name("Tree Vertices"));
    demo.tree_indices.create_with_data(device, tree.indices, debug.with_name("Tree Indices"));
    demo.plane_vertices.create_with_data(device, plane.vertices, debug.with_name("Plane Vertices"));
    demo.plane_indices.create_with_data(device, plane.indices, debug.with_name("Plane Indices"));

    // Texture.
    demo.tree_texture.create_and_transfer_baked(
        device,
        assets.coconut_tree_base_color_texture(),
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        debug.with_name("Tree Texture")
    );
    demo.plane_texture.create_and_transfer_baked(
        device,
        assets.sand_plane_base_color_texture(),
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        debug.with_name("Plane Texture")
    );

    // Depth.
    constexpr DXGI_FORMAT DEPTH_RAW_FORMAT = DXGI_FORMAT_R32_TYPELESS;
    constexpr DXGI_FORMAT DEPTH_SRV_FORMAT = DXGI_FORMAT_R32_FLOAT;
    constexpr DXGI_FORMAT DEPTH_DSV_FORMAT = DXGI_FORMAT_D32_FLOAT;
    demo.shadow_depth.create(
        device,
        GpuTextureDesc {
            .format = DEPTH_RAW_FORMAT,
            .width = SHADOW_MAP_SIZE,
            .height = SHADOW_MAP_SIZE,
            .clear_value =
                D3D12_CLEAR_VALUE {
                    .Format = DEPTH_DSV_FORMAT,
                    .DepthStencil = {1.0f, 0},
                },
            .srv_format = DEPTH_SRV_FORMAT,
            .dsv_format = DEPTH_DSV_FORMAT,
        },
        debug.with_name("Shadow Depth")
    );

    // Pipelines.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.tree_shadow_vs())
        .depth_stencil_format(DEPTH_DSV_FORMAT)
        .build(device, demo.shadow_pipeline, debug.with_name("Shadow Pipeline"));
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.tree_draw_vs())
        .pixel_shader(shaders.tree_draw_ps())
        .render_target_formats({demo.render_targets.color_format()})
        .depth_stencil_format(demo.render_targets.depth_format())
        .sample_desc(demo.render_targets.sample_desc())
        .build(device, demo.draw_pipeline, debug.with_name("Draw Pipeline"));
}

auto gui(Demo& demo, const GuiDesc&) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Gui", NAME.data());
    auto& params = demo.parameters;
    ImGui::SliderFloat("Ambient Light", &params.ambient_light, 0.0f, 1.0f);
    ImGui::SliderFloat("Light Projection Size", &params.light_projection_size, 1.0f, 200.0f);
    ImGui::SliderAngle("Light Longitude", &params.light_longitude, 0.0f, 360.0f);
    ImGui::SliderAngle("Light Latitude", &params.light_latitude, 0.0f, 180.0f);
    ImGui::SliderFloat("Light Distance", &params.light_distance, 1.0f, 200.0f);
    ImGui::SliderFloat("Shadow Near Plane", &params.shadow_near_plane, 0.0f, 10.0f);
    ImGui::SliderFloat("Shadow Far Plane", &params.shadow_far_plane, 1.0f, 100.0f);
    ImGui::SliderFloat("Camera Distance", &params.camera_distance, 0.0f, 10.0f);
    ImGui::SliderAngle("Camera FOV", &params.camera_fov, 0.0f, 90.0f);
    ImGui::SliderAngle("Camera Latitude", &params.camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera Longitude", &params.camera_longitude, 0.0f, 360.0f);
}

auto update(Demo& demo, const UpdateDesc& desc) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Update", NAME.data());
    auto& params = demo.parameters;

    // Update light angle.
    {
        params.light_longitude += desc.delta_time;
        if (params.light_longitude > PI * 2.0f) {
            params.light_longitude -= PI * 2.0f;
        }
    }

    // Light direction.
    auto light_direction = dir_from_lonlat(params.light_longitude, params.light_latitude);

    // Shadow pass - constants.
    float4x4 light_transform;
    {
        auto projection = float4x4::CreateOrthographic(
            params.light_projection_size,
            params.light_projection_size,
            params.shadow_near_plane,
            params.shadow_far_plane
        );
        auto eye = params.light_distance * light_direction;
        auto view = float4x4::CreateLookAt(eye, float3::Zero, float3::Up);

        light_transform = view * projection;
    }

    // Main pass - constants.
    float4x4 transform;
    {
        auto projection = float4x4::CreatePerspectiveFieldOfView(
            params.camera_fov,
            desc.aspect_ratio,
            0.1f,
            100.0f
        );
        auto eye = params.camera_distance
            * dir_from_lonlat(params.camera_longitude, params.camera_latitude);
        auto at = float3(0.0f, 3.0f, 0.0f);
        auto view = float4x4::CreateLookAt(eye, at, float3::Up);
        transform = view * projection;
    }

    // Debug.
    {
        demo.debug_draw.begin(desc.frame_index);
        demo.debug_draw.transform(transform);
        demo.debug_draw.axes();
        demo.debug_draw.line(float3(), 16.0f * light_direction, COLOR_YELLOW);
        demo.debug_draw.end();
    }

    // Update constants.
    demo.constants.buffer(desc.frame_index).ref() = Constants {
        .transform = transform,
        .light_transform = light_transform,
        .light_direction = light_direction,
        .ambient_light = params.ambient_light,
    };
}

auto render(Demo& demo, const RenderDesc& desc) -> void {
    auto& [cmd, device, frame_index] = desc;
    cmd.begin_pix("%s - Render", NAME.data());

    {
        cmd.begin_pix("Shadow");
        cmd.set_graphics();
        demo.shadow_depth.transition(cmd, D3D12_RESOURCE_STATE_DEPTH_WRITE);
        cmd.flush_barriers();
        cmd.set_viewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
        cmd.set_scissor(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
        cmd.set_rtv_dsv(std::nullopt, demo.shadow_depth.dsv_descriptor());
        cmd.clear_dsv(demo.shadow_depth.dsv_descriptor(), 1.0f);
        cmd.set_graphics_constants(Bindings {
            demo.constants.buffer(frame_index).cbv_descriptor().index(),
            demo.tree_vertices.srv_descriptor().index(),
        });
        cmd.set_pipeline(demo.shadow_pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(demo.tree_indices.index_buffer_view());
        cmd.draw_indexed_instanced(demo.tree_indices.element_count(), 1, 0, 0, 0);
        demo.shadow_depth.transition(
            cmd,
            D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );
        cmd.flush_barriers();
        cmd.end_pix();
    }

    {
        cmd.begin_pix("Draw");
        cmd.set_graphics();
        demo.render_targets.set(cmd);
        demo.debug_draw.render(cmd);
        cmd.set_pipeline(demo.draw_pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        cmd.set_graphics_constants(Bindings {
            demo.constants.buffer(frame_index).cbv_descriptor().index(),
            demo.tree_vertices.srv_descriptor().index(),
            demo.tree_texture.srv_descriptor().index(),
            demo.shadow_depth.srv_descriptor().index(),
        });
        cmd.set_index_buffer(demo.tree_indices.index_buffer_view());
        cmd.draw_indexed_instanced(demo.tree_indices.element_count(), 1, 0, 0, 0);

        cmd.set_graphics_constants(Bindings {
            demo.constants.buffer(frame_index).cbv_descriptor().index(),
            demo.plane_vertices.srv_descriptor().index(),
            demo.plane_texture.srv_descriptor().index(),
            demo.shadow_depth.srv_descriptor().index(),
        });
        cmd.set_index_buffer(demo.plane_indices.index_buffer_view());
        cmd.draw_indexed_instanced(demo.plane_indices.element_count(), 1, 0, 0, 0);
        cmd.end_pix();
    }

    cmd.end_pix();
}

} // namespace fb::demos::tree
