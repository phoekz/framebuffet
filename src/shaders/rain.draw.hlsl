cbuffer ConstantBuffer: register(b0) {
    float4x4 transform;
};

struct VertexInput {
    float3 position: POSITION;
};

struct VertexOutput {
    float4 position: SV_POSITION;
};

VertexOutput vs_main(VertexInput input) {
    VertexOutput output;
    output.position = mul(transform, float4(input.position, 1.0f));
    return output;
}

struct PixelOutput {
    float4 color: SV_TARGET;
};

PixelOutput ps_main(VertexOutput input) {
    PixelOutput output;
    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    return output;
}
