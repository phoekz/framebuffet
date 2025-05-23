#include "bg.hpp"

namespace fb::techniques::bg {

auto create(Technique& tech, const CreateDesc& desc) -> void {
    FB_PERF_FUNC();
    DebugScope debug(NAME);

    const auto& assets = desc.baked.stockcube.assets;
    const auto& shaders = desc.baked.stockcube.shaders;
    const auto& render_target_view = desc.render_target_view;
    auto& device = desc.device;

    tech.rad_texture = desc.rad_texture;
    tech.rad_texture_mip_count = desc.rad_texture_mip_count;
    tech.constants.create(device, 1, debug.with_name("Constants"));
    const auto mesh = assets.skybox_mesh();
    tech.vertices.create_and_transfer(
        device,
        mesh.vertices,
        D3D12_BARRIER_SYNC_VERTEX_SHADING,
        D3D12_BARRIER_ACCESS_VERTEX_BUFFER,
        debug.with_name("Vertices")
    );
    tech.indices.create_and_transfer(
        device,
        mesh.indices,
        D3D12_BARRIER_SYNC_INDEX_INPUT,
        D3D12_BARRIER_ACCESS_INDEX_BUFFER,
        debug.with_name("Indices")
    );
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.bg_vs())
        .pixel_shader(shaders.bg_ps())
        .depth_stencil(GpuDepthStencilDesc {
            .depth_read = true,
            .depth_write = true,
            .depth_func = GpuComparisonFunc::LessEqual,
        })
        .render_target_formats({render_target_view.color_format(0)})
        .depth_stencil_format(render_target_view.depth_format())
        .sample_desc(render_target_view.sample_desc())
        .build(device, tech.pipeline, debug.with_name("Pipeline"));
}

auto gui(Technique& tech, const GuiDesc&) -> void {
    FB_PERF_FUNC();

    auto& params = tech.parameters;
    ImGui::SliderFloat("Roughness", &params.roughness, 0.0f, 1.0f);
}

auto update(Technique& tech, const UpdateDesc& desc) -> void {
    FB_PERF_FUNC();

    auto env_view = desc.camera_view;
    env_view[3][0] = 0.0f;
    env_view[3][1] = 0.0f;
    env_view[3][2] = 0.0f;
    env_view[3][3] = 1.0f;
    const auto env_transform = desc.camera_projection * env_view;

    const auto& params = tech.parameters;

    tech.constants.buffer(desc.frame_index).ref() = Constants {
        .transform = env_transform,
        .roughness = params.roughness,
        .mip_count = tech.rad_texture_mip_count,
    };
}

auto render(Technique& tech, const RenderDesc& desc) -> void {
    FB_PERF_FUNC();
    auto& [cmd, device, frame_index] = desc;
    cmd.graphics_scope([&tech, frame_index](GpuGraphicsCommandList& cmd) {
        cmd.pix_begin("%s - Render", NAME.data());
        cmd.set_constants(Bindings {
            .constants = tech.constants.buffer(frame_index).cbv_descriptor().index(),
            .vertices = tech.vertices.srv_descriptor().index(),
            .texture = tech.rad_texture.index(),
        });
        cmd.set_pipeline(tech.pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(tech.indices.index_buffer_view());
        cmd.draw_indexed_instanced(tech.indices.element_count(), 1, 0, 0, 0);
        cmd.pix_end();
    });
}

} // namespace fb::techniques::bg
