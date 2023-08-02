#pragma once

#include "dx12.hpp"
#include "utils.hpp"
#include <pch.hpp>

namespace fb {

class GpuRootSignature {
  public:
    GpuRootSignature(Dx& dx, std::string_view name);

    auto get() const -> ID3D12RootSignature* { return _root_signature.get(); }

  private:
    ComPtr<ID3D12RootSignature> _root_signature;
};

}  // namespace fb
