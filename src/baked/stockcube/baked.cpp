#include "baked.hpp"

namespace fb::baked::stockcube {

#define texture_data(rp, sp, off, sz)                                                   \
    TextureData {                                                                       \
        .row_pitch = rp, .slice_pitch = sp, .data = transmuted_span<std::byte>(off, sz) \
    }

auto Assets::load() -> void {
    // hash: 7f09e67f65e57037f74b249ecf51e6bd
    ZoneScoped;
    _data = read_whole_file("fb_stockcube_assets.bin");
    FB_ASSERT(_data.size() == 134371904);
}

auto Assets::farm_field_hdr_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(32768, 33554432,         0, 33554432); // hash: 99b1db0825065596911e1fbe8a209f04, width: 2048, height: 1024
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
        .width = 2048,
        .height = 1024,
        .mip_count = 1,
        .datas = datas,
    };
}

auto Assets::winter_evening_hdr_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(32768, 33554432,  33554432, 33554432); // hash: 3a756371dff470416ac497492fe79829, width: 2048, height: 1024
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
        .width = 2048,
        .height = 1024,
        .mip_count = 1,
        .datas = datas,
    };
}

auto Assets::shanghai_bund_hdr_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(32768, 33554432,  67108864, 33554432); // hash: 61da5f094312e27801f5f466a719d765, width: 2048, height: 1024
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
        .width = 2048,
        .height = 1024,
        .mip_count = 1,
        .datas = datas,
    };
}

auto Assets::industrial_sunset_02_puresky_hdr_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(32768, 33554432, 100663296, 33554432); // hash: 40f641d2aaba7e8800dcf4e8cc0e0274, width: 2048, height: 1024
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
        .width = 2048,
        .height = 1024,
        .mip_count = 1,
        .datas = datas,
    };
}

auto Assets::skybox_mesh() const -> Mesh {
    // vertex_count: 24
    // face_count: 12
    return Mesh {
        // hash: 952f25941581c525b7890e0284fd3b52
        .vertices = transmuted_span<Vertex>(134217728, 24),
        // hash: 0bbe6ec9d4b61b792981857b935d2c96
        .indices = transmuted_span<Index>(134218880, 36),
    };
}

auto Assets::sphere_mesh() const -> Mesh {
    // vertex_count: 2145
    // face_count: 4160
    return Mesh {
        // hash: bdea0aeb8805424705ca957338103a57
        .vertices = transmuted_span<Vertex>(134219024, 2145),
        // hash: b69e987626eec7de0864668ffde28fbe
        .indices = transmuted_span<Index>(134321984, 12480),
    };
}

auto Shaders::load() -> void {
    // hash: 0b92efec90d0e00e081d51dd4262d3c6
    ZoneScoped;
    _data = read_whole_file("fb_stockcube_shaders.bin");
    FB_ASSERT(_data.size() == 65964);
}

// shader_hash: d4277363486ca3578c07a22a4024680b
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 77
// float_instruction_count: 32
// uint_instruction_count: 4
// texture_normal_instructions: 1
// texture_store_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
; shader debug name: d4277363486ca3578c07a22a4024680b.pdb
; shader hash: d4277363486ca3578c07a22a4024680b
;
; Pipeline Runtime Information: 
;
; Compute Shader
; NumThreads=(8,8,1)
;
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint rect_texture;                        ; Offset:    4
;           uint rect_sampler;                        ; Offset:    8
;           uint cube_texture;                        ; Offset:   12
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    16
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32 }

define void @cs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 16 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.threadId.i32(i32 93, i32 0)  ; ThreadId(component)
  %4 = call i32 @dx.op.threadId.i32(i32 93, i32 1)  ; ThreadId(component)
  %5 = call i32 @dx.op.threadId.i32(i32 93, i32 2)  ; ThreadId(component)
  %6 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %7 = extractvalue %dx.types.CBufRet.i32 %6, 0
  %8 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %7, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %9 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %8, %dx.types.ResourceProperties { i32 13, i32 996 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %10 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %9, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %11 = extractvalue %dx.types.CBufRet.i32 %10, 0
  %12 = extractvalue %dx.types.CBufRet.i32 %10, 1
  %13 = extractvalue %dx.types.CBufRet.i32 %6, 1
  %14 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %13, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %15 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %14, %dx.types.ResourceProperties { i32 2, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2D<4xF32>
  %16 = extractvalue %dx.types.CBufRet.i32 %6, 2
  %17 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %16, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %18 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %17, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %19 = extractvalue %dx.types.CBufRet.i32 %6, 3
  %20 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %19, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %21 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %20, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %22 = uitofp i32 %3 to float
  %23 = fadd fast float %22, 5.000000e-01
  %24 = uitofp i32 %11 to float
  %25 = fdiv fast float %23, %24
  %26 = fmul fast float %25, 2.000000e+00
  %27 = fadd fast float %26, -1.000000e+00
  %28 = uitofp i32 %4 to float
  %29 = fadd fast float %28, 5.000000e-01
  %30 = uitofp i32 %12 to float
  %31 = fdiv fast float %29, %30
  %32 = fmul fast float %31, 2.000000e+00
  %33 = fadd fast float %32, -1.000000e+00
  %34 = fsub fast float -0.000000e+00, %33
  switch i32 %5, label %43 [
    i32 0, label %35
    i32 1, label %37
    i32 2, label %38
    i32 3, label %39
    i32 4, label %40
    i32 5, label %41
  ]

; <label>:35                                      ; preds = %0
  %36 = fsub fast float -0.000000e+00, %27
  br label %43

; <label>:37                                      ; preds = %0
  br label %43

; <label>:38                                      ; preds = %0
  br label %43

; <label>:39                                      ; preds = %0
  br label %43

; <label>:40                                      ; preds = %0
  br label %43

; <label>:41                                      ; preds = %0
  %42 = fsub fast float -0.000000e+00, %27
  br label %43

; <label>:43                                      ; preds = %41, %40, %39, %38, %37, %35, %0
  %44 = phi float [ undef, %0 ], [ %42, %41 ], [ %27, %40 ], [ %27, %39 ], [ %27, %38 ], [ -1.000000e+00, %37 ], [ 1.000000e+00, %35 ]
  %45 = phi float [ undef, %0 ], [ %34, %41 ], [ %34, %40 ], [ -1.000000e+00, %39 ], [ 1.000000e+00, %38 ], [ %34, %37 ], [ %34, %35 ]
  %46 = phi float [ undef, %0 ], [ -1.000000e+00, %41 ], [ 1.000000e+00, %40 ], [ %34, %39 ], [ %33, %38 ], [ %27, %37 ], [ %36, %35 ]
  %47 = call float @dx.op.dot3.f32(i32 55, float %44, float %45, float %46, float %44, float %45, float %46)  ; Dot3(ax,ay,az,bx,by,bz)
  %48 = call float @dx.op.unary.f32(i32 25, float %47)  ; Rsqrt(value)
  %49 = fmul fast float %48, %44
  %50 = fmul fast float %48, %45
  %51 = fmul fast float %48, %46
  %52 = fdiv fast float %51, %49
  %53 = call float @dx.op.unary.f32(i32 17, float %52)  ; Atan(value)
  %54 = fadd fast float %53, 0x400921FB60000000
  %55 = fadd fast float %53, 0xC00921FB60000000
  %56 = fcmp fast olt float %49, 0.000000e+00
  %57 = fcmp fast oeq float %49, 0.000000e+00
  %58 = fcmp fast oge float %51, 0.000000e+00
  %59 = fcmp fast olt float %51, 0.000000e+00
  %60 = and i1 %56, %58
  %61 = select i1 %60, float %54, float %53
  %62 = and i1 %56, %59
  %63 = select i1 %62, float %55, float %61
  %64 = and i1 %57, %59
  %65 = and i1 %57, %58
  %66 = fmul fast float %63, 0x3FC45F3060000000
  %67 = fadd fast float %66, 5.000000e-01
  %68 = select i1 %64, float 2.500000e-01, float %67
  %69 = select i1 %65, float 7.500000e-01, float %68
  %70 = call float @dx.op.unary.f32(i32 15, float %50)  ; Acos(value)
  %71 = fmul fast float %70, 0x3FD45F3060000000
  %72 = call %dx.types.ResRet.f32 @dx.op.sample.f32(i32 60, %dx.types.Handle %15, %dx.types.Handle %18, float %69, float %71, float undef, float undef, i32 0, i32 0, i32 undef, float undef)  ; Sample(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,clamp)
  %73 = extractvalue %dx.types.ResRet.f32 %72, 0
  %74 = extractvalue %dx.types.ResRet.f32 %72, 1
  %75 = extractvalue %dx.types.ResRet.f32 %72, 2
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %21, i32 %3, i32 %4, i32 %5, float %73, float %74, float %75, float 1.000000e+00, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.threadId.i32(i32, i32) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sample.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #1

; Function Attrs: nounwind
declare void @dx.op.textureStore.f32(i32, %dx.types.Handle, i32, i32, i32, float, float, float, float, i8) #2

; Function Attrs: nounwind readnone
declare float @dx.op.dot3.f32(i32, float, float, float, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind readonly }
attributes #2 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.resources = !{!4}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 7}
!2 = !{i32 1, i32 8}
!3 = !{!"cs", i32 6, i32 7}
!4 = !{null, null, !5, null}
!5 = !{!6}
!6 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 16, null}
!7 = !{void ()* @cs, !"cs", null, !4, !8}
!8 = !{i32 0, i64 11819548928, i32 4, !9}
!9 = !{i32 8, i32 8, i32 1}
*/
auto Shaders::cfr_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(0, 4268);
}

// shader_hash: d4e7aedfe51d9638b41f5a5ab2b0dc56
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 153
// dynamic_flow_control_count: 3
// float_instruction_count: 95
// int_instruction_count: 3
// uint_instruction_count: 23
// texture_store_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
; shader debug name: d4e7aedfe51d9638b41f5a5ab2b0dc56.pdb
; shader hash: d4e7aedfe51d9638b41f5a5ab2b0dc56
;
; Pipeline Runtime Information: 
;
; Compute Shader
; NumThreads=(8,8,1)
;
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint lut_texture;                         ; Offset:    4
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:     8
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32 }

define void @cs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 8 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.threadId.i32(i32 93, i32 0)  ; ThreadId(component)
  %4 = call i32 @dx.op.threadId.i32(i32 93, i32 1)  ; ThreadId(component)
  %5 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.i32 %5, 0
  %7 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %6, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %8 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %7, %dx.types.ResourceProperties { i32 13, i32 980 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %9 = extractvalue %dx.types.CBufRet.i32 %5, 1
  %10 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %9, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %11 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %10, %dx.types.ResourceProperties { i32 4098, i32 521 })  ; AnnotateHandle(res,props)  resource: RWTexture2D<2xF32>
  %12 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %8, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %13 = extractvalue %dx.types.CBufRet.i32 %12, 0
  %14 = extractvalue %dx.types.CBufRet.i32 %12, 1
  %15 = extractvalue %dx.types.CBufRet.i32 %12, 2
  %16 = uitofp i32 %13 to float
  %17 = uitofp i32 %14 to float
  %18 = uitofp i32 %3 to float
  %19 = uitofp i32 %4 to float
  %20 = fadd fast float %18, 5.000000e-01
  %21 = fadd fast float %19, 5.000000e-01
  %22 = fdiv fast float %20, %16
  %23 = fdiv fast float %21, %17
  %24 = fmul fast float %22, %22
  %25 = fsub fast float 1.000000e+00, %24
  %26 = call float @dx.op.unary.f32(i32 24, float %25)  ; Sqrt(value)
  %27 = fmul fast float %23, %23
  %28 = fmul fast float %27, %27
  %29 = icmp eq i32 %15, 0
  br i1 %29, label %31, label %30

; <label>:30                                      ; preds = %0
  br label %40

; <label>:31                                      ; preds = %0
  %32 = uitofp i32 %15 to float
  br label %34

; <label>:33                                      ; preds = %147
  br label %34

; <label>:34                                      ; preds = %33, %31
  %35 = phi float [ %32, %31 ], [ %68, %33 ]
  %36 = phi float [ 0.000000e+00, %31 ], [ %148, %33 ]
  %37 = phi float [ 0.000000e+00, %31 ], [ %149, %33 ]
  %38 = fdiv fast float %36, %35
  %39 = fdiv fast float %37, %35
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %11, i32 %3, i32 %4, i32 undef, float %38, float %39, float %38, float %38, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  ret void

