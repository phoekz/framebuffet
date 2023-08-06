#include "gltf.hpp"
#include "utils.hpp"

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

namespace fb {

GltfModel::GltfModel(std::string_view path) {
    // Load GLTF.
    cgltf_options options = {};
    cgltf_data* data = nullptr;
    FB_ASSERT(cgltf_parse_file(&options, path.data(), &data) == cgltf_result_success);
    FB_ASSERT(cgltf_load_buffers(&options, data, path.data()) == cgltf_result_success);
    FB_ASSERT(cgltf_validate(data) == cgltf_result_success);
    FB_ASSERT(data->meshes_count == 1);
    cgltf_mesh& mesh = data->meshes[0];
    FB_ASSERT(mesh.primitives_count == 1);
    cgltf_primitive& primitive = mesh.primitives[0];
    FB_ASSERT(primitive.type == cgltf_primitive_type_triangles);

    // Find accessors.
    cgltf_accessor* position_accessor = nullptr;
    cgltf_accessor* normal_accessor = nullptr;
    cgltf_accessor* texcoord_accessor = nullptr;
    cgltf_accessor* index_accessor = primitive.indices;
    for (size_t i = 0; i < primitive.attributes_count; i++) {
        cgltf_attribute& attribute = primitive.attributes[i];
        if (attribute.type == cgltf_attribute_type_position) {
            position_accessor = attribute.data;
        } else if (attribute.type == cgltf_attribute_type_normal) {
            normal_accessor = attribute.data;
        } else if (attribute.type == cgltf_attribute_type_texcoord) {
            texcoord_accessor = attribute.data;
        }
    }
    FB_ASSERT(position_accessor->type == cgltf_type_vec3);
    FB_ASSERT(normal_accessor->type == cgltf_type_vec3);
    FB_ASSERT(texcoord_accessor->type == cgltf_type_vec2);
    FB_ASSERT(
        index_accessor->component_type == cgltf_component_type_r_16u
        || index_accessor->component_type == cgltf_component_type_r_32u);
    size_t vertex_count = position_accessor->count;
    FB_ASSERT(normal_accessor->count == vertex_count);
    FB_ASSERT(texcoord_accessor->count == vertex_count);
    size_t index_count = index_accessor->count;

    // Read geometries.
    _vertices.resize(vertex_count);
    _indices.resize(index_count);
    for (size_t i = 0; i < vertex_count; i++) {
        GltfVertex& v = _vertices[i];
        FB_ASSERT(cgltf_accessor_read_float(position_accessor, i, v.position, 3));
        FB_ASSERT(cgltf_accessor_read_float(normal_accessor, i, v.normal, 3));
        FB_ASSERT(cgltf_accessor_read_float(texcoord_accessor, i, v.texcoord, 2));
    }
    for (size_t i = 0; i < index_count; i++) {
        uint32_t& index = _indices[i];
        FB_ASSERT(cgltf_accessor_read_uint(index_accessor, i, &index, 1));
    }

    // Swap indices.
    for (size_t i = 0; i < index_count; i += 3) {
        std::swap(_indices[i + 0], _indices[i + 2]);
    }

    // Read material.
    cgltf_material* material = primitive.material;
    FB_ASSERT(material != nullptr);
    FB_ASSERT(material->has_pbr_metallic_roughness);
    cgltf_pbr_metallic_roughness& pbr = material->pbr_metallic_roughness;
    FB_ASSERT(pbr.base_color_texture.texture != nullptr);
    FB_ASSERT(pbr.base_color_texture.texture->image != nullptr);
    {
        auto& image = *pbr.base_color_texture.texture->image;
        auto image_view = image.buffer_view;
        auto image_data = (const std::byte*)cgltf_buffer_view_data(image_view);
        auto image_span = std::span(image_data, image_view->size);
        _base_color_texture = Image::load(image_span);
    }

    // Cleanup.
    cgltf_free(data);
}

}  // namespace fb
