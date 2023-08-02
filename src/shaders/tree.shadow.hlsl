#include "shaders/samplers.hlsli"

//
// Types
//

struct Constants {
    float4x4 transform;
    float pad[48];
};

struct VertexInput {
    float3 position: POSITION;
    float3 normal: NORMAL;
    float2 texcoord: TEXCOORD;
};

struct VertexOutput {
    float4 position: SV_POSITION;
};

//
// Bindings
//

struct Bindings {
    uint constants;
};
ConstantBuffer<Bindings> g_bindings: register(b0);

//
// Entry points
//

VertexOutput vs_main(VertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];

    VertexOutput output;
    output.position = mul(constants.transform, float4(input.position, 1.0f));
    return output;
}
