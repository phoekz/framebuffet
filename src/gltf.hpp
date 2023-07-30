#pragma once

#include "image.hpp"
#include <string_view>
#include <vector>

namespace fb {

struct GltfVertex {
    float position[3];
    float normal[3];
    float texcoord[2];
};

typedef uint32_t GltfIndex;

struct GltfModel {
    static GltfModel load(std::string_view path);

    inline uint32_t vertex_count() const {
        return (uint32_t)vertices.size();
    }
    inline uint32_t index_count() const {
        return (uint32_t)indices.size();
    }
    inline const GltfVertex* vertex_data() const {
        return vertices.data();
    }
    inline const uint32_t* index_data() const {
        return indices.data();
    }
    inline uint32_t vertex_buffer_size() const {
        return vertex_count() * sizeof(GltfVertex);
    }
    inline uint32_t index_buffer_size() const {
        return index_count() * sizeof(uint32_t);
    }

    std::vector<GltfVertex> vertices;
    std::vector<uint32_t> indices;
    Image base_color_texture;
};

}  // namespace fb
