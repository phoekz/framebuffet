#pragma once

#include <pch.hpp>
#include <shaders/samplers.hlsli>
#include "win32.hpp"
#include "utils.hpp"
#include "maths.hpp"

namespace fb {

#pragma region Constants

constexpr uint32_t FRAME_COUNT = 2;
constexpr uint32_t BINDINGS_CAPACITY = 16;
constexpr D3D_FEATURE_LEVEL MIN_FEATURE_LEVEL = D3D_FEATURE_LEVEL_12_2;

#pragma endregion

#pragma region Forward declarations

class GpuDevice;
class GpuPipeline;

#pragma endregion

#pragma region Descriptors

class GpuDescriptor {
    friend class GpuDescriptorHeap;

  public:
    auto type() const -> D3D12_DESCRIPTOR_HEAP_TYPE { return _type; }
    auto index() const -> uint32_t { return _index; }
    auto cpu() const -> D3D12_CPU_DESCRIPTOR_HANDLE { return _cpu_handle; }
    auto cpu_ptr() const -> const D3D12_CPU_DESCRIPTOR_HANDLE* { return &_cpu_handle; }
    auto gpu() const -> D3D12_GPU_DESCRIPTOR_HANDLE { return _gpu_handle; }
    auto gpu_ptr() const -> const D3D12_GPU_DESCRIPTOR_HANDLE* { return &_gpu_handle; }

  private:
    D3D12_DESCRIPTOR_HEAP_TYPE _type = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
    uint32_t _index = UINT32_MAX;
    D3D12_CPU_DESCRIPTOR_HANDLE _cpu_handle = {};
    D3D12_GPU_DESCRIPTOR_HANDLE _gpu_handle = {};
};

class GpuDescriptorHeap {
  public:
    GpuDescriptorHeap(
        GpuDevice& device,
        std::string_view name,
        D3D12_DESCRIPTOR_HEAP_TYPE type,
        uint32_t capacity);

    auto alloc() -> GpuDescriptor;
    auto type() const -> D3D12_DESCRIPTOR_HEAP_TYPE { return _type; }
    auto count() const -> uint32_t { return _count; }
    auto capacity() const -> uint32_t { return _capacity; }
    auto heap() const -> ID3D12DescriptorHeap* { return _heap.get(); }
    auto heap_ptr() const -> ID3D12DescriptorHeap* const* { return _heap.addressof(); }

  private:
    D3D12_DESCRIPTOR_HEAP_TYPE _type = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
    uint32_t _descriptor_size = 0;
    uint32_t _count = 0;
    uint32_t _capacity = 0;
    ComPtr<ID3D12DescriptorHeap> _heap;
    D3D12_CPU_DESCRIPTOR_HANDLE _cpu_heap_start = {};
    D3D12_GPU_DESCRIPTOR_HANDLE _gpu_heap_start = {};
};

class GpuDescriptors {
  public:
    static constexpr uint32_t CBV_SRV_UAV_DESCRIPTOR_CAPACITY = 128;
    static constexpr uint32_t SAMPLER_DESCRIPTOR_CAPACITY = 6;
    static constexpr uint32_t RTV_DESCRIPTOR_CAPACITY = 16;
    static constexpr uint32_t DSV_DESCRIPTOR_CAPACITY = 16;

    GpuDescriptors(GpuDevice& device, std::string_view name);

    auto rtv() -> GpuDescriptorHeap& { return _rtv_heap; }
    auto dsv() -> GpuDescriptorHeap& { return _dsv_heap; }
    auto cbv_srv_uav() -> GpuDescriptorHeap& { return _cbv_srv_uav_heap; }
    auto sampler() -> GpuDescriptorHeap& { return _sampler_heap; }
    auto log_stats() -> void;

  private:
    GpuDescriptorHeap _cbv_srv_uav_heap;
    GpuDescriptorHeap _sampler_heap;
    GpuDescriptorHeap _rtv_heap;
    GpuDescriptorHeap _dsv_heap;
};

#pragma endregion

#pragma region Samplers

class GpuSamplers {
  public:
    GpuSamplers(GpuDevice& device, GpuDescriptors& descriptors);

