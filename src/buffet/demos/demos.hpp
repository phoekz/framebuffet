#pragma once

#include "../pch.hpp"
#include "common.hpp"
#include "anim/anim.hpp"
#include "conras/conras.hpp"
#include "crate/crate.hpp"
#include "env/env.hpp"
#include "fibers/fibers.hpp"
#include "grass/grass.hpp"
#include "rain/rain.hpp"
#include "saber/saber.hpp"
#include "text/text.hpp"
#include "tree/tree.hpp"
#include "cards/cards.hpp"

// clang-format off
#define DEMO_LIST(x) \
    x(anim, Anim) \
    x(conras, Conras) \
    x(crate, Crate) \
    x(env, Env) \
    x(fibers, Fibers) \
    x(grass, Grass) \
    x(rain, Rain) \
    x(saber, Saber) \
    x(text, Text) \
    x(tree, Tree)
// clang-format on

namespace fb::demos {

inline constexpr std::string_view NAME = "Buffet"sv;

enum class Demo : uint {
#define X(_, name) name,
    DEMO_LIST(X)
#undef X
    Count
};

struct DemoRenderTargetViews {
    KcnRenderTargetView* pre = nullptr;
    KcnRenderTargetView* post = nullptr;
};

struct Demos {
#define X(name, _) name::Demo name;
    DEMO_LIST(X);
#undef X

    cards::Demo cards;

    std::array<DemoRenderTargetViews, (uint)Demo::Count> render_target_views = {};
};

struct CreateDesc {
    const Baked& baked;
    GpuDevice& device;
};

auto create(Demos& demos, const CreateDesc& desc) -> void;
auto gui(Demos& demos, const GuiDesc& desc) -> void;
auto update(Demos& demos, const UpdateDesc& desc) -> void;
auto transition_to_render_target(Demos& demos, const RenderDesc& desc) -> void;
auto clear_render_targets(Demos& demos, const RenderDesc& desc) -> void;
auto render_demos(Demos& demos, const RenderDesc& desc) -> void;
auto transition_to_resolve(Demos& demos, const RenderDesc& desc) -> void;
auto resolve_render_targets(Demos& demos, const RenderDesc& desc) -> void;
auto transition_to_shader_resource(Demos& demos, const RenderDesc& desc) -> void;
auto render_compose(Demos& demos, const RenderDesc& desc) -> void;

template<Archive A>
auto archive(Demos& demos, A& arc) -> void {
#define X(name, _) name::archive(demos.name, arc);
    DEMO_LIST(X);
#undef X
    cards::archive(demos.cards, arc);
}

} // namespace fb::demos
