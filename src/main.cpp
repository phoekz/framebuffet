// Framebuffet
#include "maths.hpp"
#include "utils.hpp"
#include "pcg.hpp"
#include "win32.hpp"
#include "gui.hpp"

// Framebuffet - demos
#include "cards.hpp"
#include "demo/cube.hpp"
#include "demo/rain.hpp"
#include "demo/tree.hpp"

// PCH.
#include <pch.hpp>

// Constants.
constexpr std::string_view WINDOW_TITLE = "framebuffet 😎"sv;
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 800;
constexpr float WINDOW_ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

//
// Main.
//

auto main() -> int {
    // Optimistically try to attach to the parent process console to capture
    // stdout/stderr prints. It is useful when we run framebuffet without a
    // debugger, but within a console instead. It's not a big deal if this
    // fails.
    AttachConsole(ATTACH_PARENT_PROCESS);

    // Init.
    auto window =
        std::make_unique<fb::Window>(fb::Window::Desc {WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT});
    auto device = std::make_unique<fb::GpuDevice>(*window);
    auto gui = std::make_unique<fb::gui::Gui>(*window, *device);
    auto cube_demo = std::make_unique<fb::cube::Demo>(*device);
    auto rain_demo = std::make_unique<fb::rain::Demo>(*device);
    auto tree_demo = std::make_unique<fb::tree::Demo>(*device);
    auto cards = std::make_unique<fb::cards::Cards>(
        *device,
        fb::cards::Params {
            .card_textures = {
                cube_demo->render_targets.color(),
                rain_demo->render_targets.color(),
                tree_demo->render_targets.color(),
            }});

    // Main loop.
    bool running = true;
    uint64_t frame_count = 0;
    fb::FrameTiming ft = {};
    while (running) {
        // Handle window messages.
        {
            MSG msg = {};
            if (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageA(&msg);
            }
            if (msg.message == WM_QUIT) {
                running = false;
            }
        }

        // Update frame timing.
        ft.update();

        // Update Dear ImGui.
        gui->update();

        // Update demos.
        fb::demo::UpdateDesc update_desc = {
            .aspect_ratio = WINDOW_ASPECT_RATIO,
            .delta_time = ft.delta_time(),
            .elapsed_time = ft.elapsed_time(),
            .frame_index = device->frame_index(),
        };
        cube_demo->update(update_desc);
        rain_demo->update(update_desc);
        tree_demo->update(update_desc);
        cards->update(*device);

        // Begin frame.
        device->begin_frame();
        auto* cmd = device->command_list();
        PIXBeginEvent(cmd, PIX_COLOR_DEFAULT, std::format("Frame {}", frame_count).c_str());

        // Demo pass.
        {
            PIXBeginEvent(cmd, PIX_COLOR_DEFAULT, "Demo pass");

            PIXBeginEvent(cmd, PIX_COLOR_DEFAULT, "Cube");
            cube_demo->render(*device);
            PIXEndEvent(cmd);

            PIXBeginEvent(cmd, PIX_COLOR_DEFAULT, "Rain");
            rain_demo->render(*device);
            PIXEndEvent(cmd);

            PIXBeginEvent(cmd, PIX_COLOR_DEFAULT, "Tree");
            tree_demo->render(*device);
            PIXEndEvent(cmd);

            PIXEndEvent(cmd);
        }

        // Main pass.
        {
            PIXBeginEvent(cmd, PIX_COLOR_DEFAULT, "Main pass");
            device->begin_main_pass();

            PIXBeginEvent(cmd, PIX_COLOR_DEFAULT, "Cards");
            cards->render(*device);
            PIXEndEvent(cmd);

            PIXBeginEvent(cmd, PIX_COLOR_DEFAULT, "Gui");
            gui->render(*device);
            PIXEndEvent(cmd);

            device->end_main_pass();
            PIXEndEvent(cmd);
        }

        // End frame.
        PIXEndEvent(cmd);
        device->end_frame();

        // Update frame count.
        frame_count++;
    }

    // Wait for pending GPU work to complete.
    device->wait();

    // Cleanup.
    cards = nullptr;
    tree_demo = nullptr;
    rain_demo = nullptr;
    cube_demo = nullptr;
    gui = nullptr;
    device = nullptr;
    window = nullptr;

    return 0;
}
