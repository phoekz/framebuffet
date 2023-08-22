#include "mikktspace.hpp"

#include <mikktspace.h>

namespace fb {

static auto get_num_faces(const SMikkTSpaceContext* ctx) -> int {
    const auto& desc = *(const GenerateTangentsDesc*)ctx->m_pUserData;
    return (int)desc.indices.size() / 3;
}

static auto get_num_vertices_of_face(const SMikkTSpaceContext*, int) -> int {
    return 3;
}

static auto get_position(const SMikkTSpaceContext* ctx, float* dst, int face_id, int vertex_id)
    -> void {
    const auto& desc = *(const GenerateTangentsDesc*)ctx->m_pUserData;
    const auto index = desc.indices[face_id * 3 + vertex_id];
    const auto& position = desc.positions[index];
    dst[0] = position.x;
    dst[1] = position.y;
    dst[2] = position.z;
}

static auto get_normal(const SMikkTSpaceContext* ctx, float* dst, int face_id, int vertex_id)
    -> void {
    const auto& desc = *(const GenerateTangentsDesc*)ctx->m_pUserData;
    const auto index = desc.indices[face_id * 3 + vertex_id];
    const auto& normal = desc.normals[index];
    dst[0] = normal.x;
    dst[1] = normal.y;
    dst[2] = normal.z;
}

static auto get_texcoord(const SMikkTSpaceContext* ctx, float* dst, int face_id, int vertex_id)
    -> void {
    const auto& desc = *(const GenerateTangentsDesc*)ctx->m_pUserData;
    const auto index = desc.indices[face_id * 3 + vertex_id];
    const auto& texcoord = desc.texcoords[index];
    dst[0] = texcoord.x;
    dst[1] = texcoord.y;
}

static auto set_tspace_basic(
    const SMikkTSpaceContext* ctx,
    const float* tangent,
    float sign,
    int face_id,
    int vertex_id
) -> void {
    auto& desc = *(GenerateTangentsDesc*)ctx->m_pUserData;
    const auto index = desc.indices[face_id * 3 + vertex_id];
    desc.tangents[index] = Float4(tangent[0], tangent[1], tangent[2], sign);
}

auto generate_tangents(const GenerateTangentsDesc& desc) -> void {
    SMikkTSpaceInterface callbacks = {
        .m_getNumFaces = get_num_faces,
        .m_getNumVerticesOfFace = get_num_vertices_of_face,
        .m_getPosition = get_position,
        .m_getNormal = get_normal,
        .m_getTexCoord = get_texcoord,
        .m_setTSpaceBasic = set_tspace_basic,
    };
    SMikkTSpaceContext context = {
        .m_pInterface = &callbacks,
        .m_pUserData = (void*)&desc,
    };
    genTangSpaceDefault(&context);
}

} // namespace fb
