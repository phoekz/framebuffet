#pragma once

#include <shaders/samplers.hlsli>
#include "descriptors.hpp"
#include <pch.hpp>

namespace fb {

class GpuSamplers {
  public:
    GpuSamplers(GpuDevice& device, GpuDescriptors& descriptors);

  private:
    std::array<GpuDescriptorHandle, (size_t)GpuSamplerType::Count> _handles;
};

}  // namespace fb