; <label>:40                                      ; preds = %147, %30
  %41 = phi float [ %148, %147 ], [ 0.000000e+00, %30 ]
  %42 = phi float [ %149, %147 ], [ 0.000000e+00, %30 ]
  %43 = phi i32 [ %150, %147 ], [ 0, %30 ]
  %44 = shl i32 %43, 16
  %45 = lshr i32 %43, 16
  %46 = or i32 %44, %45
  %47 = shl i32 %46, 1
  %48 = and i32 %47, -1431655766
  %49 = lshr i32 %46, 1
  %50 = and i32 %49, 1431655765
  %51 = or i32 %48, %50
  %52 = shl i32 %51, 2
  %53 = and i32 %52, -858993460
  %54 = lshr i32 %51, 2
  %55 = and i32 %54, 858993459
  %56 = or i32 %53, %55
  %57 = shl i32 %56, 4
  %58 = and i32 %57, -252645136
  %59 = lshr i32 %56, 4
  %60 = and i32 %59, 252645135
  %61 = or i32 %58, %60
  %62 = shl i32 %61, 8
  %63 = and i32 %62, -16711936
  %64 = lshr i32 %61, 8
  %65 = and i32 %64, 16711935
  %66 = or i32 %63, %65
  %67 = uitofp i32 %43 to float
  %68 = uitofp i32 %15 to float
  %69 = fdiv fast float %67, %68
  %70 = uitofp i32 %66 to float
  %71 = fmul fast float %70, 0x3DF0000000000000
  %72 = fmul fast float %27, %26
  %73 = call float @dx.op.dot3.f32(i32 55, float %72, float 0.000000e+00, float %22, float %72, float 0.000000e+00, float %22)  ; Dot3(ax,ay,az,bx,by,bz)
  %74 = call float @dx.op.unary.f32(i32 25, float %73)  ; Rsqrt(value)
  %75 = fmul fast float %74, %72
  %76 = fmul fast float %74, %22
  %77 = fmul fast float %69, 0x401921FB60000000
  %78 = fsub fast float 1.000000e+00, %71
  %79 = fadd fast float %76, 1.000000e+00
  %80 = fmul fast float %78, %79
  %81 = fsub fast float %80, %76
  %82 = fmul fast float %81, %81
  %83 = fsub fast float 1.000000e+00, %82
  %84 = call float @dx.op.binary.f32(i32 35, float %83, float 0.000000e+00)  ; FMax(a,b)
  %85 = call float @dx.op.binary.f32(i32 36, float %84, float 1.000000e+00)  ; FMin(a,b)
  %86 = call float @dx.op.unary.f32(i32 24, float %85)  ; Sqrt(value)
  %87 = call float @dx.op.unary.f32(i32 12, float %77)  ; Cos(value)
  %88 = fmul fast float %87, %86
  %89 = call float @dx.op.unary.f32(i32 13, float %77)  ; Sin(value)
  %90 = fadd fast float %88, %75
  %91 = fmul fast float %90, %27
  %92 = fmul fast float %86, %27
  %93 = fmul fast float %92, %89
  %94 = call float @dx.op.binary.f32(i32 35, float 0.000000e+00, float %80)  ; FMax(a,b)
  %95 = call float @dx.op.dot3.f32(i32 55, float %91, float %93, float %94, float %91, float %93, float %94)  ; Dot3(ax,ay,az,bx,by,bz)
  %96 = call float @dx.op.unary.f32(i32 25, float %95)  ; Rsqrt(value)
  %97 = fmul fast float %96, %91
  %98 = fmul fast float %96, %93
  %99 = fmul fast float %96, %94
  %100 = fsub fast float -0.000000e+00, %26
  %101 = fsub fast float -0.000000e+00, %22
  %102 = call float @dx.op.dot3.f32(i32 55, float %100, float -0.000000e+00, float %101, float %97, float %98, float %99)  ; Dot3(ax,ay,az,bx,by,bz)
  %103 = fmul fast float %102, 2.000000e+00
  %104 = fmul fast float %103, %97
  %105 = fmul fast float %103, %99
  %106 = fsub fast float %100, %104
  %107 = fmul fast float %98, %103
  %108 = fsub fast float -0.000000e+00, %107
  %109 = fsub fast float %101, %105
  %110 = call float @dx.op.dot3.f32(i32 55, float 0.000000e+00, float 0.000000e+00, float 1.000000e+00, float %106, float %108, float %109)  ; Dot3(ax,ay,az,bx,by,bz)
  %111 = call float @dx.op.binary.f32(i32 35, float %110, float 0x3EE4F8B580000000)  ; FMax(a,b)
  %112 = call float @dx.op.binary.f32(i32 36, float %111, float 1.000000e+00)  ; FMin(a,b)
  %113 = fcmp fast ogt float %112, 0.000000e+00
  br i1 %113, label %114, label %147

; <label>:114                                     ; preds = %40
  %115 = call float @dx.op.dot3.f32(i32 55, float %26, float 0.000000e+00, float %22, float %97, float %98, float %99)  ; Dot3(ax,ay,az,bx,by,bz)
  %116 = call float @dx.op.binary.f32(i32 35, float %115, float 0x3EE4F8B580000000)  ; FMax(a,b)
  %117 = call float @dx.op.binary.f32(i32 36, float %116, float 1.000000e+00)  ; FMin(a,b)
  %118 = fsub fast float 1.000000e+00, %117
  %119 = call float @dx.op.unary.f32(i32 23, float %118)  ; Log(value)
  %120 = fmul fast float %119, 5.000000e+00
  %121 = call float @dx.op.unary.f32(i32 21, float %120)  ; Exp(value)
  %122 = fmul fast float %121, 0x3FEEB851E0000000
  %123 = fadd fast float %122, 0x3FA47AE140000000
  %124 = fmul fast float %28, %25
  %125 = fadd fast float %124, %24
  %126 = fdiv fast float %125, %24
  %127 = call float @dx.op.unary.f32(i32 24, float %126)  ; Sqrt(value)
  %128 = fadd fast float %127, 1.000000e+00
  %129 = fdiv fast float 2.000000e+00, %128
  %130 = fmul fast float %112, %112
  %131 = fsub fast float 1.000000e+00, %130
  %132 = fmul fast float %131, %28
  %133 = fadd fast float %132, %130
  %134 = fdiv fast float %133, %130
  %135 = call float @dx.op.unary.f32(i32 24, float %134)  ; Sqrt(value)
  %136 = fadd fast float %135, 1.000000e+00
  %137 = fdiv fast float 2.000000e+00, %136
  %138 = fadd fast float %137, %129
  %139 = fmul fast float %137, %129
  %140 = fsub fast float %138, %139
  %141 = fdiv fast float %137, %140
  %142 = fsub fast float 0x3FEEB851E0000000, %122
  %143 = fmul fast float %141, %142
  %144 = fadd fast float %143, %41
  %145 = fmul fast float %141, %123
  %146 = fadd fast float %145, %42
  br label %147

; <label>:147                                     ; preds = %114, %40
  %148 = phi float [ %144, %114 ], [ %41, %40 ]
  %149 = phi float [ %146, %114 ], [ %42, %40 ]
  %150 = add nuw i32 %43, 1
  %151 = icmp eq i32 %150, %15
  br i1 %151, label %33, label %40
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.threadId.i32(i32, i32) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readnone
declare float @dx.op.dot3.f32(i32, float, float, float, float, float, float) #0

; Function Attrs: nounwind readnone
declare float @dx.op.binary.f32(i32, float, float) #0

; Function Attrs: nounwind
declare void @dx.op.textureStore.f32(i32, %dx.types.Handle, i32, i32, i32, float, float, float, float, i8) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind readonly }
attributes #2 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.resources = !{!4}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 7}
!2 = !{i32 1, i32 8}
!3 = !{!"cs", i32 6, i32 7}
!4 = !{null, null, !5, null}
!5 = !{!6}
!6 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 8, null}
!7 = !{void ()* @cs, !"cs", null, !4, !8}
!8 = !{i32 0, i64 9672065280, i32 4, !9}
!9 = !{i32 8, i32 8, i32 1}
*/
auto Shaders::lut_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(4268, 4536);
}

