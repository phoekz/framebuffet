#include "lut.hpp"

#include <fstream>

namespace fb::techniques::lut {

auto create(Technique& tech, const CreateDesc& desc) -> void {
    FB_PERF_FUNC();
    DebugScope debug(NAME);

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
        debug.with_name("Texture")
    );
    tech.lut_readback.create(device, tech.lut_texture.byte_count(), debug.with_name("Readback"));

    tech.constants.create(device, 1, debug.with_name("Constants"));
    tech.constants.ref() = Constants {
        .lut_texture_size = tech.lut_texture.size(),
        .lut_sample_count = 2048,
    };
    GpuPipelineBuilder()
        .compute_shader(shaders.lut_cs())
        .build(device, tech.pipeline, debug.with_name("Pipeline"));
}

auto gui(Technique& tech, const GuiDesc&) -> void {
    FB_PERF_FUNC();

    auto& constants = tech.constants.ref();
    if (ImGui::SliderInt("samples", (int*)&constants.lut_sample_count, 1, 2048)) {
        tech.done = false;
    }
    if (ImGui::Button("save")) {
        tech.delayed_save.init();
    }
}

auto update(Technique& tech, const UpdateDesc&) -> void {
    FB_PERF_FUNC();

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

auto render(Technique& tech, const RenderDesc& desc) -> void {
    FB_PERF_FUNC();
    auto& [cmd, device, _] = desc;

    if (!tech.done) {
        cmd.compute_scope([&tech](GpuComputeCommandList& cmd) {
            cmd.pix_begin("%s - Render", NAME.data());
            tech.lut_texture.transition(
                cmd,
                D3D12_BARRIER_SYNC_COMPUTE_SHADING,
                D3D12_BARRIER_ACCESS_UNORDERED_ACCESS,
                D3D12_BARRIER_LAYOUT_UNORDERED_ACCESS
            );
            cmd.flush_barriers();
            cmd.set_pipeline(tech.pipeline);
            cmd.set_constants(Bindings {
                .constants = tech.constants.cbv_descriptor().index(),
                .lut_texture = tech.lut_texture.uav_descriptor().index(),
            });
            cmd.dispatch(
                tech.lut_texture.width() / DISPATCH_X,
                tech.lut_texture.height() / DISPATCH_Y,
                1
            );
            tech.lut_texture.transition(
                cmd,
                D3D12_BARRIER_SYNC_ALL_SHADING,
                D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
                D3D12_BARRIER_LAYOUT_SHADER_RESOURCE
            );
            cmd.flush_barriers();
            cmd.pix_end();

            tech.done = true;
        });
    }

    if (tech.delayed_save.initiated() && tech.done) {
        FB_LOG_INFO("Reading LUT texture back...");

        cmd.pix_begin("%s - Render", NAME.data());
        tech.lut_texture.transition(
            cmd,
            D3D12_BARRIER_SYNC_COPY,
            D3D12_BARRIER_ACCESS_COPY_SOURCE,
            D3D12_BARRIER_LAYOUT_COPY_SOURCE
        );
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
        tech.lut_texture.transition(
            cmd,
            D3D12_BARRIER_SYNC_ALL_SHADING,
            D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
            D3D12_BARRIER_LAYOUT_SHADER_RESOURCE
        );
        cmd.flush_barriers();
        cmd.pix_end();
        tech.delayed_save.set_pending();
    }
}

} // namespace fb::techniques::lut
