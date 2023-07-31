cbuffer ConstantBuffer: register(b0) {
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

VertexOutput vs_main(VertexInput input) {
    VertexOutput output;
    output.position = mul(transform, float4(input.position, 1.0f));
    return output;
}
