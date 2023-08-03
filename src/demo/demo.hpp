#pragma once

#include <pch.hpp>
#include <gpu/gpu.hpp>
#include "../utils.hpp"
#include "../maths.hpp"
#include "../pcg.hpp"
#include "../gltf.hpp"

namespace fb::demo {

struct UpdateDesc {
    float aspect_ratio;
    float delta_time;
    float elapsed_time;
    uint32_t frame_index;
};

}  // namespace fb::demo
