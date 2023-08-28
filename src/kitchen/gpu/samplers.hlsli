#pragma once

#include "hlsl_cpp.hlsli"

FB_NAMESPACE_BEGIN(fb)

enum class GpuSampler : uint {
    LinearClamp = 0,
    LinearWrap,
    AnisotropicLinearClamp,
    AnisotropicLinearWrap,
    PointClamp,
    PointWrap,
    Shadow,
    Count,
};

FB_NAMESPACE_END(fb)
