#pragma once

// Windows.
#include <Windows.h>
#include <Unknwn.h>

// GLM.
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_precision.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp>

// DirectX.
#include <dxgiformat.h>

// Tracy.
#include <tracy/Tracy.hpp>

// Standard library.
#include <algorithm>
#include <array>
#include <format>
#include <optional>
#include <source_location>
#include <span>
#include <string_view>
#include <unordered_set>
#include <variant>
#include <vector>

// Literals.
using namespace std::literals;

// Defines.
namespace fb {

template<typename T>
using Option = std::optional<T>;
template<typename T>
using MutRef = std::reference_wrapper<T>;
template<typename T>
using ConstRef = std::reference_wrapper<const T>;

using float2 = glm::vec<2, float, glm::highp>;
using float3 = glm::vec<3, float, glm::highp>;
using float4 = glm::vec<4, float, glm::highp>;
using float4x4 = glm::mat<4, 4, float, glm::highp>;
using float_quat = glm::qua<float, glm::highp>;

using ubyte4 = glm::vec<4, uint8_t, glm::highp>;

using int2 = glm::vec<2, int, glm::highp>;
using int3 = glm::vec<3, int, glm::highp>;
using int4 = glm::vec<4, int, glm::highp>;

using uint = uint32_t;
using uint2 = glm::vec<2, uint, glm::highp>;
using uint3 = glm::vec<3, uint, glm::highp>;
using uint4 = glm::vec<4, uint, glm::highp>;

} // namespace fb
