// Framebuffet
#include "maths.hpp"
#include "utils.hpp"
#include "pcg.hpp"
#include "win32.hpp"
#include "gui.hpp"
#include "exr.hpp"

// Framebuffet - demos
#include "cards.hpp"
#include "demo/cube.hpp"
#include "demo/rain.hpp"
#include "demo/tree.hpp"
#include "demo/env.hpp"

// PCH.
#include <pch.hpp>

// Constants.
constexpr std::string_view WINDOW_TITLE = "framebuffet 😎"sv;
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 800;
constexpr float WINDOW_ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

//
// GUI.
//

template<typename T>
concept GuiWrappable = requires(T demo, const fb::gui::Desc& desc) {
    { demo->NAME } -> std::convertible_to<std::string_view>;
    { demo->gui(desc) } -> std::same_as<void>;
};

template<typename T>
constexpr auto gui_wrapper(T& demo, const fb::gui::Desc& desc) -> void
    requires GuiWrappable<T>
{
    auto name = demo->NAME;
    ImGui::PushID(name.data());
    if (ImGui::CollapsingHeader(name.data())) {
        demo->gui(desc);
    }
    ImGui::PopID();
};

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
    auto env_demo = std::make_unique<fb::env::Demo>(*device);
    auto cards = std::make_unique<fb::cards::Cards>(
        *device,
        fb::cards::Params {
            .card_texture_descriptors = {
                cube_demo->rt_color().srv_descriptor(),
                rain_demo->rt_color().srv_descriptor(),
                tree_demo->rt_color().srv_descriptor(),
                env_demo->rt_color().srv_descriptor(),
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

        // Update gui.
        {
            gui->begin_frame();
            ImGui::SetNextWindowSize({300, 300}, ImGuiCond_FirstUseEver);
            if (ImGui::Begin("framebuffet")) {
                const auto desc = fb::gui::Desc {.window_size = device->swapchain_size()};
                gui_wrapper(cube_demo, desc);
                gui_wrapper(rain_demo, desc);
                gui_wrapper(tree_demo, desc);
                gui_wrapper(env_demo, desc);
                gui_wrapper(cards, desc);
            }
            ImGui::End();
            gui->end_frame();
        }

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
        env_demo->update(update_desc);
        cards->update(*device);

        // Begin frame.
        auto cmd = device->begin_frame();
        cmd.begin_pix(std::format("Frame {}", frame_count));

        // Demo pass.
        {
            cmd.begin_pix("Demo pass");

            cmd.begin_pix("Cube");
            cube_demo->render(*device, cmd);
            cmd.end_pix();

            cmd.begin_pix("Rain");
            rain_demo->render(*device, cmd);
            cmd.end_pix();

            cmd.begin_pix("Tree");
            tree_demo->render(*device, cmd);
            cmd.end_pix();

            cmd.begin_pix("Env");
            env_demo->render(*device, cmd);
            cmd.end_pix();

            cmd.end_pix();
        }

        // Main pass.
        {
            cmd.begin_pix("Main pass");
            device->begin_main_pass();

            cmd.begin_pix("Cards");
            cards->render(*device, cmd);
            cmd.end_pix();

            cmd.begin_pix("Gui");
            gui->render(*device, cmd);
            cmd.end_pix();

            device->end_main_pass();
            cmd.end_pix();
        }

        // End frame.
        cmd.end_pix();
        device->end_frame(std::move(cmd));

        // Update frame count.
        frame_count++;
    }

    // Wait for pending GPU work to complete.
    device->wait();

    return 0;
}
