#include "pch.hpp"
#include "demos/demos.hpp"
#include "demos/cards.hpp"
#include "demos/crate/crate.hpp"
#include "demos/tree/tree.hpp"
#include "demos/rain/rain.hpp"
#include "demos/anim/anim.hpp"
#include "demos/fibers/fibers.hpp"
#include "demos/env/env.hpp"

namespace fb {

//
// Constants.
//

inline constexpr std::string_view WINDOW_TITLE = "framebuffet 😎"sv;
inline constexpr int WINDOW_WIDTH = 1280;
inline constexpr int WINDOW_HEIGHT = 832;
inline constexpr float WINDOW_ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
static_assert(
    WINDOW_WIDTH % 64 == 0 && WINDOW_HEIGHT % 64 == 0,
    "Window sizes must be a multiple of 64 due to SPD limitations."
);

//
// Gui.
//

template<typename T>
concept GuiWrappable = requires(T demo, const demos::GuiDesc& desc) {
    { demo.NAME } -> std::convertible_to<std::string_view>;
    { demo.gui(desc) } -> std::same_as<void>;
};

template<typename T>
inline constexpr auto
gui_wrapper(T& demo, const demos::GuiDesc& desc, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None)
    -> void
    requires GuiWrappable<T>
{
    auto name = demo.NAME;
    ImGui::PushID(name.data());
    if (ImGui::CollapsingHeader(name.data(), nullptr, flags)) {
        demo.gui(desc);
    }
    ImGui::PopID();
}

//
// Demo.
//

template<typename T>
concept Demo =
    std::default_initializable<T> && !std::copyable<T> && !std::movable<T> && requires(T demo) {
        { demo.NAME } -> std::convertible_to<std::string_view>;
        {
            demo.create(std::declval<GpuDevice&>(), std::declval<const demos::Baked&>())
        } -> std::same_as<void>;
        { demo.gui(std::declval<const demos::GuiDesc&>()) } -> std::same_as<void>;
        { demo.update(std::declval<const demos::UpdateDesc&>()) } -> std::same_as<void>;
        {
            demo.render(std::declval<GpuDevice&>(), std::declval<GpuCommandList&>())
        } -> std::same_as<void>;
        { demo.rt() } -> std::same_as<graphics::render_targets::RenderTargets&>;
    };

static_assert(Demo<demos::crate::CrateDemo>);
static_assert(Demo<demos::tree::TreeDemo>);
static_assert(Demo<demos::rain::RainDemo>);
static_assert(Demo<demos::anim::AnimDemo>);
static_assert(Demo<demos::fibers::FibersDemo>);
static_assert(Demo<demos::env::EnvDemo>);

//
// Buffet.
//

struct Buffet {
    auto run() -> void;

    // Systems.
    Window window;
    GpuDevice device;

    // Demos.
    demos::Baked baked;
    demos::crate::CrateDemo crate_demo;
    demos::tree::TreeDemo tree_demo;
    demos::rain::RainDemo rain_demo;
    demos::anim::AnimDemo anim_demo;
    demos::fibers::FibersDemo fibers_demo;
    demos::env::EnvDemo env_demo;
    template<typename F>
    auto every_demo(F f) -> void {
        f(crate_demo);
        f(tree_demo);
        f(rain_demo);
        f(anim_demo);
        f(fibers_demo);
        f(env_demo);
    }

    // Demo support.
    demos::cards::Cards cards;
    graphics::gui::Gui gui;

    // Frame.
    Frame frame;

