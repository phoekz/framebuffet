#pragma once

#include "../pch.hpp"

namespace fb {

class GpuTransfer {
public:
    GpuTransfer(const ComPtr<ID3D12Device>& device);
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
    auto end(const ComPtr<ID3D12CommandQueue>& command_queue) -> void;

private:
    struct Stats {
        uint64_t transfers = 0;
        uint64_t bytes = 0;
    } _stats;

    DirectX::ResourceUploadBatch _batch;
};

} // namespace fb
