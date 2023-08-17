#pragma once

#include "../pch.hpp"
#include "../gpu/gpu.hpp"
#include "../baked/baked.hpp"

namespace fb::demos {

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

}  // namespace fb::demos

#include "gui.hpp"
#include "debug_draw.hpp"
#include "render_targets.hpp"
