#include "pch.hpp"
#include "gpu/gpu.hpp"
#include "demos/demos.hpp"
#include "demos/cards.hpp"
#include "demos/crate/crate.hpp"
#include "demos/tree/tree.hpp"
#include "demos/rain/rain.hpp"
#include "demos/anim/anim.hpp"
#include "demos/fibers/fibers.hpp"
#include "demos/env/env.hpp"

// Constants.
inline constexpr std::string_view WINDOW_TITLE = "framebuffet 😎"sv;
inline constexpr int WINDOW_WIDTH = 1280;
inline constexpr int WINDOW_HEIGHT = 832;
inline constexpr float WINDOW_ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
static_assert(
    WINDOW_WIDTH % 64 == 0 && WINDOW_HEIGHT % 64 == 0,
    "Window sizes must be a multiple of 64 due to SPD limitations."
);

//
// GUI.
//

namespace fb {

template<typename T>
concept GuiWrappable = requires(T demo, const demos::GuiDesc& desc) {
    { demo->NAME } -> std::convertible_to<std::string_view>;
    { demo->gui(desc) } -> std::same_as<void>;
};

template<typename T>
inline constexpr auto
gui_wrapper(T& demo, const demos::GuiDesc& desc, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None)
    -> void
    requires GuiWrappable<T>
{
    auto name = demo->NAME;
    ImGui::PushID(name.data());
    if (ImGui::CollapsingHeader(name.data(), nullptr, flags)) {
        demo->gui(desc);
    }
    ImGui::PopID();
}

} // namespace fb

