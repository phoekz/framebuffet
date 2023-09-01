#pragma once

#include "../pch.hpp"
#include <baked/kitchen/baked.hpp>
#include <baked/buffet/baked.hpp>

using RenderTargets = fb::graphics::render_targets::RenderTargets;
using DebugDraw = fb::graphics::debug_draw::DebugDraw;
using Gui = fb::graphics::gui::Gui;

namespace fb::demos {

struct Baked {
    struct {
        baked::kitchen::Assets assets;
        baked::kitchen::Shaders shaders;
    } kitchen;
    struct {
        baked::buffet::Assets assets;
        baked::buffet::Shaders shaders;
    } buffet;
};

struct UpdateDesc {
    uint2 window_size;
    float aspect_ratio;
    float delta_time;
    float elapsed_time;
    uint frame_index;
};

struct GuiDesc {
    uint2 window_size;
};

struct RenderDesc {
    GpuCommandList& cmd;
    GpuDevice& device;
};

} // namespace fb::demos
