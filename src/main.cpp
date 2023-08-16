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
#include "demo/anim.hpp"
#include "demo/fibers.hpp"

// PCH.
#include <pch.hpp>

// Constants.
inline constexpr std::string_view WINDOW_TITLE = "framebuffet 😎"sv;
inline constexpr int WINDOW_WIDTH = 1280;
inline constexpr int WINDOW_HEIGHT = 800;
inline constexpr float WINDOW_ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

//
// GUI.
//

template<typename T>
concept GuiWrappable = requires(T demo, const fb::gui::Desc& desc) {
    { demo->NAME } -> std::convertible_to<std::string_view>;
    { demo->gui(desc) } -> std::same_as<void>;
};

template<typename T>
inline constexpr auto
gui_wrapper(T& demo, const fb::gui::Desc& desc, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None)
    -> void
    requires GuiWrappable<T>
{
    auto name = demo->NAME;
    ImGui::PushID(name.data());
    if (ImGui::CollapsingHeader(name.data(), nullptr, flags)) {
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
    auto anim_demo = std::make_unique<fb::anim::Demo>(*device);
    auto fibers_demo = std::make_unique<fb::fibers::Demo>(*device);
    auto cards = std::make_unique<fb::cards::Cards>(
        *device,
        fb::cards::Params {
            .card_texture_descriptors = {
                cube_demo->rt_color().srv_descriptor(),
                rain_demo->rt_color().srv_descriptor(),
                tree_demo->rt_color().srv_descriptor(),
                env_demo->rt_color().srv_descriptor(),
                anim_demo->rt_color().srv_descriptor(),
                fibers_demo->rt_color().srv_descriptor(),
            }});
    device->log_stats();

    // Main loop.
    bool running = true;
    uint64_t frame_count = 0;
    fb::FrameTiming ft = {};
    double update_time = 0.0;
    double gui_time = 0.0;
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
            auto timer = fb::Instant();
            gui->begin_frame();
            ImGui::SetNextWindowSize({300, 300}, ImGuiCond_FirstUseEver);
            if (ImGui::Begin("Framebuffet")) {
                ImGui::Text(
                    "Frame time: %.3f ms (%.3f fps)",
                    1e3f * ft.last_delta_time(),
                    ft.last_fps());
                ImGui::Text("Update time: %.3f ms", 1e3f * update_time);
                ImGui::Text("GUI time: %.3f ms", 1e3f * gui_time);

                const auto desc = fb::gui::Desc {.window_size = device->swapchain_size()};
                gui_wrapper(cards, desc, ImGuiTreeNodeFlags_DefaultOpen);
                gui_wrapper(cube_demo, desc);
                gui_wrapper(rain_demo, desc);
                gui_wrapper(tree_demo, desc);
                gui_wrapper(env_demo, desc);
                gui_wrapper(anim_demo, desc);
                gui_wrapper(fibers_demo, desc);
            }
            ImGui::End();
            gui->end_frame();
            gui_time = timer.elapsed_time();
        }

        // Update demos.
        {
            auto timer = fb::Instant();
            fb::demo::UpdateDesc update_desc = {
                .window_size = device->swapchain_size(),
                .aspect_ratio = WINDOW_ASPECT_RATIO,
                .delta_time = ft.delta_time(),
                .elapsed_time = ft.elapsed_time(),
                .frame_index = device->frame_index(),
            };
            cube_demo->update(update_desc);
            rain_demo->update(update_desc);
            tree_demo->update(update_desc);
            env_demo->update(update_desc);
            anim_demo->update(update_desc);
            fibers_demo->update(update_desc);
            cards->update(*device);
            update_time = timer.elapsed_time();
        }

        // Begin frame.
        auto cmd = device->begin_frame();
        cmd.begin_pix(std::format("Frame {}", frame_count));

        // Demo pass.
        {
            cmd.begin_pix("Demo pass");

            cmd.begin_pix(cube_demo->NAME);
            cube_demo->render(*device, cmd);
            cmd.end_pix();

            cmd.begin_pix(rain_demo->NAME);
            rain_demo->render(*device, cmd);
            cmd.end_pix();

            cmd.begin_pix(tree_demo->NAME);
            tree_demo->render(*device, cmd);
            cmd.end_pix();

            cmd.begin_pix(env_demo->NAME);
            env_demo->render(*device, cmd);
            cmd.end_pix();

            cmd.begin_pix(anim_demo->NAME);
            anim_demo->render(*device, cmd);
            cmd.end_pix();

            cmd.begin_pix(fibers_demo->NAME);
            fibers_demo->render(*device, cmd);
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
