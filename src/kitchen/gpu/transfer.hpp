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
    auto resource(
        const ComPtr<ID3D12Resource2>& resource,
        const D3D12_SUBRESOURCE_DATA& data,
        D3D12_BARRIER_SYNC sync_before,
        D3D12_BARRIER_SYNC sync_after,
        D3D12_BARRIER_ACCESS access_before,
        D3D12_BARRIER_ACCESS access_after,
        Option<D3D12_BARRIER_LAYOUT> layout_before = std::nullopt,
        Option<D3D12_BARRIER_LAYOUT> layout_after = std::nullopt
    ) -> void;
    auto resource(
        const ComPtr<ID3D12Resource2>& resource,
        std::span<const D3D12_SUBRESOURCE_DATA> datas,
        D3D12_BARRIER_SYNC sync_before,
        D3D12_BARRIER_SYNC sync_after,
        D3D12_BARRIER_ACCESS access_before,
        D3D12_BARRIER_ACCESS access_after,
        Option<D3D12_BARRIER_LAYOUT> layout_before = std::nullopt,
        Option<D3D12_BARRIER_LAYOUT> layout_after = std::nullopt
    ) -> void;
    auto flush(const GpuDevice& device) -> void;

private:
    struct Stats {
        uint64_t transfers = 0;
        uint64_t bytes = 0;
    } _stats;

    GpuTransferImpl* _impl = nullptr;
};

} // namespace fb
