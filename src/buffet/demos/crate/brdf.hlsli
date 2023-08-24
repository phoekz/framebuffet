// This implementation is adapted from https://github.com/boksajak/brdf.
//
// The implementation remains identical to the original, except for the naming
// conventions and contains direct lighting only, for now.

#pragma once

#include <demos/core.hlsli>

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

float shadowed_f90(float3 F0) {
    float t = (1.0f / MIN_DIELECTRICS_F0);
    return min(1.0f, t * luminance(F0));
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
    data.ndotl = min(max(0.00001f, ndotl), 1.0f);
    data.ndotv = min(max(0.00001f, ndotv), 1.0f);
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
