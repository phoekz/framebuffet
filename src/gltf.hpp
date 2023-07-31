#pragma once

#include "image.hpp"
#include <string_view>
#include <vector>
#include <memory>

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

    auto vertex_count() const -> uint32_t { return (uint32_t)_vertices.size(); }
    auto index_count() const -> uint32_t { return (uint32_t)_indices.size(); }
    auto vertex_data() const -> const GltfVertex* { return _vertices.data(); }
    auto index_data() const -> const uint32_t* { return _indices.data(); }
    auto vertex_buffer_size() const -> uint32_t { return vertex_count() * sizeof(GltfVertex); }
    auto index_buffer_size() const -> uint32_t { return index_count() * sizeof(uint32_t); }

    auto base_color_texture() const -> const Image& { return *_base_color_texture; }

  private:
    std::vector<GltfVertex> _vertices;
    std::vector<uint32_t> _indices;
    std::unique_ptr<Image> _base_color_texture;
};

}  // namespace fb
