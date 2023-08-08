#pragma once

// Windows.
#include <windows.h>
#include <comdef.h>

// Windows Implementation Library (WIL).
#define WIL_SUPPRESS_EXCEPTIONS
#include <wil/com.h>
#include <wil/resource.h>

// DirectX.
#include <d3dx12/d3dx12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>
#include <dxgidebug.h>
#include <directxtk12/ResourceUploadBatch.h>
#include <directxtk12/SimpleMath.h>
#include <directxtk12/GeometricPrimitive.h>

// PIX.
#define USE_PIX
#include <WinPixEventRuntime/pix3.h>

// ImGui.
#include <imgui.h>

// Standard library.
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <format>
#include <memory>
#include <optional>
#include <ranges>
#include <source_location>
#include <span>
#include <sstream>
#include <string_view>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

// Literals.
using namespace std::literals;
