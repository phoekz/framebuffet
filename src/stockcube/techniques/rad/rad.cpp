#include "rad.hpp"

namespace fb::techniques::rad {

auto create(Technique& tech, const CreateDesc& desc) -> void {
    FB_PERF_FUNC();
    DebugScope debug(NAME);

    const auto& shaders = desc.baked.stockcube.shaders;
    auto& device = desc.device;

    tech.texture_name = desc.texture_name;

    tech.cube_texture = desc.cube_texture;
    constexpr auto ACC_FORMAT = DXGI_FORMAT_R32G32B32A32_FLOAT;
    constexpr auto DIV_FORMAT = DXGI_FORMAT_R16G16B16A16_FLOAT;
    constexpr auto WIDTH = 1024u;
    constexpr auto HEIGHT = 1024u;
    constexpr auto SIZE = uint2(WIDTH, HEIGHT);
    constexpr auto DEPTH = 6u;
    constexpr auto MIP_COUNT = mip_count_from_size(WIDTH, HEIGHT);
    tech.acc_texture.create(
        device,
        GpuTextureDesc {
            .format = ACC_FORMAT,
            .width = WIDTH,
            .height = HEIGHT,
            .depth = DEPTH,
            .mip_count = MIP_COUNT,
        },
        debug.with_name("Acc Texture")
    );
    tech.div_texture.create(
        device,
        GpuTextureDesc {
            .format = DIV_FORMAT,
            .width = WIDTH,
            .height = HEIGHT,
            .depth = DEPTH,
            .mip_count = MIP_COUNT,
        },
        debug.with_name("Div Texture")
    );
    tech.readback.create(device, tech.div_texture.byte_count(), debug.with_name("Readback"));

    const auto sample_count_lg2 = 16;
    const auto sample_count = 1u << sample_count_lg2;
    const auto sample_count_per_dispatch = 128;
    const auto dispatch_count = sample_count / sample_count_per_dispatch;
    tech.constants.create(device, 1, debug.with_name("Constants"));
    tech.constants.ref() = Constants {
        .rad_texture_size = SIZE,
        .rad_texture_mip_count = MIP_COUNT,
        .rad_sample_count = sample_count,
        .rad_sample_count_per_dispatch = sample_count_per_dispatch,
    };
    tech.dispatch_id = 0;
    tech.dispatch_count = dispatch_count;
    tech.sample_count_lg2 = sample_count_lg2;

    GpuPipelineBuilder()
        .compute_shader(shaders.rad_acc_cs())
        .build(device, tech.acc_pipeline, debug.with_name("Acc Pipeline"));
    GpuPipelineBuilder()
        .compute_shader(shaders.rad_div_cs())
        .build(device, tech.div_pipeline, debug.with_name("Div Pipeline"));
}

auto gui(Technique& tech, const GuiDesc&) -> void {
    FB_PERF_FUNC();

    if (ImGui::InputInt("RAD Sample Count Lg2", (int*)&tech.sample_count_lg2, 1)) {
        const auto sample_count = 1 << tech.sample_count_lg2;
        auto& constants = tech.constants.ref();
        tech.dispatch_id = 0;
        tech.dispatch_count = sample_count / constants.rad_sample_count_per_dispatch;
        constants.rad_sample_count = sample_count;
    }
    ImGui::ProgressBar((float)tech.dispatch_id / (float)tech.dispatch_count, ImVec2());
    ImGui::SameLine();
    ImGui::Text("Radiance");

    if (ImGui::Button("save")) {
        tech.delayed_save.init();
    }
}

auto update(Technique& tech, const UpdateDesc&) -> void {
    FB_PERF_FUNC();

    tech.delayed_save.until([&]() {
        const auto bin_path =
            std::format(FB_STOCKCUBE_INTERMEDIATE_DIR "/{}_rad.bin", tech.texture_name);
        FB_LOG_INFO("Saving RAD texture to {}...", bin_path);
        write_whole_file(bin_path, tech.readback.span());

        const auto json_path =
            std::format(FB_STOCKCUBE_INTERMEDIATE_DIR "/{}_rad.json", tech.texture_name);
        FB_LOG_INFO("Saving RAD texture metadata to {}...", json_path);
        write_output_metadata_file(
            json_path,
            {
                .format = tech.div_texture.format(),
                .unit_byte_count = tech.div_texture.unit_byte_count(),
                .width = tech.div_texture.width(),
                .height = tech.div_texture.height(),
                .depth = tech.div_texture.depth(),
                .mip_count = tech.div_texture.mip_count(),
            }
        );
    });
}

auto render(Technique& tech, const RenderDesc& desc) -> void {
    FB_PERF_FUNC();
    auto& [cmd, device, _] = desc;

    if (tech.dispatch_id < tech.dispatch_count) {
        cmd.compute_scope([&tech](GpuComputeCommandList& cmd) {
            // Begin.
            cmd.pix_begin("%s - Render", NAME.data());

            // Barrier.
            tech.acc_texture.transition(
                cmd,
                D3D12_BARRIER_SYNC_COMPUTE_SHADING,
                D3D12_BARRIER_ACCESS_UNORDERED_ACCESS,
                D3D12_BARRIER_LAYOUT_UNORDERED_ACCESS
            );
            cmd.flush_barriers();

            // Accumulate pass.
            cmd.set_pipeline(tech.acc_pipeline);
            for (uint mip = 0; mip < tech.acc_texture.mip_count(); mip++) {
                cmd.set_constants(
                    AccBindings {
                        .dispatch_id = tech.dispatch_id,
                        .constants = tech.constants.cbv_descriptor().index(),
                        .cube_texture = tech.cube_texture.index(),
                        .cube_sampler = (uint)GpuSampler::LinearClamp,
                        .rad_texture = tech.acc_texture.uav_descriptor().index(),
                        .rad_texture_mip_id = mip,
                    }
                );
                const auto top_width = tech.acc_texture.size().x;
                const auto top_height = tech.acc_texture.size().y;
                const auto mip_width = std::max(1u, top_width >> mip);
                const auto mip_height = std::max(1u, top_height >> mip);
                cmd.dispatch(
                    (mip_width + (DISPATCH_X - 1)) / DISPATCH_X,
                    (mip_height + (DISPATCH_Y - 1)) / DISPATCH_Y,
                    6
                );
            }

            // Barrier.
            tech.div_texture.transition(
                cmd,
                D3D12_BARRIER_SYNC_COMPUTE_SHADING,
                D3D12_BARRIER_ACCESS_UNORDERED_ACCESS,
                D3D12_BARRIER_LAYOUT_UNORDERED_ACCESS
            );
            cmd.flush_barriers();

            // Division pass.
            cmd.set_pipeline(tech.div_pipeline);
            for (uint mip = 0; mip < tech.acc_texture.mip_count(); mip++) {
                cmd.set_constants(
                    DivBindings {
                        .dispatch_id = tech.dispatch_id,
                        .mip_id = mip,
                        .constants = tech.constants.cbv_descriptor().index(),
                        .acc_texture = tech.acc_texture.uav_descriptor().index(),
                        .div_texture = tech.div_texture.uav_descriptor().index(),
                    }
                );
                const auto top_width = tech.acc_texture.size().x;
                const auto top_height = tech.acc_texture.size().y;
                const auto mip_width = std::max(1u, top_width >> mip);
                const auto mip_height = std::max(1u, top_height >> mip);
                cmd.dispatch(
                    (mip_width + (DISPATCH_X - 1)) / DISPATCH_X,
                    (mip_height + (DISPATCH_Y - 1)) / DISPATCH_Y,
                    6
                );
            }

            // Barrier.
            tech.div_texture.transition(
                cmd,
                D3D12_BARRIER_SYNC_ALL_SHADING,
                D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
                D3D12_BARRIER_LAYOUT_SHADER_RESOURCE
            );
            cmd.flush_barriers();

            // End.
            cmd.pix_end();

            tech.dispatch_id++;
        });
    } else if (tech.delayed_save.initiated()) {
        FB_LOG_INFO("Reading RAD texture back...");

        // Begin.
        cmd.pix_begin("%s - Render", NAME.data());

        // Barrier.
        tech.div_texture.transition(
            cmd,
            D3D12_BARRIER_SYNC_COPY,
            D3D12_BARRIER_ACCESS_COPY_SOURCE,
            D3D12_BARRIER_LAYOUT_COPY_SOURCE
        );
        cmd.flush_barriers();

        // Copy.
        uint64_t offset = 0;
        for (uint slice = 0; slice < 6; slice++) {
            for (uint mip = 0; mip < tech.div_texture.mip_count(); mip++) {
                const auto mip_width = std::max(1u, tech.div_texture.size().x >> mip);
                const auto mip_height = std::max(1u, tech.div_texture.size().y >> mip);
                const auto subresource_index = mip + slice * tech.div_texture.mip_count();
                cmd.copy_texture_to_buffer(
                    tech.readback.resource(),
                    offset,
                    tech.div_texture.resource(),
                    subresource_index,
                    tech.div_texture.format(),
                    mip_width,
                    mip_height,
                    mip_width * dxgi_format_unit_byte_count(tech.div_texture.format())
                );
                offset += mip_width * mip_height * tech.div_texture.unit_byte_count();
            }
        }
        FB_ASSERT(offset == tech.readback.byte_count());

        // Barrier.
        tech.div_texture.transition(
            cmd,
            D3D12_BARRIER_SYNC_ALL_SHADING,
            D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
            D3D12_BARRIER_LAYOUT_SHADER_RESOURCE
        );
        cmd.flush_barriers();

        // End.
        cmd.pix_end();

        tech.delayed_save.set_pending();
    }
}

} // namespace fb::techniques::rad
