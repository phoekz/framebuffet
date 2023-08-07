#pragma once

#include <shaders/samplers.hlsli>
#include <pch.hpp>
#include "device.hpp"

namespace fb {

class GpuSamplers {
  public:
    GpuSamplers(GpuDevice& device, GpuDescriptors& descriptors);

  private:
    std::array<GpuDescriptorHandle, (size_t)GpuSamplerType::Count> _handles;
};

}  // namespace fb