// shader_hash: a6d8db82a47f080c40fc68bee34f7561
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 235
// dynamic_flow_control_count: 4
// float_instruction_count: 141
// int_instruction_count: 6
// uint_instruction_count: 23
// texture_normal_instructions: 1
// texture_load_instructions: 1
// texture_store_instructions: 2
/* disassembly:
;
; Note: shader requires additional functionality:
;       Typed UAV Load Additional Formats
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
; shader debug name: a6d8db82a47f080c40fc68bee34f7561.pdb
; shader hash: a6d8db82a47f080c40fc68bee34f7561
;
; Pipeline Runtime Information: 
;
; Compute Shader
; NumThreads=(8,8,1)
;
;
; Buffer Definitions:
;
; cbuffer g_acc_bindings
; {
;
;   struct g_acc_bindings
;   {
;
;       struct struct.AccBindings
;       {
;
;           uint dispatch_id;                         ; Offset:    0
;           uint constants;                           ; Offset:    4
;           uint cube_texture;                        ; Offset:    8
;           uint cube_sampler;                        ; Offset:   12
;           uint irr_texture;                         ; Offset:   16
;       
;       } g_acc_bindings;                             ; Offset:    0
;
;   
;   } g_acc_bindings;                                 ; Offset:    0 Size:    20
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_acc_bindings                    cbuffer      NA          NA     CB0            cb0     1
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_acc_bindings = type { %struct.AccBindings }
%struct.AccBindings = type { i32, i32, i32, i32, i32 }

define void @acc_cs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 20 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.threadId.i32(i32 93, i32 0)  ; ThreadId(component)
  %4 = call i32 @dx.op.threadId.i32(i32 93, i32 1)  ; ThreadId(component)
  %5 = call i32 @dx.op.threadId.i32(i32 93, i32 2)  ; ThreadId(component)
  %6 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %7 = extractvalue %dx.types.CBufRet.i32 %6, 1
  %8 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %7, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %9 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %8, %dx.types.ResourceProperties { i32 13, i32 964 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %10 = extractvalue %dx.types.CBufRet.i32 %6, 2
  %11 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %10, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %12 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %11, %dx.types.ResourceProperties { i32 5, i32 777 })  ; AnnotateHandle(res,props)  resource: TextureCube<3xF32>
  %13 = extractvalue %dx.types.CBufRet.i32 %6, 3
  %14 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %13, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %15 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %14, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %16 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %17 = extractvalue %dx.types.CBufRet.i32 %16, 0
  %18 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %17, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %19 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %18, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %20 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %9, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %21 = extractvalue %dx.types.CBufRet.i32 %20, 0
  %22 = extractvalue %dx.types.CBufRet.i32 %20, 1
  %23 = extractvalue %dx.types.CBufRet.i32 %20, 2
  %24 = extractvalue %dx.types.CBufRet.i32 %20, 3
  %25 = extractvalue %dx.types.CBufRet.i32 %6, 0
  %26 = uitofp i32 %3 to float
  %27 = fadd fast float %26, 5.000000e-01
  %28 = uitofp i32 %21 to float
  %29 = fdiv fast float %27, %28
  %30 = fmul fast float %29, 2.000000e+00
  %31 = fadd fast float %30, -1.000000e+00
  %32 = uitofp i32 %4 to float
  %33 = fadd fast float %32, 5.000000e-01
  %34 = uitofp i32 %22 to float
  %35 = fdiv fast float %33, %34
  %36 = fmul fast float %35, 2.000000e+00
  %37 = fadd fast float %36, -1.000000e+00
  %38 = fsub fast float -0.000000e+00, %37
  switch i32 %5, label %47 [
    i32 0, label %39
    i32 1, label %41
    i32 2, label %42
    i32 3, label %43
    i32 4, label %44
    i32 5, label %45
  ]

; <label>:39                                      ; preds = %0
  %40 = fsub fast float -0.000000e+00, %31
  br label %47

; <label>:41                                      ; preds = %0
  br label %47

; <label>:42                                      ; preds = %0
  br label %47

; <label>:43                                      ; preds = %0
  br label %47

; <label>:44                                      ; preds = %0
  br label %47

; <label>:45                                      ; preds = %0
  %46 = fsub fast float -0.000000e+00, %31
  br label %47

; <label>:47                                      ; preds = %45, %44, %43, %42, %41, %39, %0
  %48 = phi float [ undef, %0 ], [ %46, %45 ], [ %31, %44 ], [ %31, %43 ], [ %31, %42 ], [ -1.000000e+00, %41 ], [ 1.000000e+00, %39 ]
  %49 = phi float [ undef, %0 ], [ %38, %45 ], [ %38, %44 ], [ -1.000000e+00, %43 ], [ 1.000000e+00, %42 ], [ %38, %41 ], [ %38, %39 ]
  %50 = phi float [ undef, %0 ], [ -1.000000e+00, %45 ], [ 1.000000e+00, %44 ], [ %38, %43 ], [ %37, %42 ], [ %31, %41 ], [ %40, %39 ]
  %51 = call float @dx.op.dot3.f32(i32 55, float %48, float %49, float %50, float %48, float %49, float %50)  ; Dot3(ax,ay,az,bx,by,bz)
  %52 = call float @dx.op.unary.f32(i32 25, float %51)  ; Rsqrt(value)
  %53 = fmul fast float %52, %48
  %54 = fmul fast float %52, %49
  %55 = fmul fast float %52, %50
  %56 = icmp eq i32 %25, 0
  br i1 %56, label %57, label %58

; <label>:57                                      ; preds = %47
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %19, i32 %3, i32 %4, i32 %5, float 0.000000e+00, float 0.000000e+00, float 0.000000e+00, float 0.000000e+00, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  br label %58

; <label>:58                                      ; preds = %57, %47
  %59 = icmp eq i32 %24, 0
  br i1 %59, label %62, label %60

; <label>:60                                      ; preds = %58
  br label %75

; <label>:61                                      ; preds = %118
  br label %62

; <label>:62                                      ; preds = %61, %58
  %63 = phi float [ 0.000000e+00, %58 ], [ %228, %61 ]
  %64 = phi float [ 0.000000e+00, %58 ], [ %229, %61 ]
  %65 = phi float [ 0.000000e+00, %58 ], [ %230, %61 ]
  %66 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %19, i32 undef, i32 %3, i32 %4, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %67 = extractvalue %dx.types.ResRet.f32 %66, 0
  %68 = extractvalue %dx.types.ResRet.f32 %66, 1
  %69 = extractvalue %dx.types.ResRet.f32 %66, 2
  %70 = extractvalue %dx.types.ResRet.f32 %66, 3
  %71 = fadd fast float %67, %63
  %72 = fadd fast float %68, %64
  %73 = fadd fast float %69, %65
  %74 = fadd fast float %70, 1.000000e+00
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %19, i32 %3, i32 %4, i32 %5, float %71, float %72, float %73, float %74, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  ret void

; <label>:75                                      ; preds = %118, %60
  %76 = phi float [ %228, %118 ], [ 0.000000e+00, %60 ]
  %77 = phi float [ %229, %118 ], [ 0.000000e+00, %60 ]
  %78 = phi float [ %230, %118 ], [ 0.000000e+00, %60 ]
  %79 = phi i32 [ %231, %118 ], [ 0, %60 ]
  %80 = mul i32 %25, %24
  %81 = add i32 %79, %80
  %82 = shl i32 %81, 16
  %83 = lshr i32 %81, 16
  %84 = or i32 %82, %83
  %85 = shl i32 %84, 1
  %86 = and i32 %85, -1431655766
  %87 = lshr i32 %84, 1
  %88 = and i32 %87, 1431655765
  %89 = or i32 %86, %88
  %90 = shl i32 %89, 2
  %91 = and i32 %90, -858993460
  %92 = lshr i32 %89, 2
  %93 = and i32 %92, 858993459
  %94 = or i32 %91, %93
  %95 = shl i32 %94, 4
  %96 = and i32 %95, -252645136
  %97 = lshr i32 %94, 4
  %98 = and i32 %97, 252645135
  %99 = or i32 %96, %98
  %100 = shl i32 %99, 8
  %101 = and i32 %100, -16711936
  %102 = lshr i32 %99, 8
  %103 = and i32 %102, 16711935
  %104 = or i32 %101, %103
  %105 = uitofp i32 %81 to float
  %106 = uitofp i32 %23 to float
  %107 = fdiv fast float %105, %106
  %108 = uitofp i32 %104 to float
  %109 = fcmp fast olt float %55, 0xBFEFFFEB00000000
  br i1 %109, label %118, label %110

; <label>:110                                     ; preds = %75
  %111 = fsub fast float -0.000000e+00, %53
  %112 = fadd fast float %55, 1.000000e+00
  %113 = call float @dx.op.dot4.f32(i32 56, float %54, float %111, float 0.000000e+00, float %112, float %54, float %111, float 0.000000e+00, float %112)  ; Dot4(ax,ay,az,aw,bx,by,bz,bw)
  %114 = call float @dx.op.unary.f32(i32 25, float %113)  ; Rsqrt(value)
  %115 = fmul fast float %114, %54
  %116 = fmul fast float %114, %111
  %117 = fmul fast float %114, %112
  br label %118

; <label>:118                                     ; preds = %110, %75
  %119 = phi float [ %115, %110 ], [ 1.000000e+00, %75 ]
  %120 = phi float [ %116, %110 ], [ 0.000000e+00, %75 ]
  %121 = phi float [ %117, %110 ], [ 0.000000e+00, %75 ]
  %122 = fsub fast float -0.000000e+00, %119
  %123 = fsub fast float -0.000000e+00, %120
  %124 = call float @dx.op.dot3.f32(i32 55, float %119, float %120, float 0.000000e+00, float %53, float %54, float %55)  ; Dot3(ax,ay,az,bx,by,bz)
  %125 = fmul fast float %124, 2.000000e+00
  %126 = fmul fast float %125, %119
  %127 = fmul fast float %125, %120
  %128 = fmul fast float %121, %121
  %129 = call float @dx.op.dot3.f32(i32 55, float %119, float %120, float 0.000000e+00, float %119, float %120, float 0.000000e+00)  ; Dot3(ax,ay,az,bx,by,bz)
  %130 = fsub fast float %128, %129
  %131 = fmul fast float %130, %53
  %132 = fmul fast float %130, %54
  %133 = fmul fast float %130, %55
  %134 = fmul fast float %121, 2.000000e+00
  %135 = fmul fast float %120, %55
  %136 = fmul fast float %119, %55
  %137 = fmul fast float %119, %54
  %138 = fmul fast float %120, %53
  %139 = fsub fast float %137, %138
  %140 = fmul fast float %135, %134
  %141 = fmul fast float %136, %134
  %142 = fmul fast float %134, %139
  %143 = fadd fast float %126, %140
  %144 = fadd fast float %143, %131
  %145 = fsub fast float %127, %141
  %146 = fadd fast float %145, %132
  %147 = fadd fast float %133, %142
  %148 = call float @dx.op.unary.f32(i32 24, float %107)  ; Sqrt(value)
  %149 = fmul fast float %108, 0x3E1921FB60000000
  %150 = call float @dx.op.unary.f32(i32 12, float %149)  ; Cos(value)
  %151 = fmul fast float %150, %148
  %152 = call float @dx.op.unary.f32(i32 13, float %149)  ; Sin(value)
  %153 = fmul fast float %152, %148
  %154 = fsub fast float 1.000000e+00, %107
  %155 = call float @dx.op.unary.f32(i32 24, float %154)  ; Sqrt(value)
  %156 = fadd fast float %144, %151
  %157 = fadd fast float %153, %146
  %158 = fadd fast float %155, %147
  %159 = call float @dx.op.dot3.f32(i32 55, float %156, float %157, float %158, float %156, float %157, float %158)  ; Dot3(ax,ay,az,bx,by,bz)
  %160 = call float @dx.op.unary.f32(i32 25, float %159)  ; Rsqrt(value)
  %161 = fmul fast float %160, %156
  %162 = fmul fast float %160, %157
  %163 = fmul fast float %160, %158
  %164 = call float @dx.op.dot3.f32(i32 55, float 0.000000e+00, float 0.000000e+00, float 1.000000e+00, float %151, float %153, float %155)  ; Dot3(ax,ay,az,bx,by,bz)
  %165 = call float @dx.op.dot3.f32(i32 55, float 0.000000e+00, float 0.000000e+00, float 1.000000e+00, float %144, float %146, float %147)  ; Dot3(ax,ay,az,bx,by,bz)
  %166 = call float @dx.op.binary.f32(i32 35, float %164, float 0x3EE4F8B580000000)  ; FMax(a,b)
  %167 = call float @dx.op.binary.f32(i32 36, float %166, float 1.000000e+00)  ; FMin(a,b)
  %168 = call float @dx.op.binary.f32(i32 35, float %165, float 0x3EE4F8B580000000)  ; FMax(a,b)
  %169 = call float @dx.op.binary.f32(i32 36, float %168, float 1.000000e+00)  ; FMin(a,b)
  %170 = call float @dx.op.dot3.f32(i32 55, float %151, float %153, float %155, float %161, float %162, float %163)  ; Dot3(ax,ay,az,bx,by,bz)
  %171 = call float @dx.op.unary.f32(i32 7, float %170)  ; Saturate(value)
  %172 = call float @dx.op.unary.f32(i32 23, float 0.000000e+00)  ; Log(value)
  %173 = fmul fast float %172, 5.000000e+00
  %174 = call float @dx.op.unary.f32(i32 21, float %173)  ; Exp(value)
  %175 = fmul fast float %174, 0x3FEEB851E0000000
  %176 = fsub fast float 0x3FEEB851E0000000, %175
  %177 = fmul fast float %171, %171
  %178 = fmul fast float %177, 2.000000e+00
  %179 = fadd fast float %178, -5.000000e-01
  %180 = fsub fast float 1.000000e+00, %167
  %181 = call float @dx.op.unary.f32(i32 23, float %180)  ; Log(value)
  %182 = fmul fast float %181, 5.000000e+00
  %183 = call float @dx.op.unary.f32(i32 21, float %182)  ; Exp(value)
  %184 = fmul fast float %183, %179
  %185 = fadd fast float %184, 1.000000e+00
  %186 = fsub fast float 1.000000e+00, %169
  %187 = call float @dx.op.unary.f32(i32 23, float %186)  ; Log(value)
  %188 = fmul fast float %187, 5.000000e+00
  %189 = call float @dx.op.unary.f32(i32 21, float %188)  ; Exp(value)
  %190 = fmul fast float %189, %179
  %191 = fadd fast float %190, 1.000000e+00
  %192 = fmul fast float %191, %185
  %193 = fmul fast float %192, %176
  %194 = call float @dx.op.dot3.f32(i32 55, float %122, float %123, float -0.000000e+00, float %151, float %153, float %155)  ; Dot3(ax,ay,az,bx,by,bz)
  %195 = fmul fast float %194, 2.000000e+00
  %196 = fmul fast float %195, %122
  %197 = fmul fast float %195, %123
  %198 = call float @dx.op.dot3.f32(i32 55, float %122, float %123, float -0.000000e+00, float %122, float %123, float -0.000000e+00)  ; Dot3(ax,ay,az,bx,by,bz)
  %199 = fsub fast float %128, %198
  %200 = fmul fast float %199, %151
  %201 = fmul fast float %199, %153
  %202 = fmul fast float %199, %155
  %203 = fmul fast float %153, %122
  %204 = fmul fast float %151, %123
  %205 = fsub fast float %203, %204
  %206 = fmul fast float %134, %123
  %207 = fmul fast float %206, %155
  %208 = fmul fast float %134, %122
  %209 = fmul fast float %208, %155
  %210 = fmul fast float %205, %134
  %211 = fadd fast float %196, %207
  %212 = fadd fast float %211, %200
  %213 = fsub fast float %197, %209
  %214 = fadd fast float %213, %201
  %215 = fadd fast float %202, %210
  %216 = call float @dx.op.dot3.f32(i32 55, float %212, float %214, float %215, float %212, float %214, float %215)  ; Dot3(ax,ay,az,bx,by,bz)
  %217 = call float @dx.op.unary.f32(i32 25, float %216)  ; Rsqrt(value)
  %218 = fmul fast float %212, %217
  %219 = fmul fast float %214, %217
  %220 = fmul fast float %215, %217
  %221 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %12, %dx.types.Handle %15, float %218, float %219, float %220, float undef, i32 undef, i32 undef, i32 undef, float 0.000000e+00)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %222 = extractvalue %dx.types.ResRet.f32 %221, 0
  %223 = extractvalue %dx.types.ResRet.f32 %221, 1
  %224 = extractvalue %dx.types.ResRet.f32 %221, 2
  %225 = fmul fast float %193, %222
  %226 = fmul fast float %193, %223
  %227 = fmul fast float %193, %224
  %228 = fadd fast float %225, %76
  %229 = fadd fast float %226, %77
  %230 = fadd fast float %227, %78
  %231 = add nuw i32 %79, 1
  %232 = icmp eq i32 %231, %24
  br i1 %232, label %61, label %75
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.threadId.i32(i32, i32) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind
declare void @dx.op.textureStore.f32(i32, %dx.types.Handle, i32, i32, i32, float, float, float, float, i8) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32, %dx.types.Handle, i32, i32, i32, i32, i32, i32, i32) #1

; Function Attrs: nounwind readnone
declare float @dx.op.dot3.f32(i32, float, float, float, float, float, float) #0

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #1

; Function Attrs: nounwind readnone
declare float @dx.op.dot4.f32(i32, float, float, float, float, float, float, float, float) #0

; Function Attrs: nounwind readnone
declare float @dx.op.binary.f32(i32, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind readonly }
attributes #2 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.resources = !{!4}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 7}
!2 = !{i32 1, i32 8}
!3 = !{!"cs", i32 6, i32 7}
!4 = !{null, null, !5, null}
!5 = !{!6}
!6 = !{i32 0, %g_acc_bindings* undef, !"", i32 0, i32 0, i32 1, i32 20, null}
!7 = !{void ()* @acc_cs, !"acc_cs", null, !4, !8}
!8 = !{i32 0, i64 11819557120, i32 4, !9}
!9 = !{i32 8, i32 8, i32 1}
*/
auto Shaders::irr_acc_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(8804, 5588);
}

// shader_hash: cb848a5af58ac55bdef9ce8a7f43fdce
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 33
// float_instruction_count: 6
// int_instruction_count: 2
// texture_load_instructions: 1
// texture_store_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Typed UAV Load Additional Formats
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
; shader debug name: cb848a5af58ac55bdef9ce8a7f43fdce.pdb
; shader hash: cb848a5af58ac55bdef9ce8a7f43fdce
;
; Pipeline Runtime Information: 
;
; Compute Shader
; NumThreads=(8,8,1)
;
;
; Buffer Definitions:
;
; cbuffer g_div_bindings
; {
;
;   struct g_div_bindings
;   {
;
;       struct struct.DivBindings
;       {
;
;           uint dispatch_id;                         ; Offset:    0
;           uint constants;                           ; Offset:    4
;           uint acc_texture;                         ; Offset:    8
;           uint div_texture;                         ; Offset:   12
;       
;       } g_div_bindings;                             ; Offset:    0
;
;   
;   } g_div_bindings;                                 ; Offset:    0 Size:    16
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_div_bindings                    cbuffer      NA          NA     CB0            cb0     1
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_div_bindings = type { %struct.DivBindings }
%struct.DivBindings = type { i32, i32, i32, i32 }

define void @div_cs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 16 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.threadId.i32(i32 93, i32 0)  ; ThreadId(component)
  %4 = call i32 @dx.op.threadId.i32(i32 93, i32 1)  ; ThreadId(component)
  %5 = call i32 @dx.op.threadId.i32(i32 93, i32 2)  ; ThreadId(component)
  %6 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %7 = extractvalue %dx.types.CBufRet.i32 %6, 1
  %8 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %7, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %9 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %8, %dx.types.ResourceProperties { i32 13, i32 964 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %10 = extractvalue %dx.types.CBufRet.i32 %6, 2
  %11 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %10, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %12 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %11, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %13 = extractvalue %dx.types.CBufRet.i32 %6, 3
  %14 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %13, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %15 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %14, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %16 = extractvalue %dx.types.CBufRet.i32 %6, 0
  %17 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %9, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %18 = extractvalue %dx.types.CBufRet.i32 %17, 3
  %19 = add i32 %16, 1
  %20 = mul i32 %19, %18
  %21 = uitofp i32 %20 to float
  %22 = fdiv fast float 1.000000e+00, %21
  %23 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %12, i32 undef, i32 %3, i32 %4, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %24 = extractvalue %dx.types.ResRet.f32 %23, 0
  %25 = extractvalue %dx.types.ResRet.f32 %23, 1
  %26 = extractvalue %dx.types.ResRet.f32 %23, 2
  %27 = extractvalue %dx.types.ResRet.f32 %23, 3
  %28 = fmul fast float %24, %22
  %29 = fmul fast float %25, %22
  %30 = fmul fast float %26, %22
  %31 = fmul fast float %27, %22
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %15, i32 %3, i32 %4, i32 %5, float %28, float %29, float %30, float %31, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.threadId.i32(i32, i32) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32, %dx.types.Handle, i32, i32, i32, i32, i32, i32, i32) #1

; Function Attrs: nounwind
declare void @dx.op.textureStore.f32(i32, %dx.types.Handle, i32, i32, i32, float, float, float, float, i8) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind readonly }
attributes #2 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.resources = !{!4}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 7}
!2 = !{i32 1, i32 8}
!3 = !{!"cs", i32 6, i32 7}
!4 = !{null, null, !5, null}
!5 = !{!6}
!6 = !{i32 0, %g_div_bindings* undef, !"", i32 0, i32 0, i32 1, i32 16, null}
!7 = !{void ()* @div_cs, !"div_cs", null, !4, !8}
!8 = !{i32 0, i64 9672073472, i32 4, !9}
!9 = !{i32 8, i32 8, i32 1}
*/
auto Shaders::irr_div_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(14392, 3744);
}

