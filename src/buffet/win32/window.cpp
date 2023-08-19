#include "window.hpp"

// ImGui input handler.
extern auto IMGUI_IMPL_API
ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
    -> LRESULT;

namespace fb {

// Window procedure.
static auto CALLBACK win32_window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
    -> LRESULT {
    // ImGui input handling.
    if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param)) {
        return true;
    }

    // Window messages.
    switch (message) {
        case WM_KEYDOWN:
            if (w_param == VK_ESCAPE) {
                PostQuitMessage(0);
                return 0;
            }
            break;
        case WM_DESTROY: PostQuitMessage(0); return 0;
        case WM_CLOSE: PostQuitMessage(0); return 0;
    }

    // Default window procedure.
    return DefWindowProcA(window, message, w_param, l_param);
}

class WindowImpl {
public:
    wil::unique_hwnd handle;
};

Window::Window(const Desc& desc) {
    // Module handle.
    HMODULE module_handle = GetModuleHandleA(nullptr);

    // Register window class.
    UINT class_style = 0;
    class_style |= CS_OWNDC;   // Allocates a unique device context for each window in the class.
    class_style |= CS_HREDRAW; // Redraws the client area if the width changes.
    class_style |= CS_VREDRAW; // Redraws the client area if the height changes.
    HICON icon = LoadIconA(nullptr, IDI_WINLOGO);
    HCURSOR cursor = LoadCursorA(nullptr, IDC_ARROW);
    HBRUSH background_brush = (HBRUSH)GetStockObject(BLACK_BRUSH);
    WNDCLASSEXA window_class = {
        .cbSize = sizeof(WNDCLASSEXA),
        .style = class_style,
        .lpfnWndProc = win32_window_proc,
        .hInstance = module_handle,
        .hIcon = icon,
        .hCursor = cursor,
        .hbrBackground = background_brush,
        .lpszMenuName = nullptr,
        .lpszClassName = desc.title.data(),
        .hIconSm = window_class.hIcon};
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
        WS_EX_APPWINDOW,
        desc.title.data(),
        desc.title.data(),
        window_style,
        window_x,
        window_h,
        window_width,
        window_height,
        nullptr,
        nullptr,
        module_handle,
        nullptr
    );
    FB_ASSERT_MSG(window_handle != nullptr, "Failed to create window.");

    // Show window.
    ShowWindow(window_handle, SW_SHOWDEFAULT);
    UpdateWindow(window_handle);

    // Logging.
    FB_LOG_INFO("Created window: {}", desc.title);

    // Create window implementation.
    impl = std::make_unique<WindowImpl>();
    impl->handle.reset(window_handle);
}

Window::~Window() {
    impl = nullptr;
}

auto Window::hwnd() const -> HWND {
    return impl->handle.get();
}

} // namespace fb
