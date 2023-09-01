#pragma once

#include <kitchen/kitchen.hpp>

#include "common.hpp"

#include "cfr/cfr.hpp"
#include "lut/lut.hpp"
#include "irr/irr.hpp"
#include "rad/rad.hpp"
#include "bg/bg.hpp"
#include "model/model.hpp"
#include "screen/screen.hpp"
#include "blit/blit.hpp"

namespace fb::techniques {

inline constexpr std::string_view NAME = "Stockcube"sv;

struct Techniques {
    RenderTargets render_targets;
    std::string rect_texture_name;
    GpuTextureSrv rect_texture;
    cfr::Technique cfr;
    lut::Technique lut;
    irr::Technique irr;
    rad::Technique rad;
    bg::Technique bg;
    DebugDraw debug_draw;
    model::Technique model;
    screen::Technique screen;
    blit::Technique blit;
};

struct CreateDesc {
    const Baked& baked;
    GpuDevice& device;
};

auto create(Techniques& techs, const CreateDesc& desc) -> void;
auto gui(Techniques& techs, const GuiDesc& desc) -> void;
auto update(Techniques& techs, const UpdateDesc& desc) -> void;
auto render_main(Techniques& techs, RenderTargets& render_targets, const RenderDesc& desc) -> void;
auto render_compositing(Techniques& techs, const RenderDesc& desc) -> void;

} // namespace fb::techniques
