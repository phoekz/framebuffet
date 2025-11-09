#include "file.hpp"
#include "error.hpp"
#include "time.hpp"

#include <filesystem>

namespace fb {

auto FileBuffer::from_path(std::string_view path) -> FileBuffer {
    const auto timer = Instant();

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

    FileBuffer buffer;
    buffer._byte_count = (uint)file_size.QuadPart;
    buffer._bytes = (std::byte*)
        VirtualAlloc(nullptr, buffer._byte_count, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    DWORD bytes_read;
    FB_ASSERT_MSG(
        ReadFile(file, buffer._bytes, (DWORD)buffer._byte_count, &bytes_read, nullptr),
        "Failed to read file: {}",
        path
    );

    const double elapsed_time = timer.elapsed_time();
    FB_LOG_INFO(
        "Read file {}: {} bytes, {:.3f} ms, {:.3f} MB/s, ",
        path,
        bytes_read,
        elapsed_time * 1e3,
        (bytes_read / 1e6) / elapsed_time
    );

    CloseHandle(file);

    return buffer;
}

FileBuffer::FileBuffer(FileBuffer&& other) noexcept
    : _bytes(std::exchange(other._bytes, nullptr))
    , _byte_count(std::exchange(other._byte_count, 0)) {}

FileBuffer& FileBuffer::operator=(FileBuffer&& other) noexcept {
    if (this != &other) {
        if (_bytes != nullptr) {
            VirtualFree(_bytes, 0, MEM_RELEASE);
            _bytes = nullptr;
        }
        _bytes = std::exchange(other._bytes, nullptr);
        _byte_count = std::exchange(other._byte_count, 0);
    }
    return *this;
}

FileBuffer::~FileBuffer() {
    if (_bytes != nullptr) {
        VirtualFree(_bytes, 0, MEM_RELEASE);
        _bytes = nullptr;
    }
}

auto write_whole_file(std::string_view path, Span<const std::byte> data) -> void {
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
        const auto dst_data = FileBuffer::from_path(dst_path);
        const auto src_data = FileBuffer::from_path(src_path);
        if (dst_data.byte_count() == src_data.byte_count()) {
            if (std::memcmp(dst_data.bytes(), src_data.bytes(), dst_data.byte_count()) == 0) {
                return false;
            }
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
