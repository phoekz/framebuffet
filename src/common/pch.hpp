#pragma once

// Disable warnings for external headers.
#pragma warning(push, 0)

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
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxgiformat.h>

// PIX.
#define USE_PIX
#include <WinPixEventRuntime/pix3.h>

// Tracy.
#include <tracy/Tracy.hpp>

// ImGui.
#include <imgui.h>
#include <backends/imgui_impl_win32.h>

// Nlohmann.
#include <nlohmann/json.hpp>

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

// Restore warnings.
#pragma warning(pop)

// Literals.
using namespace std::literals;

// Aliases.
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
using float3x3 = glm::mat<3, 3, float, glm::highp>;
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

using json = nlohmann::json;
using ord_json = nlohmann::ordered_json;

} // namespace fb
