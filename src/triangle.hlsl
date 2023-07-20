struct VertexInput {
    float4 position: POSITION;
    float4 color: COLOR;
};

struct VertexOutput {
    float4 position: SV_POSITION;
    float4 color: COLOR;
};

VertexOutput vertex_shader(VertexInput input) {
    VertexOutput output;
    output.position = input.position;
    output.color = input.color;
    return output;
}

float4 pixel_shader(VertexOutput input) : SV_TARGET {
    return input.color;
}
