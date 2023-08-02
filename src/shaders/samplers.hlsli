#if __cplusplus
#pragma once
namespace fb {
#define uint uint32_t
#endif

enum class GpuSamplerType : uint {
    LinearClamp = 0,
    LinearWrap,
    Shadow,
    Count,
};

#if __cplusplus
#undef uint
}  // namespace fb
#endif
