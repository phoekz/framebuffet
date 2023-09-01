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
    AssetTaskStockcubeOutput {
        "winter_evening_lut",
        "intermediate/stockcube/winter_evening_lut.bin",
        "intermediate/stockcube/winter_evening_lut.json"},
    AssetTaskStockcubeOutput {
        "winter_evening_irr",
        "intermediate/stockcube/winter_evening_irr.bin",
        "intermediate/stockcube/winter_evening_irr.json"},
    AssetTaskStockcubeOutput {
        "winter_evening_rad",
        "intermediate/stockcube/winter_evening_rad.bin",
        "intermediate/stockcube/winter_evening_rad.json"},
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
            "background_vs",
            "background_ps",
            "model_vs",
            "model_ps",
        },
    },
});

static auto STOCKCUBE_ASSET_TASKS = std::to_array<AssetTask>({
    AssetTaskHdrTexture {"farm_field", "envmaps/farm_field_2k.exr"},
    AssetTaskHdrTexture {"winter_evening", "envmaps/winter_evening_2k.exr"},
    AssetTaskProceduralCube {"skybox", 2.0f, true},
    AssetTaskProceduralSphere {"sphere", 1.0f, 32, false},
});

static auto STOCKCUBE_SHADER_TASKS = std::to_array<ShaderTask>({
    {"stockcube/techniques/cfr/cfr.hlsl", "cfr", {"cs"}},
    {"stockcube/techniques/lut/lut.hlsl", "lut", {"cs"}},
    {"stockcube/techniques/irr/irr.hlsl", "irr", {"acc_cs", "div_cs"}},
    {"stockcube/techniques/rad/rad.hlsl", "rad", {"acc_cs", "div_cs"}},
    {"stockcube/techniques/bg/bg.hlsl", "bg", {"vs", "ps"}},
    {"stockcube/techniques/model/model.hlsl", "model", {"vs", "ps"}},
    {"stockcube/techniques/screen/screen.hlsl", "screen", {"vs", "ps"}},
    {"stockcube/techniques/blit/blit.hlsl", "blit", {"vs", "ps"}},
});

} // namespace fb

int main() {
    using namespace fb;

    // Console.
    attach_console();

    // Bake app datas.
    using sv = std::string_view;
    FB_LOG_INFO("Output directories:");
    FB_LOG_INFO("  {}", sv(FB_BUFFET_OUTPUT_DIR));
    FB_LOG_INFO("  {}", sv(FB_STOCKCUBE_OUTPUT_DIR));
    const auto kitchen_outputs = std::to_array({
        sv(FB_BUFFET_OUTPUT_DIR),
        sv(FB_STOCKCUBE_OUTPUT_DIR),
    });
    const auto buffet_outputs = std::to_array({sv(FB_BUFFET_OUTPUT_DIR)});
    const auto stockcube_outputs = std::to_array({sv(FB_STOCKCUBE_OUTPUT_DIR)});
    bake_app_datas(kitchen_outputs, "kitchen", KITCHEN_ASSET_TASKS, KITCHEN_SHADER_TASKS);
    bake_app_datas(buffet_outputs, "buffet", BUFFET_ASSET_TASKS, BUFFET_SHADER_TASKS);
    bake_app_datas(stockcube_outputs, "stockcube", STOCKCUBE_ASSET_TASKS, STOCKCUBE_SHADER_TASKS);

    return 0;
}
