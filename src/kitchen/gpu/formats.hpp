#pragma once

#include "../pch.hpp"

namespace fb {

auto dxgi_format_unit_byte_count(DXGI_FORMAT format) -> uint;
auto dxgi_format_type_level(DXGI_FORMAT format) -> D3D_FORMAT_TYPE_LEVEL;

} // namespace fb
