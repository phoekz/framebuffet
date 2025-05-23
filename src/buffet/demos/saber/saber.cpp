#include "saber.hpp"

namespace fb::demos::saber {

auto create(Demo& demo, const CreateDesc& desc) -> void {
    FB_PERF_FUNC();
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
        scene.render_target.create(
            device,
            {
                .size = device.swapchain().size(),
                .sample_count = SAMPLE_COUNT,
                .colors = COLOR_ATTACHMENTS,
                .depth_stencil = DEPTH_STENCIL_ATTACHMENT,
            }
        );
        scene.render_target_view.create(scene.render_target.view_desc());

        // Debug draw.
        scene.debug_draw.create(device, kitchen_shaders, scene.render_target_view);

        // Pipeline.
        GpuPipelineBuilder()
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .vertex_shader(shaders.saber_scene_vs())
            .pixel_shader(shaders.saber_scene_ps())
            .render_target_formats({scene.render_target.color_format(0)})
            .depth_stencil_format(scene.render_target.depth_format())
            .sample_desc(scene.render_target.sample_desc())
            .build(device, scene.pipeline, pass_debug.with_name("Pipeline"));

        // Geometry.
        const auto mesh = assets.lightsaber_mesh();
        scene.vertices.create_and_transfer(
            device,
            mesh.vertices,
            D3D12_BARRIER_SYNC_VERTEX_SHADING,
            D3D12_BARRIER_ACCESS_VERTEX_BUFFER,
            pass_debug.with_name("Vertices")
        );
        scene.indices.create_and_transfer(
            device,
            mesh.indices,
            D3D12_BARRIER_SYNC_INDEX_INPUT,
            D3D12_BARRIER_ACCESS_INDEX_BUFFER,
            pass_debug.with_name("Indices")
        );

        // Instances.
        const auto lhs_translation = float4x4_translation(0.0f, 0.0f, -0.175f);
        const auto rhs_translation = float4x4_translation(0.0f, 0.0f, 0.175f);
        const auto lhs_rotation = float4x4_rotation_x(rad_from_deg(-20.0f));
        const auto rhs_rotation = float4x4_rotation_x(rad_from_deg(20.0f));
        const auto instances = std::to_array({
            SceneInstance {.transform = lhs_rotation * lhs_translation},
            SceneInstance {.transform = rhs_rotation * rhs_translation},
        });
        scene.instances.create_and_transfer(
            device,
            instances,
            D3D12_BARRIER_SYNC_VERTEX_SHADING,
            D3D12_BARRIER_ACCESS_VERTEX_BUFFER,
            pass_debug.with_name("Instances")
        );
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
                .format = COLOR_ATTACHMENTS[0].format,
                .width = size.x,
                .height = size.y,
                .mip_count = mip_count,
            },
            pass_debug.with_name("Downsampling")
        );
        compute.upsample.create(
            device,
            GpuTextureDesc {
                .format = COLOR_ATTACHMENTS[0].format,
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
        blit.render_target.create(
            device,
            {
                .size = device.swapchain().size(),
                .colors = {KcnColorAttachmentDesc {.format = COLOR_ATTACHMENTS[0].format}},
            }
        );
        blit.render_target_view.create(blit.render_target.view_desc());

        // Pipeline.
        GpuPipelineBuilder()
            .vertex_shader(shaders.saber_blit_vs())
            .pixel_shader(shaders.saber_blit_ps())
            .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .depth_stencil(GpuDepthStencilDesc {
                .depth_read = false,
                .depth_write = false,
            })
            .render_target_formats({blit.render_target.color_format(0)})
            .build(device, blit.pipeline, debug.with_name("Pipeline"));
    }
}

auto gui(Demo& demo, const GuiDesc&) -> void {
    FB_PERF_FUNC();
    auto& params = demo.parameters;
    if (ImGui::Button("Defaults")) {
        params = {};
    }
    float* saber_color = &params.saber_color_and_intensity.x;
    float* saber_intensity = &params.saber_color_and_intensity.w;
    ImGui::ColorEdit3("Saber Color", saber_color);
    ImGui::Checkbox("Animated", (bool*)&params.animated);
    ImGui::Checkbox("Tonemap", (bool*)&params.tonemap);
    ImGui::SliderFloat("Saber Intensity", saber_intensity, 1.0f, MAX_SABER_INTENSITY);
    ImGui::SliderFloat("Camera Distance", &params.camera_distance, 0.125f, 10.0f);
}

