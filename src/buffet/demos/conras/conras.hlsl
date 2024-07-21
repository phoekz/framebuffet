#include <buffet/demos/conras/conras.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/graphics/core.hlsli>

ConstantBuffer<Bindings> g_bindings: register(b0);

//
// Clear
//

struct ClearVertexOutput {
    float4 position: SV_Position;
    float2 texcoord: ATTRIBUTE0;
};

ClearVertexOutput clear_vs(fb::VertexInput input) {
    ClearVertexOutput output;
    fb::fullscreen_triangle(input.vertex_id, output.position, output.texcoord);
    return output;
}

void clear_ps(ClearVertexOutput vs_output) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    RWStructuredBuffer<uint3> raster_buffer = ResourceDescriptorHeap[g_bindings.raster_buffer];
    const uint2 coord2d = (uint2)vs_output.position.xy;
    const uint coord = coord2d.x + coord2d.y * constants.raster_buffer_size.x;
    raster_buffer[coord].x = 0u;
    raster_buffer[coord].y = 0u;
    raster_buffer[coord].z = 0u;
}

//
// Raster
//

struct RasterVertexOutput {
    float4 position: SV_Position;
};

RasterVertexOutput raster_vs(fb::VertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];

    RasterVertexOutput output;
    output.position = mul(constants.transform, float4(STAR_VERTICES[input.vertex_id], 1.0f));
    return output;
}

struct RasterPixelInput {
    uint inner_coverage: SV_InnerCoverage;
};

void raster_cr_off_ps(RasterVertexOutput vs_output) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    RWStructuredBuffer<uint3> raster_buffer = ResourceDescriptorHeap[g_bindings.raster_buffer];
    const uint2 coord2d = (uint2)vs_output.position.xy;
    const uint coord = coord2d.x + coord2d.y * constants.raster_buffer_size.x;
    InterlockedAdd(raster_buffer[coord].x, 1u);
}

void raster_cr_on_ps(RasterVertexOutput vs_output, RasterPixelInput ps_input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    RWStructuredBuffer<uint3> raster_buffer = ResourceDescriptorHeap[g_bindings.raster_buffer];
    const uint2 coord2d = (uint2)vs_output.position.xy;
    const uint coord = coord2d.x + coord2d.y * constants.raster_buffer_size.x;
    InterlockedAdd(raster_buffer[coord].y, 1u);
    InterlockedAdd(raster_buffer[coord].z, ps_input.inner_coverage & 1u);
}

//
// Display
//

struct DisplayVertexOutput {
    float4 position: SV_Position;
    float2 texcoord: ATTRIBUTE0;
};

DisplayVertexOutput display_vs(fb::VertexInput input) {
    DisplayVertexOutput output;
    fb::fullscreen_triangle(input.vertex_id, output.position, output.texcoord);
    return output;
}

fb::PixelOutput<1> display_ps(DisplayVertexOutput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<uint3> raster_buffer = ResourceDescriptorHeap[g_bindings.raster_buffer];
    const uint2 coord2d = (uint2)(input.texcoord * constants.raster_buffer_size);
    const uint coord = coord2d.x + coord2d.y * constants.raster_buffer_size.x;
    const uint3 raster = raster_buffer[coord];
    const uint regular = raster.x;
    const uint over = raster.y;
    const uint under = raster.z;
    float value;
    switch (g_bindings.conservative_rasterization) {
        case ConservativeRasterization::Off: {
            value = (float)regular;
            break;
        }
        case ConservativeRasterization::Over: {
            value = (float)min(over, 1u);
            break;
        }
        case ConservativeRasterization::Under: {
            value = (float)under;
            break;
        }
        case ConservativeRasterization::OverUnder: {
            value = (float)clamp((int)over - (int)under, 0, 1);
            break;
        }
    }
    const float3 color = float3(0.0f, value * input.texcoord);

    fb::PixelOutput<1> output;
    output.color = float4(color, 1.0f);
    return output;
}
