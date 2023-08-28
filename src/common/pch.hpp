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
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <source_location>
#include <span>
#include <string_view>

// Literals.
using namespace std::literals;
