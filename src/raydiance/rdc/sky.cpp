#include "sky.hpp"
#include "sky/params_r.hpp"
#include "sky/params_g.hpp"
#include "sky/params_b.hpp"
#include "sky/radiances_r.hpp"
#include "sky/radiances_g.hpp"
#include "sky/radiances_b.hpp"

namespace fb {

static auto quintic_9(const float* const data, const float t) -> float {
    const float t2 = t * t;
    const float t3 = t2 * t;
    const float t4 = t2 * t2;
    const float t5 = t4 * t;

    const float inv_t = 1.0f - t;
    const float inv_t2 = inv_t * inv_t;
    const float inv_t3 = inv_t2 * inv_t;
    const float inv_t4 = inv_t2 * inv_t2;
    const float inv_t5 = inv_t4 * inv_t;

    const float m0 = data[0] * inv_t5;
    const float m1 = data[9] * 5.0f * inv_t4 * t;
    const float m2 = data[2 * 9] * 10.0f * inv_t3 * t2;
    const float m3 = data[3 * 9] * 10.0f * inv_t2 * t3;
    const float m4 = data[4 * 9] * 5.0f * inv_t * t4;
    const float m5 = data[5 * 9] * t5;

    return m0 + m1 + m2 + m3 + m4 + m5;
}

static auto quintic_1(const float* const data, const float t) -> float {
    const float t2 = t * t;
    const float t3 = t2 * t;
    const float t4 = t2 * t2;
    const float t5 = t4 * t;

    const float inv_t = 1.0f - t;
    const float inv_t2 = inv_t * inv_t;
    const float inv_t3 = inv_t2 * inv_t;
    const float inv_t4 = inv_t2 * inv_t2;
    const float inv_t5 = inv_t4 * inv_t;

    const float m0 = data[0] * inv_t5;
    const float m1 = data[1] * 5.0f * inv_t4 * t;
    const float m2 = data[2 * 1] * 10.0f * inv_t3 * t2;
    const float m3 = data[3 * 1] * 10.0f * inv_t2 * t3;
    const float m4 = data[4 * 1] * 5.0f * inv_t * t4;
    const float m5 = data[5 * 1] * t5;

    return m0 + m1 + m2 + m3 + m4 + m5;
}

static auto init_params(
    float* const out_params,
    const float* const data,
    const float turbidity,
    const float albedo,
    const float t
) -> void {
    const size_t turbidity_int = (size_t)turbidity;
    const float turbidity_rem = fmodf(turbidity, 1.0f);
    FB_ASSERT(turbidity_int > 0);
    const size_t turbidity_min = turbidity_int - 1;
    const size_t turbidity_max = turbidity_int < 9 ? turbidity_int : 9;

    const float* const p0 = data + (9 * 6 * turbidity_min);
    const float* const p1 = data + (9 * 6 * turbidity_max);
    const float* const p2 = data + (9 * 6 * 10 + 9 * 6 * turbidity_min);
    const float* const p3 = data + (9 * 6 * 10 + 9 * 6 * turbidity_max);

    const float s0 = (1.0f - albedo) * (1.0f - turbidity_rem);
    const float s1 = (1.0f - albedo) * turbidity_rem;
    const float s2 = albedo * (1.0f - turbidity_rem);
    const float s3 = albedo * turbidity_rem;

    for (size_t i = 0; i < 9; ++i) {
        out_params[i] = 0.0f;
        out_params[i] += s0 * quintic_9(p0 + i, t);
        out_params[i] += s1 * quintic_9(p1 + i, t);
        out_params[i] += s2 * quintic_9(p2 + i, t);
        out_params[i] += s3 * quintic_9(p3 + i, t);
    }
}

static auto init_sky_radiance(
    float* const out_radiance,
    const float* const data,
    const float turbidity,
    const float albedo,
    const float t
) -> void {
    const size_t turbidity_int = (size_t)turbidity;
    const float turbidity_rem = fmodf(turbidity, 1.0f);
    FB_ASSERT(turbidity_int > 0);
    const size_t turbidity_min = turbidity_int - 1;
    const size_t turbidity_max = turbidity_int < 9 ? turbidity_int : 9;

    const float* const p0 = data + (6 * turbidity_min);
    const float* const p1 = data + (6 * turbidity_max);
    const float* const p2 = data + (6 * 10 + 6 * turbidity_min);
    const float* const p3 = data + (6 * 10 + 6 * turbidity_max);

    const float s0 = (1.0f - albedo) * (1.0f - turbidity_rem);
    const float s1 = (1.0f - albedo) * turbidity_rem;
    const float s2 = albedo * (1.0f - turbidity_rem);
    const float s3 = albedo * turbidity_rem;

    *out_radiance = 0.0f;
    *out_radiance += s0 * quintic_1(p0, t);
    *out_radiance += s1 * quintic_1(p1, t);
    *out_radiance += s2 * quintic_1(p2, t);
    *out_radiance += s3 * quintic_1(p3, t);
}

auto rdc_sky_create(RdcSkyState& state, const RdcSkyParams& params) -> RdcSkyStateResult {
    const float elevation = params.elevation;
    const float turbidity = params.turbidity;
    const float albedo[3] = {params.albedo[0], params.albedo[1], params.albedo[2]};

    // Validate.
    if (elevation < 0.0f || elevation > FLOAT_PI) {
        return RdcSkyStateResult::ElevationOutOfRange;
    }
    if (turbidity < 1.0f || turbidity > 10.0f) {
        return RdcSkyStateResult::TurbidityOutOfRange;
    }
    if ((albedo[0] < 0.0f || albedo[0] > 1.0f) || (albedo[1] < 0.0f || albedo[1] > 1.0f)
        || (albedo[2] < 0.0f || albedo[2] > 1.0f)) {
        return RdcSkyStateResult::AlbedoOutOfRange;
    }

    // Init state.
    const float t = powf((elevation / (0.5f * FLOAT_PI)), (1.0f / 3.0f));
    init_params(state.params + 0, params_r, turbidity, albedo[0], t);
    init_params(state.params + 9, params_g, turbidity, albedo[1], t);
    init_params(state.params + (9 * 2), params_b, turbidity, albedo[2], t);
    init_sky_radiance(state.sky_radiances + 0, radiances_r, turbidity, albedo[0], t);
    init_sky_radiance(state.sky_radiances + 1, radiances_g, turbidity, albedo[1], t);
    init_sky_radiance(state.sky_radiances + 2, radiances_b, turbidity, albedo[2], t);

    return RdcSkyStateResult::Success;
}

auto rdc_sky_radiance(const RdcSkyState& state, float theta, float gamma, RdcSkyChannel channel)
    -> float {
    const uint channel_idx = (uint)channel;
    const float r = state.sky_radiances[channel_idx];
    const float* p = state.params + (9 * channel_idx);
    const float p0 = p[0];
    const float p1 = p[1];
    const float p2 = p[2];
    const float p3 = p[3];
    const float p4 = p[4];
    const float p5 = p[5];
    const float p6 = p[6];
    const float p7 = p[7];
    const float p8 = p[8];

    const float cos_gamma = cosf(gamma);
    const float cos_gamma_2 = cos_gamma * cos_gamma;
    const float cos_theta = fabsf(cosf(theta));

    const float exp_m = expf(p4 * gamma);
    const float ray_m = cos_gamma_2;
    const float mie_m_lhs = 1.0f + cos_gamma_2;
    const float mie_m_rhs = powf(1.0f + p8 * p8 - 2.0f * p8 * cos_gamma, 1.5f);
    const float mie_m = mie_m_lhs / mie_m_rhs;
    const float zenith = sqrtf(cos_theta);
    const float radiance_lhs = 1.0f + p0 * expf(p1 / (cos_theta + 0.01f));
    const float radiance_rhs = p2 + p3 * exp_m + p5 * ray_m + p6 * mie_m + p7 * zenith;
    const float radiance_dist = radiance_lhs * radiance_rhs;

    return r * radiance_dist;
}

} // namespace fb