  private:
    std::array<GpuDescriptor, (size_t)GpuSamplerType::Count> _handles;
};

#pragma endregion

#pragma region Commands

enum class GpuCommandEngine {
    Unknown,
    Graphics,
    Compute,
    Copy,
};

class GpuCommandList {
    friend class GpuDevice;

  public:
    GpuCommandList(GpuCommandList&& o) { *this = std::move(o); };
    GpuCommandList& operator=(GpuCommandList&& o) {
        _cmd = std::exchange(o._cmd, nullptr);
        _engine = std::exchange(o._engine, GpuCommandEngine::Unknown);
        _root_signature = std::exchange(o._root_signature, nullptr);
        _descriptors = std::exchange(o._descriptors, nullptr);
        return *this;
    };

    auto begin_pix(std::string_view name) const -> void;
    auto end_pix() const -> void;

    auto set_graphics() -> void;
    auto set_compute() -> void;

    auto set_viewport(
        uint32_t left,
        uint32_t top,
        uint32_t right,
        uint32_t bottom,
        float min_depth = 0.0f,
        float max_depth = 1.0f) const -> void;
    auto set_scissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom) const -> void;
    auto set_rtv_dsv(
        const std::optional<GpuDescriptor>& rtv,
        const std::optional<GpuDescriptor>& dsv) const -> void;
    auto set_topology(D3D12_PRIMITIVE_TOPOLOGY topology) const -> void;
    auto set_index_buffer(D3D12_INDEX_BUFFER_VIEW ibv) const -> void;
    auto set_blend_factor(Float4 factor) const -> void;
    auto set_pipeline(const GpuPipeline& pipeline) const -> void;
    auto set_graphics_constants(std::initializer_list<uint32_t> constants) const -> void;
    auto set_compute_constants(std::initializer_list<uint32_t> constants) const -> void;

    auto clear_rtv(const GpuDescriptor& rtv, Float4 color) const -> void;
    auto clear_dsv(const GpuDescriptor& dsv, float depth) const -> void;
    auto draw_instanced(
        uint32_t vertex_count,
        uint32_t instance_count,
        uint32_t start_vertex,
        uint32_t start_instance) const -> void;
    auto draw_indexed_instanced(
        uint32_t index_count,
        uint32_t instance_count,
        uint32_t start_index,
        int32_t base_vertex,
        uint32_t start_instance) const -> void;
    auto dispatch(uint32_t x, uint32_t y, uint32_t z) const -> void;

    auto transition_barrier(
        const ComPtr<ID3D12Resource>& resource,
        D3D12_RESOURCE_STATES before,
        D3D12_RESOURCE_STATES after) const -> void;
    auto uav_barrier(const ComPtr<ID3D12Resource>& resource) const -> void;

  private:
    GpuCommandList(
        ID3D12GraphicsCommandList9* cmd,
        GpuCommandEngine engine,
        ID3D12RootSignature* root_signature,
        GpuDescriptors* descriptors) :
        _cmd(cmd),
        _engine(engine),
        _root_signature(root_signature),
        _descriptors(descriptors) {}
    GpuCommandList(GpuCommandList& o) = delete;
    GpuCommandList& operator=(GpuCommandList& o) = delete;

    auto set_global_descriptor_heap() -> void;

    ID3D12GraphicsCommandList9* _cmd = nullptr;
    GpuCommandEngine _engine = GpuCommandEngine::Unknown;
    ID3D12RootSignature* _root_signature = nullptr;
    GpuDescriptors* _descriptors = nullptr;
};

#pragma endregion

#pragma region Device

class GpuDevice {
    FB_NO_COPY_MOVE(GpuDevice);

  public:
    // Device state.
    GpuDevice(const Window& window);
    auto begin_frame() -> GpuCommandList;
    auto begin_main_pass() -> void;
    auto end_main_pass() -> void;
    auto end_frame(GpuCommandList&& cmd) -> void;
    auto wait() -> void;