// shader_hash: 7f0a2a859c49892b2c07de021886af9d
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 305
// dynamic_flow_control_count: 10
// float_instruction_count: 176
// int_instruction_count: 11
// uint_instruction_count: 29
// texture_normal_instructions: 2
// texture_load_instructions: 1
// texture_store_instructions: 3
/* disassembly:
;
; Note: shader requires additional functionality:
;       Typed UAV Load Additional Formats
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
; shader debug name: 7f0a2a859c49892b2c07de021886af9d.pdb
; shader hash: 7f0a2a859c49892b2c07de021886af9d
;
; Pipeline Runtime Information: 
;
; Compute Shader
; NumThreads=(8,8,1)
;
;
; Buffer Definitions:
;
; cbuffer g_acc_bindings
; {
;
;   struct g_acc_bindings
;   {
;
;       struct struct.AccBindings
;       {
;
;           uint dispatch_id;                         ; Offset:    0
;           uint constants;                           ; Offset:    4
;           uint cube_texture;                        ; Offset:    8
;           uint cube_sampler;                        ; Offset:   12
;           uint rad_texture;                         ; Offset:   16
;           uint rad_texture_mip_id;                  ; Offset:   20
;       
;       } g_acc_bindings;                             ; Offset:    0
;
;   
;   } g_acc_bindings;                                 ; Offset:    0 Size:    24
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_acc_bindings                    cbuffer      NA          NA     CB0            cb0     1
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_acc_bindings = type { %struct.AccBindings }
%struct.AccBindings = type { i32, i32, i32, i32, i32, i32 }

define void @acc_cs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 24 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.threadId.i32(i32 93, i32 0)  ; ThreadId(component)
  %4 = call i32 @dx.op.threadId.i32(i32 93, i32 1)  ; ThreadId(component)
  %5 = call i32 @dx.op.threadId.i32(i32 93, i32 2)  ; ThreadId(component)
  %6 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %7 = extractvalue %dx.types.CBufRet.i32 %6, 1
  %8 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %7, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %9 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %8, %dx.types.ResourceProperties { i32 13, i32 964 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %10 = extractvalue %dx.types.CBufRet.i32 %6, 2
  %11 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %10, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %12 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %11, %dx.types.ResourceProperties { i32 5, i32 777 })  ; AnnotateHandle(res,props)  resource: TextureCube<3xF32>
  %13 = extractvalue %dx.types.CBufRet.i32 %6, 3
  %14 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %13, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %15 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %14, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %16 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %17 = extractvalue %dx.types.CBufRet.i32 %16, 0
  %18 = extractvalue %dx.types.CBufRet.i32 %16, 1
  %19 = add i32 %18, %17
  %20 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %19, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %21 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %20, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %22 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %9, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %23 = extractvalue %dx.types.CBufRet.i32 %22, 0
  %24 = extractvalue %dx.types.CBufRet.i32 %22, 1
  %25 = extractvalue %dx.types.CBufRet.i32 %22, 3
  %26 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %9, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %27 = extractvalue %dx.types.CBufRet.i32 %26, 0
  %28 = extractvalue %dx.types.CBufRet.i32 %6, 0
  %29 = icmp uge i32 %3, %23
  %30 = icmp uge i32 %4, %24
  %31 = or i1 %29, %30
  br i1 %31, label %301, label %32

; <label>:32                                      ; preds = %0
  %33 = extractvalue %dx.types.CBufRet.i32 %22, 2
  %34 = and i32 %18, 31
  %35 = lshr i32 %23, %34
  %36 = call i32 @dx.op.binary.i32(i32 39, i32 1, i32 %35)  ; UMax(a,b)
  %37 = lshr i32 %24, %34
  %38 = call i32 @dx.op.binary.i32(i32 39, i32 1, i32 %37)  ; UMax(a,b)
  %39 = uitofp i32 %18 to float
  %40 = add i32 %33, -1
  %41 = uitofp i32 %40 to float
  %42 = fdiv fast float %39, %41
  %43 = uitofp i32 %3 to float
  %44 = fadd fast float %43, 5.000000e-01
  %45 = uitofp i32 %36 to float
  %46 = fdiv fast float %44, %45
  %47 = fmul fast float %46, 2.000000e+00
  %48 = fadd fast float %47, -1.000000e+00
  %49 = uitofp i32 %4 to float
  %50 = fadd fast float %49, 5.000000e-01
  %51 = uitofp i32 %38 to float
  %52 = fdiv fast float %50, %51
  %53 = fmul fast float %52, 2.000000e+00
  %54 = fadd fast float %53, -1.000000e+00
  %55 = fsub fast float -0.000000e+00, %54
  switch i32 %5, label %64 [
    i32 0, label %56
    i32 1, label %58
    i32 2, label %59
    i32 3, label %60
    i32 4, label %61
    i32 5, label %62
  ]

; <label>:56                                      ; preds = %32
  %57 = fsub fast float -0.000000e+00, %48
  br label %64

; <label>:58                                      ; preds = %32
  br label %64

; <label>:59                                      ; preds = %32
  br label %64

; <label>:60                                      ; preds = %32
  br label %64

; <label>:61                                      ; preds = %32
  br label %64

; <label>:62                                      ; preds = %32
  %63 = fsub fast float -0.000000e+00, %48
  br label %64

; <label>:64                                      ; preds = %62, %61, %60, %59, %58, %56, %32
  %65 = phi float [ undef, %32 ], [ %63, %62 ], [ %48, %61 ], [ %48, %60 ], [ %48, %59 ], [ -1.000000e+00, %58 ], [ 1.000000e+00, %56 ]
  %66 = phi float [ undef, %32 ], [ %55, %62 ], [ %55, %61 ], [ -1.000000e+00, %60 ], [ 1.000000e+00, %59 ], [ %55, %58 ], [ %55, %56 ]
  %67 = phi float [ undef, %32 ], [ -1.000000e+00, %62 ], [ 1.000000e+00, %61 ], [ %55, %60 ], [ %54, %59 ], [ %48, %58 ], [ %57, %56 ]
  %68 = call float @dx.op.dot3.f32(i32 55, float %65, float %66, float %67, float %65, float %66, float %67)  ; Dot3(ax,ay,az,bx,by,bz)
  %69 = call float @dx.op.unary.f32(i32 25, float %68)  ; Rsqrt(value)
  %70 = fmul fast float %69, %65
  %71 = fmul fast float %69, %66
  %72 = fmul fast float %69, %67
  %73 = icmp eq i32 %28, 0
  br i1 %73, label %74, label %75

; <label>:74                                      ; preds = %64
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %21, i32 %3, i32 %4, i32 %5, float 0.000000e+00, float 0.000000e+00, float 0.000000e+00, float 0.000000e+00, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  br label %75

; <label>:75                                      ; preds = %74, %64
  %76 = icmp eq i32 %18, 0
  br i1 %76, label %77, label %82

; <label>:77                                      ; preds = %75
  %78 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %12, %dx.types.Handle %15, float %70, float %71, float %72, float undef, i32 undef, i32 undef, i32 undef, float 0.000000e+00)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %79 = extractvalue %dx.types.ResRet.f32 %78, 0
  %80 = extractvalue %dx.types.ResRet.f32 %78, 1
  %81 = extractvalue %dx.types.ResRet.f32 %78, 2
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %21, i32 %3, i32 %4, i32 %5, float %79, float %80, float %81, float 1.000000e+00, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  br label %301

; <label>:82                                      ; preds = %75
  %83 = icmp eq i32 %27, 0
  br i1 %83, label %86, label %84

; <label>:84                                      ; preds = %82
  br label %99

; <label>:85                                      ; preds = %295
  br label %86

; <label>:86                                      ; preds = %85, %82
  %87 = phi float [ 0.000000e+00, %82 ], [ %296, %85 ]
  %88 = phi float [ 0.000000e+00, %82 ], [ %297, %85 ]
  %89 = phi float [ 0.000000e+00, %82 ], [ %298, %85 ]
  %90 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %21, i32 undef, i32 %3, i32 %4, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %91 = extractvalue %dx.types.ResRet.f32 %90, 0
  %92 = extractvalue %dx.types.ResRet.f32 %90, 1
  %93 = extractvalue %dx.types.ResRet.f32 %90, 2
  %94 = extractvalue %dx.types.ResRet.f32 %90, 3
  %95 = fadd fast float %91, %87
  %96 = fadd fast float %92, %88
  %97 = fadd fast float %93, %89
  %98 = fadd fast float %94, 1.000000e+00
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %21, i32 %3, i32 %4, i32 %5, float %95, float %96, float %97, float %98, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  br label %301

; <label>:99                                      ; preds = %295, %84
  %100 = phi float [ %296, %295 ], [ 0.000000e+00, %84 ]
  %101 = phi float [ %297, %295 ], [ 0.000000e+00, %84 ]
  %102 = phi float [ %298, %295 ], [ 0.000000e+00, %84 ]
  %103 = phi i32 [ %299, %295 ], [ 0, %84 ]
  %104 = mul i32 %28, %27
  %105 = add i32 %103, %104
  %106 = shl i32 %105, 16
  %107 = lshr i32 %105, 16
  %108 = or i32 %106, %107
  %109 = shl i32 %108, 1
  %110 = and i32 %109, -1431655766
  %111 = lshr i32 %108, 1
  %112 = and i32 %111, 1431655765
  %113 = or i32 %110, %112
  %114 = shl i32 %113, 2
  %115 = and i32 %114, -858993460
  %116 = lshr i32 %113, 2
  %117 = and i32 %116, 858993459
  %118 = or i32 %115, %117
  %119 = shl i32 %118, 4
  %120 = and i32 %119, -252645136
  %121 = lshr i32 %118, 4
  %122 = and i32 %121, 252645135
  %123 = or i32 %120, %122
  %124 = shl i32 %123, 8
  %125 = and i32 %124, -16711936
  %126 = lshr i32 %123, 8
  %127 = and i32 %126, 16711935
  %128 = or i32 %125, %127
  %129 = uitofp i32 %105 to float
  %130 = uitofp i32 %25 to float
  %131 = fdiv fast float %129, %130
  %132 = uitofp i32 %128 to float
  %133 = fmul fast float %132, 0x3DF0000000000000
  %134 = call float @dx.op.dot3.f32(i32 55, float %70, float %71, float %72, float %70, float %71, float %72)  ; Dot3(ax,ay,az,bx,by,bz)
  %135 = fcmp fast ugt float %134, 0.000000e+00
  br i1 %135, label %136, label %295

; <label>:136                                     ; preds = %99
  %137 = fcmp fast olt float %72, 0xBFEFFFEB00000000
  br i1 %137, label %146, label %138

; <label>:138                                     ; preds = %136
  %139 = fsub fast float -0.000000e+00, %70
  %140 = fadd fast float %72, 1.000000e+00
  %141 = call float @dx.op.dot4.f32(i32 56, float %71, float %139, float 0.000000e+00, float %140, float %71, float %139, float 0.000000e+00, float %140)  ; Dot4(ax,ay,az,aw,bx,by,bz,bw)
  %142 = call float @dx.op.unary.f32(i32 25, float %141)  ; Rsqrt(value)
  %143 = fmul fast float %142, %71
  %144 = fmul fast float %142, %139
  %145 = fmul fast float %142, %140
  br label %146

; <label>:146                                     ; preds = %138, %136
  %147 = phi float [ %143, %138 ], [ 1.000000e+00, %136 ]
  %148 = phi float [ %144, %138 ], [ 0.000000e+00, %136 ]
  %149 = phi float [ %145, %138 ], [ 0.000000e+00, %136 ]
  %150 = call float @dx.op.dot3.f32(i32 55, float %147, float %148, float 0.000000e+00, float %70, float %71, float %72)  ; Dot3(ax,ay,az,bx,by,bz)
  %151 = fmul fast float %150, 2.000000e+00
  %152 = fmul fast float %151, %147
  %153 = fmul fast float %151, %148
  %154 = fmul fast float %149, %149
  %155 = call float @dx.op.dot3.f32(i32 55, float %147, float %148, float 0.000000e+00, float %147, float %148, float 0.000000e+00)  ; Dot3(ax,ay,az,bx,by,bz)
  %156 = fsub fast float %154, %155
  %157 = fmul fast float %156, %70
  %158 = fmul fast float %156, %71
  %159 = fmul fast float %156, %72
  %160 = fmul fast float %149, 2.000000e+00
  %161 = fmul fast float %148, %72
  %162 = fmul fast float %147, %72
  %163 = fmul fast float %147, %71
  %164 = fmul fast float %148, %70
  %165 = fsub fast float %163, %164
  %166 = fmul fast float %161, %160
  %167 = fmul fast float %162, %160
  %168 = fmul fast float %160, %165
  %169 = fadd fast float %152, %166
  %170 = fadd fast float %169, %157
  %171 = fsub fast float %153, %167
  %172 = fadd fast float %171, %158
  %173 = fadd fast float %159, %168
  %174 = fmul fast float %42, %42
  %175 = fmul fast float %174, %174
  %176 = fcmp fast oeq float %174, 0.000000e+00
  br i1 %176, label %209, label %177

; <label>:177                                     ; preds = %146
  %178 = fmul fast float %170, %174
  %179 = fmul fast float %172, %174
  %180 = call float @dx.op.dot3.f32(i32 55, float %178, float %179, float %173, float %178, float %179, float %173)  ; Dot3(ax,ay,az,bx,by,bz)
  %181 = call float @dx.op.unary.f32(i32 25, float %180)  ; Rsqrt(value)
  %182 = fmul fast float %181, %178
  %183 = fmul fast float %181, %179
  %184 = fmul fast float %181, %173
  %185 = fmul fast float %131, 0x401921FB60000000
  %186 = fsub fast float 1.000000e+00, %133
  %187 = fadd fast float %184, 1.000000e+00
  %188 = fmul fast float %187, %186
  %189 = fsub fast float %188, %184
  %190 = fmul fast float %189, %189
  %191 = fsub fast float 1.000000e+00, %190
  %192 = call float @dx.op.binary.f32(i32 35, float %191, float 0.000000e+00)  ; FMax(a,b)
  %193 = call float @dx.op.binary.f32(i32 36, float %192, float 1.000000e+00)  ; FMin(a,b)
  %194 = call float @dx.op.unary.f32(i32 24, float %193)  ; Sqrt(value)
  %195 = call float @dx.op.unary.f32(i32 12, float %185)  ; Cos(value)
  %196 = fmul fast float %195, %194
  %197 = call float @dx.op.unary.f32(i32 13, float %185)  ; Sin(value)
  %198 = fmul fast float %197, %194
  %199 = fadd fast float %196, %182
  %200 = fadd fast float %198, %183
  %201 = fmul fast float %199, %174
  %202 = fmul fast float %200, %174
  %203 = call float @dx.op.binary.f32(i32 35, float 0.000000e+00, float %188)  ; FMax(a,b)
  %204 = call float @dx.op.dot3.f32(i32 55, float %201, float %202, float %203, float %201, float %202, float %203)  ; Dot3(ax,ay,az,bx,by,bz)
  %205 = call float @dx.op.unary.f32(i32 25, float %204)  ; Rsqrt(value)
  %206 = fmul fast float %205, %201
  %207 = fmul fast float %202, %205
  %208 = fmul fast float %205, %203
  br label %209

; <label>:209                                     ; preds = %177, %146
  %210 = phi float [ %206, %177 ], [ 0.000000e+00, %146 ]
  %211 = phi float [ %207, %177 ], [ 0.000000e+00, %146 ]
  %212 = phi float [ %208, %177 ], [ 1.000000e+00, %146 ]
  %213 = fsub fast float -0.000000e+00, %170
  %214 = fsub fast float -0.000000e+00, %172
  %215 = fsub fast float -0.000000e+00, %173
  %216 = call float @dx.op.dot3.f32(i32 55, float %213, float %214, float %215, float %210, float %211, float %212)  ; Dot3(ax,ay,az,bx,by,bz)
  %217 = fmul fast float %216, 2.000000e+00
  %218 = fmul fast float %217, %210
  %219 = fmul fast float %217, %211
  %220 = fmul fast float %217, %212
  %221 = fsub fast float %213, %218
  %222 = fsub fast float %214, %219
  %223 = fsub fast float %215, %220
  %224 = call float @dx.op.dot3.f32(i32 55, float 0.000000e+00, float 0.000000e+00, float 1.000000e+00, float %221, float %222, float %223)  ; Dot3(ax,ay,az,bx,by,bz)
  %225 = call float @dx.op.binary.f32(i32 35, float %224, float 0x3EE4F8B580000000)  ; FMax(a,b)
  %226 = call float @dx.op.binary.f32(i32 36, float %225, float 1.000000e+00)  ; FMin(a,b)
  %227 = call float @dx.op.dot3.f32(i32 55, float 0.000000e+00, float 0.000000e+00, float 1.000000e+00, float %170, float %172, float %173)  ; Dot3(ax,ay,az,bx,by,bz)
  %228 = call float @dx.op.binary.f32(i32 35, float %227, float 0x3EE4F8B580000000)  ; FMax(a,b)
  %229 = call float @dx.op.binary.f32(i32 36, float %228, float 1.000000e+00)  ; FMin(a,b)
  %230 = fmul fast float %229, %229
  %231 = fsub fast float 1.000000e+00, %230
  %232 = fmul fast float %231, %175
  %233 = fadd fast float %232, %230
  %234 = fdiv fast float %233, %230
  %235 = call float @dx.op.unary.f32(i32 24, float %234)  ; Sqrt(value)
  %236 = fadd fast float %235, 1.000000e+00
  %237 = fdiv fast float 2.000000e+00, %236
  %238 = fmul fast float %226, %226
  %239 = fsub fast float 1.000000e+00, %238
  %240 = fmul fast float %239, %175
  %241 = fadd fast float %240, %238
  %242 = fdiv fast float %241, %238
  %243 = call float @dx.op.unary.f32(i32 24, float %242)  ; Sqrt(value)
  %244 = fadd fast float %243, 1.000000e+00
  %245 = fdiv fast float 2.000000e+00, %244
  %246 = fadd fast float %245, %237
  %247 = fmul fast float %245, %237
  %248 = fsub fast float %246, %247
  %249 = fdiv fast float %245, %248
  %250 = call float @dx.op.dot3.f32(i32 55, float %249, float %249, float %249, float 0x3FCB367A00000000, float 0x3FE6E2EB20000000, float 0x3FB27BB300000000)  ; Dot3(ax,ay,az,bx,by,bz)
  %251 = fcmp fast oeq float %250, 0.000000e+00
  br i1 %251, label %295, label %252

; <label>:252                                     ; preds = %209
  %253 = fsub fast float -0.000000e+00, %147
  %254 = fsub fast float -0.000000e+00, %148
  %255 = call float @dx.op.dot3.f32(i32 55, float %253, float %254, float -0.000000e+00, float %221, float %222, float %223)  ; Dot3(ax,ay,az,bx,by,bz)
  %256 = fmul fast float %255, 2.000000e+00
  %257 = fmul fast float %256, %253
  %258 = fmul fast float %256, %254
  %259 = call float @dx.op.dot3.f32(i32 55, float %253, float %254, float -0.000000e+00, float %253, float %254, float -0.000000e+00)  ; Dot3(ax,ay,az,bx,by,bz)
  %260 = fsub fast float %154, %259
  %261 = fmul fast float %260, %221
  %262 = fmul fast float %260, %222
  %263 = fmul fast float %260, %223
  %264 = fmul fast float %222, %253
  %265 = fmul fast float %221, %254
  %266 = fsub fast float %264, %265
  %267 = fmul fast float %160, %254
  %268 = fmul fast float %267, %223
  %269 = fmul fast float %160, %253
  %270 = fmul fast float %269, %223
  %271 = fmul fast float %266, %160
  %272 = fadd fast float %257, %268
  %273 = fadd fast float %272, %261
  %274 = fsub fast float %258, %270
  %275 = fadd fast float %274, %262
  %276 = fadd fast float %263, %271
  %277 = call float @dx.op.dot3.f32(i32 55, float %273, float %275, float %276, float %273, float %275, float %276)  ; Dot3(ax,ay,az,bx,by,bz)
  %278 = call float @dx.op.unary.f32(i32 25, float %277)  ; Rsqrt(value)
  %279 = fmul fast float %273, %278
  %280 = fmul fast float %275, %278
  %281 = fmul fast float %276, %278
  %282 = call float @dx.op.dot3.f32(i32 55, float %70, float %71, float %72, float %279, float %280, float %281)  ; Dot3(ax,ay,az,bx,by,bz)
  %283 = fcmp fast ugt float %282, 0.000000e+00
  br i1 %283, label %284, label %295

; <label>:284                                     ; preds = %252
  %285 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %12, %dx.types.Handle %15, float %279, float %280, float %281, float undef, i32 undef, i32 undef, i32 undef, float 0.000000e+00)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %286 = extractvalue %dx.types.ResRet.f32 %285, 0
  %287 = extractvalue %dx.types.ResRet.f32 %285, 1
  %288 = extractvalue %dx.types.ResRet.f32 %285, 2
  %289 = fmul fast float %286, %249
  %290 = fmul fast float %287, %249
  %291 = fmul fast float %288, %249
  %292 = fadd fast float %289, %100
  %293 = fadd fast float %290, %101
  %294 = fadd fast float %291, %102
  br label %295

; <label>:295                                     ; preds = %284, %252, %209, %99
  %296 = phi float [ %292, %284 ], [ %100, %99 ], [ %100, %209 ], [ %100, %252 ]
  %297 = phi float [ %293, %284 ], [ %101, %99 ], [ %101, %209 ], [ %101, %252 ]
  %298 = phi float [ %294, %284 ], [ %102, %99 ], [ %102, %209 ], [ %102, %252 ]
  %299 = add nuw i32 %103, 1
  %300 = icmp eq i32 %299, %27
  br i1 %300, label %85, label %99

; <label>:301                                     ; preds = %86, %77, %0
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.threadId.i32(i32, i32) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind readnone
declare i32 @dx.op.binary.i32(i32, i32, i32) #0

; Function Attrs: nounwind
declare void @dx.op.textureStore.f32(i32, %dx.types.Handle, i32, i32, i32, float, float, float, float, i8) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32, %dx.types.Handle, i32, i32, i32, i32, i32, i32, i32) #1

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #1

; Function Attrs: nounwind readnone
declare float @dx.op.dot3.f32(i32, float, float, float, float, float, float) #0

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readnone
declare float @dx.op.binary.f32(i32, float, float) #0

; Function Attrs: nounwind readnone
declare float @dx.op.dot4.f32(i32, float, float, float, float, float, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind readonly }
attributes #2 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.resources = !{!4}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 7}
!2 = !{i32 1, i32 8}
!3 = !{!"cs", i32 6, i32 7}
!4 = !{null, null, !5, null}
!5 = !{!6}
!6 = !{i32 0, %g_acc_bindings* undef, !"", i32 0, i32 0, i32 1, i32 24, null}
!7 = !{void ()* @acc_cs, !"acc_cs", null, !4, !8}
!8 = !{i32 0, i64 11819557120, i32 4, !9}
!9 = !{i32 8, i32 8, i32 1}
*/
auto Shaders::rad_acc_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(18136, 6096);
}

