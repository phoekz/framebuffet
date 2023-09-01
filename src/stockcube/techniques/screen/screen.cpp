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
    *tech.constants.ptr() = Constants {
        .transform = Float4x4(
            Float4(2.0f / w, 0.0f, 0.0f, 0.0f),
            Float4(0.0f, -2.0f / h, 0.0f, 0.0f),
            Float4(0.0f, 0.0f, 1.0f, 0.0f),
            Float4(-1.0f, 1.0f, 0.0f, 1.0f)
        ),
    };

    const auto vertices = std::to_array({
        Vertex {Float2(0.0f, 0.0f), Float2(0.0f, 0.0f)},
        Vertex {Float2(0.0f, 1.0f), Float2(0.0f, 1.0f)},
        Vertex {Float2(1.0f, 1.0f), Float2(1.0f, 1.0f)},
        Vertex {Float2(1.0f, 0.0f), Float2(1.0f, 0.0f)},
    });
    const auto indices = std::to_array({0u, 1u, 2u, 0u, 2u, 3u});
    auto instances = std::vector<Instance>();
    const auto cubemap_offsets = std::to_array({
        Float2(200.0f, 200.0f), // +x
        Float2(0.0f, 200.0f),   // -x
        Float2(100.0f, 100.0f), // +y
        Float2(100.0f, 300.0f), // -y
        Float2(100.0f, 200.0f), // +z
        Float2(300.0f, 200.0f), // -z
    });
    instances.push_back(Instance {
        .offset = Float2(0.0f, 0.0f),
        .scale = Float2(400.0f, 200.0f),
        .texture = desc.rect_texture.index(),
        .texture_face_id = 0,
        .texture_mip_id = 0,
        .sampler = (uint32_t)GpuSampler::LinearClamp,
    });
    instances.push_back(Instance {
        .offset = Float2(400.0f, 0.0f),
        .scale = Float2(200.0f, 200.0f),
        .texture = desc.lut_texture.index(),
        .texture_face_id = 0,
        .texture_mip_id = 0,
        .sampler = (uint32_t)GpuSampler::LinearClamp,
    });
    for (uint32_t face_id = 0; const auto& offset : cubemap_offsets) {
        instances.push_back(Instance {
            .offset = offset + Float2(0.0f, 100.0f),
            .scale = Float2(100.0f, 100.0f),
            .texture = desc.cube_texture.index(),
            .texture_face_id = face_id,
            .texture_mip_id = 0,
            .sampler = (uint32_t)GpuSampler::LinearClamp,
        });
        face_id++;
    }
    for (uint32_t face_id = 0; const auto& offset : cubemap_offsets) {
        instances.push_back(Instance {
            .offset = offset + Float2(0.0f, 400.0f),
            .scale = Float2(100.0f, 100.0f),
            .texture = desc.irr_texture.index(),
            .texture_face_id = face_id,
            .texture_mip_id = 0,
            .sampler = (uint32_t)GpuSampler::PointClamp,
        });
        face_id++;
    }
    for (uint32_t mip_id = 0; mip_id < desc.rad_texture_mip_count; mip_id++) {
        for (uint32_t face_id = 0; face_id < 6; face_id++) {
            auto offset = Float2(face_id * 50.0f, mip_id * 50.0f);
            offset += Float2((float)device.swapchain().size().x, 0.0f);
            offset -= Float2(6 * 50.0f, 0.0f);
            instances.push_back(Instance {
                .offset = offset,
                .scale = Float2(50.0f, 50.0f),
                .texture = desc.rad_texture.index(),
                .texture_face_id = face_id,
                .texture_mip_id = mip_id,
                .sampler = (uint32_t)GpuSampler::PointClamp,
            });
        }
    }

    tech.vertices.create_with_data(device, vertices, debug.with_name("Vertices"));
    tech.indices.create_with_data(device, indices, debug.with_name("Indices"));
    tech.instances.create_with_data(device, instances, debug.with_name("Instances"));

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

auto gpu_commands(Technique& tech, const GpuCommandsDesc& desc) -> void {
    GpuCommandList& cmd = desc.cmd;

    cmd.begin_pix("%s - GpuCommands", NAME.data());
    cmd.set_pipeline(tech.pipeline);
    cmd.set_graphics_constants(Bindings {
        .constants = tech.constants.cbv_descriptor().index(),
        .vertices = tech.vertices.srv_descriptor().index(),
        .instances = tech.instances.srv_descriptor().index(),
    });
    cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd.set_index_buffer(tech.indices.index_buffer_view());
    cmd.draw_indexed_instanced(tech.indices.element_size(), tech.instances.element_size(), 0, 0, 0);
    cmd.end_pix();
}

} // namespace fb::techniques::screen
