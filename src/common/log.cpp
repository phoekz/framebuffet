#include "log.hpp"

namespace fb {

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
            NULL);
    }
}

}  // namespace fb
