#include "lut.hpp"

#include <fstream>

namespace fb::techniques::lut {

auto create(Technique& tech, const CreateDesc& desc) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Create", NAME.data());

    const auto& shaders = desc.baked.stockcube.shaders;
    auto& device = desc.device;

    tech.texture_name = desc.texture_name;

    constexpr auto LUT_FORMAT = DXGI_FORMAT_R16G16_FLOAT;
    constexpr auto LUT_WIDTH = 512;
    constexpr auto LUT_HEIGHT = 512;
    tech.lut_texture.create(
        device,
        GpuTextureDesc {
            .format = LUT_FORMAT,
            .width = LUT_WIDTH,
            .height = LUT_HEIGHT,
            .depth = 1,
        },
        dx_name(NAME, "Texture")
    );
    tech.lut_readback.create(device, tech.lut_texture.byte_size(), dx_name(NAME, "Readback"));

    tech.constants.create(device, 1, dx_name(NAME, "Constants"));
    *tech.constants.ptr() = Constants {
        .lut_texture_size = tech.lut_texture.size(),
        .lut_sample_count = 2048,
    };
    GpuPipelineBuilder()
        .compute_shader(shaders.lut_cs())
        .build(device, tech.pipeline, dx_name(NAME, "Pipeline"));
}

auto gui(Technique& tech, const GuiDesc&) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Gui", NAME.data());

    auto& constants = *tech.constants.ptr();
    if (ImGui::SliderInt("samples", (int*)&constants.lut_sample_count, 1, 2048)) {
        tech.done = false;
    }
    if (ImGui::Button("save")) {
        tech.delayed_save.init();
    }
}

auto update(Technique& tech, const UpdateDesc&) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Update", NAME.data());

    tech.delayed_save.until([&]() {
        const auto bin_path =
            std::format(FB_STOCKCUBE_INTERMEDIATE_DIR "/{}_lut.bin", tech.texture_name);
        FB_LOG_INFO("Saving LUT texture to {}...", bin_path);
        write_whole_file(bin_path, tech.lut_readback.span());

        const auto json_path =
            std::format(FB_STOCKCUBE_INTERMEDIATE_DIR "/{}_lut.json", tech.texture_name);
        FB_LOG_INFO("Saving LUT texture metadata to {}...", json_path);
        write_output_metadata_file(
            json_path,
            {
                .format = tech.lut_texture.format(),
                .unit_byte_count = tech.lut_texture.unit_byte_count(),
                .width = tech.lut_texture.width(),
                .height = tech.lut_texture.height(),
                .depth = tech.lut_texture.depth(),
                .mip_count = tech.lut_texture.mip_count(),
            }
        );
    });
}

auto gpu_commands(Technique& tech, const GpuCommandsDesc& desc) -> void {
    GpuCommandList& cmd = desc.cmd;

    if (!tech.done) {
        cmd.begin_pix("%s - GpuCommands", NAME.data());
        tech.lut_texture.transition(cmd, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        cmd.flush_barriers();
        cmd.set_compute();
        cmd.set_pipeline(tech.pipeline);
        cmd.set_compute_constants(Bindings {
            .constants = tech.constants.cbv_descriptor().index(),
            .lut_texture = tech.lut_texture.uav_descriptor().index(),
        });
        cmd.dispatch(
            tech.lut_texture.width() / DISPATCH_X,
            tech.lut_texture.height() / DISPATCH_Y,
            1
        );
        tech.lut_texture.transition(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        cmd.flush_barriers();
        cmd.end_pix();

        tech.done = true;
    }

    if (tech.delayed_save.initiated() && tech.done) {
        FB_LOG_INFO("Reading LUT texture back...");

        cmd.begin_pix("%s - GpuCommands", NAME.data());
        tech.lut_texture.transition(cmd, D3D12_RESOURCE_STATE_COPY_SOURCE);
        cmd.flush_barriers();
        cmd.copy_texture_to_buffer(
            tech.lut_readback.resource(),
            0,
            tech.lut_texture.resource(),
            0,
            tech.lut_texture.format(),
            tech.lut_texture.width(),
            tech.lut_texture.height()
        );
        tech.lut_texture.transition(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        cmd.flush_barriers();
        cmd.end_pix();
        tech.delayed_save.set_pending();
    }
}

} // namespace fb::techniques::lut
