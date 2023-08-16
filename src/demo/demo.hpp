#pragma once

#include <pch.hpp>
#include <gpu/gpu.hpp>
#include "../utils.hpp"
#include "../maths.hpp"
#include "../pcg.hpp"
#include "../gltf.hpp"
#include "../exr.hpp"
#include "../gui_common.hpp"

namespace fb::demo {

struct UpdateDesc {
    Uint2 window_size;
    float aspect_ratio;
    float delta_time;
    float elapsed_time;
    uint32_t frame_index;
};

}  // namespace fb::demo
