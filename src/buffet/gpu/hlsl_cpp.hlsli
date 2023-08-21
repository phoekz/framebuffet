#pragma once

// This file contains macros that are used to make the code compatible between HLSL and C++.

//
// Macros.
//

#if __cplusplus
    #define FB_CONSTANT inline constexpr
    #define FB_NAMESPACE_BEGIN(x) namespace x {
    #define FB_NAMESPACE_END(x) }
#else
    #define FB_CONSTANT static const
    #define FB_NAMESPACE_BEGIN(x)
    #define FB_NAMESPACE_END(x)
#endif

//
// Type compatibility.
//

#if __cplusplus
namespace fb {
using uint = uint32_t;
using uint2 = Uint2;
using uint3 = Uint3;
using uint4 = Uint4;
using float2 = Float2;
using float3 = Float3;
using float4 = Float4;
using float4x4 = Float4x4;
} // namespace fb
#endif
