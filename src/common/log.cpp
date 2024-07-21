#include "log.hpp"

#include <chrono>

namespace fb {

auto attach_console() -> void {
    // Optimistically try to attach to the parent process console to capture
    // stdout/stderr prints. It is useful when we run framebuffet without a
    // debugger, but within a console instead. It's not a big deal if this
    // fails.
    AttachConsole(ATTACH_PARENT_PROCESS);
}

auto output_debug_string(std::string_view str) -> void {
    OutputDebugStringA(str.data());
}

auto output_console_string(std::string_view str) -> void {
    if (!IsDebuggerPresent()) {
        WriteConsoleA(
            GetStdHandle(STD_OUTPUT_HANDLE),
            str.data(),
            (DWORD)str.size(),
            nullptr,
            nullptr
        );
    }
}

auto timestamp_string() -> std::string {
    const auto now = std::chrono::system_clock::now();
    return std::format("{}", now);
}

} // namespace fb
