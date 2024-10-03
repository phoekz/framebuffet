#pragma once

#include <common/common.hpp>
#include <kitchen/kitchen.hpp>
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
    uint2 window_size;
    float aspect_ratio;
    float delta_time;
    float elapsed_time;
    uint frame_index;
    fb::Inputs inputs;
    bool gui_wants_the_mouse;
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
