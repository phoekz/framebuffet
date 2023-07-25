#include "win32.hpp"
#include "utils.hpp"
#include <wil/result_macros.h>
#include <imgui.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// ImGui input handler.
extern IMGUI_IMPL_API LRESULT
ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM w_param, LPARAM l_param);

namespace fb {

// Window procedure.
static LRESULT CALLBACK
win32_window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
    // ImGui input handling.
    if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param))
        return true;

    // Window messages.
    switch (message) {
        case WM_KEYDOWN:
            if (w_param == VK_ESCAPE) {
                PostQuitMessage(0);
                return 0;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
    }

    // Default window procedure.
    return DefWindowProcA(window, message, w_param, l_param);
}

Window* window_create(const WindowDesc& desc) noexcept {
    // Module handle.
    HMODULE module_handle = GetModuleHandleA(nullptr);

    // Register window class.
    UINT class_style = 0;
    class_style |= CS_OWNDC;    // Allocates a unique device context for each window in the class.
    class_style |= CS_HREDRAW;  // Redraws the client area if the width changes.
    class_style |= CS_VREDRAW;  // Redraws the client area if the height changes.
    HICON icon = LoadIconA(nullptr, IDI_WINLOGO);
    HCURSOR cursor = LoadCursorA(nullptr, IDC_ARROW);
    HBRUSH background_brush = (HBRUSH)GetStockObject(BLACK_BRUSH);
    WNDCLASSEXA window_class = {
        .cbSize = sizeof(WNDCLASSEXA),      // Size of this structure in bytes.
        .style = class_style,               // Class style.
        .lpfnWndProc = win32_window_proc,   // Pointer to the window procedure.
        .hInstance = module_handle,         // Handle to the instance of the module.
        .hIcon = icon,                      // Handle to the class icon.
        .hCursor = cursor,                  // Handle to the class cursor.
        .hbrBackground = background_brush,  // Handle to the class background brush.
        .lpszMenuName = nullptr,            // No default menu.
        .lpszClassName = desc.title,        // Class name.
        .hIconSm = window_class.hIcon       // Handle to the small icon.
    };
    RegisterClassExA(&window_class);

    // Adjust window size such that the client area is the requested size.
    // Without the adjustment, the window size will be the requested size plus
    // the size of the window frame.
    RECT window_rect = {
        .left = 0,
        .top = 0,
        .right = (LONG)desc.width,
        .bottom = (LONG)desc.height,
    };
    DWORD window_style = WS_OVERLAPPEDWINDOW;
    AdjustWindowRect(&window_rect, window_style, FALSE);
    int window_width = window_rect.right - window_rect.left;
    int window_height = window_rect.bottom - window_rect.top;

    // Center window on screen.
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);
    int window_x = (screen_width - window_width) / 2;
    int window_h = (screen_height - window_height) / 2;

    // Create window.
    HWND window_handle = CreateWindowExA(
        WS_EX_APPWINDOW,  // Extended window style.
        desc.title,       // Class name.
        desc.title,       // Window name.
        window_style,     // Window style.
        window_x,         // Window horizontal position.
        window_h,         // Window vertical position.
        window_width,     // Window width.
        window_height,    // Window height.
        nullptr,          // Parent window.
        nullptr,          // Menu handle.
        module_handle,    // Handle to the instance of the module.
        nullptr           // Pointer to be passed through the CREATESTRUCT structure.
    );
    FAIL_FAST_IF_NULL_MSG(window_handle, "Failed to create window.");

    // Show window.
    ShowWindow(window_handle, SW_SHOWDEFAULT);
    UpdateWindow(window_handle);

    // Logging.
    log_info("Created window: {}", desc.title);

    return (Window*)window_handle;
}

void window_destroy(Window* window) noexcept {
    // Destroy window.
    HWND window_handle = (HWND)window;
    DestroyWindow(window_handle);
}

}  // namespace fb
