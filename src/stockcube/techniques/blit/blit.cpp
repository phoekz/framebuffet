#include "blit.hpp"

namespace fb::techniques::blit {

auto create(Technique& tech, const CreateDesc& desc) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Create", NAME.data());
    DebugScope debug(NAME);

    const auto& shaders = desc.baked.stockcube.shaders;
    auto& device = desc.device;

    tech.size = device.swapchain().size();
    tech.render_target = desc.render_targets.color().srv_descriptor();
    tech.constants.create(device, 1, debug.with_name("Constants"));
    GpuPipelineBuilder()
        .vertex_shader(shaders.blit_vs())
        .pixel_shader(shaders.blit_ps())
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .depth_stencil(GpuDepthStencilDesc {
            .depth_read = false,
            .depth_write = false,
        })
        .render_target_formats({device.swapchain().format()})
        .build(device, tech.pipeline, debug.with_name("Pipeline"));
}

auto gui(Technique& tech, const GuiDesc&) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Gui", NAME.data());

    auto& params = tech.parameters;
    ImGui::Checkbox("Tonemap", (bool*)&params.tonemap);
    ImGui::SliderFloat("Exposure Lg2", &params.exposure_lg2, -10.0f, 10.0f);
}

auto update(Technique& tech, const UpdateDesc&) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Update", NAME.data());

    const auto& params = tech.parameters;
    const auto exposure = std::pow(2.0f, -params.exposure_lg2);

    *tech.constants.ptr() = Constants {
        .tonemap = params.tonemap,
        .exposure = exposure,
    };
}

auto gpu_commands(Technique& tech, const GpuCommandsDesc& desc) -> void {
    GpuCommandList& cmd = desc.cmd;
    cmd.begin_pix("%s - GpuCommands", NAME.data());
    cmd.set_graphics();
    cmd.set_pipeline(tech.pipeline);
    cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd.set_viewport(0, 0, tech.size.x, tech.size.y);
    cmd.set_scissor(0, 0, tech.size.x, tech.size.y);
    cmd.set_graphics_constants(Bindings {
        .constants = tech.constants.cbv_descriptor().index(),
        .texture = tech.render_target.index(),
    });
    cmd.draw_instanced(3, 1, 0, 0);
    cmd.end_pix();
}

} // namespace fb::techniques::blit
