#include "tree.hpp"

namespace fb::demos::tree {

auto create(Demo& demo, const CreateDesc& desc) -> void {
    FB_PERF_FUNC();
    DebugScope debug(NAME);

    // Unpack.
    const auto& kitchen_shaders = desc.baked.kitchen.shaders;
    const auto& shaders = desc.baked.buffet.shaders;
    const auto& assets = desc.baked.buffet.assets;
    auto& device = desc.device;

    // Render targets.
    demo.render_target.create(
        device,
        {
            .size = device.swapchain().size(),
            .sample_count = SAMPLE_COUNT,
            .colors = COLOR_ATTACHMENTS,
            .depth_stencil = DEPTH_STENCIL_ATTACHMENT,
        }
    );
    demo.render_target_view.create(demo.render_target.view_desc());

    // Debug draw.
    demo.debug_draw.create(device, kitchen_shaders, demo.render_target_view);

    // Constants.
    demo.constants.create(device, 1, debug.with_name("Constants"));

    // Geometry.
    const auto tree = assets.coconut_tree_mesh();
    const auto sand = assets.sand_mesh();
    demo.tree_vertices.create_and_transfer(
        device,
        tree.vertices,
        D3D12_BARRIER_SYNC_VERTEX_SHADING,
        D3D12_BARRIER_ACCESS_VERTEX_BUFFER,
        debug.with_name("Tree Vertices")
    );
    demo.tree_indices.create_and_transfer(
        device,
        tree.indices,
        D3D12_BARRIER_SYNC_INDEX_INPUT,
        D3D12_BARRIER_ACCESS_INDEX_BUFFER,
        debug.with_name("Tree Indices")
    );
    demo.sand_vertices.create_and_transfer(
        device,
        sand.vertices,
        D3D12_BARRIER_SYNC_VERTEX_SHADING,
        D3D12_BARRIER_ACCESS_VERTEX_BUFFER,
        debug.with_name("Sand Vertices")
    );
    demo.sand_indices.create_and_transfer(
        device,
        sand.indices,
        D3D12_BARRIER_SYNC_INDEX_INPUT,
        D3D12_BARRIER_ACCESS_INDEX_BUFFER,
        debug.with_name("Sand Indices")
    );

    // Texture.
    demo.tree_texture.create_and_transfer_baked(
        device,
        assets.coconut_tree_base_color_texture(),
        D3D12_BARRIER_SYNC_PIXEL_SHADING,
        D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
        D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE,
        debug.with_name("Tree Texture")
    );
    demo.sand_texture.create_and_transfer_baked(
        device,
        assets.sand_texture(),
        D3D12_BARRIER_SYNC_PIXEL_SHADING,
        D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
        D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE,
        debug.with_name("Sand Texture")
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
        .rasterizer(GpuRasterizerDesc {
            .fill_mode = GpuFillMode::Solid,
            .cull_mode = GpuCullMode::Back,
        })
        .render_target_formats({demo.render_target.color_format(0)})
        .depth_stencil_format(demo.render_target.depth_format())
        .sample_desc(demo.render_target.sample_desc())
        .build(device, demo.draw_pipeline, debug.with_name("Draw Pipeline"));
}

auto gui(Demo& demo, const GuiDesc&) -> void {
    FB_PERF_FUNC();
    auto& params = demo.parameters;
    ImGui::SliderFloat("Ambient Light", &params.ambient_light, 0.0f, 1.0f);
    ImGui::SliderFloat("Light Projection Size", &params.light_projection_size, 1.0f, 200.0f);
    ImGui::SliderAngle("Light Longitude", &params.light_longitude, 0.0f, 360.0f);
    ImGui::SliderAngle("Light Latitude", &params.light_latitude, 0.0f, 180.0f);
    ImGui::SliderFloat("Light Distance", &params.light_distance, 1.0f, 200.0f);
    ImGui::SliderFloat("Light Speed", &params.light_speed, 0.0f, 1.0f);
    ImGui::SliderFloat("Shadow Near Plane", &params.shadow_near_plane, 0.0f, 10.0f);
    ImGui::SliderFloat("Shadow Far Plane", &params.shadow_far_plane, 1.0f, 100.0f);
    ImGui::SliderFloat("Camera Distance", &params.camera_distance, 0.0f, 10.0f);
    ImGui::SliderAngle("Camera FOV", &params.camera_fov, 0.0f, 90.0f);
    ImGui::SliderAngle("Camera Latitude", &params.camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera Longitude", &params.camera_longitude, 0.0f, 360.0f);
}

auto update(Demo& demo, const UpdateDesc& desc) -> void {
    FB_PERF_FUNC();
    auto& params = demo.parameters;

    // Update light angle.
    {
        params.light_longitude += params.light_speed * desc.delta_time;
        if (params.light_longitude > FLOAT_PI * 2.0f) {
            params.light_longitude -= FLOAT_PI * 2.0f;
        }
    }

    // Light direction.
    auto light_direction = float3_from_lonlat(params.light_longitude, params.light_latitude);

    // Shadow pass - constants.
    float4x4 light_transform;
    {
        auto projection = float4x4_orthographic(
            -0.5f * params.light_projection_size,
            0.5f * params.light_projection_size,
            -0.5f * params.light_projection_size,
            0.5f * params.light_projection_size,
            params.shadow_near_plane,
            params.shadow_far_plane
        );
        auto eye = params.light_distance * light_direction;
        auto view = float4x4_lookat(eye, FLOAT3_ZERO, FLOAT3_UP);

        light_transform = projection * view;
    }

    // Main pass - constants.
    float4x4 transform;
    {
        auto projection = float4x4_perspective(params.camera_fov, desc.aspect_ratio, 0.1f, 100.0f);
        auto eye = params.camera_distance
            * float3_from_lonlat(params.camera_longitude, params.camera_latitude);
        auto at = float3(0.0f, 3.0f, 0.0f);
        auto view = float4x4_lookat(eye, at, FLOAT3_UP);
        transform = projection * view;
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
    FB_PERF_FUNC();
    auto& [cmd, device, frame_index] = desc;
    cmd.graphics_scope([&demo, frame_index](GpuGraphicsCommandList& cmd) {
        cmd.pix_begin("%s - Render", NAME.data());

        {
            cmd.pix_begin("Shadow");
            demo.shadow_depth.transition(
                cmd,
                D3D12_BARRIER_SYNC_DEPTH_STENCIL,
                D3D12_BARRIER_ACCESS_DEPTH_STENCIL_WRITE,
                D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_WRITE
            );
            cmd.flush_barriers();
            cmd.set_viewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
            cmd.set_scissor(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
            cmd.set_rtvs_dsv({}, demo.shadow_depth.dsv_descriptor());
            cmd.clear_dsv(demo.shadow_depth.dsv_descriptor(), 1.0f, 0);
            cmd.set_constants(Bindings {
                demo.constants.buffer(frame_index).cbv_descriptor().index(),
                demo.tree_vertices.srv_descriptor().index(),
            });
            cmd.set_pipeline(demo.shadow_pipeline);
            cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            cmd.set_index_buffer(demo.tree_indices.index_buffer_view());
            cmd.draw_indexed_instanced(demo.tree_indices.element_count(), 1, 0, 0, 0);
            demo.shadow_depth.transition(
                cmd,
                D3D12_BARRIER_SYNC_PIXEL_SHADING,
                D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
                D3D12_BARRIER_LAYOUT_SHADER_RESOURCE
            );
            cmd.flush_barriers();
            cmd.pix_end();
        }

        {
            cmd.pix_begin("Draw");
            demo.render_target_view.set_graphics(cmd);
            demo.debug_draw.render(cmd);
            cmd.set_pipeline(demo.draw_pipeline);
            cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            cmd.set_constants(Bindings {
                demo.constants.buffer(frame_index).cbv_descriptor().index(),
                demo.tree_vertices.srv_descriptor().index(),
                demo.tree_texture.srv_descriptor().index(),
                demo.shadow_depth.srv_descriptor().index(),
            });
            cmd.set_index_buffer(demo.tree_indices.index_buffer_view());
            cmd.draw_indexed_instanced(demo.tree_indices.element_count(), 1, 0, 0, 0);

            cmd.set_constants(Bindings {
                demo.constants.buffer(frame_index).cbv_descriptor().index(),
                demo.sand_vertices.srv_descriptor().index(),
                demo.sand_texture.srv_descriptor().index(),
                demo.shadow_depth.srv_descriptor().index(),
            });
            cmd.set_index_buffer(demo.sand_indices.index_buffer_view());
            cmd.draw_indexed_instanced(demo.sand_indices.element_count(), 1, 0, 0, 0);

            cmd.pix_end();
        }

        cmd.pix_end();
    });
}

} // namespace fb::demos::tree
