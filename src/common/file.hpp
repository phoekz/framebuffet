#pragma once

#include "pch.hpp"

namespace fb {

auto read_whole_file(std::string_view path) -> std::vector<std::byte>;
auto write_whole_file(std::string_view path, std::span<const std::byte> data) -> void;

} // namespace fb