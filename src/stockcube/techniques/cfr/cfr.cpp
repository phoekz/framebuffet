#include "cfr.hpp"

namespace fb::techniques::cfr {

auto create(Technique& tech, const CreateDesc& desc) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Create", NAME.data());
    DebugScope debug(NAME);

    const auto& shaders = desc.baked.stockcube.shaders;
    auto& device = desc.device;

    tech.rect_texture = desc.rect_texture;
    tech.cube_texture.create(
        device,
        GpuTextureDesc {
            .format = DXGI_FORMAT_R16G16B16A16_FLOAT,
            .width = 1024,
            .height = 1024,
            .depth = 6,
        },
        debug.with_name("Cube Texture")
    );
    tech.constants.create(device, 1, debug.with_name("Constants"));
    tech.constants.ref() = Constants {
        .cube_texture_size = tech.cube_texture.size(),
    };
    GpuPipelineBuilder()
        .compute_shader(shaders.cfr_cs())
        .build(device, tech.pipeline, debug.with_name("Pipeline"));
}

auto gui(Technique&, const GuiDesc&) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Gui", NAME.data());
}

auto update(Technique&, const UpdateDesc&) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Update", NAME.data());
}

auto render(Technique& tech, const RenderDesc& desc) -> void {
    if (tech.done) {
        return;
    }

    GpuCommandList& cmd = desc.cmd;
    cmd.begin_pix("%s - GpuCommands", NAME.data());
    tech.cube_texture.transition(cmd, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    cmd.flush_barriers();
    cmd.set_compute();
    cmd.set_pipeline(tech.pipeline);
    cmd.set_compute_constants(Bindings {
        .constants = tech.constants.cbv_descriptor().index(),
        .rect_texture = tech.rect_texture.index(),
        .rect_sampler = (uint)GpuSampler::LinearClamp,
        .cube_texture = tech.cube_texture.uav_descriptor().index(),
    });
    cmd.dispatch(
        tech.cube_texture.width() / DISPATCH_X,
        tech.cube_texture.height() / DISPATCH_Y,
        6
    );
    tech.cube_texture.transition(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    cmd.flush_barriers();
    cmd.end_pix();

    tech.done = true;
}

} // namespace fb::techniques::cfr
