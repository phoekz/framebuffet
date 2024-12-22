#include <common/common.hpp>
#include <kitchen/kitchen.hpp>

namespace fb {

//
// Constants.
//

inline constexpr std::string_view WINDOW_TITLE = "raydiance 😎"sv;
inline constexpr int WINDOW_WIDTH = 1280;
inline constexpr int WINDOW_HEIGHT = 832;

//
// Raydiance.
//

struct Raydiance {
    Window window;
    GpuDevice device;
    KcnGui gui;
    Frame frame;
};

auto raydiance_run(Raydiance& rd) -> void {
    // Time since process start.
    const auto time_since_process_start = fb::get_time_since_process_start();

    // Console.
    attach_console();

    // Log time since process start.
    FB_LOG_INFO("Time since process start: {} s", time_since_process_start);

    // Main thread.
    FB_ASSERT_HR(SetThreadDescription(GetCurrentThread(), L"Main Thread"));

    // Tracy configuration.
    TracySetProgramName("raydiance");

    // Init.
    {
        FB_PERF_SCOPE("Init");

        DebugScope debug("Raydiance");
        baked::kitchen::Assets kitchen_assets;
        baked::kitchen::Shaders kitchen_shaders;
        kitchen_assets.load();
        kitchen_shaders.load();
        rd.window.create(Window::Desc {WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT});
        rd.device.create(rd.window);
        rd.gui.create(rd.window, rd.device, kitchen_assets, kitchen_shaders);
        rd.device.flush_transfers();
        rd.device.log_stats();
        rd.frame.create();
    }

    // Main loop.
    bool running = true;
    while (running) {
        FB_PERF_FRAME(rd.frame.count());

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
        rd.window.update();

        // Update gui.
        bool gui_wants_the_mouse = false;
        {
            FB_PERF_SCOPE("Gui");

            // Build gui.
            rd.gui.begin_frame();
            ImGui::SetNextWindowSize({300, 300}, ImGuiCond_FirstUseEver);
            if (ImGui::Begin("Raydiance")) {
                ImGui::Text(
                    "Frame time: %.2f ms (%.2f fps)",
                    1e3f * rd.frame.last_delta_time(),
                    rd.frame.last_fps()
                );

                ImGui::Text("Capture:");
                ImGui::SameLine();
                if (ImGui::Button("PIX")) {
                    rd.device.pix_capture();
                }
                ImGui::SameLine();
                if (ImGui::Button("Tracy")) {
                    fb::tracy_capture();
                }
            }
            ImGui::End();
            rd.gui.end_frame();

            // Inputs.
            gui_wants_the_mouse = ImGui::GetIO().WantCaptureMouse;
        }

        // Rendering.
        {
            FB_PERF_SCOPE("Rendering");

            auto cmd = rd.device.begin_frame();

            auto& swapchain = rd.device.swapchain();
            const auto frame_index = rd.device.frame_index();
            swapchain.transition_to_render_target(cmd, frame_index);
            cmd.flush_barriers();
            swapchain.clear_render_target(cmd, frame_index);
            cmd.graphics_scope([&swapchain, frame_index](GpuGraphicsCommandList& cmd) {
                swapchain.set_render_target(cmd, frame_index);
            });
            rd.gui.render(rd.device, cmd);
            swapchain.transition_to_present(cmd, frame_index);
            cmd.flush_barriers();

            rd.device.end_frame(std::move(cmd));
        }

        // Present.
        {
            FB_PERF_SCOPE("Present");
            rd.device.present();
        }

        // Update frame.
        rd.frame.update();
    }

    // Wait for pending GPU work to complete before running destructors.
    rd.device.wait();
}

} // namespace fb

//
// Main.
//

auto main() -> int {
    FB_LOG_INFO("sizeof(Raydiance): {} bytes", sizeof(fb::Raydiance));
    auto raydiance = std::make_unique<fb::Raydiance>();
    fb::raydiance_run(*raydiance);
    return 0;
}
