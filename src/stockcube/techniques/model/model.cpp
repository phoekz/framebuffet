#include "model.hpp"

namespace fb::techniques::model {

auto create(Technique& tech, const CreateDesc& desc) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Create", NAME.data());

    const auto& assets = desc.baked.stockcube.assets;
    const auto& shaders = desc.baked.stockcube.shaders;
    const auto& render_targets = desc.render_targets;
    auto& device = desc.device;

    tech.lut_texture = desc.lut_texture;
    tech.irr_texture = desc.irr_texture;
    tech.rad_texture = desc.rad_texture;
    tech.rad_texture_mip_count = desc.rad_texture_mip_count;
    tech.constants.create(device, 1, dx_name(NAME, "Constants"));
    const auto mesh = assets.sphere_mesh();
    tech.vertices.create_with_data(device, mesh.vertices, dx_name(NAME, "Vertices"));
    tech.indices.create_with_data(device, mesh.indices, dx_name(NAME, "Indices"));
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.model_vs())
        .pixel_shader(shaders.model_ps())
        .depth_stencil(GpuDepthStencilDesc {
            .depth_read = true,
            .depth_write = true,
            .depth_func = GpuComparisonFunc::LessEqual,
        })
        .render_target_formats({render_targets.color_format()})
        .depth_stencil_format(render_targets.depth_format())
        .sample_desc(render_targets.sample_desc())
        .build(device, tech.pipeline, dx_name(NAME, "Pipeline"));
}

auto gui(Technique& tech, const GuiDesc&) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Gui", NAME.data());

    auto& params = tech.parameters;
    ImGui::SliderFloat("Metallic", &params.metallic, 0.0f, 1.0f);
    ImGui::SliderFloat("Roughness", &params.roughness, 0.0f, 1.0f);
}

auto update(Technique& tech, const UpdateDesc& desc) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Update", NAME.data());

    const auto camera_transform = desc.camera_view * desc.camera_projection;
    const auto& params = tech.parameters;

    *tech.constants.ptr() = Constants {
        .transform = camera_transform,
        .camera_position = desc.camera_position,
        .metallic = params.metallic,
        .roughness = params.roughness,
        .mip_count = tech.rad_texture_mip_count,
    };
}

auto gpu_commands(Technique& tech, const GpuCommandsDesc& desc) -> void {
    GpuCommandList& cmd = desc.cmd;

    cmd.begin_pix("%s - GpuCommands", NAME.data());
    cmd.set_graphics_constants(Bindings {
        .constants = tech.constants.cbv_descriptor().index(),
        .vertices = tech.vertices.srv_descriptor().index(),
        .lut_texture = tech.lut_texture.index(),
        .irr_texture = tech.irr_texture.index(),
        .rad_texture = tech.rad_texture.index(),
    });
    cmd.set_pipeline(tech.pipeline);
    cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd.set_index_buffer(tech.indices.index_buffer_view());
    cmd.draw_indexed_instanced(tech.indices.element_size(), 1, 0, 0, 0);
    cmd.end_pix();
}

} // namespace fb::techniques::model
