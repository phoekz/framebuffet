#include "saber.hpp"

namespace fb::demos::saber {

auto create(Demo& demo, const CreateDesc& desc) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Create", NAME.data());
    DebugScope debug(NAME);

    // Unpack.
    const auto& kitchen_shaders = desc.baked.kitchen.shaders;
    const auto& shaders = desc.baked.buffet.shaders;
    const auto& assets = desc.baked.buffet.assets;
    auto& device = desc.device;

    // Constants.
    demo.constants.create(device, 1, debug.with_name("Constants"));

    // Scene pass.
    {
        DebugScope pass_debug("Scene");

        auto& scene = demo.scene;

        // Render targets.
        scene.render_targets.create(
            device,
            {
                .size = device.swapchain().size(),
                .color_format = COLOR_FORMAT,
                .color_clear_value = COLOR_CLEAR_VALUE,
                .depth_format = DEPTH_FORMAT,
                .depth_clear_value = DEPTH_CLEAR_VALUE,
                .sample_count = SAMPLE_COUNT,
            }
        );

        // Debug draw.
        scene.debug_draw.create(device, kitchen_shaders, scene.render_targets);

        // Pipeline.
        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(shaders.saber_scene_vs())
            .pixel_shader(shaders.saber_scene_ps())
            .render_target_formats({scene.render_targets.color_format()})
            .depth_stencil_format(scene.render_targets.depth_format())
            .sample_desc(scene.render_targets.sample_desc())
            .build(device, scene.pipeline, pass_debug.with_name("Pipeline"));

        // Geometry.
        const auto mesh = assets.lightsaber_mesh();
        scene.vertices.create_with_data(device, mesh.vertices, pass_debug.with_name("Vertices"));
        scene.indices.create_with_data(device, mesh.indices, pass_debug.with_name("Indices"));
    }

    // Compute pass.
    {
        DebugScope pass_debug("Compute");

        auto& compute = demo.compute;

        // Pipeline.
        GpuPipelineBuilder()
            .compute_shader(shaders.saber_threshold_cs())
            .build(device, compute.threshold_pipeline, debug.with_name("Threshold Pipeline"));
        GpuPipelineBuilder()
            .compute_shader(shaders.saber_downsample_cs())
            .build(device, compute.downsample_pipeline, debug.with_name("Downsample Pipeline"));
        GpuPipelineBuilder()
            .compute_shader(shaders.saber_upsample_cs())
            .build(device, compute.upsample_pipeline, debug.with_name("Upsample Pipeline"));

        // Sampling chains.
        const auto size = device.swapchain().size();
        const auto mip_count = mip_count_from_size(size);
        compute.downsample.create(
            device,
            GpuTextureDesc {
                .format = COLOR_FORMAT,
                .width = size.x,
                .height = size.y,
                .mip_count = mip_count,
            },
            pass_debug.with_name("Downsampling")
        );
        compute.upsample.create(
            device,
            GpuTextureDesc {
                .format = COLOR_FORMAT,
                .width = size.x,
                .height = size.y,
                .mip_count = mip_count,
            },
            pass_debug.with_name("Upsampling")
        );
    }

    // Blit pass.
    {
        DebugScope pass_debug("Blit");

        auto& blit = demo.blit;

        // Render targets.
        blit.render_targets.create(
            device,
            {
                .size = device.swapchain().size(),
                .color_format = COLOR_FORMAT,
                .color_clear_value = COLOR_CLEAR_VALUE,
            }
        );

        // Pipeline.
        GpuPipelineBuilder()
            .vertex_shader(shaders.saber_blit_vs())
            .pixel_shader(shaders.saber_blit_ps())
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .depth_stencil(GpuDepthStencilDesc {
                .depth_read = false,
                .depth_write = false,
            })
            .render_target_formats({blit.render_targets.color_format()})
            .build(device, blit.pipeline, debug.with_name("Pipeline"));
    }
}

