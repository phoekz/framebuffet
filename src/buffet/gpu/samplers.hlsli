#pragma once

#include "hlsl_cpp.hlsli"

FB_NAMESPACE_BEGIN(fb)

enum class GpuSamplerType : uint {
    LinearClamp = 0,
    LinearWrap,
    Shadow,
    Count,
};

FB_NAMESPACE_END(fb)