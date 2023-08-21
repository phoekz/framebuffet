#include "pch.hpp"
#include "gpu/gpu.hpp"
#include "demos/demos.hpp"
#include "demos/cards.hpp"
#include "demos/cube/cube.hpp"
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
    auto cube_demo = std::make_unique<demos::cube::CubeDemo>(*device, assets, shaders);
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
                std::ref(cube_demo->rt()),
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

    // Archives.
    if (file_exists(ARCHIVE_FILE_NAME)) {
        auto archive_buf = read_whole_file(ARCHIVE_FILE_NAME);
        auto archive = DeserializingArchive(archive_buf);
        cube_demo->archive(archive);
        tree_demo->archive(archive);
        rain_demo->archive(archive);
        anim_demo->archive(archive);
        fibers_demo->archive(archive);
        env_demo->archive(archive);
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
            cube_demo->archive(archive);
            tree_demo->archive(archive);
            rain_demo->archive(archive);
            anim_demo->archive(archive);
            fibers_demo->archive(archive);
            env_demo->archive(archive);
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

                const auto desc = demos::GuiDesc {.window_size = device->swapchain_size()};
                gui_wrapper(cards, desc, ImGuiTreeNodeFlags_DefaultOpen);
                gui_wrapper(cube_demo, desc);
                gui_wrapper(tree_demo, desc);
                gui_wrapper(rain_demo, desc);
                gui_wrapper(anim_demo, desc);
                gui_wrapper(fibers_demo, desc);
                gui_wrapper(env_demo, desc);
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
                .window_size = device->swapchain_size(),
                .aspect_ratio = WINDOW_ASPECT_RATIO,
                .delta_time = frame.delta_time(),
                .elapsed_time = frame.elapsed_time(),
                .frame_index = device->frame_index(),
            };
            cube_demo->update(update_desc);
            tree_demo->update(update_desc);
            rain_demo->update(update_desc);
            anim_demo->update(update_desc);
            fibers_demo->update(update_desc);
            env_demo->update(update_desc);
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
                    cmd.begin_pix("Demo pass");

                    cmd.begin_pix(cube_demo->NAME);
                    cube_demo->render(*device, cmd);
                    cmd.end_pix();

                    cmd.begin_pix(tree_demo->NAME);
                    tree_demo->render(*device, cmd);
                    cmd.end_pix();

                    cmd.begin_pix(rain_demo->NAME);
                    rain_demo->render(*device, cmd);
                    cmd.end_pix();

                    cmd.begin_pix(anim_demo->NAME);
                    anim_demo->render(*device, cmd);
                    cmd.end_pix();

                    cmd.begin_pix(fibers_demo->NAME);
                    fibers_demo->render(*device, cmd);
                    cmd.end_pix();

                    cmd.begin_pix(env_demo->NAME);
                    env_demo->render(*device, cmd);
                    cmd.end_pix();

                    cmd.end_pix();
                }

                {
                    cmd.begin_pix("Main pass");
                    device->begin_main_pass();

                    cmd.begin_pix(cards->NAME);
                    cards->render(*device, cmd);
                    cmd.end_pix();

                    cmd.begin_pix(gui->NAME);
                    gui->render(*device, cmd);
                    cmd.end_pix();

                    device->end_main_pass();
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

    return 0;
}
