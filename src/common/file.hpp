#pragma once

#include "pch.hpp"

namespace fb {

class FileBuffer {
public:
    static auto from_path(std::string_view path) -> FileBuffer;

    FileBuffer() = default;
    FileBuffer(const FileBuffer&) = delete;
    auto operator=(const FileBuffer&) -> FileBuffer& = delete;
    FileBuffer(FileBuffer&& other) noexcept;
    auto operator=(FileBuffer&& other) noexcept -> FileBuffer&;
    ~FileBuffer();

    auto bytes() const -> const std::byte* { return _bytes; }
    auto byte_count() const -> uint { return _byte_count; }
    auto as_span() const -> Span<const std::byte> {
        return Span<const std::byte>(_bytes, _byte_count);
    }

private:
    std::byte* _bytes = nullptr;
    uint _byte_count = 0;
};

auto write_whole_file(std::string_view path, Span<const std::byte> data) -> void;
auto move_file_if_different(std::string_view dst_path, std::string_view src_path) -> bool;
auto delete_file(std::string_view path) -> void;
auto file_exists(std::string_view path) -> bool;
auto create_temp_path() -> std::string;
auto create_directory(std::string_view path) -> void;
auto create_directories(std::string_view path) -> void;
auto make_absolute_path(std::string_view path) -> std::string;

} // namespace fb
