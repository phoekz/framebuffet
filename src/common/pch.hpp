#pragma once

// Windows.
#include <windows.h>
#include <comdef.h>

// Windows Implementation Library (WIL).
#define WIL_SUPPRESS_EXCEPTIONS
#include <wil/com.h>
#include <wil/resource.h>

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
