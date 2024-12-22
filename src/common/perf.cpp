#include "perf.hpp"

#include "GetTimeSinceProcessStart.h"
#include <shellapi.h>

namespace fb {

auto get_time_since_process_start() -> double {
    return GetTimeSinceProcessStart();
}

auto tracy_capture() -> void {
    const wchar_t* path = L"tracy-profiler.exe";
    const wchar_t* args = L"-a 127.0.0.1";
    ShellExecuteW(nullptr, L"open", path, args, nullptr, SW_SHOW);
}

} // namespace fb
