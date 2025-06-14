#pragma once

#include "gltf.hpp"

namespace fb {

struct GenerateTangentsDesc {
    Span<const GltfVertexPosition> positions;
    Span<const GltfVertexNormal> normals;
    Span<const GltfVertexTexcoord> texcoords;
    Span<const GltfIndex> indices;
    Span<float4> tangents;
};

auto generate_tangents(const GenerateTangentsDesc& desc) -> void;

} // namespace fb
