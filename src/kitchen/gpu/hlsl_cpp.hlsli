// This file contains macros that are used to make the code compatible between HLSL and C++.

#pragma once

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