    // Archive.
    std::vector<std::byte> archive_buf;
    double time_since_last_archive = 0.0;
};

auto Buffet::run() -> void {
    // Console.
    attach_console();

    // Init.
    {
        auto timer = Instant();
        window.create(Window::Desc {WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT});
        device.create(window);

        device.begin_transfer();
        crate_demo.create(device, baked);
        tree_demo.create(device, baked);
        rain_demo.create(device, baked);
        anim_demo.create(device, baked);
        fibers_demo.create(device, baked);
        env_demo.create(device, baked);
        cards.create(
            device,
            baked,
            demos::cards::CardsDesc {
                .card_render_targets = std::to_array({
                    std::ref(crate_demo.rt()),
                    std::ref(tree_demo.rt()),
                    std::ref(rain_demo.rt()),
                    std::ref(anim_demo.rt()),
                    std::ref(fibers_demo.rt()),
                    std::ref(env_demo.rt()),
                })}
        );
        gui.create(window, device, baked.kitchen.assets, baked.kitchen.shaders);

        device.end_transfer();
        device.log_stats();

        FB_LOG_INFO("Init time: {} ms", 1e3f * timer.elapsed_time());
    }

    // Archive.
    if (file_exists(ARCHIVE_FILE_NAME)) {
        auto buf = read_whole_file(ARCHIVE_FILE_NAME);
        FB_LOG_INFO("Archive size: {} bytes", buf.size());
        auto archive = DeserializingArchive(buf);
        every_demo([&archive](auto& demo) { demo.archive(archive); });
        cards.archive(archive);
        FB_ASSERT(archive.fully_consumed());
    }

    // Other frame resources.
    {
        archive_buf.reserve(1024);
        frame.create();
    }

    // Main loop.
    bool running = true;
    double update_time = 0.0;
    double gui_time = 0.0;
    FRAME_ALLOCATION_TRAP = true;
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

        // Update archive.
        time_since_last_archive += frame.last_delta_time();
        if (time_since_last_archive > 1.0) {
            PIXSetMarker(PIX_COLOR_DEFAULT, "Archive");

            time_since_last_archive = 0.0;
            auto archive = SerializingArchive(archive_buf);
            every_demo([&archive](auto& demo) { demo.archive(archive); });
            cards.archive(archive);
            write_whole_file(ARCHIVE_FILE_NAME, archive_buf);
            archive_buf.clear();
        }

        // Update gui.
        {
            PIXScopedEvent(PIX_COLOR_DEFAULT, "Gui");

            auto timer = fb::Instant();
            gui.begin_frame();
            ImGui::SetNextWindowSize({300, 300}, ImGuiCond_FirstUseEver);
            if (ImGui::Begin("Framebuffet")) {
                ImGui::Text(
                    "Frame time: %.3f ms (%.3f fps)",
                    1e3f * frame.last_delta_time(),
                    frame.last_fps()
                );
                ImGui::Text("Update time: %.3f ms", 1e3f * update_time);
                ImGui::Text("GUI time: %.3f ms", 1e3f * gui_time);

                if (ImGui::Button("PIX Capture")) {
                    device.pix_capture();
                }

                const auto desc = demos::GuiDesc {.window_size = device.swapchain().size()};
                gui_wrapper(cards, desc, ImGuiTreeNodeFlags_DefaultOpen);
                every_demo([&desc](auto& demo) { gui_wrapper(demo, desc); });
            }
            ImGui::End();
            gui.end_frame();
            gui_time = timer.elapsed_time();
        }

        // Update demos.
        {
            PIXScopedEvent(PIX_COLOR_DEFAULT, "Update");

            auto timer = Instant();
            const auto update_desc = demos::UpdateDesc {
                .window_size = device.swapchain().size(),
                .aspect_ratio = WINDOW_ASPECT_RATIO,
                .delta_time = frame.delta_time(),
                .elapsed_time = frame.elapsed_time(),
                .frame_index = device.frame_index(),
            };
            every_demo([&update_desc](auto& demo) {
                PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - %s", demo.NAME.data(), "Update");
                demo.update(update_desc);
            });
            {
                PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - %s", cards.NAME.data(), "Update");
                cards.update(device);
            }
            update_time = timer.elapsed_time();
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
                    every_demo([&cmd](auto& demo) { demo.rt().transition_to_render_target(cmd); });
                    swapchain.transition_to_render_target(cmd, frame_index);
                    cmd.flush_barriers();
                    every_demo([&cmd](auto& demo) { demo.rt().clear_all(cmd); });
                    swapchain.clear_render_target(cmd, frame_index);
                    cmd.end_pix();
                }

                {
                    cmd.begin_pix("Demo pass");
                    every_demo([&](auto& demo) {
                        cmd.begin_pix(demo.NAME.data());
                        demo.render(device, cmd);
                        cmd.end_pix();
                    });
                    cmd.end_pix();
                }

                {
                    cmd.begin_pix("Resolve render targets");
                    every_demo([&cmd](auto& demo) { demo.rt().transition_to_resolve(cmd); });
                    cmd.flush_barriers();
                    every_demo([&cmd](auto& demo) { demo.rt().resolve_all(cmd); });
                    every_demo([&cmd](auto& demo) {
                        demo.rt().transition_to_pixel_shader_resource(cmd);
                    });
                    cmd.flush_barriers();
                    cmd.end_pix();
                }

                {
                    cmd.begin_pix("Main pass");

                    swapchain.set_render_target(cmd, frame_index);

                    cmd.begin_pix(cards.NAME.data());
                    cards.render(device, cmd);
                    cmd.end_pix();

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
    FRAME_ALLOCATION_TRAP = false;

    // Wait for pending GPU work to complete before running destructors.
    device.wait();

    // Archive one more time.
    {
        auto archive = SerializingArchive(archive_buf);
        every_demo([&archive](auto& demo) { demo.archive(archive); });
        cards.archive(archive);
        write_whole_file(ARCHIVE_FILE_NAME, archive_buf);
        FB_LOG_INFO("Final archive size: {} bytes", archive_buf.size());
    }
}

} // namespace fb

//
// Main.
//

int main() {
    FB_LOG_INFO("sizeof(Buffet): {} bytes", sizeof(fb::Buffet));
    auto buffet = std::make_unique<fb::Buffet>();
    buffet->run();
    return 0;
}
