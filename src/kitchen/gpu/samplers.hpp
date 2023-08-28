#pragma once

#include "../pch.hpp"
#include "descriptors.hpp"
#include "samplers.hlsli"

namespace fb {

class GpuDevice;

class GpuSamplers {
    FB_NO_COPY_MOVE(GpuSamplers);

public:
    GpuSamplers() = default;

    auto create(GpuDevice& device, GpuDescriptors& descriptors) -> void;

private:
    std::array<GpuDescriptor, (size_t)GpuSampler::Count> _handles;
};

} // namespace fb
