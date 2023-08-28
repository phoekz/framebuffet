#pragma once

#include "pch.hpp"

namespace fb {

auto from_wstr(std::wstring_view wstr) -> std::string;
auto to_wstr(std::string_view str) -> std::wstring;
auto str_replace(std::string_view str, std::string_view from, std::string_view to) -> std::string;

} // namespace fb