auto update(Demo& demo, const UpdateDesc& desc) -> void {
    FB_PERF_FUNC();
    auto& params = demo.parameters;

    // Update camera.
    const auto projection =
        float4x4_perspective(params.camera_fov, desc.aspect_ratio, 0.1f, 100.0f);
    const auto eye = params.camera_distance
        * float3_from_lonlat(params.camera_longitude, params.camera_latitude);
    const auto view = float4x4_lookat(eye, FLOAT3_ZERO, FLOAT3_UP);
    const auto camera_transform = projection * view;

    // Update saber.
    {
        // Clamp to always have some contribution.
        static constexpr auto MIN_COLOR = 1.0f / 256.0f;
        float* saber_color = &params.saber_color_and_intensity.x;
        saber_color[0] = std::max(MIN_COLOR, saber_color[0]);
        saber_color[1] = std::max(MIN_COLOR, saber_color[1]);
        saber_color[2] = std::max(MIN_COLOR, saber_color[2]);

        // Animate.
        if (params.animated) {
            const auto amplitude = std::cos(1.0f * desc.elapsed_time) * 0.5f + 0.5f;
            params.saber_color_and_intensity.w = 1.0f + MAX_SABER_INTENSITY * amplitude;
        }
    }
    // Update debug draw.
    demo.scene.debug_draw.begin(desc.frame_index);
    demo.scene.debug_draw.transform(camera_transform);
    demo.scene.debug_draw.axes();
    demo.scene.debug_draw.end();

    // Update constants.
    demo.constants.buffer(desc.frame_index).ref() = Constants {
        .transform = camera_transform,
        .saber_color_and_intensity = params.saber_color_and_intensity,
        .tonemap = params.tonemap,
    };
}

auto render(Demo& demo, const RenderDesc& desc) -> void {
    FB_PERF_FUNC();
    auto& [cmd, device, frame_index] = desc;
    cmd.pix_begin("%s - Render", NAME.data());

    // Scene pass.
    cmd.graphics_scope([&demo, frame_index](GpuGraphicsCommandList& cmd) {
        auto& scene = demo.scene;

        cmd.pix_begin("Scene");

        scene.render_target_view.set_graphics(cmd);
        scene.debug_draw.render(cmd);
        cmd.set_pipeline(scene.pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(scene.indices.index_buffer_view());
        cmd.set_constants(SceneBindings {
            .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
            .vertices = scene.vertices.srv_descriptor().index(),
            .instances = scene.instances.srv_descriptor().index(),
        });
        cmd.draw_indexed_instanced(scene.indices.element_count(), 2, 0, 0, 0);

        scene.render_target_view.transition_to_resolve(cmd);
        cmd.flush_barriers();

        scene.render_target_view.resolve(cmd);

        scene.render_target_view.transition_to_shader_resource(cmd);
        cmd.flush_barriers();

        cmd.pix_end();
    });

    // Compute pass.
    cmd.compute_scope([&demo, frame_index](GpuComputeCommandList& cmd) {
        const auto& scene = demo.scene;
        auto& compute = demo.compute;

        cmd.pix_begin("Compute");

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
        const auto size = scene.render_target.color(0).size();
        const auto mip_count = mip_count_from_size(size);

        // Thresholding.
        cmd.pix_begin("Thresholding");
        cmd.set_pipeline(compute.threshold_pipeline);
        cmd.set_constants(ThresholdBindings {
            .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
            .scene_texture = scene.render_target.color(0).srv_descriptor().index(),
            .downsample_texture = compute.downsample.uav_descriptor().index(),
            .texture_width = size.x,
            .texture_height = size.y,
        });
        cmd.dispatch(
            (size.x + (DISPATCH_X - 1)) / DISPATCH_X,
            (size.y + (DISPATCH_Y - 1)) / DISPATCH_Y,
            1
        );
        cmd.pix_end();

        // Downsampling.
        cmd.pix_begin("Downsampling");
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
        cmd.pix_end();

        // Upsampling.
        cmd.pix_begin("Upsampling");
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
        cmd.pix_end();

        cmd.pix_end();
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
        blit.render_target_view.transition_to_render_target(cmd);
        cmd.flush_barriers();

        cmd.pix_begin("Blit");

        blit.render_target_view.set_graphics(cmd);
        cmd.set_pipeline(blit.pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_constants(BlitBindings {
            .constants = demo.constants.buffer(frame_index).cbv_descriptor().index(),
            .scene_texture = scene.render_target.color(0).srv_descriptor().index(),
            .bloom_texture = compute.upsample.srv_descriptor().index(),
        });
        cmd.draw_instanced(3, 1, 0, 0);

        cmd.pix_end();
    });

    cmd.pix_end();
}

} // namespace fb::demos::saber
