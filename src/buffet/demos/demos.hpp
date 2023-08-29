#pragma once

#include "../pch.hpp"
#include <baked/kitchen/baked.hpp>
#include <baked/buffet/baked.hpp>
#include <kitchen/graphics/graphics.hpp>
#include "textures.hpp"

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

