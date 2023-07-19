struct VertexOutput {
    float4 position: SV_POSITION;
};

VertexOutput vertex_shader(uint vertex_id : SV_VERTEXID) {
    VertexOutput output;
    output.position = float4(0.0f, 0.0f, 0.0f, 1.0f);
    return output;
}

float4 pixel_shader(VertexOutput input) : SV_TARGET {
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
