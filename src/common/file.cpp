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
    FB_ASSERT_MSG(file != INVALID_HANDLE_VALUE, "Failed to open file: {}", path);

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

auto move_file_if_different(std::string_view dst_path, std::string_view src_path) -> bool {
    if (file_exists(dst_path)) {
        const auto dst_data = read_whole_file(dst_path);
        const auto src_data = read_whole_file(src_path);
        if (dst_data == src_data) {
            return false;
        }
    }
    MoveFileExA(src_path.data(), dst_path.data(), MOVEFILE_REPLACE_EXISTING);
    return true;
}

auto delete_file(std::string_view path) -> void {
    DeleteFileA(path.data());
}

auto file_exists(std::string_view path) -> bool {
    DWORD attributes = GetFileAttributesA(path.data());
    return attributes != INVALID_FILE_ATTRIBUTES;
}

auto create_temp_path() -> std::string {
    char temp_dir[MAX_PATH] = {};
    char temp_path[MAX_PATH] = {};
    GetTempPath2A(MAX_PATH, temp_dir);
    GetTempFileNameA(temp_dir, "fb", 0, temp_path);
    return std::string(temp_path);
}

auto create_directory(std::string_view path) -> void {
    std::filesystem::create_directory(path);
}

auto create_directories(std::string_view path) -> void {
    std::filesystem::create_directories(path);
}

auto make_absolute_path(std::string_view path) -> std::string {
    return std::filesystem::absolute(path).string();
}

} // namespace fb
