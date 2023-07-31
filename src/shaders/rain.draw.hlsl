//
// Types
//

struct Constants {
    float4x4 transform;
};

struct VertexInput {
    float3 position: POSITION;
};

struct VertexOutput {
    float4 position: SV_POSITION;
};

struct PixelOutput {
    float4 color: SV_TARGET;
};

//
// I/O
//

ConstantBuffer<Constants> g_constants: register(b0);

//
// Entry points
//

VertexOutput vs_main(VertexInput input) {
    VertexOutput output;
    output.position = mul(g_constants.transform, float4(input.position, 1.0f));
    return output;
}

PixelOutput ps_main(VertexOutput input) {
    PixelOutput output;
    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    return output;
}