    // Direct3D 12 wrappers.
    // clang-format off
    [[nodiscard]] auto create_root_signature(const ComPtr<ID3DBlob>& signature, std::string_view name) const -> ComPtr<ID3D12RootSignature>;
    [[nodiscard]] auto create_descriptor_heap(const D3D12_DESCRIPTOR_HEAP_DESC& desc, std::string_view name) const -> ComPtr<ID3D12DescriptorHeap>;
    [[nodiscard]] auto create_pipeline_state( const D3D12_PIPELINE_STATE_STREAM_DESC& desc, std::string_view name) -> ComPtr<ID3D12PipelineState>;
    [[nodiscard]] auto create_committed_resource(const CD3DX12_HEAP_PROPERTIES& heap_props, const D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES init_state, const std::optional<D3D12_CLEAR_VALUE>& clear_value, std::string_view name) const -> ComPtr<ID3D12Resource>;
    [[nodiscard]] auto create_fence(uint64_t init_value, std::string_view name) const -> ComPtr<ID3D12Fence1>;
    auto create_constant_buffer_view(const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc, D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const -> void;
    auto create_shader_resource_view(const ComPtr<ID3D12Resource>& resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc, D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const -> void;
    auto create_unordered_access_view(const ComPtr<ID3D12Resource>& resource, const std::optional<std::reference_wrapper<ComPtr<ID3D12Resource>>> counter, const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc, D3D12_CPU_DESCRIPTOR_HANDLE descriptor) -> void;
    auto create_render_target_view(const ComPtr<ID3D12Resource>& resource, const std::optional<D3D12_RENDER_TARGET_VIEW_DESC>& desc, D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const -> void;
    auto create_depth_stencil_view(const ComPtr<ID3D12Resource>& resource, const std::optional<D3D12_DEPTH_STENCIL_VIEW_DESC>& desc, D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const -> void;
    auto create_sampler(const D3D12_SAMPLER_DESC& desc, D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const -> void;
    auto descriptor_size(D3D12_DESCRIPTOR_HEAP_TYPE heap_type) const -> uint32_t;
    // clang-format on

    // Resource utilities.
    auto easy_upload(
        const D3D12_SUBRESOURCE_DATA& data,
        const ComPtr<ID3D12Resource>& resource,
        D3D12_RESOURCE_STATES before_state,
        D3D12_RESOURCE_STATES after_state) const -> void;
    auto easy_multi_upload(
        std::span<const D3D12_SUBRESOURCE_DATA> datas,
        const ComPtr<ID3D12Resource>& resource,
        D3D12_RESOURCE_STATES before_state,
        D3D12_RESOURCE_STATES after_state) const -> void;

    // Getters.
    auto swapchain_size() const -> Uint2 { return _swapchain_size; }
    auto frame_index() const -> uint32_t { return _frame_index; }
    auto root_signature() const -> ID3D12RootSignature* { return _root_signature.get(); }
    auto descriptors() -> GpuDescriptors& { return *_descriptors; }

    // Debugging.
    auto log_stats() -> void;

  private:
    struct LeakTracker {
        ~LeakTracker();
        ComPtr<ID3D12DebugDevice2> debug_device;
    };

    LeakTracker _leak_tracker;
    ComPtr<ID3D12Device12> _device;
    ComPtr<ID3D12CommandQueue> _command_queue;
    std::array<ComPtr<ID3D12CommandAllocator>, FRAME_COUNT> _command_allocators;
    ComPtr<ID3D12GraphicsCommandList9> _command_list;
    ComPtr<IDXGISwapChain4> _swapchain;
    Uint2 _swapchain_size = {};
    ComPtr<ID3D12DescriptorHeap> _rtv_descriptor_heap;
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, FRAME_COUNT> _rtv_descriptors = {};
    std::array<ComPtr<ID3D12Resource>, FRAME_COUNT> _rtvs = {};

    uint32_t _frame_index = 0;
    ComPtr<ID3D12Fence1> _fence;
    wil::unique_handle _fence_event;
    std::array<uint64_t, FRAME_COUNT> _fence_values = {};

    ComPtr<ID3D12RootSignature> _root_signature;
    std::unique_ptr<GpuDescriptors> _descriptors;
    std::unique_ptr<GpuSamplers> _samplers;
};

#pragma endregion

#pragma region Debugging

template<typename... Args>
[[nodiscard]] auto dx_name(Args&&... args) -> std::string {
    constexpr std::string_view DELIMITER = " - ";
    std::ostringstream oss;
    ((oss << args << DELIMITER), ...);
    std::string str = oss.str();
    str.erase(str.size() - DELIMITER.size());
    return str;
}

#pragma endregion

}  // namespace fb
