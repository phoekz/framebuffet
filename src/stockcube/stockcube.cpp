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

    Window window;
    GpuDevice device;
    KcnGui gui;
    Techniques techniques;
    Frame frame;
    KcnCamera camera;
};

auto stockcube_run(Stockcube& sc) -> void {
    // Time since process start.
    const auto time_since_process_start = fb::get_time_since_process_start();

    // Console.
    attach_console();

    // Log time since process start.
    FB_LOG_INFO("Time since process start: {} s", time_since_process_start);

    // Main thread.
    FB_ASSERT_HR(SetThreadDescription(GetCurrentThread(), L"Main Thread"));

    // Tracy configuration.
    FB_PERF_TRACY_SET_PROGRAM_NAME("stockcube");

    // Init.
    {
        FB_PERF_SCOPE("Init");

        DebugScope debug("Stockcube");
        techniques::Baked baked;
        baked.kitchen.assets.load();
        baked.kitchen.shaders.load();
        baked.stockcube.assets.load();
        baked.stockcube.shaders.load();
        sc.window.create(Window::Desc {WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT});
        sc.device.create(sc.window);
        sc.gui.create(sc.window, sc.device, baked.kitchen.assets, baked.kitchen.shaders);
        techniques::create(sc.techniques, {.baked = baked, .device = sc.device});
        sc.device.flush_transfers();
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
        FB_PERF_FRAME(sc.frame.count());

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
        sc.window.update();

        // Update gui.
        bool gui_wants_the_mouse = false;
        {
            FB_PERF_SCOPE("Gui");

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
            FB_PERF_SCOPE("Update");

            const auto& inputs = sc.window.inputs();
            sc.camera.update({
                .delta_time = sc.frame.delta_time(),
                .cursor_position = float2((float)inputs.mouse_left.x, (float)inputs.mouse_left.y),
                .mouse_left = gui_wants_the_mouse ? false : inputs.mouse_left.is_down,
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
            FB_PERF_SCOPE("Rendering");

            auto cmd = sc.device.begin_frame();

            {
                auto& swapchain = sc.device.swapchain();
                const auto frame_index = sc.device.frame_index();
                auto& render_target_view = sc.techniques.render_target_view;

                cmd.pix_begin("Frame %zu", sc.frame.count());

                {
                    cmd.pix_begin("Clear");
                    render_target_view.transition_to_render_target(cmd);
                    swapchain.transition_to_render_target(cmd, frame_index);
                    cmd.flush_barriers();
                    render_target_view.clear(cmd);
                    swapchain.clear_render_target(cmd, frame_index);
                    cmd.pix_end();
                }

                {
                    cmd.pix_begin("Techniques");
                    const auto desc = techniques::RenderDesc {
                        .cmd = cmd,
                        .device = sc.device,
                        .frame_index = frame_index,
                    };
                    techniques::render_main(sc.techniques, render_target_view, desc);
                    cmd.pix_end();
                }

                {
                    cmd.pix_begin("Resolve");
                    render_target_view.transition_to_resolve(cmd);
                    cmd.flush_barriers();
                    render_target_view.resolve(cmd);
                    render_target_view.transition_to_shader_resource(cmd);
                    cmd.flush_barriers();
                    cmd.pix_end();
                }

                {
                    cmd.pix_begin("Compose");

                    cmd.graphics_scope([&swapchain, frame_index](GpuGraphicsCommandList& cmd) {
                        swapchain.set_render_target(cmd, frame_index);
                    });
                    techniques::render_compositing(
                        sc.techniques,
                        {
                            .cmd = cmd,
                            .device = sc.device,
                            .frame_index = frame_index,
                        }
                    );
                    sc.gui.render(sc.device, cmd);

                    swapchain.transition_to_present(cmd, frame_index);
                    cmd.flush_barriers();

                    cmd.pix_end();
                }

                cmd.pix_end();
            }

            sc.device.end_frame(std::move(cmd));
        }

        // Present.
        {
            FB_PERF_SCOPE("Present");
            sc.device.present();
        }

        // Update frame.
        sc.frame.update();
    }

    // Wait for pending GPU work to complete before running destructors.
    sc.device.wait();
}

} // namespace fb

auto main() -> int {
    FB_LOG_INFO("sizeof(Stockcube): {} bytes", sizeof(fb::Stockcube));
    auto stockcube = std::make_unique<fb::Stockcube>();
    fb::stockcube_run(*stockcube);
    return 0;
}
