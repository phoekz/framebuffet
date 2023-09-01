#include "file.hpp"
#include "error.hpp"

#include <filesystem>

namespace fb {

auto read_whole_file(std::string_view path) -> std::vector<std::byte> {
    HANDLE file = CreateFileA(
        path.data(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );
    FB_ASSERT_MSG(file != nullptr, "Failed to open file: {}", path);

    LARGE_INTEGER file_size;
    FB_ASSERT_MSG(GetFileSizeEx(file, &file_size), "Failed to get file size: {}", path);

    std::vector<std::byte> buffer(file_size.QuadPart);
    DWORD bytes_read;
    FB_ASSERT_MSG(
        ReadFile(file, buffer.data(), (DWORD)buffer.size(), &bytes_read, nullptr),
        "Failed to read file: {}",
        path
    );

    FB_LOG_TRACE("Read {} bytes from file: {}", bytes_read, path);

    CloseHandle(file);

    return buffer;
}

auto write_whole_file(std::string_view path, std::span<const std::byte> data) -> void {
    HANDLE file = CreateFileA(
        path.data(),
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );
    FB_ASSERT_MSG(file != nullptr, "Failed to create file: {}", path);

    FB_ASSERT_MSG(
        WriteFile(file, data.data(), (DWORD)data.size(), nullptr, nullptr),
        "Failed to write file: {}",
        path
    );

    FB_LOG_TRACE("Wrote {} bytes to file: {}", data.size(), path);

    CloseHandle(file);
}

auto file_exists(std::string_view path) -> bool {
    return std::filesystem::exists(path);
}

auto create_directory(std::string_view path) -> void {
    std::filesystem::create_directory(path);
}

auto create_directories(std::string_view path) -> void {
    std::filesystem::create_directories(path);
}

} // namespace fb
