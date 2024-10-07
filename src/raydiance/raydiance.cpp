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
    // Console.
    attach_console();

    // Main thread.
    FB_ASSERT_HR(SetThreadDescription(GetCurrentThread(), L"Main Thread"));

    // Tracy configuration.
    TracySetProgramName("raydiance");

    // Init.
    {
        ZoneScopedN("Init");
        PIXScopedEvent(PIX_COLOR_DEFAULT, "Init");

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
        FrameMark;
        PIXScopedEvent(PIX_COLOR_DEFAULT, "Frame %zu", rd.frame.count());

        // Handle window messages.
        {
            ZoneScopedN("Events");
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

        // Update window.
        rd.window.update();

        // Update gui.
        bool gui_wants_the_mouse = false;
        {
            PIXScopedEvent(PIX_COLOR_DEFAULT, "Gui");

            // Build gui.
            rd.gui.begin_frame();
            ImGui::SetNextWindowSize({300, 300}, ImGuiCond_FirstUseEver);
            if (ImGui::Begin("Raydiance")) {}
            ImGui::End();
            rd.gui.end_frame();

            // Inputs.
            gui_wants_the_mouse = ImGui::GetIO().WantCaptureMouse;
        }

        // Rendering.
        {
            PIXBeginEvent(PIX_COLOR_DEFAULT, "Rendering");

            PIXBeginEvent(PIX_COLOR_DEFAULT, "Prepare");
            auto cmd = rd.device.begin_frame();
            PIXEndEvent();

            PIXBeginEvent(PIX_COLOR_DEFAULT, "Commands");
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
            PIXEndEvent();

            PIXBeginEvent(PIX_COLOR_DEFAULT, "Present");
            rd.device.end_frame(std::move(cmd));
            PIXEndEvent();

            PIXEndEvent();
        }

        // Present.
        {
            PIXScopedEvent(PIX_COLOR_DEFAULT, "Present");
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
