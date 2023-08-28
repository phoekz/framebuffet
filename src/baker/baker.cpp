#include <common/common.hpp>
#include "shaders/shaders.hpp"
#include "formats/gltf.hpp"
#include "utils/names.hpp"

#include "assets/tasks.hpp"
#include "assets/types.hpp"
#include "outputs/outputs.hpp"

namespace fb {

static auto KITCHEN_ASSET_TASKS = std::to_array<AssetTask>({
    AssetTaskCopy {"imgui_font", "fonts/Roboto-Medium.ttf"},
});

static auto KITCHEN_SHADER_TASKS = std::to_array<ShaderTask>({
    {"kitchen/graphics/gui.hlsl", "gui", {"draw_vs", "draw_ps"}},
    {"kitchen/graphics/debug_draw.hlsl", "debug_draw", {"draw_vs", "draw_ps"}},
    {"kitchen/graphics/spd.hlsl", "spd", {"downsample_cs"}},
});

static auto BUFFET_ASSET_TASKS = std::to_array<AssetTask>({
    AssetTaskTexture {
        "heatmap_magma",
        "heatmaps/magma.png",
        GLTF_BASE_COLOR_TEXTURE_FORMAT,
        AssetColorSpace::Srgb},
    AssetTaskTexture {
        "heatmap_viridis",
        "heatmaps/viridis.png",
        GLTF_BASE_COLOR_TEXTURE_FORMAT,
        AssetColorSpace::Srgb},
    AssetTaskGltf {"sci_fi_case", "models/sci_fi_case.glb"},
    AssetTaskGltf {"metal_plane", "models/metal_plane.glb"},
    AssetTaskGltf {"coconut_tree", "models/coconut_tree.glb"},
    AssetTaskGltf {"sand_plane", "models/sand_plane.glb"},
    AssetTaskGltf {"raccoon", "models/low-poly_racoon_run_animation.glb"},
    AssetTaskProceduralCube {"light_bounds", 2.0f, false},
    AssetTaskProceduralCube {"skybox", 2.0f, true},
    AssetTaskCubeTexture {
        "winter_evening",
        {
            "envmaps/winter_evening_1k_+X.exr",
            "envmaps/winter_evening_1k_-X.exr",
            "envmaps/winter_evening_1k_+Y.exr",
            "envmaps/winter_evening_1k_-Y.exr",
            "envmaps/winter_evening_1k_+Z.exr",
            "envmaps/winter_evening_1k_-Z.exr",
        },
    },
    AssetTaskHdrTexture {"farm_field", "envmaps/farm_field_2k.exr"},
    AssetTaskHdrTexture {"winter_evening", "envmaps/winter_evening_2k.exr"},
    AssetTaskProceduralSphere {"sphere", 1.0f, 32, false},
});

static auto BUFFET_SHADER_TASKS = std::to_array<ShaderTask>({
    {"buffet/demos/cards.hlsl", "cards", {"draw_vs", "draw_ps"}},
    {"buffet/demos/crate/crate.hlsl", "crate", {"draw_vs", "draw_ps"}},
    {"buffet/demos/tree/tree.hlsl", "tree", {"shadow_vs", "draw_vs", "draw_ps"}},
    {"buffet/demos/rain/rain.hlsl", "rain", {"sim_cs", "draw_vs", "draw_ps"}},
    {"buffet/demos/anim/anim.hlsl", "anim", {"draw_vs", "draw_ps"}},
    {
        "buffet/demos/fibers/fibers.hlsl",
        "fibers",
        {
            "sim_cs",
            "reset_cs",
            "cull_cs",
            "light_vs",
            "light_ps",
            "plane_vs",
            "plane_ps",
            "debug_vs",
            "debug_ps",
        },
    },
    {
        "buffet/demos/env/env.hlsl",
        "env",
        {
            "cfr_cs",
            "lut_cs",
            "irr_cs",
            "rad_cs",
            "background_vs",
            "background_ps",
            "model_vs",
            "model_ps",
            "screen_vs",
            "screen_ps",
        },
    },
});

} // namespace fb

int main() {
    using namespace fb;

    // Console.
    attach_console();

    // Bake app datas.
    bake_app_datas(FB_BUFFET_OUTPUT_DIR, "kitchen", KITCHEN_ASSET_TASKS, KITCHEN_SHADER_TASKS);
    bake_app_datas(FB_BUFFET_OUTPUT_DIR, "buffet", BUFFET_ASSET_TASKS, BUFFET_SHADER_TASKS);

    return 0;
}
