#pragma once

#include "../baked_types.hpp"

namespace fb::baked::raydiance {

class Assets {
public:
    auto load() -> void;

    auto cube_mesh() const -> Mesh;
    auto sphere_mesh() const -> Mesh;
    auto rounded_cube_mesh() const -> Mesh;
    auto rounded_cube_base_color_texture() const -> Texture;
    auto rounded_cube_material() const -> Material;
    auto plane_mesh() const -> Mesh;
    auto plane_texture() const -> Texture;

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

private:
    std::vector<std::byte> _data;
};

} // namespace fb::baked::raydiance
