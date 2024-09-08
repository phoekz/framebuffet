// This implementation is adapted from
// https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK.
//
// The original source was ran through the preprocessor and dead code
// elimination. This version uses wave intrinsics, linear sampling of mip 0,
// SM6.6 dynamic resources, and half precision floats. ALl original comments are
// replaced with our own notes.

#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/kcn/core.hlsli>
#include <kitchen/kcn/spd.hlsli>

//
// Globals.
//

// clang-format off
ConstantBuffer<Bindings> g_bindings: register(b0);
#define G_CONSTANTS(name) ConstantBuffer<Constants> name = ResourceDescriptorHeap[g_bindings.constants]
#define G_ATOMICS(name) globallycoherent RWStructuredBuffer<Atomics> name = ResourceDescriptorHeap[g_bindings.atomics]
#define G_TEXTURE_SRC(name) Texture2DArray<float4> name = ResourceDescriptorHeap[g_bindings.texture_src]
#define G_TEXTURE_MID(name) globallycoherent RWTexture2DArray<float4> name = ResourceDescriptorHeap[g_bindings.texture_mid]
#define G_TEXTURE_DST(name, mip) RWTexture2DArray<float4> name = ResourceDescriptorHeap[g_bindings.texture_dst_begin + (mip)]
#define G_SAMPLER(name, type) SamplerState name = SamplerDescriptorHeap[(uint)GpuSampler::type]
// clang-format on

//
// Groupshared.
//

groupshared uint gs_counter;
groupshared half2 gs_tmp_rg[16][16];
groupshared half2 gs_tmp_ba[16][16];

//
// Reductions.
//

void reduce(out half4 reduced, half4 v0, half4 v1, half4 v2, half4 v3) {
    reduced = (v0 + v1 + v2 + v3) * half(0.25);
}

void reduce_quad_wave(out half4 reduced, half4 v) {
    const uint lane = WaveGetLaneIndex();
    const uint quad = lane & ~0b11u;
    const half4 v0 = v;
    const half4 v1 = WaveReadLaneAt(v, quad | 0b01);
    const half4 v2 = WaveReadLaneAt(v, quad | 0b10);
    const half4 v3 = WaveReadLaneAt(v, quad | 0b11);
    reduce(reduced, v0, v1, v2, v3);
}

//
// I/O helpers.
//

class SourceIO {
    void sample(out half4 src, uint2 src_id, uint slice_id, float2 inv_size) {
        G_TEXTURE_SRC(texture_src);
        G_SAMPLER(linear_clamp, LinearClamp);
        const float2 src_texcoord = float2(src_id) * inv_size + inv_size;
        src = (half4)texture_src.SampleLevel(linear_clamp, float3(src_texcoord, slice_id), 0);
    }
};

class DstIO {
    void store(half4 dst, uint2 dst_id, uint mip_id, uint slice_id) {
        G_TEXTURE_DST(texture_dst, mip_id + 1);
        texture_dst[uint3(dst_id, slice_id)] = float4(dst);
    }
};

class MidIO {
    void load(out half4 mid, uint2 mid_id, uint slice_id) {
        G_TEXTURE_MID(texture_mid);
        mid = half4(texture_mid[uint3(mid_id, slice_id)]);
    }

    void load_and_reduce(out half4 reduced, uint2 mid_id, uint slice_id) {
        half4 v0, v1, v2, v3;
        load(v0, mid_id + uint2(0, 0), slice_id);
        load(v1, mid_id + uint2(0, 1), slice_id);
        load(v2, mid_id + uint2(1, 0), slice_id);
        load(v3, mid_id + uint2(1, 1), slice_id);
        reduce(reduced, v0, v1, v2, v3);
    }

    void store(half4 mid, uint2 mid_id, uint slice_id) {
        G_TEXTURE_MID(texture_mid);
        texture_mid[uint3(mid_id, slice_id)] = float4(mid);
    }
};

class TmpIO {
    void load(out half4 tmp, uint2 tmp_id) {
        tmp = half4(
            gs_tmp_rg[tmp_id.x][tmp_id.y].x,
            gs_tmp_rg[tmp_id.x][tmp_id.y].y,
            gs_tmp_ba[tmp_id.x][tmp_id.y].x,
            gs_tmp_ba[tmp_id.x][tmp_id.y].y
        );
    }

    void store(half4 tmp, uint2 tmp_id) {
        gs_tmp_rg[tmp_id.x][tmp_id.y] = tmp.xy;
        gs_tmp_ba[tmp_id.x][tmp_id.y] = tmp.zw;
    }
};

//
// Wave utilities.
//

