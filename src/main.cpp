// Framebuffet
#include "maths.hpp"
#include "utils.hpp"
#include "win32.hpp"
#include "dx12.hpp"
#include "shaders.hpp"
#include "gui.hpp"

// Framebuffet - demos
#include "demo/cube.hpp"

// DirectX.
#include <d3dx12/d3dx12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

// WinPixEventRuntime.
#define USE_PIX
#include <WinPixEventRuntime/pix3.h>

// Standard libraries.
#include <array>
#include <vector>

// Constants.
constexpr const char* WINDOW_TITLE = "framebuffet 😎";
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 800;
constexpr float WINDOW_ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
constexpr float CLEAR_COLOR[4] = {0.1f, 0.1f, 0.1f, 1.0f};

//
// Main.
//

int main() {
    // Window.
    fb::Window* window = fb::window_create({
        .title = WINDOW_TITLE,
        .width = WINDOW_WIDTH,
        .height = WINDOW_HEIGHT,
    });

    // D3D12.
    fb::Dx dx;
    fb::dx_create(&dx, window);

    // Gui.
    auto gui = std::make_unique<fb::Gui>(window, dx);

    // Demos.
    auto cube_demo = std::make_unique<fb::cube::Demo>(dx);

    // Main loop.
    bool running = true;
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
        });

        // Populate command list.
        dx.command_allocators[dx.frame_index]->Reset();
        dx.command_list->Reset(dx.command_allocators[dx.frame_index], nullptr);

        {
            auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                dx.rtvs[dx.frame_index],
                D3D12_RESOURCE_STATE_PRESENT,
                D3D12_RESOURCE_STATE_RENDER_TARGET);
            dx.command_list->ResourceBarrier(1, &barrier);
        }

        {
            PIXBeginEvent(dx.command_list, PIX_COLOR_DEFAULT, "Setup");
            dx.command_list->ClearRenderTargetView(
                dx.rtv_descriptors[dx.frame_index],
                CLEAR_COLOR,
                0,
                nullptr);
            CD3DX12_VIEWPORT viewport(0.0f, 0.0f, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);
            dx.command_list->RSSetViewports(1, &viewport);
            CD3DX12_RECT scissor_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
            dx.command_list->RSSetScissorRects(1, &scissor_rect);
            dx.command_list
                ->OMSetRenderTargets(1, &dx.rtv_descriptors[dx.frame_index], FALSE, nullptr);
            PIXEndEvent(dx.command_list);
        }

        {
            PIXBeginEvent(dx.command_list, PIX_COLOR_DEFAULT, "Cube");
            cube_demo->render(dx.command_list);
            PIXEndEvent(dx.command_list);
        }

        {
            PIXBeginEvent(dx.command_list, PIX_COLOR_DEFAULT, "Gui");
            gui->render(dx);
            PIXEndEvent(dx.command_list);
        }

        {
            auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                dx.rtvs[dx.frame_index],
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PRESENT);
            dx.command_list->ResourceBarrier(1, &barrier);
        }

        dx.command_list->Close();

        // Execute command list.
        ID3D12CommandList* command_lists[] = {(ID3D12CommandList*)dx.command_list};
        dx.command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

        // Present.
        dx.swapchain->Present(1, 0);

        // Move to next frame.
        {
            // Schedule a Signal command in the queue.
            uint64_t current_fence_value = dx.fence_values[dx.frame_index];
            FAIL_FAST_IF_FAILED(
                dx.command_queue->Signal(dx.fence, dx.fence_values[dx.frame_index]));

            // Update the frame index.
            dx.frame_index = dx.swapchain->GetCurrentBackBufferIndex();
            uint64_t* fence_value = &dx.fence_values[dx.frame_index];

            // If the next frame is not ready to be rendered yet, wait until it is ready.
            if (dx.fence->GetCompletedValue() < *fence_value) {
                FAIL_FAST_IF_FAILED(dx.fence->SetEventOnCompletion(*fence_value, dx.fence_event));
                WaitForSingleObjectEx(dx.fence_event, INFINITE, FALSE);
            }

            // Set the fence value for the next frame.
            *fence_value = current_fence_value + 1;
        }
    }

    // Wait for pending GPU work to complete.
    {
        // Schedule a Signal command in the queue.
        FAIL_FAST_IF_FAILED(dx.command_queue->Signal(dx.fence, dx.fence_values[dx.frame_index]));

        // Wait until the fence has been processed.
        FAIL_FAST_IF_FAILED(
            dx.fence->SetEventOnCompletion(dx.fence_values[dx.frame_index], dx.fence_event));
        WaitForSingleObjectEx(dx.fence_event, INFINITE, FALSE);
    }

    // Cleanup.
    cube_demo = nullptr;
    gui = nullptr;
    fb::dx_destroy(&dx);
    fb::window_destroy(window);

    return 0;
}
