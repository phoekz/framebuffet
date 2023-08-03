#pragma once

#include <pch.hpp>
#include "../utils.hpp"
#include "../maths.hpp"
#include "../pcg.hpp"
#include "../dx12.hpp"
#include "../shaders.hpp"
#include "../buffers.hpp"
#include "../gltf.hpp"
#include "../descriptors.hpp"
#include "../samplers.hpp"
#include "../root_signature.hpp"
#include "../render_targets.hpp"
#include "../debug_draw.hpp"

namespace fb::demo {

struct UpdateDesc {
    float aspect_ratio;
    float delta_time;
    float elapsed_time;
    uint32_t frame_index;
};

}  // namespace fb::demo