uint2 map_64x1_into_8x8_tile(uint i) {
    // Maps 64-wide wave index into 8x8 in the following way:
    //
    //  00 01 08 09 16 17 24 25
    //  02 03 10 11 18 19 26 27
    //  04 05 12 13 20 21 28 29
    //  06 07 14 15 22 23 30 31
    //  32 33 40 41 48 49 56 57
    //  34 35 42 43 50 51 58 59
    //  36 37 44 45 52 53 60 61
    //  38 39 46 47 54 55 62 63
    //
    const uint x0 = (i >> 0) & 0b0001;
    const uint y0 = (i >> 1) & 0b0011;
    const uint x1 = (i >> 2) & 0b0110;
    const uint y1 = (i >> 3) & 0b0100;
    return uint2(x0 | x1, y0 | y1);
}

//
// Entry point.
//

FB_ATTRIBUTE(numthreads, 256, 1, 1)
void downsample_cs(fb::ComputeInput input) {
    // Globals.
    G_ATOMICS(atomics);
    G_CONSTANTS(constants);

    // Unpack.
    const uint2 group_id = input.group_id.xy;
    const uint slice_id = input.group_id.z;
    const uint group_index = input.group_index;
    const uint mip_count = constants.mip_count;
    const uint threadgroup_count = constants.threadgroup_count;
    const float2 inv_size = constants.inv_texture_size;

    // I/O helpers.
    // - src: read-only, device.
    // - dst: write-only, device.
    // - mid: read-write, device, globallycoherent.
    // - tmp: read-write, groupshared.
    SourceIO src;
    DstIO dst;
    MidIO mid;
    TmpIO tmp;

    // Maps group index for wave reduction.
    const uint2 tile = map_64x1_into_8x8_tile(group_index % 64);

    // 8x8 tile is placed into a 16x16 space, which spans over 256 threads.
    const uint x = tile.x + 8 * ((group_index / 64) % 2);
    const uint y = tile.y + 8 * (group_index / 128);

    // Registers.
    half4 v0, v1, v2, v3;
    uint mip_id;
    uint2 src_id, dst_id, tmp_id, mid_id;

    // Mip 0: reduce 64x64 -> 32x32.
    //
    // Note: Mip 0 is the only level which samples from the source texture.
    //
    // Note: Mip 0 samples with a bilinear filter, which means that these four
    // samples sample from four different texels, totaling 16 texels.
    //
    // Note: Mip 0 does not write the reduced samples into groupshared memory.
    // The reduced samples are kept in registers for Mip 1.
    //
    // Note: Each of these code blocks can early out if the are no more mip
    // levels to downsample.
    {
        mip_id = 0;

        src_id = uint2(x * 2, y * 2) + uint2(group_id * 64);
        dst_id = uint2(x, y) + uint2(group_id * 32);
        src.sample(v0, src_id, slice_id, inv_size);
        dst.store(v0, dst_id, mip_id, slice_id);

        src_id = uint2(x * 2 + 32, y * 2) + uint2(group_id * 64);
        dst_id = uint2(x + 16, y) + uint2(group_id * 32);
        src.sample(v1, src_id, slice_id, inv_size);
        dst.store(v1, dst_id, mip_id, slice_id);

        src_id = uint2(x * 2, y * 2 + 32) + uint2(group_id * 64);
        dst_id = uint2(x, y + 16) + uint2(group_id * 32);
        src.sample(v2, src_id, slice_id, inv_size);
        dst.store(v2, dst_id, mip_id, slice_id);

        src_id = uint2(x * 2 + 32, y * 2 + 32) + uint2(group_id * 64);
        dst_id = uint2(x + 16, y + 16) + uint2(group_id * 32);
        src.sample(v3, src_id, slice_id, inv_size);
        dst.store(v3, dst_id, mip_id, slice_id);

        if (mip_count <= 1) {
            return;
        }
    }

    // Mip 1: reduce 32x32 -> 16x16.
    //
    // Note: Mip 1 is able to take the reduced samples from Mip 0 which are
    // already in registers and perform the reduction using wave intrinsics.
    //
    // Note: The pattern of `group_index % 4 == 0` is make sure only one thread
    // in a quad does the write.
    //
    // Note: This is also the first time we write to groupshared memory.
    {
        mip_id = 1;

        reduce_quad_wave(v0, v0);
        reduce_quad_wave(v1, v1);
        reduce_quad_wave(v2, v2);
        reduce_quad_wave(v3, v3);

        if ((group_index % 4) == 0) {
            dst_id = uint2(x / 2, y / 2) + uint2(group_id * 16);
            tmp_id = uint2(x / 2, y / 2);
            dst.store(v0, dst_id, mip_id, slice_id);
            tmp.store(v0, tmp_id);

            dst_id = uint2(x / 2 + 8, y / 2) + uint2(group_id * 16);
            tmp_id = uint2(x / 2 + 8, y / 2);
            dst.store(v1, dst_id, mip_id, slice_id);
            tmp.store(v1, tmp_id);

            dst_id = uint2(x / 2, y / 2 + 8) + uint2(group_id * 16);
            tmp_id = uint2(x / 2, y / 2 + 8);
            dst.store(v2, dst_id, mip_id, slice_id);
            tmp.store(v2, tmp_id);

            dst_id = uint2(x / 2 + 8, y / 2 + 8) + uint2(group_id * 16);
            tmp_id = uint2(x / 2 + 8, y / 2 + 8);
            dst.store(v3, dst_id, mip_id, slice_id);
            tmp.store(v3, tmp_id);
        }

        if (mip_count <= 2) {
            return;
        }

        GroupMemoryBarrierWithGroupSync();
    }

    // Mip 2: reduce 16x16 -> 8x8.
    //
    // Note: A similar pattern to Mip 1 is used here.
    {
        mip_id = 2;

        if (group_index < 16 * 16) {
            tmp_id = uint2(x, y);
            tmp.load(v0, tmp_id);
            reduce_quad_wave(v0, v0);

            if (group_index % 4 == 0) {
                dst_id = uint2(x / 2, y / 2) + uint2(group_id * 8);
                tmp_id = uint2(x + (y / 2) % 2, y);
                dst.store(v0, dst_id, mip_id, slice_id);
                tmp.store(v0, tmp_id);
            }
        }

        if (mip_count <= 3) {
            return;
        }

        GroupMemoryBarrierWithGroupSync();
    }

    // Mip 3: reduce 8x8 -> 4x4.
    //
    // Note: The mips are getting so small that we now have less work than
    // threads (64 vs 256), so most threads will be inactive from here on.
    {
        mip_id = 3;

        if (group_index < 8 * 8) {
            tmp_id = uint2(x * 2 + y % 2, y * 2);
            tmp.load(v0, tmp_id);
            reduce_quad_wave(v0, v0);

            if (group_index % 4 == 0) {
                dst_id = uint2(x / 2, y / 2) + uint2(group_id * 4);
                tmp_id = uint2(x * 2 + y / 2, y * 2);
                dst.store(v0, dst_id, mip_id, slice_id);
                tmp.store(v0, tmp_id);
            }
        }

        if (mip_count <= 4) {
            return;
        }

        GroupMemoryBarrierWithGroupSync();
    }

    // Mip 4: reduce 4x4 -> 2x2.
    //
    // Note: A similar pattern to Mip 3 is used here.
    {
        mip_id = 4;

        if (group_index < 4 * 4) {
            tmp_id = uint2(x * 4 + y, y * 4);
            tmp.load(v0, tmp_id);
            reduce_quad_wave(v0, v0);

            if (group_index % 4 == 0) {
                dst_id = uint2(x / 2, y / 2) + uint2(group_id * 2);
                tmp_id = uint2(x / 2 + y, 0);
                dst.store(v0, dst_id, mip_id, slice_id);
                tmp.store(v0, tmp_id);
            }
        }

        if (mip_count <= 5) {
            return;
        }

        GroupMemoryBarrierWithGroupSync();
    }

    // Mip 5: reduce 2x2 -> 1x1.
    //
    // Note: This level is special as it is right before we switch to a single
    // threadgroup mode for the smallest mip levels. This switch happens at the
    // global level, meaning all threadgroups must synchronize. This is done
    // with the `globallycoherent` specifier on the atomic counters and the
    // "mid" texture.
    {
        mip_id = 5;

        if (group_index < 2 * 2) {
            tmp_id = uint2(group_index, 0);
            tmp.load(v0, tmp_id);
            reduce_quad_wave(v0, v0);

            if (group_index % 4 == 0) {
                mid_id = uint2(group_id);
                mid.store(v0, mid_id, slice_id);
            }
        }

        if (mip_count <= 6) {
            return;
        }
    }

    // This is the point where most threadgroups will exit. The last threadgroup
    // will continue on.
    {
        if (group_index == 0) {
            InterlockedAdd(atomics[0].counter[slice_id], 1, gs_counter);
        }

        GroupMemoryBarrierWithGroupSync();

        if (gs_counter != (threadgroup_count - 1)) {
            return;
        }
    }

    // The final threadgroup also resets the atomic counter for the next
    // dispatch. This avoids having to do a separate dispatch to reset the
    // atomic counters.
    atomics[0].counter[slice_id] = 0;

    // Mip 6: reduce 64x64 -> 32x32.
    //
    // Note: This level reads from the "mid" texture, which was written to by
    // Mip 5. Similar to Mip 0, this level reduces four quads, totaling 16
    // texels, except it does it without a bilinear fetch.
    //
    // Note: This level does not need a barrier as the values are already
    // synchronized across all threads/lanes.
    {
        mip_id = 6;

        mid_id = uint2(x * 4 + 0, y * 4 + 0);
        dst_id = uint2(x * 2 + 0, y * 2 + 0);
        mid.load_and_reduce(v0, mid_id, slice_id);
        dst.store(v0, dst_id, mip_id, slice_id);

        mid_id = uint2(x * 4 + 2, y * 4 + 0);
        dst_id = uint2(x * 2 + 1, y * 2 + 0);
        mid.load_and_reduce(v1, mid_id, slice_id);
        dst.store(v1, dst_id, mip_id, slice_id);

        mid_id = uint2(x * 4 + 0, y * 4 + 2);
        dst_id = uint2(x * 2 + 0, y * 2 + 1);
        mid.load_and_reduce(v2, mid_id, slice_id);
        dst.store(v2, dst_id, mip_id, slice_id);

        mid_id = uint2(x * 4 + 2, y * 4 + 2);
        dst_id = uint2(x * 2 + 1, y * 2 + 1);
        mid.load_and_reduce(v3, mid_id, slice_id);
        dst.store(v3, dst_id, mip_id, slice_id);

        if (mip_count <= 7) {
            return;
        }

        // No barrier required here.
    }

    // Mip 7: reduce 32x32 -> 16x16.
    //
    // Note: A similar pattern to Mip 1 is used here. The values are already in
    // registers so we can just reduce them directly.
    {
        mip_id = 7;

        reduce(v0, v0, v1, v2, v3);

        dst_id = uint2(x, y);
        tmp_id = uint2(x, y);
        dst.store(v0, dst_id, mip_id, slice_id);
        tmp.store(v0, tmp_id);

        if (mip_count <= 8) {
            return;
        }

        GroupMemoryBarrierWithGroupSync();
    }

    // Mip 8: reduce 16x16 -> 8x8.
    //
    // Note: Mip 8, 9, 10, and 11 are all very similar.
    {
        mip_id = 8;

        if (group_index < 16 * 16) {
            tmp_id = uint2(x, y);
            tmp.load(v0, tmp_id);
            reduce_quad_wave(v0, v0);

            if (group_index % 4 == 0) {
                dst_id = uint2(x / 2, y / 2);
                tmp_id = uint2(x + (y / 2) % 2, y);
                dst.store(v0, dst_id, mip_id, slice_id);
                tmp.store(v0, tmp_id);
            }
        }

        if (mip_count <= 9) {
            return;
        }

        GroupMemoryBarrierWithGroupSync();
    }

    // Mip 9: reduce 8x8 -> 4x4.
    {
        mip_id = 9;

        if (group_index < 8 * 8) {
            tmp_id = uint2(x * 2 + y % 2, y * 2);
            tmp.load(v0, tmp_id);
            reduce_quad_wave(v0, v0);

            if (group_index % 4 == 0) {
                dst_id = uint2(x / 2, y / 2);
                tmp_id = uint2(x * 2 + y / 2, y * 2);
                dst.store(v0, dst_id, mip_id, slice_id);
                tmp.store(v0, tmp_id);
            }
        }

        if (mip_count <= 10) {
            return;
        }

        GroupMemoryBarrierWithGroupSync();
    }

    // Mip 10: reduce 4x4 -> 2x2.
    {
        mip_id = 10;

        if (group_index < 4 * 4) {
            tmp_id = uint2(x * 4 + y, y * 4);
            tmp.load(v0, tmp_id);
            reduce_quad_wave(v0, v0);

            if (group_index % 4 == 0) {
                dst_id = uint2(x / 2, y / 2);
                tmp_id = uint2(x / 2 + y, 0);
                dst.store(v0, dst_id, mip_id, slice_id);
                tmp.store(v0, tmp_id);
            }
        }

        if (mip_count <= 11) {
            return;
        }

        GroupMemoryBarrierWithGroupSync();
    }

    // Mip 11: reduce 2x2 -> 1x1.
    {
        mip_id = 11;

        if (group_index < 2 * 2) {
            tmp_id = uint2(group_index, 0);
            tmp.load(v0, tmp_id);
            reduce_quad_wave(v0, v0);

            if (group_index % 4 == 0) {
                dst_id = uint2(0, 0);
                dst.store(v0, dst_id, mip_id, slice_id);
            }
        }
    }
}
