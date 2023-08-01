#include "utils.hpp"

namespace fb {

//
// File I/O.
//

auto read_whole_file(std::string_view path) -> std::vector<std::byte> {
    wil::unique_hfile file(CreateFileA(
        path.data(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr));
    FAIL_FAST_IF_NULL_MSG(file.get(), "Failed to open file.");

    LARGE_INTEGER file_size;
    FAIL_FAST_IF_WIN32_BOOL_FALSE_MSG(
        GetFileSizeEx(file.get(), &file_size),
        "Failed to get file size.");

    std::vector<std::byte> buffer(file_size.QuadPart);
    DWORD bytes_read;
    FAIL_FAST_IF_WIN32_BOOL_FALSE_MSG(
        ReadFile(file.get(), buffer.data(), (DWORD)buffer.size(), &bytes_read, nullptr),
        "Failed to read file.");

    return buffer;
}

//
// Logging.
//

auto log_level_name(LogLevel level) -> std::string_view {
    switch (level) {
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warn:
            return "WARN";
        case LogLevel::Error:
            return "ERROR";
    }
    return "UNKNOWN";
}

auto log_output_debug_string(std::string_view str) -> void {
    OutputDebugStringA(str.data());
}

//
// String conversions.
//

auto from_wstr(std::wstring_view wstr) -> std::string {
    const int size = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), -1, &str[0], size, nullptr, nullptr);
    str.pop_back();
    return str;
}

auto to_wstr(std::string_view str) -> std::wstring {
    const int size = MultiByteToWideChar(CP_UTF8, 0, str.data(), -1, nullptr, 0);
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.data(), -1, &wstr[0], size);
    wstr.pop_back();
    return wstr;
}

//
// Frame timing.
//

static auto win32_get_frequency() -> uint64_t {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return frequency.QuadPart;
}

static auto win32_get_performance_counter() -> uint64_t {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter.QuadPart;
}

FrameTiming::FrameTiming() {
    m_frequency = win32_get_frequency();
    m_curr_time = win32_get_performance_counter();
    m_prev_time = m_curr_time;
    m_delta_time = 0;
    m_elapsed_time = 0;
    m_delta_time_sec = 0.0f;
    m_elapsed_time_sec = 0.0f;
}

auto FrameTiming::update() -> void {
    m_prev_time = m_curr_time;
    m_curr_time = win32_get_performance_counter();
    m_delta_time = m_curr_time - m_prev_time;
    m_elapsed_time += m_delta_time;
    m_delta_time_sec = (float)((double)m_delta_time / (double)m_frequency);
    m_elapsed_time_sec = (float)((double)m_elapsed_time / (double)m_frequency);
}

auto FrameTiming::delta_time() const -> float {
    return m_delta_time_sec;
}

auto FrameTiming::elapsed_time() const -> float {
    return m_elapsed_time_sec;
}

}  // namespace fb
