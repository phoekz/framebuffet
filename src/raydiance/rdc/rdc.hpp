#pragma once

#include <common/common.hpp>
#include <kitchen/kitchen.hpp>
#include <baked/raydiance/baked.hpp>

#include "geometry.hpp"
#include "sampling.hpp"

namespace fb {

auto rdc_render(const baked::raydiance::Assets& assets, const baked::raydiance::Shaders& shaders)
    -> void;

} // namespace fb
