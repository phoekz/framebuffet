#pragma once

#include "../pch.hpp"

namespace fb {

class GpuDevice;
struct GpuTransferImpl;
class GpuTransfer {
    FB_NO_COPY_MOVE(GpuTransfer);

public:
    GpuTransfer() = default;
    ~GpuTransfer();

    auto create(const ComPtr<ID3D12Device12>& device) -> void;
    auto begin() -> void;
    auto resource(
        const ComPtr<ID3D12Resource>& resource,
        const D3D12_SUBRESOURCE_DATA& data,
        D3D12_RESOURCE_STATES before_state,
        D3D12_RESOURCE_STATES after_state
    ) -> void;
    auto resource(
        const ComPtr<ID3D12Resource>& resource,
        std::span<const D3D12_SUBRESOURCE_DATA> datas,
        D3D12_RESOURCE_STATES before_state,
        D3D12_RESOURCE_STATES after_state
    ) -> void;
    auto end(const GpuDevice& device) -> void;

private:
    struct Stats {
        uint64_t transfers = 0;
        uint64_t bytes = 0;
    } _stats;

    GpuTransferImpl* _impl = nullptr;
};

} // namespace fb
