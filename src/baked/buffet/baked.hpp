#pragma once

#include "../baked_types.hpp"

namespace fb::baked::buffet {

class Assets {
public:
    auto load() -> void;

    auto heatmap_magma_texture() const -> Texture;
    auto heatmap_viridis_texture() const -> Texture;
    auto sci_fi_case_mesh() const -> Mesh;
    auto sci_fi_case_base_color_texture() const -> Texture;
    auto sci_fi_case_normal_texture() const -> Texture;
    auto sci_fi_case_metallic_roughness_texture() const -> Texture;
    auto sci_fi_case_material() const -> Material;
    auto metal_plane_mesh() const -> Mesh;
    auto metal_plane_base_color_texture() const -> Texture;
    auto metal_plane_normal_texture() const -> Texture;
    auto metal_plane_metallic_roughness_texture() const -> Texture;
    auto metal_plane_material() const -> Material;
    auto coconut_tree_mesh() const -> Mesh;
    auto coconut_tree_base_color_texture() const -> Texture;
    auto coconut_tree_material() const -> Material;
    auto sand_texture() const -> Texture;
    auto sand_mesh() const -> Mesh;
    auto raccoon_animation_mesh() const -> AnimationMesh;
    auto raccoon_base_color_texture() const -> Texture;
    auto raccoon_metallic_roughness_texture() const -> Texture;
    auto raccoon_material() const -> Material;
    auto mixamo_run_female_animation_mesh() const -> AnimationMesh;
    auto mixamo_run_female_base_color_texture() const -> Texture;
    auto mixamo_run_female_material() const -> Material;
    auto mixamo_run_male_animation_mesh() const -> AnimationMesh;
    auto mixamo_run_male_base_color_texture() const -> Texture;
    auto mixamo_run_male_material() const -> Material;
    auto light_bounds_mesh() const -> Mesh;
    auto skybox_mesh() const -> Mesh;
    auto sphere_mesh() const -> Mesh;
    auto winter_evening_lut() const -> Texture;
    auto winter_evening_irr() const -> CubeTexture;
    auto winter_evening_rad() const -> CubeTexture;
    auto shanghai_bund_lut() const -> Texture;
    auto shanghai_bund_irr() const -> CubeTexture;
    auto shanghai_bund_rad() const -> CubeTexture;
    auto industrial_sunset_02_puresky_irr() const -> CubeTexture;
    auto roboto_medium_font() const -> Font;
    auto roboto_medium_mesh() const -> Mesh;
    auto lightsaber_mesh() const -> Mesh;
    auto lightsaber_base_color_texture() const -> Texture;
    auto lightsaber_material() const -> Material;
    auto grass_mesh() const -> Mesh;
    auto grass_base_color_texture() const -> Texture;
    auto grass_material() const -> Material;

private:
    template<typename T>
    auto transmuted_span(size_t offset, size_t element_count) const -> Span<const T> {
        return Span((const T*)(_data.data() + offset), element_count);
    }

    std::vector<std::byte> _data;
};

class Shaders {
public:
    auto load() -> void;

    auto cards_background_vs() const -> Span<const std::byte>;
    auto cards_background_ps() const -> Span<const std::byte>;
    auto cards_draw_vs() const -> Span<const std::byte>;
    auto cards_draw_ps() const -> Span<const std::byte>;
    auto conras_clear_vs() const -> Span<const std::byte>;
    auto conras_clear_ps() const -> Span<const std::byte>;
    auto conras_raster_vs() const -> Span<const std::byte>;
    auto conras_raster_cr_off_ps() const -> Span<const std::byte>;
    auto conras_raster_cr_on_ps() const -> Span<const std::byte>;
    auto conras_display_vs() const -> Span<const std::byte>;
    auto conras_display_ps() const -> Span<const std::byte>;
    auto crate_draw_vs() const -> Span<const std::byte>;
    auto crate_draw_ps() const -> Span<const std::byte>;
    auto tree_shadow_vs() const -> Span<const std::byte>;
    auto tree_draw_vs() const -> Span<const std::byte>;
    auto tree_draw_ps() const -> Span<const std::byte>;
    auto rain_sim_cs() const -> Span<const std::byte>;
    auto rain_draw_vs() const -> Span<const std::byte>;
    auto rain_draw_ps() const -> Span<const std::byte>;
    auto anim_shadow_vs() const -> Span<const std::byte>;
    auto anim_ground_vs() const -> Span<const std::byte>;
    auto anim_ground_ps() const -> Span<const std::byte>;
    auto anim_anim_vs() const -> Span<const std::byte>;
    auto anim_anim_ps() const -> Span<const std::byte>;
    auto fibers_sim_cs() const -> Span<const std::byte>;
    auto fibers_reset_cs() const -> Span<const std::byte>;
    auto fibers_cull_cs() const -> Span<const std::byte>;
    auto fibers_light_vs() const -> Span<const std::byte>;
    auto fibers_light_ps() const -> Span<const std::byte>;
    auto fibers_plane_vs() const -> Span<const std::byte>;
    auto fibers_plane_ps() const -> Span<const std::byte>;
    auto fibers_debug_vs() const -> Span<const std::byte>;
    auto fibers_debug_ps() const -> Span<const std::byte>;
    auto env_background_vs() const -> Span<const std::byte>;
    auto env_background_ps() const -> Span<const std::byte>;
    auto env_model_vs() const -> Span<const std::byte>;
    auto env_model_ps() const -> Span<const std::byte>;
    auto text_background_vs() const -> Span<const std::byte>;
    auto text_background_ps() const -> Span<const std::byte>;
    auto text_glyph_vs() const -> Span<const std::byte>;
    auto text_glyph_ps() const -> Span<const std::byte>;
    auto saber_scene_vs() const -> Span<const std::byte>;
    auto saber_scene_ps() const -> Span<const std::byte>;
    auto saber_threshold_cs() const -> Span<const std::byte>;
    auto saber_downsample_cs() const -> Span<const std::byte>;
    auto saber_upsample_cs() const -> Span<const std::byte>;
    auto saber_blit_vs() const -> Span<const std::byte>;
    auto saber_blit_ps() const -> Span<const std::byte>;
    auto grass_draw_vs() const -> Span<const std::byte>;
    auto grass_draw_naive_ps() const -> Span<const std::byte>;
    auto grass_draw_atoc_ps() const -> Span<const std::byte>;

private:
    std::vector<std::byte> _data;
};

} // namespace fb::baked::buffet
