#include "pch.hpp"
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
    using Baked = demos::Baked;
    using Demos = demos::Demos;

    Window window;
    GpuDevice device;
    Baked baked;
    Demos demos;
    Gui gui;
    Frame frame;
    std::vector<std::byte> archive_buf;
    double time_since_last_archive = 0.0;
};

auto buffet_run(Buffet& bf) -> void {
    // Console.
    attach_console();

    // Init.
    {
        auto timer = Instant();
        DebugScope debug("Buffet");
        bf.window.create(Window::Desc {WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT});
        bf.device.create(bf.window);
        bf.device.begin_transfer();
        demos::create(bf.demos, {.baked = bf.baked, .device = bf.device});
        bf.gui.create(bf.window, bf.device, bf.baked.kitchen.assets, bf.baked.kitchen.shaders);
        bf.device.end_transfer();
        bf.device.log_stats();
        bf.frame.create();
        bf.archive_buf.reserve(1024);
        FB_LOG_INFO("Init time: {} ms", 1e3f * timer.elapsed_time());
    }

    // Archive.
    if (file_exists(ARCHIVE_FILE_NAME)) {
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
        PIXScopedEvent(PIX_COLOR_DEFAULT, "Frame %zu", bf.frame.count());

        // Handle window messages.
        {
            PIXScopedEvent(PIX_COLOR_DEFAULT, "Events");

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
            PIXSetMarker(PIX_COLOR_DEFAULT, "Archive");

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
            PIXScopedEvent(PIX_COLOR_DEFAULT, "Gui");

            // Build gui.
            bf.gui.begin_frame();
            ImGui::SetNextWindowSize({300, 300}, ImGuiCond_FirstUseEver);
            if (ImGui::Begin(demos::NAME.data())) {
                ImGui::Text(
                    "Frame time: %.3f ms (%.3f fps)",
                    1e3f * bf.frame.last_delta_time(),
                    bf.frame.last_fps()
                );
                if (ImGui::Button("PIX Capture")) {
                    bf.device.pix_capture();
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
            PIXScopedEvent(PIX_COLOR_DEFAULT, "Update");

            demos::update(
                bf.demos,
                demos::UpdateDesc {
                    .window_size = bf.device.swapchain().size(),
                    .aspect_ratio = WINDOW_ASPECT_RATIO,
                    .delta_time = bf.frame.delta_time(),
                    .elapsed_time = bf.frame.elapsed_time(),
                    .frame_index = bf.device.frame_index(),
                }
            );
        }

        // Rendering.
        {
            auto& swapchain = bf.device.swapchain();
            const auto frame_index = bf.device.frame_index();

            PIXBeginEvent(PIX_COLOR_DEFAULT, "Rendering");

            PIXBeginEvent(PIX_COLOR_DEFAULT, "Prepare");
            auto cmd = bf.device.begin_frame();
            PIXEndEvent();

            PIXBeginEvent(PIX_COLOR_DEFAULT, "Frame");
            cmd.begin_pix("Frame %zu", bf.frame.count());

            const auto render_desc = demos::RenderDesc {
                .cmd = cmd,
                .device = bf.device,
                .frame_index = frame_index,
            };

            cmd.begin_pix("Transition to Clear");
            demos::transition_to_render_target(bf.demos, render_desc);
            swapchain.transition_to_render_target(cmd, frame_index);
            cmd.flush_barriers();
            cmd.end_pix();

            cmd.begin_pix("Clear");
            demos::clear_render_targets(bf.demos, render_desc);
            swapchain.clear_render_target(cmd, frame_index);
            cmd.end_pix();

            cmd.begin_pix("Demos");
            demos::render_demos(bf.demos, render_desc);
            cmd.end_pix();

            cmd.begin_pix("Transition to Resolve");
            demos::transition_to_resolve(bf.demos, render_desc);
            cmd.flush_barriers();
            cmd.end_pix();

            cmd.begin_pix("Resolve");
            demos::resolve_render_targets(bf.demos, render_desc);
            cmd.end_pix();

            cmd.begin_pix("Transition to SRV");
            demos::transition_to_srv(bf.demos, render_desc);
            cmd.flush_barriers();
            cmd.end_pix();

            cmd.begin_pix("Compose");
            swapchain.set_render_target(cmd, frame_index);
            demos::render_compose(bf.demos, render_desc);
            bf.gui.render(bf.device, cmd);
            cmd.end_pix();

            cmd.begin_pix("Transition to Present");
            swapchain.transition_to_present(cmd, frame_index);
            cmd.flush_barriers();
            cmd.end_pix();

            cmd.end_pix();
            PIXEndEvent();

            PIXBeginEvent(PIX_COLOR_DEFAULT, "Present");
            bf.device.end_frame(std::move(cmd));
            PIXEndEvent();

            PIXEndEvent();
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

int main() {
    FB_LOG_INFO("sizeof(Buffet): {} bytes", sizeof(fb::Buffet));
    auto buffet = std::make_unique<fb::Buffet>();
    fb::buffet_run(*buffet);
    return 0;
}
