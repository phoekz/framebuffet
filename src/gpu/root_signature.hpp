#pragma once

#include <pch.hpp>
#include "device.hpp"
#include "utils.hpp"

namespace fb {

class GpuRootSignature {
  public:
    GpuRootSignature(GpuDevice& device, std::string_view name);

    auto get() const -> ID3D12RootSignature* { return _root_signature.get(); }

  private:
    ComPtr<ID3D12RootSignature> _root_signature;
};

}  // namespace fb
