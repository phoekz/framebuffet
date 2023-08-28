// This implementation is adapted from https://github.com/boksajak/brdf.
//
// The implementation remains mostly identical to the original, except for
// naming conventions and some variants are removed. All comments are replaced
// with my own.

#pragma once

#include <graphics/core.hlsli>

#define MIN_DIELECTRICS_F0 0.04f

struct MaterialProperties {
    float3 base_color;
    float metalness;
    float roughness;
};

struct BrdfData {
    float3 specular_f0;
    float3 diffuse_reflectance;
    float roughness;
    float alpha;
    float alpha_squared;
    float3 f;
    float3 v;
    float3 n;
    float3 h;
    float3 l;
    float ndotl;
    float ndotv;
    float ldoth;
    float ndoth;
    float vdoth;
    bool v_backfacing;
    bool l_backfacing;
};

float luminance(float3 rgb) {
    return dot(rgb, float3(0.2126f, 0.7152f, 0.0722f));
}

float3 specular_f0_from_base_color(float3 base_color, float metalness) {
    float min_dielectrics_f0 = MIN_DIELECTRICS_F0;
    return lerp(min_dielectrics_f0.xxx, base_color, metalness);
}

float3 diffuse_reflectance_from_base_color(float3 base_color, float metalness) {
    return base_color * (1.0f - metalness);
}

float3 evaluate_fresnel(float3 f0, float f90, float ndots) {
    return f0 + (f90 - f0) * pow(1.0f - ndots, 5.0f);
}

float shadowed_f90(float3 f0) {
    float t = (1.0f / MIN_DIELECTRICS_F0);
    return min(1.0f, t * luminance(f0));
}

float disney_diffuse(BrdfData data) {
    float fd90_minus_one = 2.0f * data.roughness * data.ldoth * data.ldoth - 0.5f;
    float fdl = 1.0f + (fd90_minus_one * pow(1.0f - data.ndotl, 5.0f));
    float fdv = 1.0f + (fd90_minus_one * pow(1.0f - data.ndotv, 5.0f));
    return fdl * fdv;
}

float3 evaluate_disney_diffuse(BrdfData data) {
    return data.diffuse_reflectance * (disney_diffuse(data) * FB_ONE_OVER_PI * data.ndotl);
}

float smith_g2_height_correlated_ggx_lagarde(float alpha_squared, float ndotl, float ndotv) {
    float a = ndotv * sqrt(alpha_squared + ndotl * (ndotl - alpha_squared * ndotl));
    float b = ndotl * sqrt(alpha_squared + ndotv * (ndotv - alpha_squared * ndotv));
    return 0.5f / (a + b);
}

float smith_g2(float alpha, float alpha_squared, float ndotl, float ndotv) {
    return smith_g2_height_correlated_ggx_lagarde(alpha_squared, ndotl, ndotv);
}

float ggx_d(float alpha_squared, float ndoth) {
    float b = ((alpha_squared - 1.0f) * ndoth * ndoth + 1.0f);
    return alpha_squared / (FB_PI * b * b);
}

float3 evaluate_microfacet(BrdfData data) {
    float d = ggx_d(max(0.00001f, data.alpha_squared), data.ndoth);
    float g2 = smith_g2(data.alpha, data.alpha_squared, data.ndotl, data.ndotv);
    return data.f * (g2 * d * data.ndotl);
}

BrdfData prepare_brdf_data(float3 n, float3 l, float3 v, MaterialProperties material) {
    BrdfData data;
    data.v = v;
    data.n = n;
    data.h = normalize(l + v);
    data.l = l;
    float ndotl = dot(n, l);
    float ndotv = dot(n, v);
    data.v_backfacing = (ndotv <= 0.0f);
    data.l_backfacing = (ndotl <= 0.0f);
    data.ndotl = clamp(ndotl, 0.00001f, 1.0f);
    data.ndotv = clamp(ndotv, 0.00001f, 1.0f);
    data.ldoth = saturate(dot(l, data.h));
    data.ndoth = saturate(dot(n, data.h));
    data.vdoth = saturate(dot(v, data.h));
    data.specular_f0 = specular_f0_from_base_color(material.base_color, material.metalness);
    data.diffuse_reflectance =
        diffuse_reflectance_from_base_color(material.base_color, material.metalness);
    data.roughness = material.roughness;
    data.alpha = material.roughness * material.roughness;
    data.alpha_squared = data.alpha * data.alpha;
    data.f = evaluate_fresnel(data.specular_f0, shadowed_f90(data.specular_f0), data.ldoth);
    return data;
}

float3 evaluate_combined_brdf(float3 n, float3 l, float3 v, MaterialProperties material) {
    BrdfData data = prepare_brdf_data(n, l, v, material);
    if (data.v_backfacing || data.l_backfacing) {
        return float3(0.0f, 0.0f, 0.0f);
    }
    float3 specular = evaluate_microfacet(data);
    float3 diffuse = evaluate_disney_diffuse(data);
    return (float3(1.0f, 1.0f, 1.0f) - data.f) * diffuse + specular;
}

float4 get_rotation_to_z_axis(float3 input) {
    if (input.z < -0.99999f) {
        return float4(1.0f, 0.0f, 0.0f, 0.0f);
    }
    return normalize(float4(input.y, -input.x, 0.0f, 1.0f + input.z));
}

float4 invert_rotation(float4 q) {
    return float4(-q.x, -q.y, -q.z, q.w);
}

