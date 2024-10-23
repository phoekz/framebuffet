#pragma once

#include "pch.hpp"

namespace fb {

auto read_whole_file(std::string_view path) -> std::vector<std::byte>;
auto write_whole_file(std::string_view path, std::span<const std::byte> data) -> void;
auto move_file_if_different(std::string_view dst_path, std::string_view src_path) -> bool;
auto delete_file(std::string_view path) -> void;
auto file_exists(std::string_view path) -> bool;
auto create_temp_path() -> std::string;
auto create_directory(std::string_view path) -> void;
auto create_directories(std::string_view path) -> void;

} // namespace fb
