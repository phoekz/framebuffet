#include "gltf.hpp"
#include "utils.hpp"

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

namespace fb {

GltfModel::GltfModel(std::string_view gltf_path) {
    // Load GLTF.
    cgltf_options options = {};
    cgltf_data* data = nullptr;
    FB_ASSERT(cgltf_parse_file(&options, gltf_path.data(), &data) == cgltf_result_success);
    FB_ASSERT(cgltf_load_buffers(&options, data, gltf_path.data()) == cgltf_result_success);
    FB_ASSERT(cgltf_validate(data) == cgltf_result_success);
    FB_ASSERT(data->meshes_count == 1);
    const auto& mesh = data->meshes[0];
    FB_ASSERT(mesh.primitives_count == 1);
    const auto& primitive = mesh.primitives[0];
    FB_ASSERT(primitive.type == cgltf_primitive_type_triangles);

    // Ensure parent are initialized before children when iterating in order.
    {
        auto visited = std::vector<bool>(data->nodes_count, false);
        for (size_t i = 0; i < data->nodes_count; i++) {
            const auto* node = &data->nodes[i];
            const auto curr = (size_t)(node - data->nodes);
            if (node->parent) {
                const auto parent = (size_t)(node->parent - data->nodes);
                FB_ASSERT(visited[parent]);
            }
            visited[curr] = true;
        }
    }

    // Read geometries.
    const cgltf_accessor* position_accessor = nullptr;
    const cgltf_accessor* normal_accessor = nullptr;
    const cgltf_accessor* texcoord_accessor = nullptr;
    const cgltf_accessor* index_accessor = primitive.indices;
    for (size_t i = 0; i < primitive.attributes_count; i++) {
        const auto& attribute = primitive.attributes[i];
        if (attribute.type == cgltf_attribute_type_position) {
            position_accessor = attribute.data;
        } else if (attribute.type == cgltf_attribute_type_normal) {
            normal_accessor = attribute.data;
        } else if (attribute.type == cgltf_attribute_type_texcoord) {
            texcoord_accessor = attribute.data;
        }
    }
    FB_ASSERT(position_accessor != nullptr);
    FB_ASSERT(normal_accessor != nullptr);
    FB_ASSERT(texcoord_accessor != nullptr);
    FB_ASSERT(index_accessor != nullptr);
    FB_ASSERT(position_accessor->type == cgltf_type_vec3);
    FB_ASSERT(normal_accessor->type == cgltf_type_vec3);
    FB_ASSERT(texcoord_accessor->type == cgltf_type_vec2);
    FB_ASSERT(
        index_accessor->component_type == cgltf_component_type_r_16u
        || index_accessor->component_type == cgltf_component_type_r_32u);
    const auto vertex_count = position_accessor->count;
    FB_ASSERT(vertex_count > 0);
    FB_ASSERT(normal_accessor->count == vertex_count);
    FB_ASSERT(texcoord_accessor->count == vertex_count);
    const auto index_count = index_accessor->count;
    FB_ASSERT(index_count > 0);
    FB_ASSERT(index_count % 3 == 0);
    _vertices.resize(vertex_count);
    _indices.resize(index_count);
    for (size_t i = 0; i < vertex_count; i++) {
        auto& v = _vertices[i];
        FB_ASSERT(cgltf_accessor_read_float(position_accessor, i, (float*)&v.position, 3));
        FB_ASSERT(cgltf_accessor_read_float(normal_accessor, i, (float*)&v.normal, 3));
        FB_ASSERT(cgltf_accessor_read_float(texcoord_accessor, i, (float*)&v.texcoord, 2));
    }
    for (size_t i = 0; i < index_count; i += 3) {
        FB_ASSERT(cgltf_accessor_read_uint(index_accessor, i + 0, &_indices[i + 0], 1));
        FB_ASSERT(cgltf_accessor_read_uint(index_accessor, i + 1, &_indices[i + 1], 1));
        FB_ASSERT(cgltf_accessor_read_uint(index_accessor, i + 2, &_indices[i + 2], 1));
        std::swap(_indices[i + 0], _indices[i + 2]);
    }

    // Read material.
    const auto* material = primitive.material;
    FB_ASSERT(material != nullptr);
    FB_ASSERT(material->has_pbr_metallic_roughness);
    const auto& pbr = material->pbr_metallic_roughness;
    FB_ASSERT(pbr.base_color_texture.texture != nullptr);
    FB_ASSERT(pbr.base_color_texture.texture->image != nullptr);
    {
        const auto& image = *pbr.base_color_texture.texture->image;
        const auto image_view = image.buffer_view;
        const auto image_data = (const std::byte*)cgltf_buffer_view_data(image_view);
        const auto image_span = std::span(image_data, image_view->size);
        _base_color_texture = Image::load(image_span);
    }

    if (data->skins_count > 0) {
        // Find accessors.
        const cgltf_accessor* joints_accessor = nullptr;
        const cgltf_accessor* weights_accessor = nullptr;
        for (size_t i = 0; i < primitive.attributes_count; i++) {
            const auto& attribute = primitive.attributes[i];
            if (attribute.type == cgltf_attribute_type_joints) {
                joints_accessor = attribute.data;
            } else if (attribute.type == cgltf_attribute_type_weights) {
                weights_accessor = attribute.data;
            }
        }

        // Validate.
        {
            FB_ASSERT(joints_accessor != nullptr);
            FB_ASSERT(
                joints_accessor->component_type == cgltf_component_type_r_16u
                || joints_accessor->component_type == cgltf_component_type_r_32u);
            FB_ASSERT(joints_accessor->type == cgltf_type_vec4);
            FB_ASSERT(joints_accessor->count == vertex_count);

            FB_ASSERT(weights_accessor != nullptr);
            FB_ASSERT(weights_accessor->component_type == cgltf_component_type_r_32f);
            FB_ASSERT(weights_accessor->type == cgltf_type_vec4);
            FB_ASSERT(weights_accessor->count == vertex_count);

            FB_ASSERT(data->skins_count == 1);
            const auto& skin = data->skins[0];
            const auto* ibms = skin.inverse_bind_matrices;
            FB_ASSERT(ibms != nullptr);
            FB_ASSERT(ibms->component_type == cgltf_component_type_r_32f);
            FB_ASSERT(ibms->type == cgltf_type_mat4);
            FB_ASSERT(ibms->count == skin.joints_count);

            FB_ASSERT(data->animations_count == 1);
            const auto& animation = data->animations[0];
            FB_ASSERT(animation.samplers_count > 0);
            FB_ASSERT(animation.channels_count > 0);
            FB_ASSERT(animation.channels_count == animation.samplers_count);
            for (size_t i = 0; i < animation.channels_count; i++) {
                const auto& channel = animation.channels[i];
                const auto& sampler = animation.samplers[i];
                FB_ASSERT(channel.target_node != nullptr);
                FB_ASSERT(
                    channel.target_path == cgltf_animation_path_type_translation
                    || channel.target_path == cgltf_animation_path_type_rotation
                    || channel.target_path == cgltf_animation_path_type_scale);
                FB_ASSERT(sampler.interpolation == cgltf_interpolation_type_linear);
                FB_ASSERT(sampler.input->component_type == cgltf_component_type_r_32f);
                FB_ASSERT(sampler.input->type == cgltf_type_scalar);
                FB_ASSERT(sampler.input->count > 0);
                FB_ASSERT(sampler.input->has_min);
                FB_ASSERT(sampler.input->min[0] == 0.0f);
                FB_ASSERT(sampler.input->has_max);
                FB_ASSERT(sampler.input->max[0] > 0.0f);
                FB_ASSERT(sampler.output->component_type == cgltf_component_type_r_32f);
                FB_ASSERT(sampler.output->count > 0);
                FB_ASSERT(sampler.input->count == sampler.output->count);
                switch (channel.target_path) {
                    case cgltf_animation_path_type_translation:
                        FB_ASSERT(sampler.output->type == cgltf_type_vec3);
                        break;
                    case cgltf_animation_path_type_rotation:
                        FB_ASSERT(sampler.output->type == cgltf_type_vec4);
                        break;
                    case cgltf_animation_path_type_scale:
                        FB_ASSERT(sampler.output->type == cgltf_type_vec3);
                        break;
                }
            }
            FB_ASSERT(std::ranges::all_of(
                std::span(animation.samplers, animation.samplers_count),
                [&](const auto& sampler) {
                    return animation.samplers[0].input->max[0] == sampler.input->max[0];
                }));
        }

        // Read skinning vertex data.
        auto skinning_vertices = std::vector<GltfSkinningVertex>(vertex_count);
        for (size_t i = 0; i < vertex_count; i++) {
            auto* joints = (cgltf_uint*)&skinning_vertices[i].joints;
            auto* weights = (cgltf_float*)&skinning_vertices[i].weights;
            FB_ASSERT(cgltf_accessor_read_uint(joints_accessor, i, joints, 4));
            FB_ASSERT(cgltf_accessor_read_float(weights_accessor, i, weights, 4));
            skinning_vertices[i].position = _vertices[i].position;
            skinning_vertices[i].normal = _vertices[i].normal;
            skinning_vertices[i].texcoord = _vertices[i].texcoord;
        }

        // Read additional node data.
        auto node_transforms = std::vector<Matrix>(data->nodes_count);
        auto node_parents = std::vector<uint32_t>(data->nodes_count, GLTF_NULL_NODE);
        for (size_t i = 0; i < data->nodes_count; i++) {
            const auto& node = data->nodes[i];
            cgltf_node_transform_local(&node, (float*)&node_transforms[i]);
            if (node.parent != nullptr) {
                node_parents[i] = (uint32_t)(node.parent - data->nodes);
            }
        }

        // Read joint data.
        const auto& skin = data->skins[0];
        const auto* ibms = skin.inverse_bind_matrices;
        auto joint_inverse_binds = std::vector<Matrix>(skin.joints_count);
        auto joint_nodes = std::vector<uint32_t>(skin.joints_count);
        for (size_t i = 0; i < skin.joints_count; i++) {
            FB_ASSERT(cgltf_accessor_read_float(ibms, i, (float*)&joint_inverse_binds[i], 16));
            joint_nodes[i] = (uint32_t)(skin.joints[i] - data->nodes);
        }

        // Read animation data.
        const auto& animation = data->animations[0];
        auto node_channels = std::vector<GltfChannelHeader>(data->nodes_count);
        auto total_t_count = size_t(0);
        auto total_r_count = size_t(0);
        auto total_s_count = size_t(0);
        for (size_t i = 0; i < animation.channels_count; i++) {
            const auto& channel = animation.channels[i];
            const auto& sampler = animation.samplers[i];
            const auto& output = *sampler.output;
            auto node_index = channel.target_node - data->nodes;
            switch (channel.target_path) {
                case cgltf_animation_path_type_translation:
                    node_channels[node_index].t_count = output.count;
                    total_t_count += output.count;
                    break;
                case cgltf_animation_path_type_rotation:
                    node_channels[node_index].r_count = output.count;
                    total_r_count += output.count;
                    break;
                case cgltf_animation_path_type_scale:
                    node_channels[node_index].s_count = output.count;
                    total_s_count += output.count;
                    break;
            }
        }
        for (size_t i = 1; i < node_channels.size(); i++) {
            const auto& prev = node_channels[i - 1];
            auto& curr = node_channels[i];
            curr.t_offset = prev.t_offset + prev.t_count;
            curr.r_offset = prev.r_offset + prev.r_count;
            curr.s_offset = prev.s_offset + prev.s_count;
        }

        auto node_channel_times_t = std::vector<float>(total_t_count);
        auto node_channel_times_r = std::vector<float>(total_r_count);
        auto node_channel_times_s = std::vector<float>(total_s_count);
        auto node_channel_values_t = std::vector<Vector3>(total_t_count);
        auto node_channel_values_r = std::vector<Quaternion>(total_r_count);
        auto node_channel_values_s = std::vector<Vector3>(total_s_count);
        for (size_t i = 0; i < animation.samplers_count; i++) {
            const auto& sampler = animation.samplers[i];
            const auto& input = *sampler.input;
            const auto& output = *sampler.output;
            const auto& channel = animation.channels[i];
            const auto node_index = channel.target_node - data->nodes;
            const auto& node_channel = node_channels[node_index];

            switch (channel.target_path) {
                case cgltf_animation_path_type_translation: {
                    auto* times = &node_channel_times_t[node_channel.t_offset];
                    auto* values = &node_channel_values_t[node_channel.t_offset];
                    for (size_t j = 0; j < output.count; j++) {
                        FB_ASSERT(cgltf_accessor_read_float(&input, j, &times[j], 1));
                        FB_ASSERT(cgltf_accessor_read_float(&output, j, (float*)&values[j], 3));
                    }
                    break;
                }
                case cgltf_animation_path_type_rotation: {
                    auto* times = &node_channel_times_r[node_channel.r_offset];
                    auto* values = &node_channel_values_r[node_channel.r_offset];
                    for (size_t j = 0; j < output.count; j++) {
                        FB_ASSERT(cgltf_accessor_read_float(&input, j, &times[j], 1));
                        FB_ASSERT(cgltf_accessor_read_float(&output, j, (float*)&values[j], 4));
                    }
                    break;
                }
                case cgltf_animation_path_type_scale: {
                    auto* times = &node_channel_times_s[node_channel.s_offset];
                    auto* values = &node_channel_values_s[node_channel.s_offset];
                    for (size_t j = 0; j < output.count; j++) {
                        FB_ASSERT(cgltf_accessor_read_float(&input, j, &times[j], 1));
                        FB_ASSERT(cgltf_accessor_read_float(&output, j, (float*)&values[j], 3));
                    }
                    break;
                }
            }
        }

        // Animation duration.
        const auto animation_duration = animation.samplers[0].input->max[0];

        // Save.
        std::swap(_skinning_vertices, skinning_vertices);
        std::swap(_node_transforms, node_transforms);
        std::swap(_node_parents, node_parents);
        std::swap(_joint_inverse_binds, joint_inverse_binds);
        std::swap(_joint_nodes, joint_nodes);
        std::swap(_node_channels, node_channels);
        std::swap(_node_channel_times_t, node_channel_times_t);
        std::swap(_node_channel_times_r, node_channel_times_r);
        std::swap(_node_channel_times_s, node_channel_times_s);
        std::swap(_node_channel_values_t, node_channel_values_t);
        std::swap(_node_channel_values_r, node_channel_values_r);
        std::swap(_node_channel_values_s, node_channel_values_s);
        _animation_duration = animation_duration;
    }

    // Cleanup.
    cgltf_free(data);
}

}  // namespace fb
