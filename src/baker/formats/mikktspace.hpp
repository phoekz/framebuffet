#pragma once

#include "../pch.hpp"
#include "gltf.hpp"

namespace fb {

struct GenerateTangentsDesc {
    std::span<const GltfVertexPosition> positions;
    std::span<const GltfVertexNormal> normals;
    std::span<const GltfVertexTexcoord> texcoords;
    std::span<const GltfIndex> indices;
    std::span<float4> tangents;
};

auto generate_tangents(const GenerateTangentsDesc& desc) -> void;

} // namespace fb
