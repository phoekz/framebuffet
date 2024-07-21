#include "screen.hpp"

namespace fb::techniques::screen {

auto create(Technique& tech, const CreateDesc& desc) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Create", NAME.data());
    DebugScope debug(NAME);

    const auto& render_targets = desc.render_targets;
    const auto& shaders = desc.baked.stockcube.shaders;
    auto& device = desc.device;

    const auto w = device.swapchain().size().x;
    const auto h = device.swapchain().size().y;
    tech.constants.create(device, 1, debug.with_name("Constants"));
    tech.constants.ref() = Constants {
        .transform = float4x4(
            float4(2.0f / w, 0.0f, 0.0f, 0.0f),
            float4(0.0f, -2.0f / h, 0.0f, 0.0f),
            float4(0.0f, 0.0f, 1.0f, 0.0f),
            float4(-1.0f, 1.0f, 0.0f, 1.0f)
        ),
    };

    const auto vertices = std::to_array({
        Vertex {float2(0.0f, 0.0f), float2(0.0f, 0.0f)},
        Vertex {float2(0.0f, 1.0f), float2(0.0f, 1.0f)},
        Vertex {float2(1.0f, 1.0f), float2(1.0f, 1.0f)},
        Vertex {float2(1.0f, 0.0f), float2(1.0f, 0.0f)},
    });
    const auto indices = std::to_array({0u, 1u, 2u, 0u, 2u, 3u});
    auto instances = std::vector<Instance>();
    const auto cubemap_offsets = std::to_array({
        float2(200.0f, 200.0f), // +x
        float2(0.0f, 200.0f),   // -x
        float2(100.0f, 100.0f), // +y
        float2(100.0f, 300.0f), // -y
        float2(100.0f, 200.0f), // +z
        float2(300.0f, 200.0f), // -z
    });
    instances.push_back(Instance {
        .offset = float2(0.0f, 0.0f),
        .scale = float2(400.0f, 200.0f),
        .texture = desc.rect_texture.index(),
        .texture_face_id = 0,
        .texture_mip_id = 0,
        .sampler = (uint)GpuSampler::LinearClamp,
    });
    instances.push_back(Instance {
        .offset = float2(400.0f, 0.0f),
        .scale = float2(200.0f, 200.0f),
        .texture = desc.lut_texture.index(),
        .texture_face_id = 0,
        .texture_mip_id = 0,
        .sampler = (uint)GpuSampler::LinearClamp,
    });
    for (uint face_id = 0; const auto& offset : cubemap_offsets) {
        instances.push_back(Instance {
            .offset = offset + float2(0.0f, 100.0f),
            .scale = float2(100.0f, 100.0f),
            .texture = desc.cube_texture.index(),
            .texture_face_id = face_id,
            .texture_mip_id = 0,
            .sampler = (uint)GpuSampler::LinearClamp,
        });
        face_id++;
    }
    for (uint face_id = 0; const auto& offset : cubemap_offsets) {
        instances.push_back(Instance {
            .offset = offset + float2(0.0f, 400.0f),
            .scale = float2(100.0f, 100.0f),
            .texture = desc.irr_texture.index(),
            .texture_face_id = face_id,
            .texture_mip_id = 0,
            .sampler = (uint)GpuSampler::PointClamp,
        });
        face_id++;
    }
    for (uint mip_id = 0; mip_id < desc.rad_texture_mip_count; mip_id++) {
        for (uint face_id = 0; face_id < 6; face_id++) {
            auto offset = float2(face_id * 50.0f, mip_id * 50.0f);
            offset += float2((float)device.swapchain().size().x, 0.0f);
            offset -= float2(6 * 50.0f, 0.0f);
            instances.push_back(Instance {
                .offset = offset,
                .scale = float2(50.0f, 50.0f),
                .texture = desc.rad_texture.index(),
                .texture_face_id = face_id,
                .texture_mip_id = mip_id,
                .sampler = (uint)GpuSampler::PointClamp,
            });
        }
    }

    tech.vertices.create_and_transfer(
        device,
        vertices,
        D3D12_BARRIER_SYNC_VERTEX_SHADING,
        D3D12_BARRIER_ACCESS_VERTEX_BUFFER,
        debug.with_name("Vertices")
    );
    tech.indices.create_and_transfer(
        device,
        indices,
        D3D12_BARRIER_SYNC_INDEX_INPUT,
        D3D12_BARRIER_ACCESS_INDEX_BUFFER,
        debug.with_name("Indices")
    );
    tech.instances.create_and_transfer(
        device,
        instances,
        D3D12_BARRIER_SYNC_VERTEX_SHADING,
        D3D12_BARRIER_ACCESS_VERTEX_BUFFER,
        debug.with_name("Instances")
    );

    GpuPipelineBuilder()
        .vertex_shader(shaders.screen_vs())
        .pixel_shader(shaders.screen_ps())
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .depth_stencil(GpuDepthStencilDesc {
            .depth_read = false,
            .depth_write = false,
        })
        .rasterizer(GpuRasterizerDesc {
            .cull_mode = GpuCullMode::None,
        })
        .render_target_formats({render_targets.color_format()})
        .sample_desc(render_targets.sample_desc())
        .build(device, tech.pipeline, debug.with_name("Pipeline"));
}

auto gui(Technique&, const GuiDesc&) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Gui", NAME.data());
}

auto update(Technique&, const UpdateDesc&) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Update", NAME.data());
}

auto render(Technique& tech, const RenderDesc& desc) -> void {
    auto& [cmd, device, _] = desc;
    cmd.graphics_scope([&tech](GpuGraphicsCommandList& cmd) {
        cmd.pix_begin("%s - Render", NAME.data());
        cmd.set_pipeline(tech.pipeline);
        cmd.set_constants(Bindings {
            .constants = tech.constants.cbv_descriptor().index(),
            .vertices = tech.vertices.srv_descriptor().index(),
            .instances = tech.instances.srv_descriptor().index(),
        });
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(tech.indices.index_buffer_view());
        cmd.draw_indexed_instanced(
            tech.indices.element_count(),
            tech.instances.element_count(),
            0,
            0,
            0
        );
        cmd.pix_end();
    });
}

} // namespace fb::techniques::screen
