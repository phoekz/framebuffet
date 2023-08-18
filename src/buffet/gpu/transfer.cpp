#include "transfer.hpp"

namespace fb {

GpuTransfer::GpuTransfer(const ComPtr<ID3D12Device>& device) : _batch(device.get()) {}

auto GpuTransfer::begin() -> void {
    _batch.Begin(D3D12_COMMAND_LIST_TYPE_DIRECT);
}

auto GpuTransfer::resource(
    const ComPtr<ID3D12Resource>& resource,
    const D3D12_SUBRESOURCE_DATA& data,
    D3D12_RESOURCE_STATES before_state,
    D3D12_RESOURCE_STATES after_state) -> void {
    this->resource(resource, std::span(&data, 1), before_state, after_state);
}

auto GpuTransfer::resource(
    const ComPtr<ID3D12Resource>& resource,
    std::span<const D3D12_SUBRESOURCE_DATA> datas,
    D3D12_RESOURCE_STATES before_state,
    D3D12_RESOURCE_STATES after_state) -> void {
    _batch.Transition(resource.get(), before_state, D3D12_RESOURCE_STATE_COPY_DEST);
    _batch.Upload(resource.get(), 0, datas.data(), (uint32_t)datas.size());
    _batch.Transition(resource.get(), D3D12_RESOURCE_STATE_COPY_DEST, after_state);
    _stats.transfers += datas.size();
    for (const auto& data : datas) {
        _stats.bytes += data.SlicePitch;
    }
}

auto GpuTransfer::end(const ComPtr<ID3D12CommandQueue>& command_queue) -> void {
    _batch.End(command_queue.get()).wait();
    FB_LOG_INFO(
        "Transfered resources: {} ({:.3f} MBytes)",
        _stats.transfers,
        (double)_stats.bytes / 1024.0 / 1024.0);
    _stats = {};
}

} // namespace fb
