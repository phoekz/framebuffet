#include "shaders/samplers.hlsli"

//
// Types
//

struct Constants {
    float4x4 transform;
};

struct VertexInput {
    float2 position: POSITION;
    float4 color: COLOR0;
    float2 texcoord: TEXCOORD0;
};

struct VertexOutput {
    float4 position: SV_POSITION;
    float4 color: COLOR0;
    float2 texcoord: TEXCOORD0;
};

struct PixelOutput {
    float4 color: SV_TARGET;
};

//
// Bindings
//

struct Bindings {
    uint constants;
    uint texture;
};
ConstantBuffer<Bindings> g_bindings: register(b0);

//
// Entry points
//

VertexOutput vs_main(VertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];

    VertexOutput output;
    output.position = mul(constants.transform, float4(input.position, 0.0f, 1.0f));
    output.color = input.color;
    output.texcoord = input.texcoord;
    return output;
};

PixelOutput ps_main(VertexOutput input) {
    Texture2D texture = ResourceDescriptorHeap[g_bindings.texture];
    SamplerState samp = SamplerDescriptorHeap[(uint)GpuSamplerType::LinearWrap];

    PixelOutput output;
    output.color = input.color * texture.Sample(samp, input.texcoord);
    return output;
};
