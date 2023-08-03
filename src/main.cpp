// Framebuffet
#include "maths.hpp"
#include "utils.hpp"
#include "pcg.hpp"
#include "win32.hpp"
#include "dx12.hpp"
#include "shaders.hpp"
#include "gui.hpp"

// Framebuffet - demos
#include "demo/cards.hpp"
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
constexpr float CLEAR_COLOR[4] = {0.1f, 0.1f, 0.1f, 1.0f};

//
// Main.
//

auto main() -> int {
    // Init.
    auto window =
        std::make_unique<fb::Window>(fb::Window::Desc {WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT});
    auto dx = std::make_unique<fb::Dx>(*window);
    auto gui = std::make_unique<fb::gui::Gui>(*window, *dx);
    auto cube_demo = std::make_unique<fb::cube::Demo>(*dx);
    auto rain_demo = std::make_unique<fb::rain::Demo>(*dx);
    auto tree_demo = std::make_unique<fb::tree::Demo>(*dx);
    auto cards = std::make_unique<fb::cards::Cards>(
        *dx,
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
        cube_demo->update({
            .aspect_ratio = WINDOW_ASPECT_RATIO,
            .elapsed_time = ft.elapsed_time(),
            .frame_index = dx->frame_index,
        });
        rain_demo->update({
            .aspect_ratio = WINDOW_ASPECT_RATIO,
            .delta_time = ft.delta_time(),
            .frame_index = dx->frame_index,
        });
        tree_demo->update({
            .aspect_ratio = WINDOW_ASPECT_RATIO,
            .elapsed_time = ft.elapsed_time(),
            .frame_index = dx->frame_index,
        });
        cards->update(*dx);

        // Populate command list.
        auto* cmd_alloc = dx->command_allocators[dx->frame_index].get();
        auto* cmd = dx->command_list.get();
        cmd_alloc->Reset();
        cmd->Reset(cmd_alloc, nullptr);

        // Begin render frame.
        PIXBeginEvent(cmd, PIX_COLOR_DEFAULT, std::format("Frame {}", frame_count).c_str());

        // Render demos.
        {
            PIXBeginEvent(cmd, PIX_COLOR_DEFAULT, "Cube");
            cube_demo->render(*dx);
            PIXEndEvent(cmd);
        }
        {
            PIXBeginEvent(cmd, PIX_COLOR_DEFAULT, "Rain");
            rain_demo->render(*dx);
            PIXEndEvent(cmd);
        }
        {
            PIXBeginEvent(cmd, PIX_COLOR_DEFAULT, "Tree");
            tree_demo->render(*dx);
            PIXEndEvent(cmd);
        }

        // Clear.
        {
            PIXBeginEvent(cmd, PIX_COLOR_DEFAULT, "Clear");
            dx->transition(
                dx->rtvs[dx->frame_index],
                D3D12_RESOURCE_STATE_PRESENT,
                D3D12_RESOURCE_STATE_RENDER_TARGET);
            cmd->ClearRenderTargetView(
                dx->rtv_descriptors[dx->frame_index],
                CLEAR_COLOR,
                0,
                nullptr);
            cmd->OMSetRenderTargets(1, &dx->rtv_descriptors[dx->frame_index], FALSE, nullptr);
            PIXEndEvent(cmd);
        }

        // Render cards.
        {
            PIXBeginEvent(cmd, PIX_COLOR_DEFAULT, "Cards");
            cards->render(*dx);
            PIXEndEvent(cmd);
        }

        // Render Gui.
        {
            PIXBeginEvent(cmd, PIX_COLOR_DEFAULT, "Gui");
            gui->render(*dx);
            PIXEndEvent(cmd);
        }

        // End render frame.
        {
            dx->transition(
                dx->rtvs[dx->frame_index],
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PRESENT);
            PIXEndEvent(cmd);
            cmd->Close();
        }

        // Execute command list.
        ID3D12CommandList* command_lists[] = {(ID3D12CommandList*)cmd};
        dx->command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

        // Present.
        dx->swapchain->Present(1, 0);

        // Move to next frame.
        {
            // Schedule a Signal command in the queue.
            uint64_t current_fence_value = dx->fence_values[dx->frame_index];
            FAIL_FAST_IF_FAILED(
                dx->command_queue->Signal(dx->fence.get(), dx->fence_values[dx->frame_index]));

            // Update the frame index.
            dx->frame_index = dx->swapchain->GetCurrentBackBufferIndex();
            uint64_t* fence_value = &dx->fence_values[dx->frame_index];

            // If the next frame is not ready to be rendered yet, wait until it is ready.
            if (dx->fence->GetCompletedValue() < *fence_value) {
                FAIL_FAST_IF_FAILED(
                    dx->fence->SetEventOnCompletion(*fence_value, dx->fence_event.get()));
                WaitForSingleObjectEx(dx->fence_event.get(), INFINITE, FALSE);
            }

            // Set the fence value for the next frame.
            *fence_value = current_fence_value + 1;
        }

        // Update frame count.
        frame_count++;
    }

    // Wait for pending GPU work to complete.
    {
        // Schedule a Signal command in the queue.
        FAIL_FAST_IF_FAILED(
            dx->command_queue->Signal(dx->fence.get(), dx->fence_values[dx->frame_index]));

        // Wait until the fence has been processed.
        FAIL_FAST_IF_FAILED(dx->fence->SetEventOnCompletion(
            dx->fence_values[dx->frame_index],
            dx->fence_event.get()));
        WaitForSingleObjectEx(dx->fence_event.get(), INFINITE, FALSE);
    }

    // Cleanup.
    cards = nullptr;
    tree_demo = nullptr;
    rain_demo = nullptr;
    cube_demo = nullptr;
    gui = nullptr;
    dx = nullptr;
    window = nullptr;

    return 0;
}
