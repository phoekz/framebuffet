#include <stockcube/techniques/lut/lut.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/graphics/core.hlsli>
#include <kitchen/graphics/brdf.hlsli>

ConstantBuffer<Bindings> g_bindings: register(b0);

FB_ATTRIBUTE(numthreads, DISPATCH_X, DISPATCH_Y, DISPATCH_Z)
void cs(fb::ComputeInput input) {
    // Global resources.
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    RWTexture2D<float2> lut_texture = ResourceDescriptorHeap[g_bindings.lut_texture];
    const uint2 lut_texture_size = constants.lut_texture_size;
    const uint lut_sample_count = constants.lut_sample_count;

    // Indices.
    const uint2 dst_id = input.dispatch_thread_id.xy;

    // Texcoords.
    const float2 texture_size = (float2)lut_texture_size;
    const float2 texcoord = (float2(dst_id) + 0.5f) / texture_size;
    const float ndotv = texcoord.x;
    const float roughness = texcoord.y;

    // Compute LUT: scale and bias for F0.
    const float3 specular_f0 = MIN_DIELECTRICS_F0.xxx;
    const float specular_f90 = shadowed_f90(specular_f0);
    const float3 v = float3(sqrt(1.0f - ndotv * ndotv), 0.0f, ndotv);
    const float3 n = float3(0.0f, 0.0f, 1.0f);
    const float alpha = roughness * roughness;
    const float alpha_squared = alpha * alpha;
    float sum_scale = 0.0f;
    float sum_bias = 0.0f;
    for (uint i = 0; i < lut_sample_count; i++) {
        const float2 u = fb::hammersley2d(i, lut_sample_count);
        const float3 h = sample_ggx_vndf(v, alpha.xx, u);
        const float3 l = reflect(-v, h);
        const float hdotl = clamp(dot(h, l), 0.00001f, 1.0f);
        const float ndotl = clamp(dot(n, l), 0.00001f, 1.0f);
        const float ndoth = clamp(dot(n, h), 0.00001f, 1.0f);
        const float vdoth = clamp(dot(v, h), 0.00001f, 1.0f);
        if (ndotl > 0.0f) {
            const float fresnel = evaluate_fresnel(specular_f0, specular_f90, vdoth).x;
            const float weight =
                specular_sample_weight_ggx_vndf(alpha, alpha_squared, ndotl, ndotv, hdotl, ndoth);
            sum_scale += (1.0 - fresnel) * weight;
            sum_bias += fresnel * weight;
        }
    }
    sum_scale /= float(lut_sample_count);
    sum_bias /= float(lut_sample_count);

    // Write.
    lut_texture[dst_id] = float2(sum_scale, sum_bias);
}
