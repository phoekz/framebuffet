#pragma once

#include "../baked_types.hpp"

namespace fb::baked::stockcube {

class Assets {
public:
    Assets();

    auto farm_field_hdr_texture() const -> Texture;
    auto winter_evening_hdr_texture() const -> Texture;
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

    auto screen_draw_vs() const -> std::span<const std::byte>;
    auto screen_draw_ps() const -> std::span<const std::byte>;
    auto blit_draw_vs() const -> std::span<const std::byte>;
    auto blit_draw_ps() const -> std::span<const std::byte>;

private:
    std::vector<std::byte> _data;
};

} // namespace fb::baked::stockcube
