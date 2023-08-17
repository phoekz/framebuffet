#include "file.hpp"
#include "error.hpp"

namespace fb {

auto read_whole_file(std::string_view path) -> std::vector<std::byte> {
    wil::unique_hfile file(CreateFileA(
        path.data(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr));
    FB_ASSERT_MSG(file.is_valid(), "Failed to open file.");

    LARGE_INTEGER file_size;
    FB_ASSERT_MSG(GetFileSizeEx(file.get(), &file_size), "Failed to get file size.");

    std::vector<std::byte> buffer(file_size.QuadPart);
    DWORD bytes_read;
    FB_ASSERT_MSG(
        ReadFile(file.get(), buffer.data(), (DWORD)buffer.size(), &bytes_read, nullptr),
        "Failed to read file.");

    FB_LOG_INFO("Read {} bytes from {}", bytes_read, path);

    return buffer;
}

auto write_whole_file(std::string_view path, std::span<const std::byte> data) -> void {
    wil::unique_hfile file(CreateFileA(
        path.data(),
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr));
    FB_ASSERT_MSG(file.is_valid(), "Failed to create file.");

    FB_ASSERT_MSG(
        WriteFile(file.get(), data.data(), (DWORD)data.size(), nullptr, nullptr),
        "Failed to write file.");

    FB_LOG_INFO("Wrote {} bytes to {}", data.size(), path);
}

}  // namespace fb
