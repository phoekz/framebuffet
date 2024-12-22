#include "demos/demos.hpp"

namespace fb {

inline constexpr std::string_view WINDOW_TITLE = "framebuffet 😎"sv;
inline constexpr int WINDOW_WIDTH = 1280;
inline constexpr int WINDOW_HEIGHT = 832;
inline constexpr float WINDOW_ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
static_assert(
    WINDOW_WIDTH % 64 == 0 && WINDOW_HEIGHT % 64 == 0,
    "Window sizes must be a multiple of 64 due to SPD limitations."
);

struct Buffet {
    using Demos = demos::Demos;

    Window window;
    GpuDevice device;
    Demos demos;
    KcnGui gui;
    Frame frame;
    std::vector<std::byte> archive_buf;
    double time_since_last_archive = 0.0;
};

auto buffet_run(Buffet& bf) -> void {
    // Time since process start.
    const auto time_since_process_start = fb::get_time_since_process_start();

    // Console.
    attach_console();

    // Log time since process start.
    FB_LOG_INFO("Time since process start: {} s", time_since_process_start);

    // Main thread.
    FB_ASSERT_HR(SetThreadDescription(GetCurrentThread(), L"Main Thread"));

    // Tracy configuration.
    TracySetProgramName("framebuffet");

    // Init.
    {
        FB_PERF_SCOPE("Init");

        const auto timer = Instant();
        DebugScope debug("Buffet");

        demos::Baked baked;

        {
            FB_PERF_SCOPE("Baked");
            baked.kitchen.assets.load();
            baked.kitchen.shaders.load();
            baked.buffet.assets.load();
            baked.buffet.shaders.load();
        }

        {
            FB_PERF_SCOPE("Window");
            char temp[64] = {};
            snprintf(temp, sizeof(temp), "Width: %d", WINDOW_WIDTH);
            ZoneText(temp, strlen(temp));
            snprintf(temp, sizeof(temp), "Height: %d", WINDOW_HEIGHT);
            ZoneText(temp, strlen(temp));
            bf.window.create(Window::Desc {WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT});
        }

        {
            FB_PERF_SCOPE("Device");
            bf.device.create(bf.window);
        }

        {
            FB_PERF_SCOPE("Demos");
            demos::create(bf.demos, {.baked = baked, .device = bf.device});
        }

        {
            FB_PERF_SCOPE("Gui");
            bf.gui.create(bf.window, bf.device, baked.kitchen.assets, baked.kitchen.shaders);
        }

        {
            FB_PERF_SCOPE("Transfer");
            bf.device.flush_transfers();
        }

        bf.device.log_stats();
        bf.frame.create();
        bf.archive_buf.reserve(1024);
        FB_LOG_INFO("Init time: {} ms", 1e3f * timer.elapsed_time());
    }

    // Archive.
    if (file_exists(ARCHIVE_FILE_NAME)) {
        FB_PERF_SCOPE("Archive Load");
        auto buf = read_whole_file(ARCHIVE_FILE_NAME);
        FB_LOG_INFO("Archive size: {} bytes", buf.size());
        auto archive = DeserializingArchive(buf);
        demos::archive(bf.demos, archive);
        FB_ASSERT(archive.fully_consumed());
    }

    // Main loop.
    bool running = true;
    FRAME_ALLOCATION_TRAP = true;
    while (running) {
        FB_PERF_FRAME(bf.frame.count());

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

        // Update archive.
        bf.time_since_last_archive += bf.frame.last_delta_time();
        if (bf.time_since_last_archive > 1.0) {
            FB_PERF_SCOPE("Archive Load");
            bf.time_since_last_archive = 0.0;
            auto archive = SerializingArchive(bf.archive_buf);
            demos::archive(bf.demos, archive);
            write_whole_file(ARCHIVE_FILE_NAME, bf.archive_buf);
            bf.archive_buf.clear();
        }

        // Update window.
        bf.window.update();

        // Update gui.
        bool gui_wants_the_mouse = false;
        {
            FB_PERF_SCOPE("Gui");

            // Build gui.
            bf.gui.begin_frame();
            ImGui::SetNextWindowSize({300, 300}, ImGuiCond_FirstUseEver);
            if (ImGui::Begin(demos::NAME.data())) {
                ImGui::Text(
                    "Frame time: %.2f ms (%.2f fps)",
                    1e3f * bf.frame.last_delta_time(),
                    bf.frame.last_fps()
                );

                {
                    struct PlotData {
                        std::span<const float> data;
                        uint32_t offset;
                    };
                    const auto plot_data = PlotData {
                        .data = bf.frame.delta_time_history(),
                        .offset = (uint32_t)bf.frame.delta_time_history_index(),
                    };
                    ImGui::PlotLines(
                        "Frame time (ms)",
                        [](void* data, int32_t idx) -> float {
                            const auto& plot_data = *(PlotData*)data;
                            const auto i = (plot_data.offset + idx) % plot_data.data.size();
                            return 1e3f * plot_data.data[i];
                        },
                        (void*)&plot_data,
                        (int32_t)plot_data.data.size(),
                        0,
                        nullptr,
                        0.0f,
                        1.0f / 240.0f
                    );
                }

                const auto vmem = bf.device.video_memory_info();
                ImGui::Text(
                    "Video memory: %.2f MB / %.2f MB (%.2f%%)",
                    (float)vmem.local.current_usage / 1e6f,
                    (float)vmem.local.budget / 1e6f,
                    100.0f * (float)vmem.local.current_usage / (float)vmem.local.budget
                );
                ImGui::Text(
                    "System memory: %.2f MB / %.2f MB (%.2f%%)",
                    (float)vmem.non_local.current_usage / 1e6f,
                    (float)vmem.non_local.budget / 1e6f,
                    100.0f * (float)vmem.non_local.current_usage / (float)vmem.non_local.budget
                );

                ImGui::Text("Capture:");
                ImGui::SameLine();
                if (ImGui::Button("PIX")) {
                    bf.device.pix_capture();
                }
                ImGui::SameLine();
                if (ImGui::Button("Tracy")) {
                    fb::tracy_capture();
                }

                demos::gui(
                    bf.demos,
                    {
                        .window_size = bf.device.swapchain().size(),
                        .frame_index = bf.device.frame_index(),
                    }
                );
            }
            ImGui::End();
            bf.gui.end_frame();

            // Inputs.
            gui_wants_the_mouse = ImGui::GetIO().WantCaptureMouse;
        }

        // Update demos.
        {
            FB_PERF_SCOPE("Demos");

            demos::update(
                bf.demos,
                demos::UpdateDesc {
                    .window_size = bf.device.swapchain().size(),
                    .aspect_ratio = WINDOW_ASPECT_RATIO,
                    .delta_time = bf.frame.smoothed_delta_time(),
                    .elapsed_time = bf.frame.elapsed_time(),
                    .frame_index = bf.device.frame_index(),
                    .inputs = bf.window.inputs(),
                    .gui_wants_the_mouse = gui_wants_the_mouse,
                }
            );
        }

        // Rendering.
        {
            auto& swapchain = bf.device.swapchain();
            const auto frame_index = bf.device.frame_index();

            FB_PERF_SCOPE("Rendering");

            auto cmd = bf.device.begin_frame();

            FB_PERF_PIX_BEGIN_EVENT("Frame");
            cmd.pix_begin("Frame %zu", bf.frame.count());

            const auto render_desc = demos::RenderDesc {
                .cmd = cmd,
                .device = bf.device,
                .frame_index = frame_index,
            };

            cmd.pix_begin("Transition to Clear");
            demos::transition_to_render_target(bf.demos, render_desc);
            swapchain.transition_to_render_target(cmd, frame_index);
            cmd.flush_barriers();
            cmd.pix_end();

            cmd.pix_begin("Clear");
            demos::clear_render_targets(bf.demos, render_desc);
            swapchain.clear_render_target(cmd, frame_index);
            cmd.pix_end();

            cmd.pix_begin("Demos");
            demos::render_demos(bf.demos, render_desc);
            cmd.pix_end();

            cmd.pix_begin("Transition to Resolve");
            demos::transition_to_resolve(bf.demos, render_desc);
            cmd.flush_barriers();
            cmd.pix_end();

            cmd.pix_begin("Resolve");
            demos::resolve_render_targets(bf.demos, render_desc);
            cmd.pix_end();

            cmd.pix_begin("Transition to SRV");
            demos::transition_to_shader_resource(bf.demos, render_desc);
            cmd.flush_barriers();
            cmd.pix_end();

            cmd.pix_begin("Compose");
            cmd.graphics_scope([&swapchain, frame_index](GpuGraphicsCommandList& cmd) {
                swapchain.set_render_target(cmd, frame_index);
            });
            demos::render_compose(bf.demos, render_desc);
            bf.gui.render(bf.device, cmd);
            cmd.pix_end();

            cmd.pix_begin("Transition to Present");
            swapchain.transition_to_present(cmd, frame_index);
            cmd.flush_barriers();
            cmd.pix_end();

            cmd.pix_end();
            FB_PERF_PIX_END_EVENT();

            bf.device.end_frame(std::move(cmd));
        }

        // Present.
        {
            FB_PERF_SCOPE("Present");
            bf.device.present();
        }

        // Update frame.
        bf.frame.update();
    }
    FRAME_ALLOCATION_TRAP = false;

    // Wait for pending GPU work to complete before running destructors.
    bf.device.wait();

    // Archive one more time.
    {
        auto archive = SerializingArchive(bf.archive_buf);
        demos::archive(bf.demos, archive);
        write_whole_file(ARCHIVE_FILE_NAME, bf.archive_buf);
        FB_LOG_INFO("Final archive size: {} bytes", bf.archive_buf.size());
    }
}

} // namespace fb

//
// Main.
//

auto main() -> int {
    FB_LOG_INFO("sizeof(Buffet): {} bytes", sizeof(fb::Buffet));
    auto buffet = std::make_unique<fb::Buffet>();
    fb::buffet_run(*buffet);
    return 0;
}
