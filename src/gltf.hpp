#pragma once

#include "image.hpp"
#include <pch.hpp>

namespace fb {

struct GltfVertex {
    float position[3];
    float normal[3];
    float texcoord[2];
};

using GltfIndex = uint32_t;

class GltfModel {
  public:
    GltfModel(std::string_view path);

    auto vertex_buffer() const -> std::span<const GltfVertex> { return _vertices; }
    auto index_buffer() const -> std::span<const uint32_t> { return _indices; }

    auto base_color_texture() const -> const Image& { return _base_color_texture; }

  private:
    std::vector<GltfVertex> _vertices;
    std::vector<uint32_t> _indices;
    Image _base_color_texture;
};

}  // namespace fb
