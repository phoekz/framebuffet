#include "gltf.hpp"

#include <cgltf.h>

namespace fb {

GltfModel::GltfModel(std::string_view gltf_path) {
    // Load GLTF.
    cgltf_options options = {};
    cgltf_data* data = nullptr;
    FB_ASSERT(cgltf_parse_file(&options, gltf_path.data(), &data) == cgltf_result_success);
    FB_ASSERT(cgltf_load_buffers(&options, data, gltf_path.data()) == cgltf_result_success);
    FB_ASSERT(cgltf_validate(data) == cgltf_result_success);

    // Node remapping.
    auto gltf_from_fb = std::vector<uint>();
    auto fb_from_gltf = std::vector<uint>();
    {
        auto visited = std::vector<bool>(data->nodes_count, false);
        auto stack = std::vector<uint>();
        for (uint root_index = 0; root_index < data->nodes_count; root_index++) {
            const auto* root_node = &data->nodes[root_index];
            if (root_node->parent) {
                continue;
            }

            FB_ASSERT(!visited[root_index]);
            FB_ASSERT(stack.empty());

            stack.push_back(root_index);
            while (!stack.empty()) {
                const auto node_index = stack.back();
                stack.pop_back();

                if (!visited[node_index]) {
                    visited[node_index] = true;
                    gltf_from_fb.push_back(node_index);
                }

                const auto* gltf_node = &data->nodes[node_index];
                for (cgltf_size i = 0; i < gltf_node->children_count; i++) {
                    const auto* gltf_child = gltf_node->children[i];
                    stack.push_back((uint)cgltf_node_index(data, gltf_child));
                }
            }
        }
        FB_ASSERT(gltf_from_fb.size() == data->nodes_count);
        for (const auto& visited_node : visited) {
            FB_ASSERT(visited_node);
        }

        fb_from_gltf.resize(data->nodes_count);
        for (size_t i = 0; i < gltf_from_fb.size(); i++) {
            fb_from_gltf[gltf_from_fb[i]] = (uint)i;
        }
    }

    // Ensure parents are initialized before the children when iterating in order.
    {
        auto visited = std::vector<bool>(data->nodes_count, false);
        for (size_t fb_i = 0; fb_i < data->nodes_count; fb_i++) {
            const auto gltf_i = gltf_from_fb[fb_i];
            const auto* node = &data->nodes[gltf_i];
            const auto curr = cgltf_node_index(data, node);
            if (node->parent) {
                const auto parent = cgltf_node_index(data, node->parent);
                FB_ASSERT(visited[parent]);
            }
            visited[curr] = true;
        }
        for (const auto& visited_node : visited) {
            FB_ASSERT(visited_node);
        }
    }

    // Root transform.
    auto maybe_root_transform = std::optional<float4x4>(std::nullopt);
    for (size_t fb_i = 0; fb_i < data->nodes_count; fb_i++) {
        const auto gltf_i = gltf_from_fb[fb_i];
        const auto& node = data->nodes[gltf_i];
        if (node.parent == nullptr) {
            float4x4 root_transform;
            cgltf_node_transform_world(&node, (cgltf_float*)&root_transform);
            maybe_root_transform = root_transform;
        }
    }
    FB_ASSERT(maybe_root_transform.has_value());
    _root_transform = maybe_root_transform.value();

    // Read and merge meshes.
    for (const auto& mesh : Span(data->meshes, data->meshes_count)) {
        FB_ASSERT(mesh.primitives_count == 1);
        const auto& primitive = mesh.primitives[0];
        FB_ASSERT(primitive.type == cgltf_primitive_type_triangles);
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
        FB_ASSERT(position_accessor->component_type == cgltf_component_type_r_32f);
        FB_ASSERT(normal_accessor->type == cgltf_type_vec3);
        FB_ASSERT(normal_accessor->component_type == cgltf_component_type_r_32f);
        FB_ASSERT(texcoord_accessor->type == cgltf_type_vec2);
        FB_ASSERT(texcoord_accessor->component_type == cgltf_component_type_r_32f);
        FB_ASSERT(index_accessor->type == cgltf_type_scalar);
        FB_ASSERT(
            index_accessor->component_type == cgltf_component_type_r_8u
            || index_accessor->component_type == cgltf_component_type_r_16u
            || index_accessor->component_type == cgltf_component_type_r_32u
        );
        const auto vertex_count = position_accessor->count;
        FB_ASSERT(vertex_count > 0);
        FB_ASSERT(normal_accessor->count == vertex_count);
        FB_ASSERT(texcoord_accessor->count == vertex_count);
        const auto index_count = index_accessor->count;
        FB_ASSERT(index_count > 0);
        FB_ASSERT(index_count % 3 == 0);

        const auto vertex_offset = _vertex_positions.size();
        const auto index_offset = _indices.size();
        _vertex_positions.resize(vertex_offset + vertex_count);
        _vertex_normals.resize(vertex_offset + vertex_count);
        _vertex_texcoords.resize(vertex_offset + vertex_count);
        _indices.resize(index_offset + index_count);
        auto vertex_positions = Span(_vertex_positions).subspan(vertex_offset, vertex_count);
        auto vertex_normals = Span(_vertex_normals).subspan(vertex_offset, vertex_count);
        auto vertex_texcoords = Span(_vertex_texcoords).subspan(vertex_offset, vertex_count);
        auto indices = Span(_indices).subspan(index_offset, index_count);
        for (size_t i = 0; i < vertex_count; i++) {
            auto& vp = vertex_positions[i];
            auto& vn = vertex_normals[i];
            auto& vt = vertex_texcoords[i];
            FB_ASSERT(cgltf_accessor_read_float(position_accessor, i, (float*)&vp, 3));
            FB_ASSERT(cgltf_accessor_read_float(normal_accessor, i, (float*)&vn, 3));
            FB_ASSERT(cgltf_accessor_read_float(texcoord_accessor, i, (float*)&vt, 2));
        }
        for (size_t i = 0; i < index_count; i += 3) {
            auto& i0 = indices[i + 0];
            auto& i1 = indices[i + 1];
            auto& i2 = indices[i + 2];
            FB_ASSERT(cgltf_accessor_read_uint(index_accessor, i + 0, &i0, 1));
            FB_ASSERT(cgltf_accessor_read_uint(index_accessor, i + 1, &i1, 1));
            FB_ASSERT(cgltf_accessor_read_uint(index_accessor, i + 2, &i2, 1));
            i0 += (uint)vertex_offset;
            i1 += (uint)vertex_offset;
            i2 += (uint)vertex_offset;
        }

        _submeshes.push_back({(uint)index_count, (uint)index_offset});
    }

    // Read material.
    FB_ASSERT(data->materials_count >= 1); // Todo: support multiple materials.
    const auto& material = data->materials[0];
    FB_ASSERT(material.has_pbr_metallic_roughness);
    const auto& pbr = material.pbr_metallic_roughness;
    _metallic_factor = pbr.metallic_factor;
    _roughness_factor = pbr.roughness_factor;
    if (pbr.base_color_texture.texture != nullptr) {
        FB_ASSERT(pbr.base_color_texture.has_transform == false);
        FB_ASSERT(pbr.base_color_texture.texture->image != nullptr);
        const auto& image = *pbr.base_color_texture.texture->image;
        const auto image_view = image.buffer_view;
        const auto image_data = (const std::byte*)cgltf_buffer_view_data(image_view);
        const auto image_span = Span(image_data, image_view->size);
        _base_color_texture = LdrImage::from_image(image_span);
    } else {
        FB_ASSERT(pbr.base_color_factor[0] >= 0.0f && pbr.base_color_factor[0] <= 1.0f);
        FB_ASSERT(pbr.base_color_factor[1] >= 0.0f && pbr.base_color_factor[1] <= 1.0f);
        FB_ASSERT(pbr.base_color_factor[2] >= 0.0f && pbr.base_color_factor[2] <= 1.0f);
        FB_ASSERT(pbr.base_color_factor[3] >= 0.0f && pbr.base_color_factor[3] <= 1.0f);
        auto color = std::array<std::byte, 4> {
            (std::byte)(pbr.base_color_factor[0] * 255),
            (std::byte)(pbr.base_color_factor[1] * 255),
            (std::byte)(pbr.base_color_factor[2] * 255),
            (std::byte)(pbr.base_color_factor[3] * 255),
        };
        _base_color_texture = LdrImage::from_constant(1, 1, color);
    }
    if (material.normal_texture.texture != nullptr) {
        FB_ASSERT(material.normal_texture.has_transform == false);
        FB_ASSERT(material.normal_texture.texture->image != nullptr);
        const auto& image = *material.normal_texture.texture->image;
        const auto image_view = image.buffer_view;
        const auto image_data = (const std::byte*)cgltf_buffer_view_data(image_view);
        const auto image_span = Span(image_data, image_view->size);
        _normal_texture = LdrImage::from_image(image_span);
    }
    if (pbr.metallic_roughness_texture.texture != nullptr) {
        FB_ASSERT(pbr.metallic_roughness_texture.has_transform == false);
        FB_ASSERT(pbr.metallic_roughness_texture.texture->image != nullptr);
        const auto& image = *pbr.metallic_roughness_texture.texture->image;
        const auto image_view = image.buffer_view;
        const auto image_data = (const std::byte*)cgltf_buffer_view_data(image_view);
        const auto image_span = Span(image_data, image_view->size);
        const auto map_fn =
            [](uint, uint, std::byte& r, std::byte& /*g*/, std::byte& /*b*/, std::byte& a) {
                // Note: GLTF's metallic is defined in the blue channel,
                // roughness in the green channel. Since GLTF allows different
                // channels to overlap, for example occlusion might be in the
                // red channel, we have to mask out the other channels.
                r = (std::byte)0;
                a = (std::byte)255;
            };
        _metallic_roughness_texture = LdrImage::from_image(image_span).map(map_fn);
    }
    switch (material.alpha_mode) {
        case cgltf_alpha_mode_opaque: _alpha_mode = GltfAlphaMode::Opaque; break;
        case cgltf_alpha_mode_mask: _alpha_mode = GltfAlphaMode::Mask; break;
        case cgltf_alpha_mode_blend: FB_FATAL();
        default: FB_FATAL();
    }
    _alpha_cutoff = material.alpha_cutoff;

    if (data->skins_count > 0) {
        // Validate animations and skins.
        {
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
                    || channel.target_path == cgltf_animation_path_type_scale
                );
                FB_ASSERT(
                    sampler.interpolation == cgltf_interpolation_type_linear
                    || sampler.interpolation == cgltf_interpolation_type_step
                );
                FB_ASSERT(sampler.input->component_type == cgltf_component_type_r_32f);
                FB_ASSERT(sampler.input->type == cgltf_type_scalar);
                FB_ASSERT(sampler.input->count > 0);
                FB_ASSERT(sampler.input->has_min);
                FB_ASSERT(sampler.input->has_max);
                FB_ASSERT(sampler.input->max[0] > sampler.input->min[0]);
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
                    default: break;
                }
            }
            FB_ASSERT(std::ranges::all_of(
                Span(animation.samplers, animation.samplers_count),
                [&](const auto& sampler) {
                    return animation.samplers[0].input->max[0] == sampler.input->max[0];
                }
            ));
        }

        // Animation vertex data.
        for (const auto& mesh : Span(data->meshes, data->meshes_count)) {
            // Primitive.
            FB_ASSERT(mesh.primitives_count == 1);
            const auto& primitive = mesh.primitives[0];
            FB_ASSERT(primitive.type == cgltf_primitive_type_triangles);
            const auto vertex_count = primitive.attributes[0].data->count;

            // Find accessors.
            const cgltf_accessor* joints_accessor = nullptr;
            const cgltf_accessor* weights_accessor = nullptr;
            for (size_t i = 0; i < primitive.attributes_count; i++) {
                const auto& attribute = primitive.attributes[i];
                if (attribute.type == cgltf_attribute_type_joints) {
                    joints_accessor = attribute.data;
                    FB_ASSERT(joints_accessor != nullptr);
                    FB_ASSERT(
                        joints_accessor->component_type == cgltf_component_type_r_8u
                        || joints_accessor->component_type == cgltf_component_type_r_16u
                        || joints_accessor->component_type == cgltf_component_type_r_32u
                    );
                    FB_ASSERT(joints_accessor->type == cgltf_type_vec4);
                    FB_ASSERT(joints_accessor->count == vertex_count);
                } else if (attribute.type == cgltf_attribute_type_weights) {
                    weights_accessor = attribute.data;
                    FB_ASSERT(weights_accessor != nullptr);
                    FB_ASSERT(weights_accessor->component_type == cgltf_component_type_r_32f);
                    FB_ASSERT(weights_accessor->type == cgltf_type_vec4);
                    FB_ASSERT(weights_accessor->count == vertex_count);
                }
            }

            // Extend buffers.
            const auto vertex_offset = _vertex_joints.size();
            _vertex_joints.resize(vertex_offset + vertex_count);
            _vertex_weights.resize(vertex_offset + vertex_count);
            const auto vertex_joints = Span(_vertex_joints).subspan(vertex_offset, vertex_count);
            const auto vertex_weights = Span(_vertex_weights).subspan(vertex_offset, vertex_count);
            for (size_t i = 0; i < vertex_count; i++) {
                auto& vj = vertex_joints[i];
                auto& vw = vertex_weights[i];
                FB_ASSERT(cgltf_accessor_read_uint(joints_accessor, i, (uint*)&vj, 4));
                FB_ASSERT(cgltf_accessor_read_float(weights_accessor, i, (float*)&vw, 4));
            }
        }

        // Node hierarchy.
        auto node_parents = std::vector<uint>(data->nodes_count, GLTF_NULL_NODE);
        for (size_t fb_i = 0; fb_i < data->nodes_count; fb_i++) {
            const auto gltf_i = gltf_from_fb[fb_i];
            const auto& node = data->nodes[gltf_i];
            if (node.parent != nullptr) {
                node_parents[fb_i] = fb_from_gltf[cgltf_node_index(data, node.parent)];
            }
        }

        // Read joint data.
        const auto& skin = data->skins[0];
        const auto* ibms = skin.inverse_bind_matrices;
        auto joint_inverse_binds = std::vector<float4x4>(skin.joints_count);
        auto joint_nodes = std::vector<uint>(skin.joints_count);
        for (size_t i = 0; i < skin.joints_count; i++) {
            FB_ASSERT(cgltf_accessor_read_float(ibms, i, (float*)&joint_inverse_binds[i], 16));
            joint_nodes[i] = fb_from_gltf[cgltf_node_index(data, skin.joints[i])];
        }

        // Read animation data.
        const auto& animation = data->animations[0];
        auto node_channels = std::vector<GltfChannelHeader>(data->nodes_count);
        auto total_t_count = size_t(0);
        auto total_r_count = size_t(0);
        auto total_s_count = size_t(0);
        for (size_t gltf_i = 0; gltf_i < animation.channels_count; gltf_i++) {
            const auto& channel = animation.channels[gltf_i];
            const auto& sampler = animation.samplers[gltf_i];
            const auto& output = *sampler.output;
            const auto fb_i = fb_from_gltf[cgltf_node_index(data, channel.target_node)];
            switch (channel.target_path) {
                case cgltf_animation_path_type_translation:
                    node_channels[fb_i].t_count = output.count;
                    total_t_count += output.count;
                    break;
                case cgltf_animation_path_type_rotation:
                    node_channels[fb_i].r_count = output.count;
                    total_r_count += output.count;
                    break;
                case cgltf_animation_path_type_scale:
                    node_channels[fb_i].s_count = output.count;
                    total_s_count += output.count;
                    break;
                default: break;
            }
        }
        for (size_t fb_i = 1; fb_i < node_channels.size(); fb_i++) {
            const auto& prev = node_channels[fb_i - 1];
            auto& curr = node_channels[fb_i];
            curr.t_offset = prev.t_offset + prev.t_count;
            curr.r_offset = prev.r_offset + prev.r_count;
            curr.s_offset = prev.s_offset + prev.s_count;
        }

        auto node_channels_times_t = std::vector<float>(total_t_count);
        auto node_channels_times_r = std::vector<float>(total_r_count);
        auto node_channels_times_s = std::vector<float>(total_s_count);
        auto node_channels_values_t = std::vector<float3>(total_t_count);
        auto node_channels_values_r = std::vector<float_quat>(total_r_count);
        auto node_channels_values_s = std::vector<float3>(total_s_count);
        for (size_t gltf_i = 0; gltf_i < animation.samplers_count; gltf_i++) {
            const auto& sampler = animation.samplers[gltf_i];
            const auto& input = *sampler.input;
            const auto& output = *sampler.output;
            const auto& channel = animation.channels[gltf_i];
            const auto fb_i = fb_from_gltf[cgltf_node_index(data, channel.target_node)];
            const auto& node_channel = node_channels[fb_i];

            switch (channel.target_path) {
                case cgltf_animation_path_type_translation: {
                    auto* times = &node_channels_times_t[node_channel.t_offset];
                    auto* values = &node_channels_values_t[node_channel.t_offset];
                    for (size_t i = 0; i < output.count; i++) {
                        FB_ASSERT(cgltf_accessor_read_float(&input, i, &times[i], 1));
                        FB_ASSERT(cgltf_accessor_read_float(&output, i, (float*)&values[i], 3));
                    }
                    break;
                }
                case cgltf_animation_path_type_rotation: {
                    auto* times = &node_channels_times_r[node_channel.r_offset];
                    auto* values = &node_channels_values_r[node_channel.r_offset];
                    for (size_t i = 0; i < output.count; i++) {
                        FB_ASSERT(cgltf_accessor_read_float(&input, i, &times[i], 1));
                        FB_ASSERT(cgltf_accessor_read_float(&output, i, (float*)&values[i], 4));
                    }
                    break;
                }
                case cgltf_animation_path_type_scale: {
                    auto* times = &node_channels_times_s[node_channel.s_offset];
                    auto* values = &node_channels_values_s[node_channel.s_offset];
                    for (size_t i = 0; i < output.count; i++) {
                        FB_ASSERT(cgltf_accessor_read_float(&input, i, &times[i], 1));
                        FB_ASSERT(cgltf_accessor_read_float(&output, i, (float*)&values[i], 3));
                    }
                    break;
                }
                default: break;
            }
        }

        // Time.
        auto animation_duration = 0.0f;
        {
            auto times_t_min = FLT_MAX;
            auto times_t_max = -FLT_MAX;
            auto times_r_min = FLT_MAX;
            auto times_r_max = -FLT_MAX;
            auto times_s_min = FLT_MAX;
            auto times_s_max = -FLT_MAX;
            for (auto t : node_channels_times_t) {
                times_t_min = std::min(times_t_min, t);
                times_t_max = std::max(times_t_max, t);
            }
            for (auto r : node_channels_times_r) {
                times_r_min = std::min(times_r_min, r);
                times_r_max = std::max(times_r_max, r);
            }
            for (auto s : node_channels_times_s) {
                times_s_min = std::min(times_s_min, s);
                times_s_max = std::max(times_s_max, s);
            }
            FB_ASSERT(times_t_min >= 0.0f && times_t_max > times_t_min);
            FB_ASSERT(times_r_min >= 0.0f && times_r_max > times_r_min);
            FB_ASSERT(times_s_min >= 0.0f && times_s_max > times_s_min);
            FB_ASSERT(times_t_min == times_r_min);
            FB_ASSERT(times_t_min == times_s_min);
            FB_ASSERT(times_t_max == times_r_max);
            FB_ASSERT(times_t_max == times_s_max);
            for (auto& t : node_channels_times_t) {
                t -= times_t_min;
            }
            for (auto& r : node_channels_times_r) {
                r -= times_r_min;
            }
            for (auto& s : node_channels_times_s) {
                s -= times_s_min;
            }
            animation_duration = times_t_max - times_t_min;
        }

        // Save.
        std::swap(_joint_nodes, joint_nodes);
        std::swap(_joint_inverse_binds, joint_inverse_binds);
        std::swap(_node_parents, node_parents);
        std::swap(_node_channels, node_channels);
        std::swap(_node_channels_times_t, node_channels_times_t);
        std::swap(_node_channels_times_r, node_channels_times_r);
        std::swap(_node_channels_times_s, node_channels_times_s);
        std::swap(_node_channels_values_t, node_channels_values_t);
        std::swap(_node_channels_values_r, node_channels_values_r);
        std::swap(_node_channels_values_s, node_channels_values_s);
        _animation_duration = animation_duration;
    }

    // Validate.
    FB_ASSERT(_vertex_positions.size() == _vertex_normals.size());
    FB_ASSERT(_vertex_positions.size() == _vertex_texcoords.size());
    if (_vertex_joints.size() > 0) {
        FB_ASSERT(_vertex_positions.size() == _vertex_joints.size());
        FB_ASSERT(_vertex_positions.size() == _vertex_weights.size());
    }

    // Cleanup.
    cgltf_free(data);
}

} // namespace fb
