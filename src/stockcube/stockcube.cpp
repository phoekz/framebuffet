#include <common/common.hpp>
#include <kitchen/kitchen.hpp>
#include <baked/stockcube/baked.hpp>

#define USE_PIX
#include <WinPixEventRuntime/pix3.h>

#include "screen.hlsli"

namespace fb {

//
// Constants.
//

inline constexpr std::string_view WINDOW_TITLE = "stockcube 😎"sv;
inline constexpr int WINDOW_WIDTH = 1280;
inline constexpr int WINDOW_HEIGHT = 832;

//
// Baked.
//

struct Baked {
    struct {
        baked::kitchen::Assets assets;
        baked::kitchen::Shaders shaders;
    } kitchen;
    struct {
        baked::stockcube::Assets assets;
        baked::stockcube::Shaders shaders;
    } stockcube;
};

//
// Stockcube.
//

struct Stockcube {
    auto run() -> void;

    // Systems.
    Window window;
    GpuDevice device;
    Baked baked;
    graphics::gui::Gui gui;
    Frame frame;
};

auto Stockcube::run() -> void {
    // Console.
    attach_console();

    // Init.
    {
        auto timer = Instant();
        window.create(Window::Desc {WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT});
        device.create(window);
        device.begin_transfer();
        gui.create(window, device, baked.kitchen.assets, baked.kitchen.shaders);
        device.end_transfer();
        device.log_stats();
        frame.create();
    }

    // Main loop.
    bool running = true;
    while (running) {
        PIXScopedEvent(PIX_COLOR_DEFAULT, "Main - Frame %zu", frame.count());

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

        // Update gui.
        {
            PIXScopedEvent(PIX_COLOR_DEFAULT, "Gui");

            auto timer = fb::Instant();
            gui.begin_frame();
            ImGui::SetNextWindowSize({300, 300}, ImGuiCond_FirstUseEver);
            if (ImGui::Begin("Stockcube")) {
                ImGui::Text(
                    "Frame time: %.3f ms (%.3f fps)",
                    1e3f * frame.last_delta_time(),
                    frame.last_fps()
                );

                if (ImGui::Button("PIX Capture")) {
                    device.pix_capture();
                }
            }
            ImGui::End();
            gui.end_frame();
        }

        // Graphics.
        {
            PIXBeginEvent(PIX_COLOR_DEFAULT, "Graphics");

            PIXBeginEvent(PIX_COLOR_DEFAULT, "Prepare");
            auto cmd = device.begin_frame();
            PIXEndEvent(); // Prepare.

            PIXBeginEvent(PIX_COLOR_DEFAULT, "Commands");
            {
                auto& swapchain = device.swapchain();
                const auto frame_index = device.frame_index();

                cmd.begin_pix("Frame %zu", frame.count());

                {
                    cmd.begin_pix("Clear render targets");
                    cmd.set_graphics();
                    swapchain.transition_to_render_target(cmd, frame_index);
                    cmd.flush_barriers();
                    swapchain.clear_render_target(cmd, frame_index);
                    cmd.end_pix();
                }

                {
                    cmd.begin_pix("Main pass");

                    swapchain.set_render_target(cmd, frame_index);

                    cmd.begin_pix(gui.NAME.data());
                    gui.render(device, cmd);
                    cmd.end_pix();

                    swapchain.transition_to_present(cmd, frame_index);
                    cmd.flush_barriers();

                    cmd.end_pix();
                }

                cmd.end_pix();
            }
            PIXEndEvent(); // Commands.

            PIXBeginEvent(PIX_COLOR_DEFAULT, "Present");
            device.end_frame(std::move(cmd));
            PIXEndEvent(); // Present.

            PIXEndEvent(); // Graphics.
        }

        // Update frame.
        frame.update();
    }

    // Wait for pending GPU work to complete before running destructors.
    device.wait();
}

} // namespace fb

int main() {
    FB_LOG_INFO("sizeof(Stockcube): {} bytes", sizeof(fb::Stockcube));
    auto stockcube = std::make_unique<fb::Stockcube>();
    stockcube->run();
    return 0;
}
