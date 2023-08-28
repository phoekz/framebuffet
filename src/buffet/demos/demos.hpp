#pragma once

#include "../pch.hpp"
#include <baked/kitchen/baked.hpp>
#include <baked/buffet/baked.hpp>

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
    Uint2 window_size;
    float aspect_ratio;
    float delta_time;
    float elapsed_time;
    uint32_t frame_index;
};

struct GuiDesc {
    Uint2 window_size;
};

} // namespace fb::demos

#include <graphics/gui.hpp>
#include <graphics/debug_draw.hpp>
#include <graphics/render_targets.hpp>
#include "textures.hpp"
