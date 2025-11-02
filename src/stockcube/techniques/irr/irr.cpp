#include "irr.hpp"

namespace fb::techniques::irr {

auto create(Technique& tech, const CreateDesc& desc) -> void {
    FB_PERF_FUNC();
    DebugScope debug(NAME);

    const auto& shaders = desc.baked.stockcube.shaders;
    auto& device = desc.device;

    tech.texture_name = desc.texture_name;

    tech.cube_texture = desc.cube_texture;
    constexpr auto ACC_FORMAT = DXGI_FORMAT_R32G32B32A32_FLOAT;
    constexpr auto DIV_FORMAT = DXGI_FORMAT_R16G16B16A16_FLOAT;
    constexpr auto WIDTH = 128u;
    constexpr auto HEIGHT = 128u;
    constexpr auto DEPTH = 6u;
    tech.acc_texture.create(
        device,
        GpuTextureDesc {.format = ACC_FORMAT, .width = WIDTH, .height = HEIGHT, .depth = DEPTH},
        debug.with_name("Acc Texture")
    );
    tech.div_texture.create(
        device,
        GpuTextureDesc {.format = DIV_FORMAT, .width = WIDTH, .height = HEIGHT, .depth = DEPTH},
        debug.with_name("Div Texture")
    );
    tech.readback.create(device, tech.div_texture.byte_count(), debug.with_name("Readback"));

    const auto sample_count_lg2 = 16;
    const auto sample_count = 1u << sample_count_lg2;
    const auto sample_count_per_dispatch = 256;
    const auto dispatch_count = sample_count / sample_count_per_dispatch;
    tech.constants.create(device, 1, debug.with_name("Constants"));
    tech.constants.ref() = Constants {
        .irr_texture_size = uint2(WIDTH, HEIGHT),
        .irr_sample_count = sample_count,
        .irr_sample_count_per_dispatch = sample_count_per_dispatch,
    };
    tech.dispatch_id = 0;
    tech.dispatch_count = dispatch_count;
    tech.sample_count_lg2 = sample_count_lg2;

    GpuPipelineBuilder()
        .compute_shader(shaders.irr_acc_cs())
        .build(device, tech.acc_pipeline, debug.with_name("Acc Pipeline"));
    GpuPipelineBuilder()
        .compute_shader(shaders.irr_div_cs())
        .build(device, tech.div_pipeline, debug.with_name("Div Pipeline"));
}

auto gui(Technique& tech, const GuiDesc&) -> void {
    FB_PERF_FUNC();

    if (ImGui::InputInt("samples_lg2", (int*)&tech.sample_count_lg2, 1)) {
        const auto sample_count = 1 << tech.sample_count_lg2;
        auto& constants = tech.constants.ref();
        tech.dispatch_id = 0;
        tech.dispatch_count = sample_count / constants.irr_sample_count_per_dispatch;
        constants.irr_sample_count = sample_count;
    }
    ImGui::ProgressBar((float)tech.dispatch_id / (float)tech.dispatch_count, ImVec2());

    if (ImGui::Button("save")) {
        tech.delayed_save.init();
    }
}

auto update(Technique& tech, const UpdateDesc&) -> void {
    FB_PERF_FUNC();

    tech.delayed_save.until([&]() {
        const auto bin_path =
            std::format(FB_STOCKCUBE_INTERMEDIATE_DIR "/{}_irr.bin", tech.texture_name);
        FB_LOG_INFO("Saving IRR texture to {}...", bin_path);
        write_whole_file(bin_path, tech.readback.span());

        const auto json_path =
            std::format(FB_STOCKCUBE_INTERMEDIATE_DIR "/{}_irr.json", tech.texture_name);
        FB_LOG_INFO("Saving IRR texture metadata to {}...", json_path);
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
            cmd.set_constants(
                AccBindings {
                    .dispatch_id = tech.dispatch_id,
                    .constants = tech.constants.cbv_descriptor().index(),
                    .cube_texture = tech.cube_texture.index(),
                    .cube_sampler = (uint)GpuSampler::LinearClamp,
                    .irr_texture = tech.acc_texture.uav_descriptor().index(),
                }
            );
            cmd.dispatch(
                tech.acc_texture.width() / DISPATCH_X,
                tech.acc_texture.height() / DISPATCH_Y,
                6
            );

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
            cmd.set_constants(
                DivBindings {
                    .dispatch_id = tech.dispatch_id,
                    .constants = tech.constants.cbv_descriptor().index(),
                    .acc_texture = tech.acc_texture.uav_descriptor().index(),
                    .div_texture = tech.div_texture.uav_descriptor().index(),
                }
            );
            cmd.dispatch(
                tech.div_texture.width() / DISPATCH_X,
                tech.div_texture.height() / DISPATCH_Y,
                6
            );

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
        FB_LOG_INFO("Reading IRR texture back...");

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
        for (uint mip_slice = 0; mip_slice < 6; mip_slice++) {
            cmd.copy_texture_to_buffer(
                tech.readback.resource(),
                offset,
                tech.div_texture.resource(),
                mip_slice,
                tech.div_texture.format(),
                tech.div_texture.width(),
                tech.div_texture.height(),
                tech.div_texture.width() * dxgi_format_unit_byte_count(tech.div_texture.format())
            );
            offset += tech.div_texture.slice_pitch();
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

} // namespace fb::techniques::irr