auto gui(Demo& demo, const GuiDesc&) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Gui", NAME.data());
    auto& params = demo.parameters;
    ImGui::SliderFloat("Camera Distance", &params.camera_distance, 0.125f, 10.0f);
    float* saber_color = &params.saber_color_and_intensity.x;
    float* saber_intensity = &params.saber_color_and_intensity.w;
    ImGui::ColorEdit3("Saber Color", saber_color);
    ImGui::SliderFloat("Saber Intensity", saber_intensity, 1.0f, 16.0f);
}

auto update(Demo& demo, const UpdateDesc& desc) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Update", NAME.data());
    auto& params = demo.parameters;

    // Update camera.
    const auto projection =
        float4x4::CreatePerspectiveFieldOfView(params.camera_fov, desc.aspect_ratio, 0.1f, 100.0f);
    const auto eye =
        params.camera_distance * dir_from_lonlat(params.camera_longitude, params.camera_latitude);
    const auto view = float4x4::CreateLookAt(eye, float3::Zero, float3::Up);
    const auto camera_transform = view * projection;

    // Update saber.
    const auto amplitude = std::cos(2.0f * desc.elapsed_time) * 0.5f + 0.5f;
    params.saber_color_and_intensity.w = 1.0f + 16.0f * amplitude;

    // Update debug draw.
    demo.scene.debug_draw.begin(desc.frame_index);
    demo.scene.debug_draw.transform(camera_transform);
    demo.scene.debug_draw.axes();
    demo.scene.debug_draw.end();

    // Update constants.
    demo.constants.buffer(desc.frame_index).ref() = Constants {
        .transform = camera_transform,
        .saber_color_and_intensity = params.saber_color_and_intensity,
    };
}