float3 rotate_point(float4 q, float3 v) {
    float3 q_axis = float3(q.x, q.y, q.z);
    return 2.0f * dot(q_axis, v) * q_axis + (q.w * q.w - dot(q_axis, q_axis)) * v
        + 2.0f * q.w * cross(q_axis, v);
}

float3 sample_hemisphere(float2 u, out float pdf) {
    float a = sqrt(u.x);
    float b = FB_TWO_PI * u.y;
    float3 result = float3(a * cos(b), a * sin(b), sqrt(1.0f - u.x));
    pdf = result.z * FB_ONE_OVER_PI;
    return result;
}

float3 sample_hemisphere(float2 u) {
    float pdf;
    return sample_hemisphere(u, pdf);
}

float3 sample_ggx_vndf(float3 wi, float2 alpha_2d, float2 u) {
    float3 wi_std = normalize(float3(wi.xy * alpha_2d, wi.z));
    float phi = FB_TWO_PI * u.x;
    float z = ((1.0f - u.y) * (1.0f + wi.z)) - wi.z;
    float sin_theta = sqrt(clamp(1.0f - z * z, 0.0f, 1.0f));
    float x = sin_theta * cos(phi);
    float y = sin_theta * sin(phi);
    float3 c = float3(x, y, z);
    float3 wm_std = c + wi;
    float3 wm = normalize(float3(wm_std.xy * alpha_2d, wm_std.z));
    return wm;
}

float smith_g1_ggx(float alpha, float ndots, float alpha_squared) {
    float ndots_squared = ndots * ndots;
    return 2.0f
        / (sqrt(((alpha_squared * (1.0f - ndots_squared)) + ndots_squared) / ndots_squared) + 1.0f);
}

float smith_g2_over_g1_height_correlated(
    float alpha,
    float alpha_squared,
    float ndotl,
    float ndotv
) {
    float g1v = smith_g1_ggx(alpha, ndotv, alpha_squared);
    float g1l = smith_g1_ggx(alpha, ndotl, alpha_squared);
    return g1l / (g1v + g1l - g1v * g1l);
}

float specular_sample_weight_ggx_vndf(
    float alpha,
    float alpha_squared,
    float ndotl,
    float ndotv,
    float hdotl,
    float ndoth
) {
    return smith_g2_over_g1_height_correlated(alpha, alpha_squared, ndotl, ndotv);
}

float3 sample_specular_microfacet(
    float3 local_v,
    float alpha,
    float alpha_squared,
    float3 specular_f0,
    float2 u,
    out float3 weight
) {
    float3 local_h;
    if (alpha == 0.0f) {
        local_h = float3(0.0f, 0.0f, 1.0f);
    } else {
        local_h = sample_ggx_vndf(local_v, float2(alpha, alpha), u);
    }
    float3 local_l = reflect(-local_v, local_h);
    float3 local_n = float3(0.0f, 0.0f, 1.0f);
    float hdotl = clamp(dot(local_h, local_l), 0.00001f, 1.0f);
    float ndotl = clamp(dot(local_n, local_l), 0.00001f, 1.0f);
    float ndotv = clamp(dot(local_n, local_v), 0.00001f, 1.0f);
    float ndoth = clamp(dot(local_n, local_h), 0.00001f, 1.0f);
    float3 f = evaluate_fresnel(specular_f0, shadowed_f90(specular_f0), hdotl);
    weight = f * specular_sample_weight_ggx_vndf(alpha, alpha_squared, ndotl, ndotv, hdotl, ndoth);
    return local_l;
}

enum class IndirectBrdfType {
    Diffuse,
    Specular,
};

bool evaluate_indirect_combined_brdf(
    float2 u,
    float3 shading_normal,
    float3 geometry_normal,
    float3 v,
    MaterialProperties material,
    IndirectBrdfType brdf_type,
    out float3 ray_dir,
    out float3 weight
) {
    if (dot(shading_normal, v) <= 0.0f) {
        return false;
    }

    float4 q_rotation_to_z = get_rotation_to_z_axis(shading_normal);
    float3 local_v = rotate_point(q_rotation_to_z, v);
    float3 local_n = float3(0.0f, 0.0f, 1.0f);
    float3 local_ray_dir = float3(0.0f, 0.0f, 0.0f);

    switch (brdf_type) {
        case IndirectBrdfType::Diffuse: {
            local_ray_dir = sample_hemisphere(u);
            BrdfData data = prepare_brdf_data(local_n, local_ray_dir, local_v, material);
            weight = data.diffuse_reflectance * disney_diffuse(data);
            float3 h_specular = sample_ggx_vndf(local_v, float2(data.alpha, data.alpha), u);
            float vdoth = clamp(dot(local_v, h_specular), 0.00001f, 1.0f);
            float3 f = evaluate_fresnel(data.specular_f0, shadowed_f90(data.specular_f0), vdoth);
            weight *= (float3(1.0f, 1.0f, 1.0f) - f);
            break;
        }
        case IndirectBrdfType::Specular: {
            BrdfData data = prepare_brdf_data(local_n, float3(0.0f, 0.0f, 1.0f), local_v, material);
            local_ray_dir = sample_specular_microfacet(
                local_v,
                data.alpha,
                data.alpha_squared,
                data.specular_f0,
                u,
                weight
            );
            break;
        }
    }

    if (luminance(weight) == 0.0f) {
        return false;
    }
    ray_dir = normalize(rotate_point(invert_rotation(q_rotation_to_z), local_ray_dir));
    if (dot(geometry_normal, ray_dir) <= 0.0f) {
        return false;
    }
    return true;
}
