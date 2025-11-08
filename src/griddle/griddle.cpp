#include <common/common.hpp>
#include <kitchen/kitchen.hpp>
#include <baked/griddle/baked.hpp>
#include "griddle.hlsli"

namespace fb {

inline constexpr std::string_view PROGRAM_NAME = "Griddle"sv;

inline constexpr std::string_view WINDOW_TITLE = "griddle 😎"sv;
inline constexpr int WINDOW_WIDTH = 1280;
inline constexpr int WINDOW_HEIGHT = 832;
inline constexpr float WINDOW_ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
static_assert(
    WINDOW_WIDTH % 64 == 0 && WINDOW_HEIGHT % 64 == 0,
    "Window sizes must be a multiple of 64 due to SPD limitations."
);

struct Bc1Block {
    uint16_t endpoint0 = 0;
    uint16_t endpoint1 = 0;
    uint32_t selectors = 0;
};

struct Griddle {
    Window window;
    GpuDevice device;
    KcnGui gui;
    Frame frame;

    GpuBufferDeviceSrv<griddle::Vertex> vertices;
    GpuBufferDeviceIndex<uint16_t> indices;
    KcnMultibuffer<GpuBufferHostCbv<griddle::Constants>, FRAME_COUNT> constants;
    Pcg texture_pcg;
    Bc1Block texture_block;
    GpuBufferHostUpload<std::byte> texture_upload;
    GpuTextureSrv texture;
    GpuPipeline pipeline;
};

auto griddle_run(Griddle& gr) -> void {
    // Time since process start.
    const auto time_since_process_start = fb::get_time_since_process_start();

    // Console.
    attach_console();

    // Log time since process start.
    FB_LOG_INFO("Time since process start: {} s", time_since_process_start);

    // Main thread.
    FB_ASSERT_HR(SetThreadDescription(GetCurrentThread(), L"Main Thread"));

    // Tracy configuration.
    FB_PERF_TRACY_SET_PROGRAM_NAME("griddle");

    // Init.
    {
        FB_PERF_SCOPE("Init");
        gr.window.create(Window::Desc {WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT});
        gr.device.create(gr.window);

        baked::kitchen::Assets kitchen_assets;
        baked::kitchen::Shaders kitchen_shaders;
        baked::griddle::Shaders griddle_shaders;
        kitchen_assets.load();
        kitchen_shaders.load();
        griddle_shaders.load();
        gr.gui.create(gr.window, gr.device, kitchen_assets, kitchen_shaders);
        {
            DebugScope debug(PROGRAM_NAME);
            const griddle::Vertex vertices[4] = {
                {float2 {-0.5f, -0.5f}, float2 {0.0f, 0.0f}},
                {float2 {0.5f, -0.5f}, float2 {1.0f, 0.0f}},
                {float2 {0.5f, 0.5f}, float2 {1.0f, 1.0f}},
                {float2 {-0.5f, 0.5f}, float2 {0.0f, 1.0f}},
            };
            const uint16_t indices[6] = {0, 1, 2, 2, 3, 0};
            gr.vertices.create_and_transfer(
                gr.device,
                vertices,
                D3D12_BARRIER_SYNC_VERTEX_SHADING,
                D3D12_BARRIER_ACCESS_VERTEX_BUFFER,
                debug.with_name("Vertices")
            );
            gr.indices.create_and_transfer(
                gr.device,
                indices,
                D3D12_BARRIER_SYNC_INDEX_INPUT,
                D3D12_BARRIER_ACCESS_INDEX_BUFFER,
                debug.with_name("Indices")
            );
            gr.constants.create(gr.device, 1, debug.with_name("Constants"));

            const uint width = 4;
            const uint height = 4;
            const DXGI_FORMAT block_format = DXGI_FORMAT_BC1_UNORM;
            const uint block_width = (width + 3) / 4;
            const uint block_height = (height + 3) / 4;
            const uint block_byte_count = dxgi_format_unit_byte_count(block_format);
            gr.texture_upload.create(
                gr.device,
                block_width * block_height * block_byte_count,
                debug.with_name("Texture Upload Buffer")
            );
            gr.texture_block.endpoint0 = (uint16_t)gr.texture_pcg.random_uint();
            gr.texture_block.endpoint1 = (uint16_t)gr.texture_pcg.random_uint();
            gr.texture_block.selectors = gr.texture_pcg.random_uint();
            const uint row_pitch = block_width * block_byte_count;
            const uint slice_pitch = block_height * row_pitch;
            gr.texture.create_and_transfer(
                gr.device,
                GpuTextureDesc {
                    .format = block_format,
                    .width = width,
                    .height = height,
                    .mip_count = 1,
                    .sample_count = 1,
                },
                GpuTextureTransferDesc {
                    .row_pitch = row_pitch,
                    .slice_pitch = slice_pitch,
                    .data = &gr.texture_block,
                },
                D3D12_BARRIER_SYNC_PIXEL_SHADING,
                D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
                D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE,
                debug.with_name("Texture")
            );
            GpuPipelineBuilder()
                .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
                .vertex_shader(griddle_shaders.griddle_vs())
                .pixel_shader(griddle_shaders.griddle_ps())
                .render_target_formats({gr.device.swapchain().format()})
                .depth_stencil({.depth_read = false, .depth_write = false})
                .sample_desc({.Count = 1, .Quality = 0})
                .build(gr.device, gr.pipeline, debug.with_name("Pipeline"));
        }

        gr.device.flush_transfers();
        gr.device.log_stats();
        gr.frame.create();
    }

    // Main loop.
    bool running = true;
    while (running) {
        FB_PERF_FRAME(gr.frame.count());

        // Handle window messages.
        {
            FB_PERF_SCOPE("Events");

            MSG msg = {};
            if (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageA(&msg);
            }
            if (msg.message == WM_QUIT) {
                running = false;
            }
        }

        // Update window.
        gr.window.update();

        // Update gui.
        bool gui_wants_the_mouse = false;
        {
            FB_PERF_SCOPE("Gui");

            // Build gui.
            gr.gui.begin_frame();
            ImGui::SetNextWindowSize({300, 300}, ImGuiCond_FirstUseEver);
            if (ImGui::Begin(PROGRAM_NAME.data())) {
                ImGui::Text(
                    "Frame time: %.3f ms (%.3f fps)",
                    1e3f * gr.frame.last_delta_time(),
                    gr.frame.last_fps()
                );
                if (ImGui::Button("PIX Capture")) {
                    gr.device.pix_capture();
                }

                if (ImGui::Button("Randomize Block")) {
                    gr.texture_block.endpoint0 = (uint16_t)gr.texture_pcg.random_uint();
                    gr.texture_block.endpoint1 = (uint16_t)gr.texture_pcg.random_uint();
                    gr.texture_block.selectors = gr.texture_pcg.random_uint();
                }

                if (ImGui::Button("Randomize Endpoints")) {
                    gr.texture_block.endpoint0 = (uint16_t)gr.texture_pcg.random_uint();
                    gr.texture_block.endpoint1 = (uint16_t)gr.texture_pcg.random_uint();
                }
                Bc1Block& blk = gr.texture_block;
                int e0_r = (blk.endpoint0 >> 11) & 0x1f;
                int e0_g = (blk.endpoint0 >> 5) & 0x3f;
                int e0_b = (blk.endpoint0 >> 0) & 0x1f;
                int e1_r = (blk.endpoint1 >> 11) & 0x1f;
                int e1_g = (blk.endpoint1 >> 5) & 0x3f;
                int e1_b = (blk.endpoint1 >> 0) & 0x1f;
                ImGui::SliderInt("e0_r", &e0_r, 0, 31);
                ImGui::SliderInt("e0_g", &e0_g, 0, 63);
                ImGui::SliderInt("e0_b", &e0_b, 0, 31);
                ImGui::SliderInt("e1_r", &e1_r, 0, 31);
                ImGui::SliderInt("e1_g", &e1_g, 0, 63);
                ImGui::SliderInt("e1_b", &e1_b, 0, 31);
                blk.endpoint0 = (uint16_t)((e0_r << 11) | (e0_g << 5) | (e0_b << 0));
                blk.endpoint1 = (uint16_t)((e1_r << 11) | (e1_g << 5) | (e1_b << 0));
                ImGui::Text("Endpoint 0: 0x%04x", gr.texture_block.endpoint0);
                ImGui::Text("Endpoint 1: 0x%04x", gr.texture_block.endpoint1);

                if (ImGui::Button("Randomize Selectors")) {
                    gr.texture_block.selectors = gr.texture_pcg.random_uint();
                }
                ImGui::Text("Selectors: 0x%08x", gr.texture_block.selectors);
            }
            ImGui::End();
            gr.gui.end_frame();

            // Inputs.
            gui_wants_the_mouse = ImGui::GetIO().WantCaptureMouse;
        }

        // Update.
        {
            const auto frame_index = gr.device.frame_index();
            const auto width = (float)gr.device.swapchain().size().x;
            const auto height = (float)gr.device.swapchain().size().y;
            const auto aspect = width / height;
            const auto left = -aspect;
            const auto right = aspect;
            const auto bottom = -1.0f;
            const auto top = 1.0f;
            const auto projection = float4x4_orthographic(left, right, bottom, top, 0.0f, 1.0f);
            gr.constants.buffer(frame_index).ref() = griddle::Constants {
                .transform = projection,
            };
            memcpy(gr.texture_upload.ptr(), &gr.texture_block, sizeof(gr.texture_block));
        }

        // Rendering.
        {
            auto& swapchain = gr.device.swapchain();
            const auto frame_index = gr.device.frame_index();

            FB_PERF_SCOPE("Rendering");

            auto cmd = gr.device.begin_frame();

            FB_PERF_PIX_BEGIN_EVENT("Frame");
            cmd.pix_begin("Frame %zu", gr.frame.count());

            swapchain.transition_to_render_target(cmd, frame_index);
            cmd.flush_barriers();
            swapchain.clear_render_target(cmd, frame_index);
            {
                gr.texture.transition(
                    cmd,
                    D3D12_BARRIER_SYNC_COPY,
                    D3D12_BARRIER_ACCESS_COPY_DEST,
                    D3D12_BARRIER_LAYOUT_COPY_DEST
                );
                cmd.flush_barriers();
                cmd.copy_buffer_to_texture(
                    gr.texture.resource(),
                    0,
                    gr.texture.format(),
                    gr.texture.width(),
                    gr.texture.height(),
                    gr.texture_upload.byte_count(),
                    gr.texture_upload.resource(),
                    0
                );
                gr.texture.transition(
                    cmd,
                    D3D12_BARRIER_SYNC_PIXEL_SHADING,
                    D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
                    D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE
                );
            }
            cmd.graphics_scope([&gr, &swapchain, frame_index](GpuGraphicsCommandList& cmd) {
                swapchain.set_render_target(cmd, frame_index);
                cmd.set_pipeline(gr.pipeline);
                cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                cmd.set_constants(
                    griddle::Bindings {
                        .constants = gr.constants.buffer(frame_index).cbv_descriptor().index(),
                        .vertices = gr.vertices.srv_descriptor().index(),
                        .texture = gr.texture.srv_descriptor().index(),
                        .sampler = (uint)GpuSampler::PointClamp,
                    }
                );
                cmd.set_index_buffer(gr.indices.index_buffer_view());
                cmd.draw_indexed_instanced(gr.indices.element_count(), 1, 0, 0, 0);
            });
            gr.gui.render(gr.device, cmd);
            swapchain.transition_to_present(cmd, frame_index);
            cmd.flush_barriers();

            cmd.pix_end();
            FB_PERF_PIX_END_EVENT();

            gr.device.end_frame(std::move(cmd));
        }

        // Present.
        {
            FB_PERF_SCOPE("Present");
            gr.device.present();
        }

        // Update frame.
        gr.frame.update();
    }

    // Wait for pending GPU work to complete before running destructors.
    gr.device.wait();
}

} // namespace fb

//
// Main.
//

auto main() -> int {
    FB_LOG_INFO("sizeof(Griddle): {} bytes", sizeof(fb::Griddle));
    auto griddle = std::make_unique<fb::Griddle>();
    fb::griddle_run(*griddle);
    return 0;
}
