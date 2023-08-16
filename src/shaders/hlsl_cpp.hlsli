#pragma once

// This file contains macros that are used to make the code compatible between HLSL and C++.

//
// Macros.
//

#if __cplusplus
    #define FB_CONSTANT inline constexpr
    #define FB_NAMESPACE_BEGIN namespace fb {
    #define FB_NAMESPACE_END }  // namespace fb
#else
    #define FB_CONSTANT static const
    #define FB_NAMESPACE_BEGIN
    #define FB_NAMESPACE_END
#endif

//
// Type compatibility.
//

#if __cplusplus
using uint = uint32_t;
#endif
