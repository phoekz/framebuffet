#pragma once

#include "../baked_types.hpp"

namespace fb::baked::stockcube {

class Assets {
public:
    Assets();

    auto farm_field_hdr_texture() const -> Texture;
    auto winter_evening_hdr_texture() const -> Texture;
    auto skybox_mesh() const -> Mesh;
    auto sphere_mesh() const -> Mesh;

private:
    template<typename T>
    auto transmuted_span(size_t offset, size_t element_size) const -> std::span<const T> {
        return std::span((const T*)(_data.data() + offset), element_size);
    }

    std::vector<std::byte> _data;
};

class Shaders {
public:
    Shaders();

    auto cfr_cs() const -> std::span<const std::byte>;
    auto lut_cs() const -> std::span<const std::byte>;
    auto irr_acc_cs() const -> std::span<const std::byte>;
    auto irr_div_cs() const -> std::span<const std::byte>;
    auto rad_acc_cs() const -> std::span<const std::byte>;
    auto rad_div_cs() const -> std::span<const std::byte>;
    auto bg_vs() const -> std::span<const std::byte>;
    auto bg_ps() const -> std::span<const std::byte>;
    auto model_vs() const -> std::span<const std::byte>;
    auto model_ps() const -> std::span<const std::byte>;
    auto screen_vs() const -> std::span<const std::byte>;
    auto screen_ps() const -> std::span<const std::byte>;
    auto blit_vs() const -> std::span<const std::byte>;
    auto blit_ps() const -> std::span<const std::byte>;

private:
    std::vector<std::byte> _data;
};

} // namespace fb::baked::stockcube
