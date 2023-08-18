#pragma once

#include "../pch.hpp"
#include "descriptors.hpp"
#include "samplers.hlsli"

namespace fb {

class GpuDevice;

class GpuSamplers {
  public:
    GpuSamplers(GpuDevice& device, GpuDescriptors& descriptors);

  private:
    std::array<GpuDescriptor, (size_t)GpuSamplerType::Count> _handles;
};

} // namespace fb
