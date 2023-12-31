#pragma once

#include "../pch.hpp"
#include <baked/kitchen/baked.hpp>
#include <baked/buffet/baked.hpp>

namespace fb {
template<typename Buffer, uint BufferCount>
using Multibuffer = graphics::multibuffer::Multibuffer<Buffer, BufferCount>;
using RenderTargets = graphics::render_targets::RenderTargets;
using DebugDraw = graphics::debug_draw::DebugDraw;
using Gui = graphics::gui::Gui;
} // namespace fb

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
    uint frame_index;
};

struct RenderDesc {
    GpuCommandList& cmd;
    GpuDevice& device;
    uint frame_index;
};

} // namespace fb::demos