// shader_hash: b4b57014dba716bde3d03360d4388809
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 44
// dynamic_flow_control_count: 1
// float_instruction_count: 6
// int_instruction_count: 5
// texture_load_instructions: 1
// texture_store_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Typed UAV Load Additional Formats
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
; shader debug name: b4b57014dba716bde3d03360d4388809.pdb
; shader hash: b4b57014dba716bde3d03360d4388809
;
; Pipeline Runtime Information: 
;
; Compute Shader
; NumThreads=(8,8,1)
;
;
; Buffer Definitions:
;
; cbuffer g_div_bindings
; {
;
;   struct g_div_bindings
;   {
;
;       struct struct.DivBindings
;       {
;
;           uint dispatch_id;                         ; Offset:    0
;           uint mip_id;                              ; Offset:    4
;           uint constants;                           ; Offset:    8
;           uint acc_texture;                         ; Offset:   12
;           uint div_texture;                         ; Offset:   16
;       
;       } g_div_bindings;                             ; Offset:    0
;
;   
;   } g_div_bindings;                                 ; Offset:    0 Size:    20
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_div_bindings                    cbuffer      NA          NA     CB0            cb0     1
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_div_bindings = type { %struct.DivBindings }
%struct.DivBindings = type { i32, i32, i32, i32, i32 }

define void @div_cs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 20 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.threadId.i32(i32 93, i32 0)  ; ThreadId(component)
  %4 = call i32 @dx.op.threadId.i32(i32 93, i32 1)  ; ThreadId(component)
  %5 = call i32 @dx.op.threadId.i32(i32 93, i32 2)  ; ThreadId(component)
  %6 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %7 = extractvalue %dx.types.CBufRet.i32 %6, 3
  %8 = extractvalue %dx.types.CBufRet.i32 %6, 1
  %9 = add i32 %8, %7
  %10 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %9, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %11 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %12 = extractvalue %dx.types.CBufRet.i32 %11, 0
  %13 = add i32 %8, %12
  %14 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %13, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %15 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %14, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %16 = icmp eq i32 %8, 0
  %17 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %10, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %18 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %17, i32 undef, i32 %3, i32 %4, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %19 = extractvalue %dx.types.ResRet.f32 %18, 0
  %20 = extractvalue %dx.types.ResRet.f32 %18, 1
  %21 = extractvalue %dx.types.ResRet.f32 %18, 2
  %22 = extractvalue %dx.types.ResRet.f32 %18, 3
  br i1 %16, label %38, label %23

; <label>:23                                      ; preds = %0
  %24 = extractvalue %dx.types.CBufRet.i32 %6, 2
  %25 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %24, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %26 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %25, %dx.types.ResourceProperties { i32 13, i32 964 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %27 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %26, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %28 = extractvalue %dx.types.CBufRet.i32 %27, 0
  %29 = extractvalue %dx.types.CBufRet.i32 %6, 0
  %30 = add i32 %29, 1
  %31 = mul i32 %30, %28
  %32 = uitofp i32 %31 to float
  %33 = fdiv fast float 1.000000e+00, %32
  %34 = fmul fast float %19, %33
  %35 = fmul fast float %20, %33
  %36 = fmul fast float %21, %33
  %37 = fmul fast float %22, %33
  br label %38

; <label>:38                                      ; preds = %23, %0
  %39 = phi float [ %34, %23 ], [ %19, %0 ]
  %40 = phi float [ %35, %23 ], [ %20, %0 ]
  %41 = phi float [ %36, %23 ], [ %21, %0 ]
  %42 = phi float [ %37, %23 ], [ %22, %0 ]
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %15, i32 %3, i32 %4, i32 %5, float %39, float %40, float %41, float %42, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.threadId.i32(i32, i32) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32, %dx.types.Handle, i32, i32, i32, i32, i32, i32, i32) #1

; Function Attrs: nounwind
declare void @dx.op.textureStore.f32(i32, %dx.types.Handle, i32, i32, i32, float, float, float, float, i8) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind readonly }
attributes #2 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.resources = !{!4}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 7}
!2 = !{i32 1, i32 8}
!3 = !{!"cs", i32 6, i32 7}
!4 = !{null, null, !5, null}
!5 = !{!6}
!6 = !{i32 0, %g_div_bindings* undef, !"", i32 0, i32 0, i32 1, i32 20, null}
!7 = !{void ()* @div_cs, !"div_cs", null, !4, !8}
!8 = !{i32 0, i64 9672073472, i32 4, !9}
!9 = !{i32 8, i32 8, i32 1}
*/
auto Shaders::rad_div_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(24232, 3876);
}

