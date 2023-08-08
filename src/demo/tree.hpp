#pragma once

#include "demo.hpp"

namespace fb::tree {

constexpr uint32_t SHADOW_MAP_SIZE = 1024;

struct ShadowConstants {
    Matrix transform;
    float pad[48] = {};
};

struct MainConstants {
    Matrix transform;
    Matrix light_transform;
    Vector3 light_direction;
    float ambient_light = 0.25f;
    float pad[28] = {};
};

class Demo {
  public:
    static constexpr std::string_view NAME = "Tree"sv;
    static constexpr Vector4 CLEAR_COLOR = {0.32549f, 0.51373f, 0.56078f, 1.0f};

    Demo(GpuDevice& device);
    auto gui(const gui::Desc& desc) -> void;
    auto update(const demo::UpdateDesc& desc) -> void;
    auto render(GpuDevice& device, GpuCommandList& cmd) -> void;

    auto rt_color() const -> const GpuTexture2dSrvRtv& { return _render_targets.color(); }

  private:
    GpuRenderTargets _render_targets;
    GpuDebugDraw _debug_draw;

    struct Scene {
        Scene(GpuDevice& device);
        struct Model {
            Model(GpuDevice& device, const GltfModel& gltf_model, std::string_view model_name);
            GpuBufferHostSrv<GltfVertex> vertex_buffer;
            GpuBufferHostIndex<GltfIndex> index_buffer;
            GpuTexture2dSrv texture;
        };
        Model tree;
        Model plane;
    } _scene;

    struct ShadowPass {
        static constexpr std::string_view NAME = "Shadow"sv;
        ShadowPass(GpuDevice& device);
        GpuPipeline pipeline;
        GpuBufferHostCbv<ShadowConstants> constants;
        GpuTexture2dSrvDsv depth;
    } _shadow_pass;

    struct MainPass {
        static constexpr std::string_view NAME = "Main"sv;
        MainPass(GpuDevice& device);
        GpuPipeline pipeline;
        GpuBufferHostCbv<MainConstants> constants;
    } _main_pass;
};

}  // namespace fb::tree