auto render(Demo& demo, const RenderDesc& desc) -> void {
    auto& [cmd, device, frame_index] = desc;
    cmd.begin_pix("%s - Render", NAME.data());

    // Scene pass.
    cmd.graphics_scope([&demo, frame_index](GpuGraphicsCommandList& cmd) {
        auto& scene = demo.scene;

        cmd.begin_pix("Scene");

        scene.render_targets.set(cmd);
        scene.debug_draw.render(cmd);
        cmd.set_pipeline(scene.pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(scene.indices.index_buffer_view());
        cmd.set_constants(SceneBindings {
            .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
            .vertices = scene.vertices.srv_descriptor().index(),
        });
        cmd.draw_indexed_instanced(scene.indices.element_count(), 1, 0, 0, 0);

        scene.render_targets.transition_to_resolve(cmd);
        cmd.flush_barriers();

        scene.render_targets.resolve(cmd);

        scene.render_targets.transition_to_shader_resource(cmd);
        cmd.flush_barriers();

        cmd.end_pix();
    });

    // Compute pass.
    cmd.compute_scope([&demo, frame_index](GpuComputeCommandList& cmd) {
        const auto& scene = demo.scene;
        auto& compute = demo.compute;

        cmd.begin_pix("Compute");

        compute.downsample.transition(
            cmd,
            D3D12_BARRIER_SYNC_COMPUTE_SHADING,
            D3D12_BARRIER_ACCESS_UNORDERED_ACCESS,
            D3D12_BARRIER_LAYOUT_UNORDERED_ACCESS
        );
        compute.upsample.transition(
            cmd,
            D3D12_BARRIER_SYNC_COMPUTE_SHADING,
            D3D12_BARRIER_ACCESS_UNORDERED_ACCESS,
            D3D12_BARRIER_LAYOUT_UNORDERED_ACCESS
        );
        cmd.flush_barriers();

        // Common.
        const auto size = scene.render_targets.color().size();
        const auto mip_count = mip_count_from_size(size);

        // Thresholding.
        cmd.begin_pix("Thresholding");
        cmd.set_pipeline(compute.threshold_pipeline);
        cmd.set_constants(ThresholdBindings {
            .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
            .scene_texture = scene.render_targets.color().srv_descriptor().index(),
            .downsample_texture = compute.downsample.uav_descriptor().index(),
            .texture_width = size.x,
            .texture_height = size.y,
        });
        cmd.dispatch(
            (size.x + (DISPATCH_X - 1)) / DISPATCH_X,
            (size.y + (DISPATCH_Y - 1)) / DISPATCH_Y,
            1
        );
        cmd.end_pix();

        // Downsampling.
        cmd.begin_pix("Downsampling");
        cmd.set_pipeline(compute.downsample_pipeline);
        for (uint mip = 0; mip < mip_count - 1; mip++) {
            const auto src_level = mip;
            const auto src_width = std::max(1u, size.x >> src_level);
            const auto src_height = std::max(1u, size.y >> src_level);
            const auto dst_level = mip + 1;
            const auto dst_width = std::max(1u, size.x >> dst_level);
            const auto dst_height = std::max(1u, size.y >> dst_level);
            cmd.set_constants(DownsampleBindings {
                .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
                .src_texture = compute.downsample.srv_descriptor().index(),
                .src_texture_level = src_level,
                .src_texture_width = src_width,
                .src_texture_height = src_height,
                .dst_texture = compute.downsample.uav_descriptor().index(),
                .dst_texture_level = dst_level,
                .dst_texture_width = dst_width,
                .dst_texture_height = dst_height,
            });
            cmd.dispatch(
                (dst_width + (DISPATCH_X - 1)) / DISPATCH_X,
                (dst_height + (DISPATCH_Y - 1)) / DISPATCH_Y,
                1
            );
        }
        cmd.end_pix();

        // Upsampling.
        cmd.begin_pix("Upsampling");
        cmd.set_pipeline(compute.upsample_pipeline);
        for (uint mip_inv = 0; mip_inv < mip_count - 1; mip_inv++) {
            const auto mip = mip_count - 1 - mip_inv;
            const auto src_level = mip;
            const auto src_width = std::max(1u, size.x >> src_level);
            const auto src_height = std::max(1u, size.y >> src_level);
            const auto dst_level = mip - 1;
            const auto dst_width = std::max(1u, size.x >> dst_level);
            const auto dst_height = std::max(1u, size.y >> dst_level);
            cmd.set_constants(UpsampleBindings {
                .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
                .src_dn_texture = compute.downsample.srv_descriptor().index(),
                .src_up_texture = compute.upsample.srv_descriptor().index(),
                .src_texture_level = src_level,
                .src_texture_width = src_width,
                .src_texture_height = src_height,
                .dst_texture = compute.upsample.uav_descriptor().index(),
                .dst_texture_level = dst_level,
                .dst_texture_width = dst_width,
                .dst_texture_height = dst_height,
            });
            cmd.dispatch(
                (dst_width + (DISPATCH_X - 1)) / DISPATCH_X,
                (dst_height + (DISPATCH_Y - 1)) / DISPATCH_Y,
                1
            );
        }
        cmd.end_pix();

        cmd.end_pix();
    });

    // Blit pass.
    cmd.graphics_scope([&demo, frame_index](GpuGraphicsCommandList& cmd) {
        const auto& scene = demo.scene;
        auto& compute = demo.compute;
        auto& blit = demo.blit;

        compute.upsample.transition(
            cmd,
            D3D12_BARRIER_SYNC_PIXEL_SHADING,
            D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
            D3D12_BARRIER_LAYOUT_SHADER_RESOURCE
        );
        blit.render_targets.transition_to_render_target(cmd);
        cmd.flush_barriers();

        cmd.begin_pix("Blit");

        blit.render_targets.set(cmd);
        cmd.set_pipeline(blit.pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_constants(BlitBindings {
            .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
            .scene_texture = scene.render_targets.color().srv_descriptor().index(),
            .bloom_texture = compute.upsample.srv_descriptor().index(),
        });
        cmd.draw_instanced(3, 1, 0, 0);

        cmd.end_pix();
    });

    cmd.end_pix();
}

} // namespace fb::demos::saber