// shader_hash: 65cd8c2a779bb4dfda25b20192bee578
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 4
// instruction_count: 62
// float_instruction_count: 6
// texture_load_instructions: 3
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xyz         1     NONE   float   xyz 
; ATTRIBUTE                1   xy          2     NONE   float   xy  
; ATTRIBUTE                2   xyz         3     NONE   float   xyz 
;
; shader debug name: 65cd8c2a779bb4dfda25b20192bee578.pdb
; shader hash: 65cd8c2a779bb4dfda25b20192bee578
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
; ATTRIBUTE                2                 linear       
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint texture;                             ; Offset:    8
;           uint sampler;                             ; Offset:   12
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    16
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 15
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 2 depends on inputs: { 0 }
;   output 3 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;   output 6 depends on inputs: { 0 }
;   output 8 depends on inputs: { 0 }
;   output 9 depends on inputs: { 0 }
;   output 12 depends on inputs: { 0 }
;   output 13 depends on inputs: { 0 }
;   output 14 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32 }

define void @vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 16 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %5 = extractvalue %dx.types.CBufRet.i32 %4, 0
  %6 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %5, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %7 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %6, %dx.types.ResourceProperties { i32 13, i32 804 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %8 = extractvalue %dx.types.CBufRet.i32 %4, 1
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %8, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 524, i32 48 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=48>
  %11 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %12 = extractvalue %dx.types.ResRet.f32 %11, 0
  %13 = extractvalue %dx.types.ResRet.f32 %11, 1
  %14 = extractvalue %dx.types.ResRet.f32 %11, 2
  %15 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 12, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %16 = extractvalue %dx.types.ResRet.f32 %15, 0
  %17 = extractvalue %dx.types.ResRet.f32 %15, 1
  %18 = extractvalue %dx.types.ResRet.f32 %15, 2
  %19 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 24, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %20 = extractvalue %dx.types.ResRet.f32 %19, 0
  %21 = extractvalue %dx.types.ResRet.f32 %19, 1
  %22 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %23 = extractvalue %dx.types.CBufRet.f32 %22, 0
  %24 = extractvalue %dx.types.CBufRet.f32 %22, 1
  %25 = extractvalue %dx.types.CBufRet.f32 %22, 3
  %26 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %27 = extractvalue %dx.types.CBufRet.f32 %26, 0
  %28 = extractvalue %dx.types.CBufRet.f32 %26, 1
  %29 = extractvalue %dx.types.CBufRet.f32 %26, 3
  %30 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %31 = extractvalue %dx.types.CBufRet.f32 %30, 0
  %32 = extractvalue %dx.types.CBufRet.f32 %30, 1
  %33 = extractvalue %dx.types.CBufRet.f32 %30, 3
  %34 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %35 = extractvalue %dx.types.CBufRet.f32 %34, 0
  %36 = extractvalue %dx.types.CBufRet.f32 %34, 1
  %37 = extractvalue %dx.types.CBufRet.f32 %34, 3
  %38 = fmul fast float %23, %12
  %39 = call float @dx.op.tertiary.f32(i32 46, float %27, float %13, float %38)  ; FMad(a,b,c)
  %40 = call float @dx.op.tertiary.f32(i32 46, float %31, float %14, float %39)  ; FMad(a,b,c)
  %41 = fadd fast float %40, %35
  %42 = fmul fast float %24, %12
  %43 = call float @dx.op.tertiary.f32(i32 46, float %28, float %13, float %42)  ; FMad(a,b,c)
  %44 = call float @dx.op.tertiary.f32(i32 46, float %32, float %14, float %43)  ; FMad(a,b,c)
  %45 = fadd fast float %44, %36
  %46 = fmul fast float %25, %12
  %47 = call float @dx.op.tertiary.f32(i32 46, float %29, float %13, float %46)  ; FMad(a,b,c)
  %48 = call float @dx.op.tertiary.f32(i32 46, float %33, float %14, float %47)  ; FMad(a,b,c)
  %49 = fadd fast float %48, %37
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %41)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %45)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %49)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %49)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %16)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %17)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 2, float %18)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 0, float %20)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 1, float %21)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 0, float %12)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 1, float %13)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 2, float %14)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.resources = !{!4}
!dx.viewIdState = !{!7}
!dx.entryPoints = !{!8}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 7}
!2 = !{i32 1, i32 8}
!3 = !{!"vs", i32 6, i32 7}
!4 = !{null, null, !5, null}
!5 = !{!6}
!6 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 16, null}
!7 = !{[7 x i32] [i32 5, i32 15, i32 29567, i32 0, i32 0, i32 0, i32 0]}
!8 = !{void ()* @vs, !"vs", !9, !4, !25}
!9 = !{!10, !15, null}
!10 = !{!11, !14}
!11 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !12, i8 0, i32 1, i8 1, i32 0, i8 0, !13}
!12 = !{i32 0}
!13 = !{i32 3, i32 1}
!14 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !12, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!15 = !{!16, !18, !20, !23}
!16 = !{i32 0, !"SV_Position", i8 9, i8 3, !12, i8 4, i32 1, i8 4, i32 0, i8 0, !17}
!17 = !{i32 3, i32 15}
!18 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !12, i8 2, i32 1, i8 3, i32 1, i8 0, !19}
!19 = !{i32 3, i32 7}
!20 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !21, i8 2, i32 1, i8 2, i32 2, i8 0, !22}
!21 = !{i32 1}
!22 = !{i32 3, i32 3}
!23 = !{i32 3, !"ATTRIBUTE", i8 9, i8 0, !24, i8 2, i32 1, i8 3, i32 3, i8 0, !19}
!24 = !{i32 2}
!25 = !{i32 0, i64 1082130688}
*/
auto Shaders::bg_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(28108, 5120);
}

// shader_hash: 0e0482c316f041b4bebc9942264b52e4
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 4
// output_parameters: 1
// instruction_count: 31
// float_instruction_count: 2
// int_instruction_count: 1
// texture_normal_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xyz         1     NONE   float       
; ATTRIBUTE                1   xy          2     NONE   float       
; ATTRIBUTE                2   xyz         3     NONE   float   xyz 
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: 0e0482c316f041b4bebc9942264b52e4.pdb
; shader hash: 0e0482c316f041b4bebc9942264b52e4
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
; ATTRIBUTE                2                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint texture;                             ; Offset:    8
;           uint sampler;                             ; Offset:   12
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    16
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 15, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 12, 13, 14 }
;   output 1 depends on inputs: { 12, 13, 14 }
;   output 2 depends on inputs: { 12, 13, 14 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32 }

define void @ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 16 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %6 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %7 = extractvalue %dx.types.CBufRet.i32 %6, 0
  %8 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %7, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %9 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %8, %dx.types.ResourceProperties { i32 13, i32 804 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %10 = extractvalue %dx.types.CBufRet.i32 %6, 2
  %11 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %10, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %12 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %11, %dx.types.ResourceProperties { i32 5, i32 1033 })  ; AnnotateHandle(res,props)  resource: TextureCube<4xF32>
  %13 = extractvalue %dx.types.CBufRet.i32 %6, 3
  %14 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %13, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %15 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %14, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %16 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %9, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %17 = extractvalue %dx.types.CBufRet.f32 %16, 0
  %18 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %9, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %19 = extractvalue %dx.types.CBufRet.i32 %18, 1
  %20 = add i32 %19, -1
  %21 = uitofp i32 %20 to float
  %22 = fmul fast float %21, %17
  %23 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %12, %dx.types.Handle %15, float %3, float %4, float %5, float undef, i32 undef, i32 undef, i32 undef, float %22)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %24 = extractvalue %dx.types.ResRet.f32 %23, 0
  %25 = extractvalue %dx.types.ResRet.f32 %23, 1
  %26 = extractvalue %dx.types.ResRet.f32 %23, 2
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %24)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %25)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %26)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.resources = !{!4}
!dx.viewIdState = !{!7}
!dx.entryPoints = !{!8}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 7}
!2 = !{i32 1, i32 8}
!3 = !{!"ps", i32 6, i32 7}
!4 = !{null, null, !5, null}
!5 = !{!6}
!6 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 16, null}
!7 = !{[17 x i32] [i32 15, i32 4, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 7, i32 7, i32 7]}
!8 = !{void ()* @ps, !"ps", !9, !4, !22}
!9 = !{!10, !19, null}
!10 = !{!11, !13, !14, !16}
!11 = !{i32 0, !"SV_Position", i8 9, i8 3, !12, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!12 = !{i32 0}
!13 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !12, i8 2, i32 1, i8 3, i32 1, i8 0, null}
!14 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !15, i8 2, i32 1, i8 2, i32 2, i8 0, null}
!15 = !{i32 1}
!16 = !{i32 3, !"ATTRIBUTE", i8 9, i8 0, !17, i8 2, i32 1, i8 3, i32 3, i8 0, !18}
!17 = !{i32 2}
!18 = !{i32 3, i32 7}
!19 = !{!20}
!20 = !{i32 0, !"SV_Target", i8 9, i8 16, !12, i8 0, i32 1, i8 4, i32 0, i8 0, !21}
!21 = !{i32 3, i32 15}
!22 = !{i32 0, i64 3229614336}
*/
auto Shaders::bg_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(33228, 4696);
}

// shader_hash: 4c9579dd8bb09ea78354e64341064c74
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 3
// instruction_count: 65
// float_instruction_count: 8
// texture_load_instructions: 2
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xyz         1     NONE   float   xyz 
; ATTRIBUTE                1   xyz         2     NONE   float   xyz 
;
; shader debug name: 4c9579dd8bb09ea78354e64341064c74.pdb
; shader hash: 4c9579dd8bb09ea78354e64341064c74
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint lut_texture;                         ; Offset:    8
;           uint irr_texture;                         ; Offset:   12
;           uint rad_texture;                         ; Offset:   16
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    20
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 11
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 2 depends on inputs: { 0 }
;   output 3 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;   output 6 depends on inputs: { 0 }
;   output 8 depends on inputs: { 0 }
;   output 9 depends on inputs: { 0 }
;   output 10 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32, i32 }

define void @vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 20 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %5 = extractvalue %dx.types.CBufRet.i32 %4, 0
  %6 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %5, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %7 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %6, %dx.types.ResourceProperties { i32 13, i32 756 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %8 = extractvalue %dx.types.CBufRet.i32 %4, 1
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %8, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 524, i32 48 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=48>
  %11 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %12 = extractvalue %dx.types.ResRet.f32 %11, 0
  %13 = extractvalue %dx.types.ResRet.f32 %11, 1
  %14 = extractvalue %dx.types.ResRet.f32 %11, 2
  %15 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 12, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %16 = extractvalue %dx.types.ResRet.f32 %15, 0
  %17 = extractvalue %dx.types.ResRet.f32 %15, 1
  %18 = extractvalue %dx.types.ResRet.f32 %15, 2
  %19 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %20 = extractvalue %dx.types.CBufRet.f32 %19, 0
  %21 = extractvalue %dx.types.CBufRet.f32 %19, 1
  %22 = extractvalue %dx.types.CBufRet.f32 %19, 2
  %23 = extractvalue %dx.types.CBufRet.f32 %19, 3
  %24 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %25 = extractvalue %dx.types.CBufRet.f32 %24, 0
  %26 = extractvalue %dx.types.CBufRet.f32 %24, 1
  %27 = extractvalue %dx.types.CBufRet.f32 %24, 2
  %28 = extractvalue %dx.types.CBufRet.f32 %24, 3
  %29 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %30 = extractvalue %dx.types.CBufRet.f32 %29, 0
  %31 = extractvalue %dx.types.CBufRet.f32 %29, 1
  %32 = extractvalue %dx.types.CBufRet.f32 %29, 2
  %33 = extractvalue %dx.types.CBufRet.f32 %29, 3
  %34 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %35 = extractvalue %dx.types.CBufRet.f32 %34, 0
  %36 = extractvalue %dx.types.CBufRet.f32 %34, 1
  %37 = extractvalue %dx.types.CBufRet.f32 %34, 2
  %38 = extractvalue %dx.types.CBufRet.f32 %34, 3
  %39 = fmul fast float %20, %12
  %40 = call float @dx.op.tertiary.f32(i32 46, float %25, float %13, float %39)  ; FMad(a,b,c)
  %41 = call float @dx.op.tertiary.f32(i32 46, float %30, float %14, float %40)  ; FMad(a,b,c)
  %42 = fadd fast float %41, %35
  %43 = fmul fast float %21, %12
  %44 = call float @dx.op.tertiary.f32(i32 46, float %26, float %13, float %43)  ; FMad(a,b,c)
  %45 = call float @dx.op.tertiary.f32(i32 46, float %31, float %14, float %44)  ; FMad(a,b,c)
  %46 = fadd fast float %45, %36
  %47 = fmul fast float %22, %12
  %48 = call float @dx.op.tertiary.f32(i32 46, float %27, float %13, float %47)  ; FMad(a,b,c)
  %49 = call float @dx.op.tertiary.f32(i32 46, float %32, float %14, float %48)  ; FMad(a,b,c)
  %50 = fadd fast float %49, %37
  %51 = fmul fast float %23, %12
  %52 = call float @dx.op.tertiary.f32(i32 46, float %28, float %13, float %51)  ; FMad(a,b,c)
  %53 = call float @dx.op.tertiary.f32(i32 46, float %33, float %14, float %52)  ; FMad(a,b,c)
  %54 = fadd fast float %53, %38
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %42)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %46)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %50)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %54)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %16)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %17)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 2, float %18)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 0, float %12)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 1, float %13)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 2, float %14)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.resources = !{!4}
!dx.viewIdState = !{!7}
!dx.entryPoints = !{!8}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 7}
!2 = !{i32 1, i32 8}
!3 = !{!"vs", i32 6, i32 7}
!4 = !{null, null, !5, null}
!5 = !{!6}
!6 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 20, null}
!7 = !{[7 x i32] [i32 5, i32 11, i32 1919, i32 0, i32 0, i32 0, i32 0]}
!8 = !{void ()* @vs, !"vs", !9, !4, !22}
!9 = !{!10, !15, null}
!10 = !{!11, !14}
!11 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !12, i8 0, i32 1, i8 1, i32 0, i8 0, !13}
!12 = !{i32 0}
!13 = !{i32 3, i32 1}
!14 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !12, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!15 = !{!16, !18, !20}
!16 = !{i32 0, !"SV_Position", i8 9, i8 3, !12, i8 4, i32 1, i8 4, i32 0, i8 0, !17}
!17 = !{i32 3, i32 15}
!18 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !12, i8 2, i32 1, i8 3, i32 1, i8 0, !19}
!19 = !{i32 3, i32 7}
!20 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !21, i8 2, i32 1, i8 3, i32 2, i8 0, !19}
!21 = !{i32 1}
!22 = !{i32 0, i64 1082130688}
*/
auto Shaders::model_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(37924, 5048);
}