int main() {
    using namespace fb;

    // Console.
    attach_console();

    // Init.
    auto init_time = Instant();
    auto window =
        std::make_unique<Window>(Window::Desc {WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT});
    auto device = std::make_unique<GpuDevice>(*window);
    auto assets = baked::Assets();
    auto shaders = baked::Shaders();
    device->begin_transfer();
    auto crate_demo = std::make_unique<demos::crate::CrateDemo>(*device, assets, shaders);
    auto tree_demo = std::make_unique<demos::tree::TreeDemo>(*device, assets, shaders);
    auto rain_demo = std::make_unique<demos::rain::RainDemo>(*device, assets, shaders);
    auto anim_demo = std::make_unique<demos::anim::AnimDemo>(*device, assets, shaders);
    auto fibers_demo = std::make_unique<demos::fibers::FibersDemo>(*device, assets, shaders);
    auto env_demo = std::make_unique<demos::env::EnvDemo>(*device, assets, shaders);
    auto cards = std::make_unique<demos::cards::Cards>(
        *device,
        shaders,
        demos::cards::CardsDesc {
            .card_render_targets = std::to_array({
                std::ref(crate_demo->rt()),
                std::ref(tree_demo->rt()),
                std::ref(rain_demo->rt()),
                std::ref(anim_demo->rt()),
                std::ref(fibers_demo->rt()),
                std::ref(env_demo->rt()),
            })}
    );
    auto gui = std::make_unique<demos::gui::Gui>(*window, *device, assets, shaders);
    device->end_transfer();
    device->log_stats();
    FB_LOG_INFO("Init time: {} ms", 1e3f * init_time.elapsed_time());

    auto every_demo =
        [&crate_demo, &tree_demo, &rain_demo, &anim_demo, &fibers_demo, &env_demo](auto f) {
            f(crate_demo);
            f(tree_demo);
            f(rain_demo);
            f(anim_demo);
            f(fibers_demo);
            f(env_demo);
        };

    // Archives.
    if (file_exists(ARCHIVE_FILE_NAME)) {
        auto archive_buf = read_whole_file(ARCHIVE_FILE_NAME);
        FB_LOG_INFO("Archive size: {} bytes", archive_buf.size());
        auto archive = DeserializingArchive(archive_buf);
        every_demo([&archive](auto& demo) { demo->archive(archive); });
        cards->archive(archive);
        FB_ASSERT(archive.fully_consumed());
    }

    // Main loop.
    bool running = true;
    uint64_t frame_count = 0;
    Frame frame = {};
    double update_time = 0.0;
    double gui_time = 0.0;
    double time_since_last_archive = 0.0;
    while (running) {
        const auto frame_name = std::format("Frame {}", frame_count);
        PIXScopedEvent(PIX_COLOR_DEFAULT, dx_name("Main", frame_name).data());

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

        // Update frame.
        frame.update();

        // Update archive.
        time_since_last_archive += frame.last_delta_time();
        if (time_since_last_archive > 1.0) {
            PIXSetMarker(PIX_COLOR_DEFAULT, "Archive");

            time_since_last_archive = 0.0;
            auto archive_buf = std::vector<std::byte>();
            auto archive = SerializingArchive(archive_buf);
            every_demo([&archive](auto& demo) { demo->archive(archive); });
            cards->archive(archive);
            write_whole_file(ARCHIVE_FILE_NAME, archive_buf);
        }

        // Update gui.
        {
            PIXScopedEvent(PIX_COLOR_DEFAULT, "Gui");

            auto timer = fb::Instant();
            gui->begin_frame();
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
                    device->pix_capture();
                }

                const auto desc = demos::GuiDesc {.window_size = device->swapchain().size()};
                gui_wrapper(cards, desc, ImGuiTreeNodeFlags_DefaultOpen);
                every_demo([&desc](auto& demo) { gui_wrapper(demo, desc); });
            }
            ImGui::End();
            gui->end_frame();
            gui_time = timer.elapsed_time();
        }

        // Update demos.
        {
            PIXScopedEvent(PIX_COLOR_DEFAULT, "Update");

            auto timer = Instant();
            const auto update_desc = demos::UpdateDesc {
                .window_size = device->swapchain().size(),
                .aspect_ratio = WINDOW_ASPECT_RATIO,
                .delta_time = frame.delta_time(),
                .elapsed_time = frame.elapsed_time(),
                .frame_index = device->frame_index(),
            };
            every_demo([&update_desc](auto& demo) { demo->update(update_desc); });
            cards->update(*device);
            update_time = timer.elapsed_time();
        }

        // Graphics.
        {
            PIXBeginEvent(PIX_COLOR_DEFAULT, "Graphics");

            PIXBeginEvent(PIX_COLOR_DEFAULT, "Prepare");
            auto cmd = device->begin_frame();
            PIXEndEvent();

            PIXBeginEvent(PIX_COLOR_DEFAULT, "Commands");
            {
                cmd.begin_pix(frame_name);

                {
                    cmd.begin_pix("Clear render targets");
                    cmd.set_graphics();
                    every_demo([&cmd](auto& demo) { demo->rt().transition_to_render_target(cmd); });
                    cmd.flush_barriers();
                    every_demo([&cmd](auto& demo) { demo->rt().clear_all(cmd); });
                    cmd.end_pix();
                }

                {
                    cmd.begin_pix("Demo pass");
                    every_demo([&device, &cmd](auto& demo) {
                        cmd.begin_pix(demo->NAME);
                        demo->render(*device, cmd);
                        cmd.end_pix();
                    });
                    cmd.end_pix();
                }

                {
                    cmd.begin_pix("Resolve render targets");
                    every_demo([&cmd](auto& demo) { demo->rt().transition_to_resolve(cmd); });
                    cmd.flush_barriers();
                    every_demo([&cmd](auto& demo) { demo->rt().resolve_all(cmd); });
                    every_demo([&cmd](auto& demo) {
                        demo->rt().transition_to_pixel_shader_resource(cmd);
                    });
                    cmd.flush_barriers();
                    cmd.end_pix();
                }

                {
                    auto& swapchain = device->swapchain();
                    const auto frame_index = device->frame_index();

                    cmd.begin_pix("Main pass");

                    swapchain.transition_to_render_target(cmd, frame_index);
                    cmd.flush_barriers();
                    swapchain.clear_render_target(cmd, frame_index);
                    swapchain.set_render_target(cmd, frame_index);

                    cmd.begin_pix(cards->NAME);
                    cards->render(*device, cmd);
                    cmd.end_pix();

                    cmd.begin_pix(gui->NAME);
                    gui->render(*device, cmd);
                    cmd.end_pix();

                    swapchain.transition_to_present(cmd, frame_index);
                    cmd.flush_barriers();

                    cmd.end_pix();
                }

                cmd.end_pix();
            }
            PIXEndEvent();

            PIXBeginEvent(PIX_COLOR_DEFAULT, "Present");
            device->end_frame(std::move(cmd));
            PIXEndEvent();

            PIXEndEvent();
        }

        // Update frame count.
        frame_count++;
    }

    // Wait for pending GPU work to complete before running destructors.
    device->wait();

    // Archive one more time.
    {
        auto archive_buf = std::vector<std::byte>();
        auto archive = SerializingArchive(archive_buf);
        every_demo([&archive](auto& demo) { demo->archive(archive); });
        cards->archive(archive);
        write_whole_file(ARCHIVE_FILE_NAME, archive_buf);
        FB_LOG_INFO("Final archive size: {} bytes", archive_buf.size());
    }

    return 0;
}
