#include "formats.hpp"

#include <d3dx12/d3dx12_property_format_table.h>

namespace fb {

auto dxgi_format_unit_byte_count(DXGI_FORMAT format) -> uint {
    return D3D12_PROPERTY_LAYOUT_FORMAT_TABLE::GetBitsPerUnit(format) / 8;
}

auto dxgi_format_type_level(DXGI_FORMAT format) -> D3D_FORMAT_TYPE_LEVEL {
    return D3D12_PROPERTY_LAYOUT_FORMAT_TABLE::GetTypeLevel(format);
}

} // namespace fb
