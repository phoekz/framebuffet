#include <common/common.hpp>
#include <kitchen/kitchen.hpp>
#include <baked/stockcube/baked.hpp>
#include "techniques/techniques.hpp"

namespace fb {

//
// Constants.
//

inline constexpr std::string_view WINDOW_TITLE = "stockcube 😎"sv;
inline constexpr int WINDOW_WIDTH = 1280;
inline constexpr int WINDOW_HEIGHT = 832;

//
// Stockcube.
//

struct Stockcube {
    using Techniques = techniques::Techniques;
    using Baked = techniques::Baked;
    using Camera = camera::Camera;

    Window window;
    GpuDevice device;
    Baked baked;
    Gui gui;
    Techniques techniques;
    Frame frame;
    Camera camera;
};

auto stockcube_run(Stockcube& sc) -> void {
    // Console.
    attach_console();

    // Init.
    {
        DebugScope debug("Stockcube");
        sc.window.create(Window::Desc {WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT});
        sc.device.create(sc.window);
        sc.device.begin_transfer();
        sc.gui.create(sc.window, sc.device, sc.baked.kitchen.assets, sc.baked.kitchen.shaders);
        techniques::create(sc.techniques, {.baked = sc.baked, .device = sc.device});
        sc.device.end_transfer();
        sc.device.log_stats();
        sc.frame.create();
        sc.camera.create({
            .aspect_ratio = sc.device.swapchain().aspect_ratio(),
            .fov = rad_from_deg(90.0f),
            .distance_from_origin = 4.0f,
        });
    }

    // Main loop.
    bool running = true;
    while (running) {
        PIXScopedEvent(PIX_COLOR_DEFAULT, "Frame %zu", sc.frame.count());

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

        // Update window.
        sc.window.update();

        // Update gui.
        bool gui_wants_the_mouse = false;
        {
            PIXScopedEvent(PIX_COLOR_DEFAULT, "Gui");

            // Build gui.
            sc.gui.begin_frame();
            ImGui::SetNextWindowSize({300, 300}, ImGuiCond_FirstUseEver);
            if (ImGui::Begin(techniques::NAME.data())) {
                ImGui::Text(
                    "Frame time: %.3f ms (%.3f fps)",
                    1e3f * sc.frame.last_delta_time(),
                    sc.frame.last_fps()
                );
                if (ImGui::Button("PIX Capture")) {
                    sc.device.pix_capture();
                }

                techniques::gui(sc.techniques, {});
            }
            ImGui::End();
            sc.gui.end_frame();

            // Inputs.
            gui_wants_the_mouse = ImGui::GetIO().WantCaptureMouse;
        }

        // Update techniques.
        {
            PIXScopedEvent(PIX_COLOR_DEFAULT, "Update");

            const auto& inputs = sc.window.inputs();
            sc.camera.update({
                .delta_time = sc.frame.delta_time(),
                .cursor_position = float2((float)inputs.mouse_x, (float)inputs.mouse_y),
                .mouse_left = gui_wants_the_mouse ? false : inputs.mouse_left,
                .mouse_wheel_y = gui_wants_the_mouse ? 0.0f : inputs.mouse_wheel_y,
            });

            const auto size = sc.device.swapchain().size();
            const auto aspect_ratio = (float)size.x / (float)size.y;
            const auto projection = sc.camera.clip_from_view();
            const auto view = sc.camera.view_from_world();
            const auto eye = sc.camera.position();
            const auto frame_index = sc.device.swapchain().backbuffer_index();

            techniques::update(
                sc.techniques,
                {
                    .window_size = size,
                    .aspect_ratio = aspect_ratio,
                    .camera_view = view,
                    .camera_projection = projection,
                    .camera_position = eye,
                    .frame_index = frame_index,
                }
            );
        }

        // Rendering.
        {
            PIXBeginEvent(PIX_COLOR_DEFAULT, "Rendering");

            PIXBeginEvent(PIX_COLOR_DEFAULT, "Prepare");
            auto cmd = sc.device.begin_frame();
            PIXEndEvent();

            PIXBeginEvent(PIX_COLOR_DEFAULT, "Commands");
            {
                auto& swapchain = sc.device.swapchain();
                const auto frame_index = sc.device.frame_index();
                auto& render_targets = sc.techniques.render_targets;

                cmd.begin_pix("Frame %zu", sc.frame.count());

                {
                    cmd.begin_pix("Clear");
                    render_targets.transition_to_render_target(cmd);
                    swapchain.transition_to_render_target(cmd, frame_index);
                    cmd.flush_barriers();
                    render_targets.clear_all(cmd);
                    swapchain.clear_render_target(cmd, frame_index);
                    cmd.end_pix();
                }

                {
                    cmd.begin_pix("Techniques");
                    const auto desc = techniques::RenderDesc {.cmd = cmd, .device = sc.device};
                    techniques::render_main(sc.techniques, render_targets, desc);
                    cmd.end_pix();
                }

                {
                    cmd.begin_pix("Resolve");
                    render_targets.transition_to_resolve(cmd);
                    cmd.flush_barriers();
                    render_targets.resolve_all(cmd);
                    render_targets.transition_to_pixel_shader_resource(cmd);
                    cmd.flush_barriers();
                    cmd.end_pix();
                }

                {
                    cmd.begin_pix("Compose");

                    swapchain.set_render_target(cmd, frame_index);

                    const auto desc = techniques::RenderDesc {.cmd = cmd, .device = sc.device};
                    techniques::render_compositing(sc.techniques, desc);
                    sc.gui.render(sc.device, cmd);

                    swapchain.transition_to_present(cmd, frame_index);
                    cmd.flush_barriers();

                    cmd.end_pix();
                }

                cmd.end_pix();
            }
            PIXEndEvent();

            PIXBeginEvent(PIX_COLOR_DEFAULT, "Present");
            sc.device.end_frame(std::move(cmd));
            PIXEndEvent();

            PIXEndEvent();
        }

        // Update frame.
        sc.frame.update();
    }

    // Wait for pending GPU work to complete before running destructors.
    sc.device.wait();
}

} // namespace fb

int main() {
    FB_LOG_INFO("sizeof(Stockcube): {} bytes", sizeof(fb::Stockcube));
    auto stockcube = std::make_unique<fb::Stockcube>();
    fb::stockcube_run(*stockcube);
    return 0;
}