// shader_hash: 3ca5762c204f8c5e433c101f0023f96c
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 3
// output_parameters: 1
// instruction_count: 89
// float_instruction_count: 39
// texture_normal_instructions: 3
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xyz         1     NONE   float   xyz 
; ATTRIBUTE                1   xyz         2     NONE   float   xyz 
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: 3ca5762c204f8c5e433c101f0023f96c.pdb
; shader hash: 3ca5762c204f8c5e433c101f0023f96c
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint lut_texture;                         ; Offset:    8
;           uint irr_texture;                         ; Offset:   12
;           uint rad_texture;                         ; Offset:   16
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    20
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 11, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 4, 5, 6, 8, 9, 10 }
;   output 1 depends on inputs: { 4, 5, 6, 8, 9, 10 }
;   output 2 depends on inputs: { 4, 5, 6, 8, 9, 10 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32, i32 }

define void @ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 20 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %6 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %7 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %8 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %9 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %10 = extractvalue %dx.types.CBufRet.i32 %9, 0
  %11 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %10, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %12 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %11, %dx.types.ResourceProperties { i32 13, i32 756 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %13 = extractvalue %dx.types.CBufRet.i32 %9, 2
  %14 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %13, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %15 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %14, %dx.types.ResourceProperties { i32 2, i32 521 })  ; AnnotateHandle(res,props)  resource: Texture2D<2xF32>
  %16 = extractvalue %dx.types.CBufRet.i32 %9, 3
  %17 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %16, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %18 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %17, %dx.types.ResourceProperties { i32 5, i32 777 })  ; AnnotateHandle(res,props)  resource: TextureCube<3xF32>
  %19 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %20 = extractvalue %dx.types.CBufRet.i32 %19, 0
  %21 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %20, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %22 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %21, %dx.types.ResourceProperties { i32 5, i32 777 })  ; AnnotateHandle(res,props)  resource: TextureCube<3xF32>
  %23 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 0, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %24 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %23, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %25 = call float @dx.op.dot3.f32(i32 55, float %6, float %7, float %8, float %6, float %7, float %8)  ; Dot3(ax,ay,az,bx,by,bz)
  %26 = call float @dx.op.unary.f32(i32 25, float %25)  ; Rsqrt(value)
  %27 = fmul fast float %26, %6
  %28 = fmul fast float %26, %7
  %29 = fmul fast float %26, %8
  %30 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %12, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %31 = extractvalue %dx.types.CBufRet.f32 %30, 0
  %32 = extractvalue %dx.types.CBufRet.f32 %30, 1
  %33 = extractvalue %dx.types.CBufRet.f32 %30, 2
  %34 = fsub fast float %31, %3
  %35 = fsub fast float %32, %4
  %36 = fsub fast float %33, %5
  %37 = call float @dx.op.dot3.f32(i32 55, float %34, float %35, float %36, float %34, float %35, float %36)  ; Dot3(ax,ay,az,bx,by,bz)
  %38 = call float @dx.op.unary.f32(i32 25, float %37)  ; Rsqrt(value)
  %39 = fmul fast float %34, %38
  %40 = fmul fast float %35, %38
  %41 = fmul fast float %36, %38
  %42 = fsub fast float -0.000000e+00, %39
  %43 = fsub fast float -0.000000e+00, %40
  %44 = fsub fast float -0.000000e+00, %41
  %45 = call float @dx.op.dot3.f32(i32 55, float %42, float %43, float %44, float %27, float %28, float %29)  ; Dot3(ax,ay,az,bx,by,bz)
  %46 = fmul fast float %45, 2.000000e+00
  %47 = fmul fast float %46, %27
  %48 = fmul fast float %46, %28
  %49 = fmul fast float %46, %29
  %50 = fsub fast float %42, %47
  %51 = fsub fast float %43, %48
  %52 = fsub fast float %44, %49
  %53 = call float @dx.op.dot3.f32(i32 55, float %27, float %28, float %29, float %39, float %40, float %41)  ; Dot3(ax,ay,az,bx,by,bz)
  %54 = call float @dx.op.unary.f32(i32 7, float %53)  ; Saturate(value)
  %55 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %12, i32 5)  ; CBufferLoadLegacy(handle,regIndex)
  %56 = extractvalue %dx.types.CBufRet.f32 %55, 0
  %57 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %12, i32 5)  ; CBufferLoadLegacy(handle,regIndex)
  %58 = extractvalue %dx.types.CBufRet.i32 %57, 1
  %59 = uitofp i32 %58 to float
  %60 = fadd fast float %59, -1.000000e+00
  %61 = fmul fast float %60, %56
  %62 = call %dx.types.ResRet.f32 @dx.op.sample.f32(i32 60, %dx.types.Handle %15, %dx.types.Handle %24, float %54, float %56, float undef, float undef, i32 0, i32 0, i32 undef, float undef)  ; Sample(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,clamp)
  %63 = extractvalue %dx.types.ResRet.f32 %62, 0
  %64 = extractvalue %dx.types.ResRet.f32 %62, 1
  %65 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %18, %dx.types.Handle %24, float %27, float %28, float %29, float undef, i32 undef, i32 undef, i32 undef, float 0.000000e+00)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %66 = extractvalue %dx.types.ResRet.f32 %65, 0
  %67 = extractvalue %dx.types.ResRet.f32 %65, 1
  %68 = extractvalue %dx.types.ResRet.f32 %65, 2
  %69 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %22, %dx.types.Handle %24, float %50, float %51, float %52, float undef, i32 undef, i32 undef, i32 undef, float %61)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %70 = extractvalue %dx.types.ResRet.f32 %69, 0
  %71 = extractvalue %dx.types.ResRet.f32 %69, 1
  %72 = extractvalue %dx.types.ResRet.f32 %69, 2
  %73 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %12, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %74 = extractvalue %dx.types.CBufRet.f32 %73, 3
  %75 = fmul fast float %74, 0x3FEEB851E0000000
  %76 = fadd fast float %75, 0x3FA47AE140000000
  %77 = fmul fast float %76, %63
  %78 = fadd fast float %77, %64
  %79 = fmul fast float %78, %70
  %80 = fmul fast float %78, %71
  %81 = fmul fast float %78, %72
  %82 = fadd fast float %79, %66
  %83 = fadd fast float %80, %67
  %84 = fadd fast float %81, %68
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %82)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %83)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %84)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.dot3.f32(i32, float, float, float, float, float, float) #0

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sample.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.resources = !{!4}
!dx.viewIdState = !{!7}
!dx.entryPoints = !{!8}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 7}
!2 = !{i32 1, i32 8}
!3 = !{!"ps", i32 6, i32 7}
!4 = !{null, null, !5, null}
!5 = !{!6}
!6 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 20, null}
!7 = !{[13 x i32] [i32 11, i32 4, i32 0, i32 0, i32 0, i32 0, i32 7, i32 7, i32 7, i32 0, i32 7, i32 7, i32 7]}
!8 = !{void ()* @ps, !"ps", !9, !4, !20}
!9 = !{!10, !17, null}
!10 = !{!11, !13, !15}
!11 = !{i32 0, !"SV_Position", i8 9, i8 3, !12, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!12 = !{i32 0}
!13 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !12, i8 2, i32 1, i8 3, i32 1, i8 0, !14}
!14 = !{i32 3, i32 7}
!15 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !16, i8 2, i32 1, i8 3, i32 2, i8 0, !14}
!16 = !{i32 1}
!17 = !{!18}
!18 = !{i32 0, !"SV_Target", i8 9, i8 16, !12, i8 0, i32 1, i8 4, i32 0, i8 0, !19}
!19 = !{i32 3, i32 15}
!20 = !{i32 0, i64 3229614336}
*/
auto Shaders::model_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(42972, 5288);
}

// shader_hash: c02dffcbc09c806460b8bfab37ebe785
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 6
// instruction_count: 76
// float_instruction_count: 12
// texture_load_instructions: 8
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint   x   
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xy          1     NONE   float   xy  
; ATTRIBUTE                1   x           2     NONE    uint   x   
; ATTRIBUTE                2    y          2     NONE    uint    y  
; ATTRIBUTE                3     z         2     NONE    uint     z 
; ATTRIBUTE                4      w        2     NONE    uint      w
;
; shader debug name: c02dffcbc09c806460b8bfab37ebe785.pdb
; shader hash: c02dffcbc09c806460b8bfab37ebe785
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1        nointerpolation       
; ATTRIBUTE                2        nointerpolation       
; ATTRIBUTE                3        nointerpolation       
; ATTRIBUTE                4        nointerpolation       
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint instances;                           ; Offset:    8
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    12
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 12
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0, 4 }
;   output 1 depends on inputs: { 0, 4 }
;   output 2 depends on inputs: { 0, 4 }
;   output 3 depends on inputs: { 0, 4 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;   output 8 depends on inputs: { 4 }
;   output 9 depends on inputs: { 4 }
;   output 10 depends on inputs: { 4 }
;   output 11 depends on inputs: { 4 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.ResRet.i32 = type { i32, i32, i32, i32, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32 }

define void @vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 12 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.i32 %5, 0
  %7 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %6, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %8 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %7, %dx.types.ResourceProperties { i32 13, i32 820 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %9 = extractvalue %dx.types.CBufRet.i32 %5, 1
  %10 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %9, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %11 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %10, %dx.types.ResourceProperties { i32 524, i32 16 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=16>
  %12 = extractvalue %dx.types.CBufRet.i32 %5, 2
  %13 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %12, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %14 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %13, %dx.types.ResourceProperties { i32 524, i32 32 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=32>
  %15 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %11, i32 %4, i32 0, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %16 = extractvalue %dx.types.ResRet.f32 %15, 0
  %17 = extractvalue %dx.types.ResRet.f32 %15, 1
  %18 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %11, i32 %4, i32 8, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %19 = extractvalue %dx.types.ResRet.f32 %18, 0
  %20 = extractvalue %dx.types.ResRet.f32 %18, 1
  %21 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %14, i32 %3, i32 0, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %22 = extractvalue %dx.types.ResRet.f32 %21, 0
  %23 = extractvalue %dx.types.ResRet.f32 %21, 1
  %24 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %14, i32 %3, i32 8, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %25 = extractvalue %dx.types.ResRet.f32 %24, 0
  %26 = extractvalue %dx.types.ResRet.f32 %24, 1
  %27 = call %dx.types.ResRet.i32 @dx.op.rawBufferLoad.i32(i32 139, %dx.types.Handle %14, i32 %3, i32 16, i8 1, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %28 = extractvalue %dx.types.ResRet.i32 %27, 0
  %29 = call %dx.types.ResRet.i32 @dx.op.rawBufferLoad.i32(i32 139, %dx.types.Handle %14, i32 %3, i32 20, i8 1, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %30 = extractvalue %dx.types.ResRet.i32 %29, 0
  %31 = call %dx.types.ResRet.i32 @dx.op.rawBufferLoad.i32(i32 139, %dx.types.Handle %14, i32 %3, i32 24, i8 1, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %32 = extractvalue %dx.types.ResRet.i32 %31, 0
  %33 = call %dx.types.ResRet.i32 @dx.op.rawBufferLoad.i32(i32 139, %dx.types.Handle %14, i32 %3, i32 28, i8 1, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %34 = extractvalue %dx.types.ResRet.i32 %33, 0
  %35 = fmul fast float %25, %16
  %36 = fmul fast float %26, %17
  %37 = fadd fast float %35, %22
  %38 = fadd fast float %36, %23
  %39 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %40 = extractvalue %dx.types.CBufRet.f32 %39, 0
  %41 = extractvalue %dx.types.CBufRet.f32 %39, 1
  %42 = extractvalue %dx.types.CBufRet.f32 %39, 2
  %43 = extractvalue %dx.types.CBufRet.f32 %39, 3
  %44 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %45 = extractvalue %dx.types.CBufRet.f32 %44, 0
  %46 = extractvalue %dx.types.CBufRet.f32 %44, 1
  %47 = extractvalue %dx.types.CBufRet.f32 %44, 2
  %48 = extractvalue %dx.types.CBufRet.f32 %44, 3
  %49 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %50 = extractvalue %dx.types.CBufRet.f32 %49, 0
  %51 = extractvalue %dx.types.CBufRet.f32 %49, 1
  %52 = extractvalue %dx.types.CBufRet.f32 %49, 2
  %53 = extractvalue %dx.types.CBufRet.f32 %49, 3
  %54 = fmul fast float %40, %37
  %55 = call float @dx.op.tertiary.f32(i32 46, float %45, float %38, float %54)  ; FMad(a,b,c)
  %56 = fadd fast float %50, %55
  %57 = fmul fast float %41, %37
  %58 = call float @dx.op.tertiary.f32(i32 46, float %46, float %38, float %57)  ; FMad(a,b,c)
  %59 = fadd fast float %58, %51
  %60 = fmul fast float %42, %37
  %61 = call float @dx.op.tertiary.f32(i32 46, float %47, float %38, float %60)  ; FMad(a,b,c)
  %62 = fadd fast float %61, %52
  %63 = fmul fast float %43, %37
  %64 = call float @dx.op.tertiary.f32(i32 46, float %48, float %38, float %63)  ; FMad(a,b,c)
  %65 = fadd fast float %64, %53
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %56)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %59)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %62)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %65)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %19)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %20)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.i32(i32 5, i32 2, i32 0, i8 0, i32 %28)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.i32(i32 5, i32 3, i32 0, i8 0, i32 %30)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.i32(i32 5, i32 4, i32 0, i8 0, i32 %32)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.i32(i32 5, i32 5, i32 0, i8 0, i32 %34)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind
declare void @dx.op.storeOutput.i32(i32, i32, i32, i8, i32) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.i32 @dx.op.rawBufferLoad.i32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.resources = !{!4}
!dx.viewIdState = !{!7}
!dx.entryPoints = !{!8}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 7}
!2 = !{i32 1, i32 8}
!3 = !{!"vs", i32 6, i32 7}
!4 = !{null, null, !5, null}
!5 = !{!6}
!6 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 12, null}
!7 = !{[7 x i32] [i32 5, i32 12, i32 63, i32 0, i32 0, i32 0, i32 3855]}
!8 = !{void ()* @vs, !"vs", !9, !4, !28}
!9 = !{!10, !15, null}
!10 = !{!11, !14}
!11 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !12, i8 0, i32 1, i8 1, i32 0, i8 0, !13}
!12 = !{i32 0}
!13 = !{i32 3, i32 1}
!14 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !12, i8 0, i32 1, i8 1, i32 1, i8 0, !13}
!15 = !{!16, !18, !20, !22, !24, !26}
!16 = !{i32 0, !"SV_Position", i8 9, i8 3, !12, i8 4, i32 1, i8 4, i32 0, i8 0, !17}
!17 = !{i32 3, i32 15}
!18 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !12, i8 2, i32 1, i8 2, i32 1, i8 0, !19}
!19 = !{i32 3, i32 3}
!20 = !{i32 2, !"ATTRIBUTE", i8 5, i8 0, !21, i8 1, i32 1, i8 1, i32 2, i8 0, !13}
!21 = !{i32 1}
!22 = !{i32 3, !"ATTRIBUTE", i8 5, i8 0, !23, i8 1, i32 1, i8 1, i32 2, i8 1, !13}
!23 = !{i32 2}
!24 = !{i32 4, !"ATTRIBUTE", i8 5, i8 0, !25, i8 1, i32 1, i8 1, i32 2, i8 2, !13}
!25 = !{i32 3}
!26 = !{i32 5, !"ATTRIBUTE", i8 5, i8 0, !27, i8 1, i32 1, i8 1, i32 2, i8 3, !13}
!27 = !{i32 4}
!28 = !{i32 0, i64 1082130688}
*/
auto Shaders::screen_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(48260, 5620);
}

