#pragma once

// DirectX.
#include <d3dx12/d3dx12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <directxtk12/ResourceUploadBatch.h>

// PIX.
#define USE_PIX
#include <WinPixEventRuntime/pix3.h>

// ImGui.
#include <imgui.h>
#include <backends/imgui_impl_win32.h>

// Standard library.
#include <array>
#include <deque>

// Framebuffet.
#include <common/pch.hpp>
#include <common/common.hpp>
#include <buffet/utils/frame.hpp>
#include <buffet/win32/window.hpp>