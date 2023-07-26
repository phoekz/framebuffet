#include "gui.hpp"
#include "utils.hpp"
#include "win32.hpp"
#include "dx12.hpp"

#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx12.h>

namespace fb {

struct Gui {
    ImGuiContext* ctx = nullptr;
    ID3D12DescriptorHeap* cbv_srv_heap = nullptr;
};

Gui* gui_create(Window* window, Dx* dx) {
    // GPU resources.
    D3D12_DESCRIPTOR_HEAP_DESC desc = {
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        .NumDescriptors = 1,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
        .NodeMask = 0,
    };
    ID3D12DescriptorHeap* cbv_srv_heap = nullptr;
    FAIL_FAST_IF_FAILED(dx->device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&cbv_srv_heap)));
    dx_set_name(cbv_srv_heap, "Gui Heap");

    // ImGui.
    IMGUI_CHECKVERSION();
    ImGuiContext* ctx = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init((HWND)window_handle(window));
    ImGui_ImplDX12_Init(
        dx->device,
        fb::FRAME_COUNT,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        cbv_srv_heap,
        cbv_srv_heap->GetCPUDescriptorHandleForHeapStart(),
        cbv_srv_heap->GetGPUDescriptorHandleForHeapStart());

    // Result.
    Gui* gui = new Gui();
    gui->ctx = ctx;
    gui->cbv_srv_heap = cbv_srv_heap;
    return gui;
}

void gui_destroy(Gui* gui) {
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext(gui->ctx);
    gui->cbv_srv_heap->Release();
    delete gui;
}

void gui_update(Gui*) {
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
    ImGui::Render();
}

void gui_render(Gui* gui, Dx* dx) {
    dx->command_list->SetDescriptorHeaps(1, &gui->cbv_srv_heap);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dx->command_list);
}

}  // namespace fb