// shader_hash: 95db4a2ae02b74a6f13d030705b3cbe5
// input_parameters: 6
// output_parameters: 1
// instruction_count: 21
// float_instruction_count: 2
// texture_normal_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xy          1     NONE   float   xy  
; ATTRIBUTE                1   x           2     NONE    uint   x   
; ATTRIBUTE                2    y          2     NONE    uint    y  
; ATTRIBUTE                3     z         2     NONE    uint     z 
; ATTRIBUTE                4      w        2     NONE    uint      w
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: 95db4a2ae02b74a6f13d030705b3cbe5.pdb
; shader hash: 95db4a2ae02b74a6f13d030705b3cbe5
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1        nointerpolation       
; ATTRIBUTE                2        nointerpolation       
; ATTRIBUTE                3        nointerpolation       
; ATTRIBUTE                4        nointerpolation       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
;
;
; ViewId state:
;
; Number of inputs: 12, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 4, 5, 8, 9, 10, 11 }
;   output 1 depends on inputs: { 4, 5, 8, 9, 10, 11 }
;   output 2 depends on inputs: { 4, 5, 8, 9, 10, 11 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }

define void @ps() {
  %1 = call i32 @dx.op.loadInput.i32(i32 4, i32 5, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %2 = call i32 @dx.op.loadInput.i32(i32 4, i32 4, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 3, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call i32 @dx.op.loadInput.i32(i32 4, i32 2, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %6 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %7 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %4, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %8 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %7, %dx.types.ResourceProperties { i32 7, i32 777 })  ; AnnotateHandle(res,props)  resource: Texture2DArray<3xF32>
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %1, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %11 = uitofp i32 %2 to float
  %12 = uitofp i32 %3 to float
  %13 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %8, %dx.types.Handle %10, float %5, float %6, float %12, float undef, i32 0, i32 0, i32 undef, float %11)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %14 = extractvalue %dx.types.ResRet.f32 %13, 0
  %15 = extractvalue %dx.types.ResRet.f32 %13, 1
  %16 = extractvalue %dx.types.ResRet.f32 %13, 2
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %14)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %15)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %16)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.viewIdState = !{!4}
!dx.entryPoints = !{!5}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 7}
!2 = !{i32 1, i32 8}
!3 = !{!"ps", i32 6, i32 7}
!4 = !{[14 x i32] [i32 12, i32 4, i32 0, i32 0, i32 0, i32 0, i32 7, i32 7, i32 0, i32 0, i32 7, i32 7, i32 7, i32 7]}
!5 = !{void ()* @ps, !"ps", !6, null, !24}
!6 = !{!7, !21, null}
!7 = !{!8, !10, !12, !15, !17, !19}
!8 = !{i32 0, !"SV_Position", i8 9, i8 3, !9, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!9 = !{i32 0}
!10 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !9, i8 2, i32 1, i8 2, i32 1, i8 0, !11}
!11 = !{i32 3, i32 3}
!12 = !{i32 2, !"ATTRIBUTE", i8 5, i8 0, !13, i8 1, i32 1, i8 1, i32 2, i8 0, !14}
!13 = !{i32 1}
!14 = !{i32 3, i32 1}
!15 = !{i32 3, !"ATTRIBUTE", i8 5, i8 0, !16, i8 1, i32 1, i8 1, i32 2, i8 1, !14}
!16 = !{i32 2}
!17 = !{i32 4, !"ATTRIBUTE", i8 5, i8 0, !18, i8 1, i32 1, i8 1, i32 2, i8 2, !14}
!18 = !{i32 3}
!19 = !{i32 5, !"ATTRIBUTE", i8 5, i8 0, !20, i8 1, i32 1, i8 1, i32 2, i8 3, !14}
!20 = !{i32 4}
!21 = !{!22}
!22 = !{i32 0, !"SV_Target", i8 9, i8 16, !9, i8 0, i32 1, i8 4, i32 0, i8 0, !23}
!23 = !{i32 3, i32 15}
!24 = !{i32 0, i64 3229614336}
*/
auto Shaders::screen_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(53880, 4156);
}

// shader_hash: 2886ee06f6b91aaf0d306d345dbb615f
// input_parameters: 2
// output_parameters: 2
// instruction_count: 17
// float_instruction_count: 6
// uint_instruction_count: 3
/* disassembly:
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xy          1     NONE   float   xy  
;
; shader debug name: 2886ee06f6b91aaf0d306d345dbb615f.pdb
; shader hash: 2886ee06f6b91aaf0d306d345dbb615f
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
;
; Buffer Definitions:
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 6
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

define void @vs() {
  %1 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %2 = and i32 %1, 2
  %3 = uitofp i32 %2 to float
  %4 = shl i32 %1, 1
  %5 = and i32 %4, 2
  %6 = uitofp i32 %5 to float
  %7 = fmul fast float %3, 2.000000e+00
  %8 = fmul fast float %6, 2.000000e+00
  %9 = fadd fast float %7, -1.000000e+00
  %10 = fsub fast float 1.000000e+00, %8
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %9)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %10)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float 0.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %3)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %6)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.viewIdState = !{!4}
!dx.entryPoints = !{!5}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 7}
!2 = !{i32 1, i32 8}
!3 = !{!"vs", i32 6, i32 7}
!4 = !{[7 x i32] [i32 5, i32 6, i32 51, i32 0, i32 0, i32 0, i32 0]}
!5 = !{void ()* @vs, !"vs", !6, null, !17}
!6 = !{!7, !12, null}
!7 = !{!8, !11}
!8 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !9, i8 0, i32 1, i8 1, i32 0, i8 0, !10}
!9 = !{i32 0}
!10 = !{i32 3, i32 1}
!11 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !9, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!12 = !{!13, !15}
!13 = !{i32 0, !"SV_Position", i8 9, i8 3, !9, i8 4, i32 1, i8 4, i32 0, i8 0, !14}
!14 = !{i32 3, i32 15}
!15 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !9, i8 2, i32 1, i8 2, i32 1, i8 0, !16}
!16 = !{i32 3, i32 3}
!17 = !{i32 0, i64 8388864}
*/
auto Shaders::blit_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(58036, 3244);
}

// shader_hash: e491d316a70d08ff74ffa04bb148e0cb
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 1
// instruction_count: 62
// dynamic_flow_control_count: 1
// float_instruction_count: 30
// int_instruction_count: 1
// texture_normal_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xy          1     NONE   float   xy  
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: e491d316a70d08ff74ffa04bb148e0cb.pdb
; shader hash: e491d316a70d08ff74ffa04bb148e0cb
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint texture;                             ; Offset:    4
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:     8
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 6, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 4, 5 }
;   output 1 depends on inputs: { 4, 5 }
;   output 2 depends on inputs: { 4, 5 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32 }

define void @ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 8 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.i32 %5, 0
  %7 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %6, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %8 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %7, %dx.types.ResourceProperties { i32 13, i32 996 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %9 = extractvalue %dx.types.CBufRet.i32 %5, 1
  %10 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %9, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %11 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %10, %dx.types.ResourceProperties { i32 2, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2D<4xF32>
  %12 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 0, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %13 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %12, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %14 = call %dx.types.ResRet.f32 @dx.op.sample.f32(i32 60, %dx.types.Handle %11, %dx.types.Handle %13, float %3, float %4, float undef, float undef, i32 0, i32 0, i32 undef, float undef)  ; Sample(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,clamp)
  %15 = extractvalue %dx.types.ResRet.f32 %14, 0
  %16 = extractvalue %dx.types.ResRet.f32 %14, 1
  %17 = extractvalue %dx.types.ResRet.f32 %14, 2
  %18 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %8, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %19 = extractvalue %dx.types.CBufRet.i32 %18, 0
  %20 = icmp eq i32 %19, 0
  br i1 %20, label %49, label %21

; <label>:21                                      ; preds = %0
  %22 = fmul fast float %15, 0x4004147AE0000000
  %23 = fmul fast float %16, 0x4004147AE0000000
  %24 = fmul fast float %17, 0x4004147AE0000000
  %25 = fadd fast float %22, 0x3F9EB851E0000000
  %26 = fadd fast float %23, 0x3F9EB851E0000000
  %27 = fadd fast float %24, 0x3F9EB851E0000000
  %28 = fmul fast float %25, %15
  %29 = fmul fast float %26, %16
  %30 = fmul fast float %27, %17
  %31 = fmul fast float %15, 0x400370A3E0000000
  %32 = fmul fast float %16, 0x400370A3E0000000
  %33 = fmul fast float %17, 0x400370A3E0000000
  %34 = fadd fast float %31, 0x3FE2E147A0000000
  %35 = fadd fast float %32, 0x3FE2E147A0000000
  %36 = fadd fast float %33, 0x3FE2E147A0000000
  %37 = fmul fast float %34, %15
  %38 = fmul fast float %35, %16
  %39 = fmul fast float %36, %17
  %40 = fadd fast float %37, 0x3FC1EB8520000000
  %41 = fadd fast float %38, 0x3FC1EB8520000000
  %42 = fadd fast float %39, 0x3FC1EB8520000000
  %43 = fdiv fast float %28, %40
  %44 = fdiv fast float %29, %41
  %45 = fdiv fast float %30, %42
  %46 = call float @dx.op.unary.f32(i32 7, float %43)  ; Saturate(value)
  %47 = call float @dx.op.unary.f32(i32 7, float %44)  ; Saturate(value)
  %48 = call float @dx.op.unary.f32(i32 7, float %45)  ; Saturate(value)
  br label %49

; <label>:49                                      ; preds = %21, %0
  %50 = phi float [ %46, %21 ], [ %15, %0 ]
  %51 = phi float [ %47, %21 ], [ %16, %0 ]
  %52 = phi float [ %48, %21 ], [ %17, %0 ]
  %53 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %54 = extractvalue %dx.types.CBufRet.f32 %53, 1
  %55 = fmul fast float %54, %50
  %56 = fmul fast float %54, %51
  %57 = fmul fast float %54, %52
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %55)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %56)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %57)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sample.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.resources = !{!4}
!dx.viewIdState = !{!7}
!dx.entryPoints = !{!8}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 7}
!2 = !{i32 1, i32 8}
!3 = !{!"ps", i32 6, i32 7}
!4 = !{null, null, !5, null}
!5 = !{!6}
!6 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 8, null}
!7 = !{[8 x i32] [i32 6, i32 4, i32 0, i32 0, i32 0, i32 0, i32 7, i32 7]}
!8 = !{void ()* @ps, !"ps", !9, !4, !18}
!9 = !{!10, !15, null}
!10 = !{!11, !13}
!11 = !{i32 0, !"SV_Position", i8 9, i8 3, !12, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!12 = !{i32 0}
!13 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !12, i8 2, i32 1, i8 2, i32 1, i8 0, !14}
!14 = !{i32 3, i32 3}
!15 = !{!16}
!16 = !{i32 0, !"SV_Target", i8 9, i8 16, !12, i8 0, i32 1, i8 4, i32 0, i8 0, !17}
!17 = !{i32 3, i32 15}
!18 = !{i32 0, i64 3229614336}
*/
auto Shaders::blit_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(61280, 4684);
}

#undef texture_data

} // namespace fb::baked::stockcube
