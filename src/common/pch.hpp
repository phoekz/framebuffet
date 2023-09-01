#pragma once

// Windows.
#include <windows.h>

// DirectX.
#include <directxtk12/SimpleMath.h>

// Standard library.
#include <algorithm>
#include <array>
#include <format>
#include <optional>
#include <source_location>
#include <span>
#include <string_view>
#include <vector>

// Literals.
using namespace std::literals;

// Defines.
namespace fb {

using float2 = DirectX::SimpleMath::Vector2;
using float3 = DirectX::SimpleMath::Vector3;
using float4 = DirectX::SimpleMath::Vector4;
using float4x4 = DirectX::SimpleMath::Matrix;

using int2 = DirectX::XMINT2;
using int3 = DirectX::XMINT3;
using int4 = DirectX::XMINT4;

using uint = uint32_t;
using uint2 = DirectX::XMUINT2;
using uint3 = DirectX::XMUINT3;
using uint4 = DirectX::XMUINT4;

} // namespace fb
