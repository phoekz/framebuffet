#include "baked.hpp"

namespace fb::baked::kitchen {

#define texture_data(rp, sp, off, sz)                                                   \
    TextureData {                                                                       \
        .row_pitch = rp, .slice_pitch = sp, .data = transmuted_span<std::byte>(off, sz) \
    }

auto Assets::load() -> void {
    // hash: 799fc360204416196536a93c9eff68ae
    FB_PERF_FUNC();
    _data = read_whole_file("fb_kitchen_assets.bin");
    FB_ASSERT(_data.size() == 162588);
}

auto Assets::imgui_font() const -> Copy {
    return Copy {
        // hash: 799fc360204416196536a93c9eff68ae
        .data = transmuted_span<std::byte>(0, 162588),
    };
}

auto Shaders::load() -> void {
    // hash: 5c1fcb0f3e68fd4dca87af13e32834c2
    FB_PERF_FUNC();
    _data = read_whole_file("fb_kitchen_shaders.bin");
    FB_ASSERT(_data.size() == 32068);
}

// shader_hash: 3ed0e9910f97b72a68fdcae6b89ef30d
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 3
// instruction_count: 86
// float_instruction_count: 31
// int_instruction_count: 1
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
; ATTRIBUTE                0   xyzw        1     NONE   float   xyzw
; ATTRIBUTE                1   xy          2     NONE   float   xy  
;
; shader debug name: 3ed0e9910f97b72a68fdcae6b89ef30d.pdb
; shader hash: 3ed0e9910f97b72a68fdcae6b89ef30d
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
;           uint base_vertex;                         ; Offset:    8
;           uint texture;                             ; Offset:   12
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
; Number of inputs: 5, outputs: 10
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 2 depends on inputs: { 0 }
;   output 3 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;   output 6 depends on inputs: { 0 }
;   output 7 depends on inputs: { 0 }
;   output 8 depends on inputs: { 0 }
;   output 9 depends on inputs: { 0 }
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
%dx.types.fouri32 = type { i32, i32, i32, i32 }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32 }

define void @draw_vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 16 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %5 = extractvalue %dx.types.CBufRet.i32 %4, 0
  %6 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %5, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %7 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %6, %dx.types.ResourceProperties { i32 13, i32 820 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %8 = extractvalue %dx.types.CBufRet.i32 %4, 1
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %8, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 524, i32 20 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=20>
  %11 = extractvalue %dx.types.CBufRet.i32 %4, 2
  %12 = add i32 %11, %3
  %13 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %12, i32 0, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %14 = extractvalue %dx.types.ResRet.f32 %13, 0
  %15 = extractvalue %dx.types.ResRet.f32 %13, 1
  %16 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %12, i32 8, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %17 = extractvalue %dx.types.ResRet.f32 %16, 0
  %18 = extractvalue %dx.types.ResRet.f32 %16, 1
  %19 = call %dx.types.ResRet.i32 @dx.op.rawBufferLoad.i32(i32 139, %dx.types.Handle %10, i32 %12, i32 16, i8 1, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %20 = extractvalue %dx.types.ResRet.i32 %19, 0
  %21 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %22 = extractvalue %dx.types.CBufRet.f32 %21, 0
  %23 = extractvalue %dx.types.CBufRet.f32 %21, 1
  %24 = extractvalue %dx.types.CBufRet.f32 %21, 2
  %25 = extractvalue %dx.types.CBufRet.f32 %21, 3
  %26 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %27 = extractvalue %dx.types.CBufRet.f32 %26, 0
  %28 = extractvalue %dx.types.CBufRet.f32 %26, 1
  %29 = extractvalue %dx.types.CBufRet.f32 %26, 2
  %30 = extractvalue %dx.types.CBufRet.f32 %26, 3
  %31 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %32 = extractvalue %dx.types.CBufRet.f32 %31, 0
  %33 = extractvalue %dx.types.CBufRet.f32 %31, 1
  %34 = extractvalue %dx.types.CBufRet.f32 %31, 2
  %35 = extractvalue %dx.types.CBufRet.f32 %31, 3
  %36 = fmul fast float %22, %14
  %37 = call float @dx.op.tertiary.f32(i32 46, float %27, float %15, float %36)  ; FMad(a,b,c)
  %38 = fadd fast float %32, %37
  %39 = fmul fast float %23, %14
  %40 = call float @dx.op.tertiary.f32(i32 46, float %28, float %15, float %39)  ; FMad(a,b,c)
  %41 = fadd fast float %40, %33
  %42 = fmul fast float %24, %14
  %43 = call float @dx.op.tertiary.f32(i32 46, float %29, float %15, float %42)  ; FMad(a,b,c)
  %44 = fadd fast float %43, %34
  %45 = fmul fast float %25, %14
  %46 = call float @dx.op.tertiary.f32(i32 46, float %30, float %15, float %45)  ; FMad(a,b,c)
  %47 = fadd fast float %46, %35
  %48 = call %dx.types.fouri32 @dx.op.unpack4x8.i32(i32 219, i8 0, i32 %20)  ; Unpack4x8(unpackMode,pk)
  %49 = extractvalue %dx.types.fouri32 %48, 0
  %50 = extractvalue %dx.types.fouri32 %48, 1
  %51 = extractvalue %dx.types.fouri32 %48, 2
  %52 = extractvalue %dx.types.fouri32 %48, 3
  %53 = uitofp i32 %49 to float
  %54 = uitofp i32 %50 to float
  %55 = uitofp i32 %51 to float
  %56 = uitofp i32 %52 to float
  %57 = fmul fast float %53, 0x3F70101020000000
  %58 = fmul fast float %54, 0x3F70101020000000
  %59 = fmul fast float %55, 0x3F70101020000000
  %60 = fmul fast float %56, 0x3F70101020000000
  %61 = fmul fast float %53, 0x3F539DC020000000
  %62 = fmul fast float %54, 0x3F539DC020000000
  %63 = fmul fast float %55, 0x3F539DC020000000
  %64 = fadd fast float %61, 0x3FE5D45880000000
  %65 = fadd fast float %62, 0x3FE5D45880000000
  %66 = fadd fast float %63, 0x3FE5D45880000000
  %67 = fmul fast float %64, %57
  %68 = fmul fast float %65, %58
  %69 = fmul fast float %66, %59
  %70 = fadd fast float %67, 0x3F89A59840000000
  %71 = fadd fast float %68, 0x3F89A59840000000
  %72 = fadd fast float %69, 0x3F89A59840000000
  %73 = fmul fast float %70, %57
  %74 = fmul fast float %71, %58
  %75 = fmul fast float %72, %59
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %38)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %41)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %44)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %47)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %73)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %74)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 2, float %75)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 3, float %60)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 0, float %17)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 1, float %18)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
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
declare %dx.types.ResRet.i32 @dx.op.rawBufferLoad.i32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readnone
declare %dx.types.fouri32 @dx.op.unpack4x8.i32(i32, i8, i32) #0

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
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2407.7 (416fab6b5)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 16, null}
!6 = !{[7 x i32] [i32 5, i32 10, i32 1023, i32 0, i32 0, i32 0, i32 0]}
!7 = !{void ()* @draw_vs, !"draw_vs", !8, !3, !21}
!8 = !{!9, !14, null}
!9 = !{!10, !13}
!10 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !11, i8 0, i32 1, i8 1, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 1}
!13 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !11, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!14 = !{!15, !17, !18}
!15 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 4, i32 1, i8 0, !16}
!18 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !19, i8 2, i32 1, i8 2, i32 2, i8 0, !20}
!19 = !{i32 1}
!20 = !{i32 3, i32 3}
!21 = !{i32 0, i64 1082130688}
*/
auto Shaders::gui_draw_vs() const -> Span<const std::byte> {
    return Span(_data).subspan(0, 5364);
}

// shader_hash: 1c14709bc74f6a3302945799741f3777
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 3
// output_parameters: 1
// instruction_count: 28
// float_instruction_count: 4
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
; ATTRIBUTE                0   xyzw        1     NONE   float   xyzw
; ATTRIBUTE                1   xy          2     NONE   float   xy  
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: 1c14709bc74f6a3302945799741f3777.pdb
; shader hash: 1c14709bc74f6a3302945799741f3777
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
;           uint base_vertex;                         ; Offset:    8
;           uint texture;                             ; Offset:   12
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
; Number of inputs: 10, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 4, 8, 9 }
;   output 1 depends on inputs: { 5, 8, 9 }
;   output 2 depends on inputs: { 6, 8, 9 }
;   output 3 depends on inputs: { 7, 8, 9 }
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

define void @draw_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 16 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %6 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %7 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %8 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 3, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %9 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %10 = extractvalue %dx.types.CBufRet.i32 %9, 3
  %11 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %10, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %12 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %11, %dx.types.ResourceProperties { i32 2, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2D<4xF32>
  %13 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 1, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %14 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %13, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %15 = call %dx.types.ResRet.f32 @dx.op.sample.f32(i32 60, %dx.types.Handle %12, %dx.types.Handle %14, float %3, float %4, float undef, float undef, i32 0, i32 0, i32 undef, float undef)  ; Sample(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,clamp)
  %16 = extractvalue %dx.types.ResRet.f32 %15, 0
  %17 = extractvalue %dx.types.ResRet.f32 %15, 1
  %18 = extractvalue %dx.types.ResRet.f32 %15, 2
  %19 = extractvalue %dx.types.ResRet.f32 %15, 3
  %20 = fmul fast float %16, %5
  %21 = fmul fast float %17, %6
  %22 = fmul fast float %18, %7
  %23 = fmul fast float %19, %8
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %20)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %21)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %22)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %23)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sample.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2407.7 (416fab6b5)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 16, null}
!6 = !{[12 x i32] [i32 10, i32 4, i32 0, i32 0, i32 0, i32 0, i32 1, i32 2, i32 4, i32 8, i32 15, i32 15]}
!7 = !{void ()* @draw_ps, !"draw_ps", !8, !3, !19}
!8 = !{!9, !17, null}
!9 = !{!10, !12, !14}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!11 = !{i32 0}
!12 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 4, i32 1, i8 0, !13}
!13 = !{i32 3, i32 15}
!14 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !15, i8 2, i32 1, i8 2, i32 2, i8 0, !16}
!15 = !{i32 1}
!16 = !{i32 3, i32 3}
!17 = !{!18}
!18 = !{i32 0, !"SV_Target", i8 9, i8 16, !11, i8 0, i32 1, i8 4, i32 0, i8 0, !13}
!19 = !{i32 0, i64 3229614336}
*/
auto Shaders::gui_draw_ps() const -> Span<const std::byte> {
    return Span(_data).subspan(5364, 4428);
}

// shader_hash: cb1199f8880950b8d9c0da52b5009da0
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 2
// instruction_count: 74
// float_instruction_count: 16
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
; ATTRIBUTE                0   xyzw        1     NONE   float   xyzw
;
; shader debug name: cb1199f8880950b8d9c0da52b5009da0.pdb
; shader hash: cb1199f8880950b8d9c0da52b5009da0
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
; Number of inputs: 5, outputs: 8
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 2 depends on inputs: { 0 }
;   output 3 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;   output 6 depends on inputs: { 0 }
;   output 7 depends on inputs: { 0 }
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
%dx.types.fouri32 = type { i32, i32, i32, i32 }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32 }

define void @draw_vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 8 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %5 = extractvalue %dx.types.CBufRet.i32 %4, 0
  %6 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %5, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %7 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %6, %dx.types.ResourceProperties { i32 13, i32 820 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %8 = extractvalue %dx.types.CBufRet.i32 %4, 1
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %8, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 524, i32 16 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=16>
  %11 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %12 = extractvalue %dx.types.ResRet.f32 %11, 0
  %13 = extractvalue %dx.types.ResRet.f32 %11, 1
  %14 = extractvalue %dx.types.ResRet.f32 %11, 2
  %15 = call %dx.types.ResRet.i32 @dx.op.rawBufferLoad.i32(i32 139, %dx.types.Handle %10, i32 %3, i32 12, i8 1, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %16 = extractvalue %dx.types.ResRet.i32 %15, 0
  %17 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %18 = extractvalue %dx.types.CBufRet.f32 %17, 0
  %19 = extractvalue %dx.types.CBufRet.f32 %17, 1
  %20 = extractvalue %dx.types.CBufRet.f32 %17, 2
  %21 = extractvalue %dx.types.CBufRet.f32 %17, 3
  %22 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %23 = extractvalue %dx.types.CBufRet.f32 %22, 0
  %24 = extractvalue %dx.types.CBufRet.f32 %22, 1
  %25 = extractvalue %dx.types.CBufRet.f32 %22, 2
  %26 = extractvalue %dx.types.CBufRet.f32 %22, 3
  %27 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %28 = extractvalue %dx.types.CBufRet.f32 %27, 0
  %29 = extractvalue %dx.types.CBufRet.f32 %27, 1
  %30 = extractvalue %dx.types.CBufRet.f32 %27, 2
  %31 = extractvalue %dx.types.CBufRet.f32 %27, 3
  %32 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %33 = extractvalue %dx.types.CBufRet.f32 %32, 0
  %34 = extractvalue %dx.types.CBufRet.f32 %32, 1
  %35 = extractvalue %dx.types.CBufRet.f32 %32, 2
  %36 = extractvalue %dx.types.CBufRet.f32 %32, 3
  %37 = fmul fast float %18, %12
  %38 = call float @dx.op.tertiary.f32(i32 46, float %23, float %13, float %37)  ; FMad(a,b,c)
  %39 = call float @dx.op.tertiary.f32(i32 46, float %28, float %14, float %38)  ; FMad(a,b,c)
  %40 = fadd fast float %39, %33
  %41 = fmul fast float %19, %12
  %42 = call float @dx.op.tertiary.f32(i32 46, float %24, float %13, float %41)  ; FMad(a,b,c)
  %43 = call float @dx.op.tertiary.f32(i32 46, float %29, float %14, float %42)  ; FMad(a,b,c)
  %44 = fadd fast float %43, %34
  %45 = fmul fast float %20, %12
  %46 = call float @dx.op.tertiary.f32(i32 46, float %25, float %13, float %45)  ; FMad(a,b,c)
  %47 = call float @dx.op.tertiary.f32(i32 46, float %30, float %14, float %46)  ; FMad(a,b,c)
  %48 = fadd fast float %47, %35
  %49 = fmul fast float %21, %12
  %50 = call float @dx.op.tertiary.f32(i32 46, float %26, float %13, float %49)  ; FMad(a,b,c)
  %51 = call float @dx.op.tertiary.f32(i32 46, float %31, float %14, float %50)  ; FMad(a,b,c)
  %52 = fadd fast float %51, %36
  %53 = call %dx.types.fouri32 @dx.op.unpack4x8.i32(i32 219, i8 0, i32 %16)  ; Unpack4x8(unpackMode,pk)
  %54 = extractvalue %dx.types.fouri32 %53, 0
  %55 = extractvalue %dx.types.fouri32 %53, 1
  %56 = extractvalue %dx.types.fouri32 %53, 2
  %57 = extractvalue %dx.types.fouri32 %53, 3
  %58 = uitofp i32 %54 to float
  %59 = uitofp i32 %55 to float
  %60 = uitofp i32 %56 to float
  %61 = uitofp i32 %57 to float
  %62 = fmul fast float %58, 0x3F70101020000000
  %63 = fmul fast float %59, 0x3F70101020000000
  %64 = fmul fast float %60, 0x3F70101020000000
  %65 = fmul fast float %61, 0x3F70101020000000
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %40)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %44)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %48)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %52)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %62)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %63)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 2, float %64)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 3, float %65)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
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
declare %dx.types.ResRet.i32 @dx.op.rawBufferLoad.i32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readnone
declare %dx.types.fouri32 @dx.op.unpack4x8.i32(i32, i8, i32) #0

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
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2407.7 (416fab6b5)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 8, null}
!6 = !{[7 x i32] [i32 5, i32 8, i32 255, i32 0, i32 0, i32 0, i32 0]}
!7 = !{void ()* @draw_vs, !"draw_vs", !8, !3, !18}
!8 = !{!9, !14, null}
!9 = !{!10, !13}
!10 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !11, i8 0, i32 1, i8 1, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 1}
!13 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !11, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!14 = !{!15, !17}
!15 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 4, i32 1, i8 0, !16}
!18 = !{i32 0, i64 1082130688}
*/
auto Shaders::debug_draw_draw_vs() const -> Span<const std::byte> {
    return Span(_data).subspan(9792, 5128);
}

// shader_hash: fbe5f57464d4b02a60afdfab97c43f1a
// input_parameters: 2
// output_parameters: 1
// instruction_count: 9
/* disassembly:
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xyzw        1     NONE   float   xyzw
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: fbe5f57464d4b02a60afdfab97c43f1a.pdb
; shader hash: fbe5f57464d4b02a60afdfab97c43f1a
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
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
;
;
; ViewId state:
;
; Number of inputs: 8, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 4 }
;   output 1 depends on inputs: { 5 }
;   output 2 depends on inputs: { 6 }
;   output 3 depends on inputs: { 7 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

define void @draw_ps() {
  %1 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %2 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 3, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %1)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %2)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %3)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %4)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.viewIdState = !{!3}
!dx.entryPoints = !{!4}

!0 = !{!"dxcoob 1.8.2407.7 (416fab6b5)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{[10 x i32] [i32 8, i32 4, i32 0, i32 0, i32 0, i32 0, i32 1, i32 2, i32 4, i32 8]}
!4 = !{void ()* @draw_ps, !"draw_ps", !5, null, !13}
!5 = !{!6, !11, null}
!6 = !{!7, !9}
!7 = !{i32 0, !"SV_Position", i8 9, i8 3, !8, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!8 = !{i32 0}
!9 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !8, i8 2, i32 1, i8 4, i32 1, i8 0, !10}
!10 = !{i32 3, i32 15}
!11 = !{!12}
!12 = !{i32 0, !"SV_Target", i8 9, i8 16, !8, i8 0, i32 1, i8 4, i32 0, i8 0, !10}
!13 = !{i32 0, i64 8388864}
*/
auto Shaders::debug_draw_draw_ps() const -> Span<const std::byte> {
    return Span(_data).subspan(14920, 3068);
}

// shader_hash: a1dcc597e997b635f78cc6e2d532990a
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 1532
// temp_array_count: 2048
// dynamic_flow_control_count: 30
// array_instruction_count: 76
// float_instruction_count: 449
// int_instruction_count: 347
// uint_instruction_count: 121
// texture_normal_instructions: 4
// texture_load_instructions: 16
// barrier_instructions: 9
// interlocked_instructions: 1
// texture_store_instructions: 22
/* disassembly:
;
; Note: shader requires additional functionality:
;       Typed UAV Load Additional Formats
;       Wave level operations
;       Use native low precision
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
; shader debug name: a1dcc597e997b635f78cc6e2d532990a.pdb
; shader hash: a1dcc597e997b635f78cc6e2d532990a
;
; Pipeline Runtime Information: 
;
; Compute Shader
; NumThreads=(256,1,1)
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
;           uint atomics;                             ; Offset:    4
;           uint texture_src;                         ; Offset:    8
;           uint texture_mid;                         ; Offset:   12
;           uint texture_dst_begin;                   ; Offset:   16
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

@"\01?gs_counter@@3IA" = external addrspace(3) global i32, align 4
@"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim" = addrspace(3) global [512 x half] undef, align 2
@"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim" = addrspace(3) global [512 x half] undef, align 2

define void @downsample_cs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 20 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.groupId.i32(i32 94, i32 0)  ; GroupId(component)
  %4 = call i32 @dx.op.groupId.i32(i32 94, i32 1)  ; GroupId(component)
  %5 = call i32 @dx.op.groupId.i32(i32 94, i32 2)  ; GroupId(component)
  %6 = call i32 @dx.op.flattenedThreadIdInGroup.i32(i32 96)  ; FlattenedThreadIdInGroup()
  %7 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %8 = extractvalue %dx.types.CBufRet.i32 %7, 1
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %8, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 21004, i32 24 })  ; AnnotateHandle(res,props)  resource: globallycoherent RWStructuredBuffer<stride=24>
  %11 = extractvalue %dx.types.CBufRet.i32 %7, 0
  %12 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %11, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %13 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %12, %dx.types.ResourceProperties { i32 13, i32 964 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %14 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %13, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %15 = extractvalue %dx.types.CBufRet.i32 %14, 0
  %16 = extractvalue %dx.types.CBufRet.i32 %14, 1
  %17 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %13, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %18 = extractvalue %dx.types.CBufRet.f32 %17, 2
  %19 = extractvalue %dx.types.CBufRet.f32 %17, 3
  %20 = and i32 %6, 1
  %21 = lshr i32 %6, 1
  %22 = and i32 %21, 3
  %23 = lshr i32 %6, 2
  %24 = and i32 %23, 6
  %25 = lshr i32 %6, 3
  %26 = and i32 %25, 4
  %27 = or i32 %24, %20
  %28 = or i32 %22, %26
  %29 = and i32 %25, 8
  %30 = or i32 %27, %29
  %31 = lshr i32 %6, 7
  %32 = shl nuw nsw i32 %31, 3
  %33 = or i32 %28, %32
  %34 = shl nuw nsw i32 %30, 1
  %35 = shl nuw nsw i32 %33, 1
  %36 = shl i32 %3, 6
  %37 = shl i32 %4, 6
  %38 = or i32 %34, %36
  %39 = add i32 %35, %37
  %40 = shl i32 %3, 5
  %41 = shl i32 %4, 5
  %42 = or i32 %30, %40
  %43 = add i32 %33, %41
  %44 = extractvalue %dx.types.CBufRet.i32 %7, 2
  %45 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %44, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %46 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %45, %dx.types.ResourceProperties { i32 7, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2DArray<4xF32>
  %47 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 0, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %48 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %47, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %49 = uitofp i32 %38 to float
  %50 = uitofp i32 %39 to float
  %51 = fmul fast float %18, %49
  %52 = fmul fast float %19, %50
  %53 = fadd fast float %51, %18
  %54 = fadd fast float %52, %19
  %55 = uitofp i32 %5 to float
  %56 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %46, %dx.types.Handle %48, float %53, float %54, float %55, float undef, i32 0, i32 0, i32 undef, float 0.000000e+00)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %57 = extractvalue %dx.types.ResRet.f32 %56, 0
  %58 = extractvalue %dx.types.ResRet.f32 %56, 1
  %59 = extractvalue %dx.types.ResRet.f32 %56, 2
  %60 = extractvalue %dx.types.ResRet.f32 %56, 3
  %61 = fptrunc float %57 to half
  %62 = fptrunc float %58 to half
  %63 = fptrunc float %59 to half
  %64 = fptrunc float %60 to half
  %65 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %66 = extractvalue %dx.types.CBufRet.i32 %65, 0
  %67 = add i32 %66, 1
  %68 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %67, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %69 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %68, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %70 = fpext half %61 to float
  %71 = fpext half %62 to float
  %72 = fpext half %63 to float
  %73 = fpext half %64 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %69, i32 %42, i32 %43, i32 %5, float %70, float %71, float %72, float %73, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %74 = or i32 %38, 32
  %75 = or i32 %42, 16
  %76 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %77 = extractvalue %dx.types.CBufRet.i32 %76, 2
  %78 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %77, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %79 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %78, %dx.types.ResourceProperties { i32 7, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2DArray<4xF32>
  %80 = uitofp i32 %74 to float
  %81 = fmul fast float %18, %80
  %82 = fadd fast float %81, %18
  %83 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %79, %dx.types.Handle %48, float %82, float %54, float %55, float undef, i32 0, i32 0, i32 undef, float 0.000000e+00)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %84 = extractvalue %dx.types.ResRet.f32 %83, 0
  %85 = extractvalue %dx.types.ResRet.f32 %83, 1
  %86 = extractvalue %dx.types.ResRet.f32 %83, 2
  %87 = extractvalue %dx.types.ResRet.f32 %83, 3
  %88 = fptrunc float %84 to half
  %89 = fptrunc float %85 to half
  %90 = fptrunc float %86 to half
  %91 = fptrunc float %87 to half
  %92 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %93 = extractvalue %dx.types.CBufRet.i32 %92, 0
  %94 = add i32 %93, 1
  %95 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %94, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %96 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %95, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %97 = fpext half %88 to float
  %98 = fpext half %89 to float
  %99 = fpext half %90 to float
  %100 = fpext half %91 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %96, i32 %75, i32 %43, i32 %5, float %97, float %98, float %99, float %100, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %101 = or i32 %37, 32
  %102 = add i32 %35, %101
  %103 = add i32 %43, 16
  %104 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %105 = extractvalue %dx.types.CBufRet.i32 %104, 2
  %106 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %105, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %107 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %106, %dx.types.ResourceProperties { i32 7, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2DArray<4xF32>
  %108 = uitofp i32 %102 to float
  %109 = fmul fast float %19, %108
  %110 = fadd fast float %109, %19
  %111 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %107, %dx.types.Handle %48, float %53, float %110, float %55, float undef, i32 0, i32 0, i32 undef, float 0.000000e+00)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %112 = extractvalue %dx.types.ResRet.f32 %111, 0
  %113 = extractvalue %dx.types.ResRet.f32 %111, 1
  %114 = extractvalue %dx.types.ResRet.f32 %111, 2
  %115 = extractvalue %dx.types.ResRet.f32 %111, 3
  %116 = fptrunc float %112 to half
  %117 = fptrunc float %113 to half
  %118 = fptrunc float %114 to half
  %119 = fptrunc float %115 to half
  %120 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %121 = extractvalue %dx.types.CBufRet.i32 %120, 0
  %122 = add i32 %121, 1
  %123 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %122, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %124 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %123, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %125 = fpext half %116 to float
  %126 = fpext half %117 to float
  %127 = fpext half %118 to float
  %128 = fpext half %119 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %124, i32 %42, i32 %103, i32 %5, float %125, float %126, float %127, float %128, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %129 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %130 = extractvalue %dx.types.CBufRet.i32 %129, 2
  %131 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %130, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %132 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %131, %dx.types.ResourceProperties { i32 7, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2DArray<4xF32>
  %133 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %132, %dx.types.Handle %48, float %82, float %110, float %55, float undef, i32 0, i32 0, i32 undef, float 0.000000e+00)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %134 = extractvalue %dx.types.ResRet.f32 %133, 0
  %135 = extractvalue %dx.types.ResRet.f32 %133, 1
  %136 = extractvalue %dx.types.ResRet.f32 %133, 2
  %137 = extractvalue %dx.types.ResRet.f32 %133, 3
  %138 = fptrunc float %134 to half
  %139 = fptrunc float %135 to half
  %140 = fptrunc float %136 to half
  %141 = fptrunc float %137 to half
  %142 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %143 = extractvalue %dx.types.CBufRet.i32 %142, 0
  %144 = add i32 %143, 1
  %145 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %144, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %146 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %145, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %147 = fpext half %138 to float
  %148 = fpext half %139 to float
  %149 = fpext half %140 to float
  %150 = fpext half %141 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %146, i32 %75, i32 %103, i32 %5, float %147, float %148, float %149, float %150, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %151 = icmp ult i32 %15, 2
  br i1 %151, label %1455, label %152

; <label>:152                                     ; preds = %0
  %153 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %154 = and i32 %153, -4
  %155 = or i32 %154, 1
  %156 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %61, i32 %155)  ; WaveReadLaneAt(value,lane)
  %157 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %62, i32 %155)  ; WaveReadLaneAt(value,lane)
  %158 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %63, i32 %155)  ; WaveReadLaneAt(value,lane)
  %159 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %64, i32 %155)  ; WaveReadLaneAt(value,lane)
  %160 = or i32 %154, 2
  %161 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %61, i32 %160)  ; WaveReadLaneAt(value,lane)
  %162 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %62, i32 %160)  ; WaveReadLaneAt(value,lane)
  %163 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %63, i32 %160)  ; WaveReadLaneAt(value,lane)
  %164 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %64, i32 %160)  ; WaveReadLaneAt(value,lane)
  %165 = or i32 %153, 3
  %166 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %61, i32 %165)  ; WaveReadLaneAt(value,lane)
  %167 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %62, i32 %165)  ; WaveReadLaneAt(value,lane)
  %168 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %63, i32 %165)  ; WaveReadLaneAt(value,lane)
  %169 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %64, i32 %165)  ; WaveReadLaneAt(value,lane)
  %170 = fadd fast half %156, %61
  %171 = fadd fast half %157, %62
  %172 = fadd fast half %158, %63
  %173 = fadd fast half %159, %64
  %174 = fadd fast half %170, %161
  %175 = fadd fast half %171, %162
  %176 = fadd fast half %172, %163
  %177 = fadd fast half %173, %164
  %178 = fadd fast half %174, %166
  %179 = fadd fast half %175, %167
  %180 = fadd fast half %176, %168
  %181 = fadd fast half %177, %169
  %182 = fmul fast half %178, 0xH3400
  %183 = fmul fast half %179, 0xH3400
  %184 = fmul fast half %180, 0xH3400
  %185 = fmul fast half %181, 0xH3400
  %186 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %187 = and i32 %186, -4
  %188 = or i32 %187, 1
  %189 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %88, i32 %188)  ; WaveReadLaneAt(value,lane)
  %190 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %89, i32 %188)  ; WaveReadLaneAt(value,lane)
  %191 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %90, i32 %188)  ; WaveReadLaneAt(value,lane)
  %192 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %91, i32 %188)  ; WaveReadLaneAt(value,lane)
  %193 = or i32 %187, 2
  %194 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %88, i32 %193)  ; WaveReadLaneAt(value,lane)
  %195 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %89, i32 %193)  ; WaveReadLaneAt(value,lane)
  %196 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %90, i32 %193)  ; WaveReadLaneAt(value,lane)
  %197 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %91, i32 %193)  ; WaveReadLaneAt(value,lane)
  %198 = or i32 %186, 3
  %199 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %88, i32 %198)  ; WaveReadLaneAt(value,lane)
  %200 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %89, i32 %198)  ; WaveReadLaneAt(value,lane)
  %201 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %90, i32 %198)  ; WaveReadLaneAt(value,lane)
  %202 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %91, i32 %198)  ; WaveReadLaneAt(value,lane)
  %203 = fadd fast half %189, %88
  %204 = fadd fast half %190, %89
  %205 = fadd fast half %191, %90
  %206 = fadd fast half %192, %91
  %207 = fadd fast half %203, %194
  %208 = fadd fast half %204, %195
  %209 = fadd fast half %205, %196
  %210 = fadd fast half %206, %197
  %211 = fadd fast half %207, %199
  %212 = fadd fast half %208, %200
  %213 = fadd fast half %209, %201
  %214 = fadd fast half %210, %202
  %215 = fmul fast half %211, 0xH3400
  %216 = fmul fast half %212, 0xH3400
  %217 = fmul fast half %213, 0xH3400
  %218 = fmul fast half %214, 0xH3400
  %219 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %220 = and i32 %219, -4
  %221 = or i32 %220, 1
  %222 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %116, i32 %221)  ; WaveReadLaneAt(value,lane)
  %223 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %117, i32 %221)  ; WaveReadLaneAt(value,lane)
  %224 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %118, i32 %221)  ; WaveReadLaneAt(value,lane)
  %225 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %119, i32 %221)  ; WaveReadLaneAt(value,lane)
  %226 = or i32 %220, 2
  %227 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %116, i32 %226)  ; WaveReadLaneAt(value,lane)
  %228 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %117, i32 %226)  ; WaveReadLaneAt(value,lane)
  %229 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %118, i32 %226)  ; WaveReadLaneAt(value,lane)
  %230 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %119, i32 %226)  ; WaveReadLaneAt(value,lane)
  %231 = or i32 %219, 3
  %232 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %116, i32 %231)  ; WaveReadLaneAt(value,lane)
  %233 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %117, i32 %231)  ; WaveReadLaneAt(value,lane)
  %234 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %118, i32 %231)  ; WaveReadLaneAt(value,lane)
  %235 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %119, i32 %231)  ; WaveReadLaneAt(value,lane)
  %236 = fadd fast half %222, %116
  %237 = fadd fast half %223, %117
  %238 = fadd fast half %224, %118
  %239 = fadd fast half %225, %119
  %240 = fadd fast half %236, %227
  %241 = fadd fast half %237, %228
  %242 = fadd fast half %238, %229
  %243 = fadd fast half %239, %230
  %244 = fadd fast half %240, %232
  %245 = fadd fast half %241, %233
  %246 = fadd fast half %242, %234
  %247 = fadd fast half %243, %235
  %248 = fmul fast half %244, 0xH3400
  %249 = fmul fast half %245, 0xH3400
  %250 = fmul fast half %246, 0xH3400
  %251 = fmul fast half %247, 0xH3400
  %252 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %253 = and i32 %252, -4
  %254 = or i32 %253, 1
  %255 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %138, i32 %254)  ; WaveReadLaneAt(value,lane)
  %256 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %139, i32 %254)  ; WaveReadLaneAt(value,lane)
  %257 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %140, i32 %254)  ; WaveReadLaneAt(value,lane)
  %258 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %141, i32 %254)  ; WaveReadLaneAt(value,lane)
  %259 = or i32 %253, 2
  %260 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %138, i32 %259)  ; WaveReadLaneAt(value,lane)
  %261 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %139, i32 %259)  ; WaveReadLaneAt(value,lane)
  %262 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %140, i32 %259)  ; WaveReadLaneAt(value,lane)
  %263 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %141, i32 %259)  ; WaveReadLaneAt(value,lane)
  %264 = or i32 %252, 3
  %265 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %138, i32 %264)  ; WaveReadLaneAt(value,lane)
  %266 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %139, i32 %264)  ; WaveReadLaneAt(value,lane)
  %267 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %140, i32 %264)  ; WaveReadLaneAt(value,lane)
  %268 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %141, i32 %264)  ; WaveReadLaneAt(value,lane)
  %269 = fadd fast half %255, %138
  %270 = fadd fast half %256, %139
  %271 = fadd fast half %257, %140
  %272 = fadd fast half %258, %141
  %273 = fadd fast half %269, %260
  %274 = fadd fast half %270, %261
  %275 = fadd fast half %271, %262
  %276 = fadd fast half %272, %263
  %277 = fadd fast half %273, %265
  %278 = fadd fast half %274, %266
  %279 = fadd fast half %275, %267
  %280 = fadd fast half %276, %268
  %281 = fmul fast half %277, 0xH3400
  %282 = fmul fast half %278, 0xH3400
  %283 = fmul fast half %279, 0xH3400
  %284 = fmul fast half %280, 0xH3400
  %285 = and i32 %6, 3
  %286 = icmp eq i32 %285, 0
  br i1 %286, label %287, label %415

; <label>:287                                     ; preds = %152
  %288 = lshr i32 %30, 1
  %289 = lshr i32 %33, 1
  %290 = shl i32 %3, 4
  %291 = shl i32 %4, 4
  %292 = or i32 %288, %290
  %293 = add i32 %289, %291
  %294 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %295 = extractvalue %dx.types.CBufRet.i32 %294, 0
  %296 = add i32 %295, 2
  %297 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %296, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %298 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %297, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %299 = fpext half %182 to float
  %300 = fpext half %183 to float
  %301 = fpext half %184 to float
  %302 = fpext half %185 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %298, i32 %292, i32 %293, i32 %5, float %299, float %300, float %301, float %302, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %303 = mul i32 %288, 16
  %304 = add i32 %289, %303
  %305 = mul i32 %304, 2
  %306 = add i32 0, %305
  %307 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %306
  store half %182, half addrspace(3)* %307, align 2
  %308 = mul i32 %288, 16
  %309 = add i32 %289, %308
  %310 = mul i32 %309, 2
  %311 = add i32 1, %310
  %312 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %311
  store half %183, half addrspace(3)* %312, align 2
  %313 = mul i32 %288, 16
  %314 = add i32 %289, %313
  %315 = mul i32 %314, 2
  %316 = add i32 0, %315
  %317 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %316
  store half %184, half addrspace(3)* %317, align 2
  %318 = mul i32 %288, 16
  %319 = add i32 %289, %318
  %320 = mul i32 %319, 2
  %321 = add i32 1, %320
  %322 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %321
  store half %185, half addrspace(3)* %322, align 2
  %323 = or i32 %292, 8
  %324 = or i32 %288, 8
  %325 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %326 = extractvalue %dx.types.CBufRet.i32 %325, 0
  %327 = add i32 %326, 2
  %328 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %327, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %329 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %328, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %330 = fpext half %215 to float
  %331 = fpext half %216 to float
  %332 = fpext half %217 to float
  %333 = fpext half %218 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %329, i32 %323, i32 %293, i32 %5, float %330, float %331, float %332, float %333, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %334 = mul i32 %324, 16
  %335 = add i32 %289, %334
  %336 = mul i32 %335, 2
  %337 = add i32 0, %336
  %338 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %337
  store half %215, half addrspace(3)* %338, align 2
  %339 = mul i32 %324, 16
  %340 = add i32 %289, %339
  %341 = mul i32 %340, 2
  %342 = add i32 1, %341
  %343 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %342
  store half %216, half addrspace(3)* %343, align 2
  %344 = mul i32 %324, 16
  %345 = add i32 %289, %344
  %346 = mul i32 %345, 2
  %347 = add i32 0, %346
  %348 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %347
  store half %217, half addrspace(3)* %348, align 2
  %349 = mul i32 %324, 16
  %350 = add i32 %289, %349
  %351 = mul i32 %350, 2
  %352 = add i32 1, %351
  %353 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %352
  store half %218, half addrspace(3)* %353, align 2
  %354 = or i32 %291, 8
  %355 = add i32 %289, %354
  %356 = add nuw nsw i32 %289, 8
  %357 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %358 = extractvalue %dx.types.CBufRet.i32 %357, 0
  %359 = add i32 %358, 2
  %360 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %359, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %361 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %360, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %362 = fpext half %248 to float
  %363 = fpext half %249 to float
  %364 = fpext half %250 to float
  %365 = fpext half %251 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %361, i32 %292, i32 %355, i32 %5, float %362, float %363, float %364, float %365, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %366 = mul i32 %288, 16
  %367 = add i32 %356, %366
  %368 = mul i32 %367, 2
  %369 = add i32 0, %368
  %370 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %369
  store half %248, half addrspace(3)* %370, align 2
  %371 = mul i32 %288, 16
  %372 = add i32 %356, %371
  %373 = mul i32 %372, 2
  %374 = add i32 1, %373
  %375 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %374
  store half %249, half addrspace(3)* %375, align 2
  %376 = mul i32 %288, 16
  %377 = add i32 %356, %376
  %378 = mul i32 %377, 2
  %379 = add i32 0, %378
  %380 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %379
  store half %250, half addrspace(3)* %380, align 2
  %381 = mul i32 %288, 16
  %382 = add i32 %356, %381
  %383 = mul i32 %382, 2
  %384 = add i32 1, %383
  %385 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %384
  store half %251, half addrspace(3)* %385, align 2
  %386 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %387 = extractvalue %dx.types.CBufRet.i32 %386, 0
  %388 = add i32 %387, 2
  %389 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %388, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %390 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %389, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %391 = fpext half %281 to float
  %392 = fpext half %282 to float
  %393 = fpext half %283 to float
  %394 = fpext half %284 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %390, i32 %323, i32 %355, i32 %5, float %391, float %392, float %393, float %394, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %395 = mul i32 %324, 16
  %396 = add i32 %356, %395
  %397 = mul i32 %396, 2
  %398 = add i32 0, %397
  %399 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %398
  store half %281, half addrspace(3)* %399, align 2
  %400 = mul i32 %324, 16
  %401 = add i32 %356, %400
  %402 = mul i32 %401, 2
  %403 = add i32 1, %402
  %404 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %403
  store half %282, half addrspace(3)* %404, align 2
  %405 = mul i32 %324, 16
  %406 = add i32 %356, %405
  %407 = mul i32 %406, 2
  %408 = add i32 0, %407
  %409 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %408
  store half %283, half addrspace(3)* %409, align 2
  %410 = mul i32 %324, 16
  %411 = add i32 %356, %410
  %412 = mul i32 %411, 2
  %413 = add i32 1, %412
  %414 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %413
  store half %284, half addrspace(3)* %414, align 2
  br label %415

; <label>:415                                     ; preds = %287, %152
  %416 = icmp ult i32 %15, 3
  br i1 %416, label %1455, label %417

; <label>:417                                     ; preds = %415
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  %418 = icmp ult i32 %6, 256
  br i1 %418, label %419, label %515

; <label>:419                                     ; preds = %417
  %420 = mul i32 %30, 16
  %421 = add i32 %33, %420
  %422 = mul i32 %421, 2
  %423 = add i32 0, %422
  %424 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %423
  %425 = load half, half addrspace(3)* %424, align 2
  %426 = mul i32 %30, 16
  %427 = add i32 %33, %426
  %428 = mul i32 %427, 2
  %429 = add i32 1, %428
  %430 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %429
  %431 = load half, half addrspace(3)* %430, align 2
  %432 = mul i32 %30, 16
  %433 = add i32 %33, %432
  %434 = mul i32 %433, 2
  %435 = add i32 0, %434
  %436 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %435
  %437 = load half, half addrspace(3)* %436, align 2
  %438 = mul i32 %30, 16
  %439 = add i32 %33, %438
  %440 = mul i32 %439, 2
  %441 = add i32 1, %440
  %442 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %441
  %443 = load half, half addrspace(3)* %442, align 2
  %444 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %445 = and i32 %444, -4
  %446 = or i32 %445, 1
  %447 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %425, i32 %446)  ; WaveReadLaneAt(value,lane)
  %448 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %431, i32 %446)  ; WaveReadLaneAt(value,lane)
  %449 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %437, i32 %446)  ; WaveReadLaneAt(value,lane)
  %450 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %443, i32 %446)  ; WaveReadLaneAt(value,lane)
  %451 = or i32 %445, 2
  %452 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %425, i32 %451)  ; WaveReadLaneAt(value,lane)
  %453 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %431, i32 %451)  ; WaveReadLaneAt(value,lane)
  %454 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %437, i32 %451)  ; WaveReadLaneAt(value,lane)
  %455 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %443, i32 %451)  ; WaveReadLaneAt(value,lane)
  %456 = or i32 %444, 3
  %457 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %425, i32 %456)  ; WaveReadLaneAt(value,lane)
  %458 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %431, i32 %456)  ; WaveReadLaneAt(value,lane)
  %459 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %437, i32 %456)  ; WaveReadLaneAt(value,lane)
  %460 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %443, i32 %456)  ; WaveReadLaneAt(value,lane)
  %461 = fadd fast half %447, %425
  %462 = fadd fast half %448, %431
  %463 = fadd fast half %449, %437
  %464 = fadd fast half %450, %443
  %465 = fadd fast half %461, %452
  %466 = fadd fast half %462, %453
  %467 = fadd fast half %463, %454
  %468 = fadd fast half %464, %455
  %469 = fadd fast half %465, %457
  %470 = fadd fast half %466, %458
  %471 = fadd fast half %467, %459
  %472 = fadd fast half %468, %460
  %473 = fmul fast half %469, 0xH3400
  %474 = fmul fast half %470, 0xH3400
  %475 = fmul fast half %471, 0xH3400
  %476 = fmul fast half %472, 0xH3400
  br i1 %286, label %477, label %515

; <label>:477                                     ; preds = %419
  %478 = lshr i32 %30, 1
  %479 = lshr i32 %33, 1
  %480 = shl i32 %3, 3
  %481 = shl i32 %4, 3
  %482 = or i32 %478, %480
  %483 = add i32 %479, %481
  %484 = and i32 %23, 1
  %485 = add nuw nsw i32 %30, %484
  %486 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %487 = extractvalue %dx.types.CBufRet.i32 %486, 0
  %488 = add i32 %487, 3
  %489 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %488, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %490 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %489, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %491 = fpext half %473 to float
  %492 = fpext half %474 to float
  %493 = fpext half %475 to float
  %494 = fpext half %476 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %490, i32 %482, i32 %483, i32 %5, float %491, float %492, float %493, float %494, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %495 = mul i32 %485, 16
  %496 = add i32 %33, %495
  %497 = mul i32 %496, 2
  %498 = add i32 0, %497
  %499 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %498
  store half %473, half addrspace(3)* %499, align 2
  %500 = mul i32 %485, 16
  %501 = add i32 %33, %500
  %502 = mul i32 %501, 2
  %503 = add i32 1, %502
  %504 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %503
  store half %474, half addrspace(3)* %504, align 2
  %505 = mul i32 %485, 16
  %506 = add i32 %33, %505
  %507 = mul i32 %506, 2
  %508 = add i32 0, %507
  %509 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %508
  store half %475, half addrspace(3)* %509, align 2
  %510 = mul i32 %485, 16
  %511 = add i32 %33, %510
  %512 = mul i32 %511, 2
  %513 = add i32 1, %512
  %514 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %513
  store half %476, half addrspace(3)* %514, align 2
  br label %515

; <label>:515                                     ; preds = %477, %419, %417
  %516 = icmp ult i32 %15, 4
  br i1 %516, label %1455, label %517

; <label>:517                                     ; preds = %515
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  %518 = icmp ult i32 %6, 64
  br i1 %518, label %519, label %616

; <label>:519                                     ; preds = %517
  %520 = and i32 %21, 1
  %521 = or i32 %34, %520
  %522 = mul i32 %521, 16
  %523 = add i32 %35, %522
  %524 = mul i32 %523, 2
  %525 = add i32 0, %524
  %526 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %525
  %527 = load half, half addrspace(3)* %526, align 2
  %528 = mul i32 %521, 16
  %529 = add i32 %35, %528
  %530 = mul i32 %529, 2
  %531 = add i32 1, %530
  %532 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %531
  %533 = load half, half addrspace(3)* %532, align 2
  %534 = mul i32 %521, 16
  %535 = add i32 %35, %534
  %536 = mul i32 %535, 2
  %537 = add i32 0, %536
  %538 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %537
  %539 = load half, half addrspace(3)* %538, align 2
  %540 = mul i32 %521, 16
  %541 = add i32 %35, %540
  %542 = mul i32 %541, 2
  %543 = add i32 1, %542
  %544 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %543
  %545 = load half, half addrspace(3)* %544, align 2
  %546 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %547 = and i32 %546, -4
  %548 = or i32 %547, 1
  %549 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %527, i32 %548)  ; WaveReadLaneAt(value,lane)
  %550 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %533, i32 %548)  ; WaveReadLaneAt(value,lane)
  %551 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %539, i32 %548)  ; WaveReadLaneAt(value,lane)
  %552 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %545, i32 %548)  ; WaveReadLaneAt(value,lane)
  %553 = or i32 %547, 2
  %554 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %527, i32 %553)  ; WaveReadLaneAt(value,lane)
  %555 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %533, i32 %553)  ; WaveReadLaneAt(value,lane)
  %556 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %539, i32 %553)  ; WaveReadLaneAt(value,lane)
  %557 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %545, i32 %553)  ; WaveReadLaneAt(value,lane)
  %558 = or i32 %546, 3
  %559 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %527, i32 %558)  ; WaveReadLaneAt(value,lane)
  %560 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %533, i32 %558)  ; WaveReadLaneAt(value,lane)
  %561 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %539, i32 %558)  ; WaveReadLaneAt(value,lane)
  %562 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %545, i32 %558)  ; WaveReadLaneAt(value,lane)
  %563 = fadd fast half %549, %527
  %564 = fadd fast half %550, %533
  %565 = fadd fast half %551, %539
  %566 = fadd fast half %552, %545
  %567 = fadd fast half %563, %554
  %568 = fadd fast half %564, %555
  %569 = fadd fast half %565, %556
  %570 = fadd fast half %566, %557
  %571 = fadd fast half %567, %559
  %572 = fadd fast half %568, %560
  %573 = fadd fast half %569, %561
  %574 = fadd fast half %570, %562
  %575 = fmul fast half %571, 0xH3400
  %576 = fmul fast half %572, 0xH3400
  %577 = fmul fast half %573, 0xH3400
  %578 = fmul fast half %574, 0xH3400
  br i1 %286, label %579, label %616

; <label>:579                                     ; preds = %519
  %580 = lshr i32 %30, 1
  %581 = lshr i32 %33, 1
  %582 = shl i32 %3, 2
  %583 = shl i32 %4, 2
  %584 = add i32 %580, %582
  %585 = add i32 %581, %583
  %586 = add nuw nsw i32 %34, %581
  %587 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %588 = extractvalue %dx.types.CBufRet.i32 %587, 0
  %589 = add i32 %588, 4
  %590 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %589, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %591 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %590, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %592 = fpext half %575 to float
  %593 = fpext half %576 to float
  %594 = fpext half %577 to float
  %595 = fpext half %578 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %591, i32 %584, i32 %585, i32 %5, float %592, float %593, float %594, float %595, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %596 = mul i32 %586, 16
  %597 = add i32 %35, %596
  %598 = mul i32 %597, 2
  %599 = add i32 0, %598
  %600 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %599
  store half %575, half addrspace(3)* %600, align 2
  %601 = mul i32 %586, 16
  %602 = add i32 %35, %601
  %603 = mul i32 %602, 2
  %604 = add i32 1, %603
  %605 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %604
  store half %576, half addrspace(3)* %605, align 2
  %606 = mul i32 %586, 16
  %607 = add i32 %35, %606
  %608 = mul i32 %607, 2
  %609 = add i32 0, %608
  %610 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %609
  store half %577, half addrspace(3)* %610, align 2
  %611 = mul i32 %586, 16
  %612 = add i32 %35, %611
  %613 = mul i32 %612, 2
  %614 = add i32 1, %613
  %615 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %614
  store half %578, half addrspace(3)* %615, align 2
  br label %616

; <label>:616                                     ; preds = %579, %519, %517
  %617 = icmp ult i32 %15, 5
  br i1 %617, label %1455, label %618

; <label>:618                                     ; preds = %616
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  %619 = icmp ult i32 %6, 16
  br i1 %619, label %620, label %718

; <label>:620                                     ; preds = %618
  %621 = shl nuw nsw i32 %30, 2
  %622 = add nuw nsw i32 %621, %33
  %623 = shl nuw nsw i32 %33, 2
  %624 = mul i32 %622, 16
  %625 = add i32 %623, %624
  %626 = mul i32 %625, 2
  %627 = add i32 0, %626
  %628 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %627
  %629 = load half, half addrspace(3)* %628, align 2
  %630 = mul i32 %622, 16
  %631 = add i32 %623, %630
  %632 = mul i32 %631, 2
  %633 = add i32 1, %632
  %634 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %633
  %635 = load half, half addrspace(3)* %634, align 2
  %636 = mul i32 %622, 16
  %637 = add i32 %623, %636
  %638 = mul i32 %637, 2
  %639 = add i32 0, %638
  %640 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %639
  %641 = load half, half addrspace(3)* %640, align 2
  %642 = mul i32 %622, 16
  %643 = add i32 %623, %642
  %644 = mul i32 %643, 2
  %645 = add i32 1, %644
  %646 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %645
  %647 = load half, half addrspace(3)* %646, align 2
  %648 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %649 = and i32 %648, -4
  %650 = or i32 %649, 1
  %651 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %629, i32 %650)  ; WaveReadLaneAt(value,lane)
  %652 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %635, i32 %650)  ; WaveReadLaneAt(value,lane)
  %653 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %641, i32 %650)  ; WaveReadLaneAt(value,lane)
  %654 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %647, i32 %650)  ; WaveReadLaneAt(value,lane)
  %655 = or i32 %649, 2
  %656 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %629, i32 %655)  ; WaveReadLaneAt(value,lane)
  %657 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %635, i32 %655)  ; WaveReadLaneAt(value,lane)
  %658 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %641, i32 %655)  ; WaveReadLaneAt(value,lane)
  %659 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %647, i32 %655)  ; WaveReadLaneAt(value,lane)
  %660 = or i32 %648, 3
  %661 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %629, i32 %660)  ; WaveReadLaneAt(value,lane)
  %662 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %635, i32 %660)  ; WaveReadLaneAt(value,lane)
  %663 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %641, i32 %660)  ; WaveReadLaneAt(value,lane)
  %664 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %647, i32 %660)  ; WaveReadLaneAt(value,lane)
  %665 = fadd fast half %651, %629
  %666 = fadd fast half %652, %635
  %667 = fadd fast half %653, %641
  %668 = fadd fast half %654, %647
  %669 = fadd fast half %665, %656
  %670 = fadd fast half %666, %657
  %671 = fadd fast half %667, %658
  %672 = fadd fast half %668, %659
  %673 = fadd fast half %669, %661
  %674 = fadd fast half %670, %662
  %675 = fadd fast half %671, %663
  %676 = fadd fast half %672, %664
  %677 = fmul fast half %673, 0xH3400
  %678 = fmul fast half %674, 0xH3400
  %679 = fmul fast half %675, 0xH3400
  %680 = fmul fast half %676, 0xH3400
  br i1 %286, label %681, label %718

; <label>:681                                     ; preds = %620
  %682 = lshr i32 %30, 1
  %683 = lshr i32 %33, 1
  %684 = shl i32 %3, 1
  %685 = shl i32 %4, 1
  %686 = add i32 %682, %684
  %687 = add i32 %683, %685
  %688 = add nuw nsw i32 %682, %33
  %689 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %690 = extractvalue %dx.types.CBufRet.i32 %689, 0
  %691 = add i32 %690, 5
  %692 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %691, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %693 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %692, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %694 = fpext half %677 to float
  %695 = fpext half %678 to float
  %696 = fpext half %679 to float
  %697 = fpext half %680 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %693, i32 %686, i32 %687, i32 %5, float %694, float %695, float %696, float %697, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %698 = mul i32 %688, 16
  %699 = add i32 0, %698
  %700 = mul i32 %699, 2
  %701 = add i32 0, %700
  %702 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %701
  store half %677, half addrspace(3)* %702, align 2
  %703 = mul i32 %688, 16
  %704 = add i32 0, %703
  %705 = mul i32 %704, 2
  %706 = add i32 1, %705
  %707 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %706
  store half %678, half addrspace(3)* %707, align 2
  %708 = mul i32 %688, 16
  %709 = add i32 0, %708
  %710 = mul i32 %709, 2
  %711 = add i32 0, %710
  %712 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %711
  store half %679, half addrspace(3)* %712, align 2
  %713 = mul i32 %688, 16
  %714 = add i32 0, %713
  %715 = mul i32 %714, 2
  %716 = add i32 1, %715
  %717 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %716
  store half %680, half addrspace(3)* %717, align 2
  br label %718

; <label>:718                                     ; preds = %681, %620, %618
  %719 = icmp ult i32 %15, 6
  br i1 %719, label %1455, label %720

; <label>:720                                     ; preds = %718
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  %721 = icmp ult i32 %6, 4
  br i1 %721, label %722, label %789

; <label>:722                                     ; preds = %720
  %723 = mul i32 %6, 16
  %724 = add i32 0, %723
  %725 = mul i32 %724, 2
  %726 = add i32 0, %725
  %727 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %726
  %728 = load half, half addrspace(3)* %727, align 2
  %729 = mul i32 %6, 16
  %730 = add i32 0, %729
  %731 = mul i32 %730, 2
  %732 = add i32 1, %731
  %733 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %732
  %734 = load half, half addrspace(3)* %733, align 2
  %735 = mul i32 %6, 16
  %736 = add i32 0, %735
  %737 = mul i32 %736, 2
  %738 = add i32 0, %737
  %739 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %738
  %740 = load half, half addrspace(3)* %739, align 2
  %741 = mul i32 %6, 16
  %742 = add i32 0, %741
  %743 = mul i32 %742, 2
  %744 = add i32 1, %743
  %745 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %744
  %746 = load half, half addrspace(3)* %745, align 2
  %747 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %748 = and i32 %747, -4
  %749 = or i32 %748, 1
  %750 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %728, i32 %749)  ; WaveReadLaneAt(value,lane)
  %751 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %734, i32 %749)  ; WaveReadLaneAt(value,lane)
  %752 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %740, i32 %749)  ; WaveReadLaneAt(value,lane)
  %753 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %746, i32 %749)  ; WaveReadLaneAt(value,lane)
  %754 = or i32 %748, 2
  %755 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %728, i32 %754)  ; WaveReadLaneAt(value,lane)
  %756 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %734, i32 %754)  ; WaveReadLaneAt(value,lane)
  %757 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %740, i32 %754)  ; WaveReadLaneAt(value,lane)
  %758 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %746, i32 %754)  ; WaveReadLaneAt(value,lane)
  %759 = or i32 %747, 3
  %760 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %728, i32 %759)  ; WaveReadLaneAt(value,lane)
  %761 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %734, i32 %759)  ; WaveReadLaneAt(value,lane)
  %762 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %740, i32 %759)  ; WaveReadLaneAt(value,lane)
  %763 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %746, i32 %759)  ; WaveReadLaneAt(value,lane)
  br i1 %286, label %764, label %789

; <label>:764                                     ; preds = %722
  %765 = fadd fast half %753, %746
  %766 = fadd fast half %765, %758
  %767 = fadd fast half %766, %763
  %768 = fmul fast half %767, 0xH3400
  %769 = fadd fast half %752, %740
  %770 = fadd fast half %769, %757
  %771 = fadd fast half %770, %762
  %772 = fmul fast half %771, 0xH3400
  %773 = fadd fast half %751, %734
  %774 = fadd fast half %773, %756
  %775 = fadd fast half %774, %761
  %776 = fmul fast half %775, 0xH3400
  %777 = fadd fast half %750, %728
  %778 = fadd fast half %777, %755
  %779 = fadd fast half %778, %760
  %780 = fmul fast half %779, 0xH3400
  %781 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %782 = extractvalue %dx.types.CBufRet.i32 %781, 3
  %783 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %782, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %784 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %783, %dx.types.ResourceProperties { i32 20487, i32 1033 })  ; AnnotateHandle(res,props)  resource: globallycoherent RWTexture2DArray<4xF32>
  %785 = fpext half %780 to float
  %786 = fpext half %776 to float
  %787 = fpext half %772 to float
  %788 = fpext half %768 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %784, i32 %3, i32 %4, i32 %5, float %785, float %786, float %787, float %788, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  br label %789

; <label>:789                                     ; preds = %764, %722, %720
  %790 = icmp ult i32 %15, 7
  br i1 %790, label %1455, label %791

; <label>:791                                     ; preds = %789
  %792 = icmp eq i32 %6, 0
  br i1 %792, label %793, label %796

; <label>:793                                     ; preds = %791
  %794 = shl i32 %5, 2
  %795 = call i32 @dx.op.atomicBinOp.i32(i32 78, %dx.types.Handle %10, i32 0, i32 0, i32 %794, i32 undef, i32 1)  ; AtomicBinOp(handle,atomicOp,offset0,offset1,offset2,newValue)
  store i32 %795, i32 addrspace(3)* @"\01?gs_counter@@3IA", align 4, !tbaa !9
  br label %796

; <label>:796                                     ; preds = %793, %791
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  %797 = load i32, i32 addrspace(3)* @"\01?gs_counter@@3IA", align 4, !tbaa !9
  %798 = add i32 %16, -1
  %799 = icmp eq i32 %797, %798
  br i1 %799, label %800, label %1455

; <label>:800                                     ; preds = %796
  %801 = shl i32 %5, 2
  call void @dx.op.rawBufferStore.i32(i32 140, %dx.types.Handle %10, i32 0, i32 %801, i32 0, i32 undef, i32 undef, i32 undef, i8 1, i32 4)  ; RawBufferStore(uav,index,elementOffset,value0,value1,value2,value3,mask,alignment)
  %802 = shl nuw nsw i32 %30, 2
  %803 = shl nuw nsw i32 %33, 2
  %804 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %805 = extractvalue %dx.types.CBufRet.i32 %804, 3
  %806 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %805, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %807 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %806, %dx.types.ResourceProperties { i32 20487, i32 1033 })  ; AnnotateHandle(res,props)  resource: globallycoherent RWTexture2DArray<4xF32>
  %808 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %807, i32 undef, i32 %802, i32 %803, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %809 = extractvalue %dx.types.ResRet.f32 %808, 0
  %810 = extractvalue %dx.types.ResRet.f32 %808, 1
  %811 = extractvalue %dx.types.ResRet.f32 %808, 2
  %812 = extractvalue %dx.types.ResRet.f32 %808, 3
  %813 = fptrunc float %809 to half
  %814 = fptrunc float %810 to half
  %815 = fptrunc float %811 to half
  %816 = fptrunc float %812 to half
  %817 = or i32 %803, 1
  %818 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %807, i32 undef, i32 %802, i32 %817, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %819 = extractvalue %dx.types.ResRet.f32 %818, 0
  %820 = extractvalue %dx.types.ResRet.f32 %818, 1
  %821 = extractvalue %dx.types.ResRet.f32 %818, 2
  %822 = extractvalue %dx.types.ResRet.f32 %818, 3
  %823 = fptrunc float %819 to half
  %824 = fptrunc float %820 to half
  %825 = fptrunc float %821 to half
  %826 = fptrunc float %822 to half
  %827 = or i32 %802, 1
  %828 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %807, i32 undef, i32 %827, i32 %803, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %829 = extractvalue %dx.types.ResRet.f32 %828, 0
  %830 = extractvalue %dx.types.ResRet.f32 %828, 1
  %831 = extractvalue %dx.types.ResRet.f32 %828, 2
  %832 = extractvalue %dx.types.ResRet.f32 %828, 3
  %833 = fptrunc float %829 to half
  %834 = fptrunc float %830 to half
  %835 = fptrunc float %831 to half
  %836 = fptrunc float %832 to half
  %837 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %807, i32 undef, i32 %827, i32 %817, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %838 = extractvalue %dx.types.ResRet.f32 %837, 0
  %839 = extractvalue %dx.types.ResRet.f32 %837, 1
  %840 = extractvalue %dx.types.ResRet.f32 %837, 2
  %841 = extractvalue %dx.types.ResRet.f32 %837, 3
  %842 = fptrunc float %838 to half
  %843 = fptrunc float %839 to half
  %844 = fptrunc float %840 to half
  %845 = fptrunc float %841 to half
  %846 = fadd fast half %823, %813
  %847 = fadd fast half %824, %814
  %848 = fadd fast half %825, %815
  %849 = fadd fast half %826, %816
  %850 = fadd fast half %846, %833
  %851 = fadd fast half %847, %834
  %852 = fadd fast half %848, %835
  %853 = fadd fast half %849, %836
  %854 = fadd fast half %850, %842
  %855 = fadd fast half %851, %843
  %856 = fadd fast half %852, %844
  %857 = fadd fast half %853, %845
  %858 = fmul fast half %854, 0xH3400
  %859 = fmul fast half %855, 0xH3400
  %860 = fmul fast half %856, 0xH3400
  %861 = fmul fast half %857, 0xH3400
  %862 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %863 = extractvalue %dx.types.CBufRet.i32 %862, 0
  %864 = add i32 %863, 7
  %865 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %864, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %866 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %865, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %867 = fpext half %858 to float
  %868 = fpext half %859 to float
  %869 = fpext half %860 to float
  %870 = fpext half %861 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %866, i32 %34, i32 %35, i32 %5, float %867, float %868, float %869, float %870, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %871 = or i32 %802, 2
  %872 = or i32 %34, 1
  %873 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %874 = extractvalue %dx.types.CBufRet.i32 %873, 3
  %875 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %874, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %876 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %875, %dx.types.ResourceProperties { i32 20487, i32 1033 })  ; AnnotateHandle(res,props)  resource: globallycoherent RWTexture2DArray<4xF32>
  %877 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %876, i32 undef, i32 %871, i32 %803, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %878 = extractvalue %dx.types.ResRet.f32 %877, 0
  %879 = extractvalue %dx.types.ResRet.f32 %877, 1
  %880 = extractvalue %dx.types.ResRet.f32 %877, 2
  %881 = extractvalue %dx.types.ResRet.f32 %877, 3
  %882 = fptrunc float %878 to half
  %883 = fptrunc float %879 to half
  %884 = fptrunc float %880 to half
  %885 = fptrunc float %881 to half
  %886 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %876, i32 undef, i32 %871, i32 %817, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %887 = extractvalue %dx.types.ResRet.f32 %886, 0
  %888 = extractvalue %dx.types.ResRet.f32 %886, 1
  %889 = extractvalue %dx.types.ResRet.f32 %886, 2
  %890 = extractvalue %dx.types.ResRet.f32 %886, 3
  %891 = fptrunc float %887 to half
  %892 = fptrunc float %888 to half
  %893 = fptrunc float %889 to half
  %894 = fptrunc float %890 to half
  %895 = or i32 %802, 3
  %896 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %876, i32 undef, i32 %895, i32 %803, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %897 = extractvalue %dx.types.ResRet.f32 %896, 0
  %898 = extractvalue %dx.types.ResRet.f32 %896, 1
  %899 = extractvalue %dx.types.ResRet.f32 %896, 2
  %900 = extractvalue %dx.types.ResRet.f32 %896, 3
  %901 = fptrunc float %897 to half
  %902 = fptrunc float %898 to half
  %903 = fptrunc float %899 to half
  %904 = fptrunc float %900 to half
  %905 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %876, i32 undef, i32 %895, i32 %817, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %906 = extractvalue %dx.types.ResRet.f32 %905, 0
  %907 = extractvalue %dx.types.ResRet.f32 %905, 1
  %908 = extractvalue %dx.types.ResRet.f32 %905, 2
  %909 = extractvalue %dx.types.ResRet.f32 %905, 3
  %910 = fptrunc float %906 to half
  %911 = fptrunc float %907 to half
  %912 = fptrunc float %908 to half
  %913 = fptrunc float %909 to half
  %914 = fadd fast half %891, %882
  %915 = fadd fast half %892, %883
  %916 = fadd fast half %893, %884
  %917 = fadd fast half %894, %885
  %918 = fadd fast half %914, %901
  %919 = fadd fast half %915, %902
  %920 = fadd fast half %916, %903
  %921 = fadd fast half %917, %904
  %922 = fadd fast half %918, %910
  %923 = fadd fast half %919, %911
  %924 = fadd fast half %920, %912
  %925 = fadd fast half %921, %913
  %926 = fmul fast half %922, 0xH3400
  %927 = fmul fast half %923, 0xH3400
  %928 = fmul fast half %924, 0xH3400
  %929 = fmul fast half %925, 0xH3400
  %930 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %931 = extractvalue %dx.types.CBufRet.i32 %930, 0
  %932 = add i32 %931, 7
  %933 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %932, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %934 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %933, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %935 = fpext half %926 to float
  %936 = fpext half %927 to float
  %937 = fpext half %928 to float
  %938 = fpext half %929 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %934, i32 %872, i32 %35, i32 %5, float %935, float %936, float %937, float %938, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %939 = or i32 %803, 2
  %940 = or i32 %35, 1
  %941 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %942 = extractvalue %dx.types.CBufRet.i32 %941, 3
  %943 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %942, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %944 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %943, %dx.types.ResourceProperties { i32 20487, i32 1033 })  ; AnnotateHandle(res,props)  resource: globallycoherent RWTexture2DArray<4xF32>
  %945 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %944, i32 undef, i32 %802, i32 %939, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %946 = extractvalue %dx.types.ResRet.f32 %945, 0
  %947 = extractvalue %dx.types.ResRet.f32 %945, 1
  %948 = extractvalue %dx.types.ResRet.f32 %945, 2
  %949 = extractvalue %dx.types.ResRet.f32 %945, 3
  %950 = fptrunc float %946 to half
  %951 = fptrunc float %947 to half
  %952 = fptrunc float %948 to half
  %953 = fptrunc float %949 to half
  %954 = or i32 %803, 3
  %955 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %944, i32 undef, i32 %802, i32 %954, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %956 = extractvalue %dx.types.ResRet.f32 %955, 0
  %957 = extractvalue %dx.types.ResRet.f32 %955, 1
  %958 = extractvalue %dx.types.ResRet.f32 %955, 2
  %959 = extractvalue %dx.types.ResRet.f32 %955, 3
  %960 = fptrunc float %956 to half
  %961 = fptrunc float %957 to half
  %962 = fptrunc float %958 to half
  %963 = fptrunc float %959 to half
  %964 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %944, i32 undef, i32 %827, i32 %939, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %965 = extractvalue %dx.types.ResRet.f32 %964, 0
  %966 = extractvalue %dx.types.ResRet.f32 %964, 1
  %967 = extractvalue %dx.types.ResRet.f32 %964, 2
  %968 = extractvalue %dx.types.ResRet.f32 %964, 3
  %969 = fptrunc float %965 to half
  %970 = fptrunc float %966 to half
  %971 = fptrunc float %967 to half
  %972 = fptrunc float %968 to half
  %973 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %944, i32 undef, i32 %827, i32 %954, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %974 = extractvalue %dx.types.ResRet.f32 %973, 0
  %975 = extractvalue %dx.types.ResRet.f32 %973, 1
  %976 = extractvalue %dx.types.ResRet.f32 %973, 2
  %977 = extractvalue %dx.types.ResRet.f32 %973, 3
  %978 = fptrunc float %974 to half
  %979 = fptrunc float %975 to half
  %980 = fptrunc float %976 to half
  %981 = fptrunc float %977 to half
  %982 = fadd fast half %960, %950
  %983 = fadd fast half %961, %951
  %984 = fadd fast half %962, %952
  %985 = fadd fast half %963, %953
  %986 = fadd fast half %982, %969
  %987 = fadd fast half %983, %970
  %988 = fadd fast half %984, %971
  %989 = fadd fast half %985, %972
  %990 = fadd fast half %986, %978
  %991 = fadd fast half %987, %979
  %992 = fadd fast half %988, %980
  %993 = fadd fast half %989, %981
  %994 = fmul fast half %990, 0xH3400
  %995 = fmul fast half %991, 0xH3400
  %996 = fmul fast half %992, 0xH3400
  %997 = fmul fast half %993, 0xH3400
  %998 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %999 = extractvalue %dx.types.CBufRet.i32 %998, 0
  %1000 = add i32 %999, 7
  %1001 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %1000, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %1002 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1001, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %1003 = fpext half %994 to float
  %1004 = fpext half %995 to float
  %1005 = fpext half %996 to float
  %1006 = fpext half %997 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %1002, i32 %34, i32 %940, i32 %5, float %1003, float %1004, float %1005, float %1006, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %1007 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %1008 = extractvalue %dx.types.CBufRet.i32 %1007, 3
  %1009 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %1008, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %1010 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1009, %dx.types.ResourceProperties { i32 20487, i32 1033 })  ; AnnotateHandle(res,props)  resource: globallycoherent RWTexture2DArray<4xF32>
  %1011 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %1010, i32 undef, i32 %871, i32 %939, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %1012 = extractvalue %dx.types.ResRet.f32 %1011, 0
  %1013 = extractvalue %dx.types.ResRet.f32 %1011, 1
  %1014 = extractvalue %dx.types.ResRet.f32 %1011, 2
  %1015 = extractvalue %dx.types.ResRet.f32 %1011, 3
  %1016 = fptrunc float %1012 to half
  %1017 = fptrunc float %1013 to half
  %1018 = fptrunc float %1014 to half
  %1019 = fptrunc float %1015 to half
  %1020 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %1010, i32 undef, i32 %871, i32 %954, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %1021 = extractvalue %dx.types.ResRet.f32 %1020, 0
  %1022 = extractvalue %dx.types.ResRet.f32 %1020, 1
  %1023 = extractvalue %dx.types.ResRet.f32 %1020, 2
  %1024 = extractvalue %dx.types.ResRet.f32 %1020, 3
  %1025 = fptrunc float %1021 to half
  %1026 = fptrunc float %1022 to half
  %1027 = fptrunc float %1023 to half
  %1028 = fptrunc float %1024 to half
  %1029 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %1010, i32 undef, i32 %895, i32 %939, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %1030 = extractvalue %dx.types.ResRet.f32 %1029, 0
  %1031 = extractvalue %dx.types.ResRet.f32 %1029, 1
  %1032 = extractvalue %dx.types.ResRet.f32 %1029, 2
  %1033 = extractvalue %dx.types.ResRet.f32 %1029, 3
  %1034 = fptrunc float %1030 to half
  %1035 = fptrunc float %1031 to half
  %1036 = fptrunc float %1032 to half
  %1037 = fptrunc float %1033 to half
  %1038 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %1010, i32 undef, i32 %895, i32 %954, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %1039 = extractvalue %dx.types.ResRet.f32 %1038, 0
  %1040 = extractvalue %dx.types.ResRet.f32 %1038, 1
  %1041 = extractvalue %dx.types.ResRet.f32 %1038, 2
  %1042 = extractvalue %dx.types.ResRet.f32 %1038, 3
  %1043 = fptrunc float %1039 to half
  %1044 = fptrunc float %1040 to half
  %1045 = fptrunc float %1041 to half
  %1046 = fptrunc float %1042 to half
  %1047 = fadd fast half %1025, %1016
  %1048 = fadd fast half %1026, %1017
  %1049 = fadd fast half %1027, %1018
  %1050 = fadd fast half %1028, %1019
  %1051 = fadd fast half %1047, %1034
  %1052 = fadd fast half %1048, %1035
  %1053 = fadd fast half %1049, %1036
  %1054 = fadd fast half %1050, %1037
  %1055 = fadd fast half %1051, %1043
  %1056 = fadd fast half %1052, %1044
  %1057 = fadd fast half %1053, %1045
  %1058 = fadd fast half %1054, %1046
  %1059 = fmul fast half %1055, 0xH3400
  %1060 = fmul fast half %1056, 0xH3400
  %1061 = fmul fast half %1057, 0xH3400
  %1062 = fmul fast half %1058, 0xH3400
  %1063 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %1064 = extractvalue %dx.types.CBufRet.i32 %1063, 0
  %1065 = add i32 %1064, 7
  %1066 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %1065, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %1067 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1066, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %1068 = fpext half %1059 to float
  %1069 = fpext half %1060 to float
  %1070 = fpext half %1061 to float
  %1071 = fpext half %1062 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %1067, i32 %872, i32 %940, i32 %5, float %1068, float %1069, float %1070, float %1071, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %1072 = icmp ult i32 %15, 8
  br i1 %1072, label %1455, label %1073

; <label>:1073                                    ; preds = %800
  %1074 = fadd fast half %922, %854
  %1075 = fadd fast half %923, %855
  %1076 = fadd fast half %924, %856
  %1077 = fadd fast half %925, %857
  %1078 = fadd fast half %1074, %990
  %1079 = fadd fast half %1075, %991
  %1080 = fadd fast half %1076, %992
  %1081 = fadd fast half %1077, %993
  %1082 = fadd fast half %1078, %1055
  %1083 = fadd fast half %1079, %1056
  %1084 = fadd fast half %1080, %1057
  %1085 = fadd fast half %1081, %1058
  %1086 = fmul fast half %1082, 0xH2C00
  %1087 = fmul fast half %1083, 0xH2C00
  %1088 = fmul fast half %1084, 0xH2C00
  %1089 = fmul fast half %1085, 0xH2C00
  %1090 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %1091 = extractvalue %dx.types.CBufRet.i32 %1090, 0
  %1092 = add i32 %1091, 8
  %1093 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %1092, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %1094 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1093, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %1095 = fpext half %1086 to float
  %1096 = fpext half %1087 to float
  %1097 = fpext half %1088 to float
  %1098 = fpext half %1089 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %1094, i32 %30, i32 %33, i32 %5, float %1095, float %1096, float %1097, float %1098, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %1099 = mul i32 %30, 16
  %1100 = add i32 %33, %1099
  %1101 = mul i32 %1100, 2
  %1102 = add i32 0, %1101
  %1103 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1102
  store half %1086, half addrspace(3)* %1103, align 2
  %1104 = mul i32 %30, 16
  %1105 = add i32 %33, %1104
  %1106 = mul i32 %1105, 2
  %1107 = add i32 1, %1106
  %1108 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1107
  store half %1087, half addrspace(3)* %1108, align 2
  %1109 = mul i32 %30, 16
  %1110 = add i32 %33, %1109
  %1111 = mul i32 %1110, 2
  %1112 = add i32 0, %1111
  %1113 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1112
  store half %1088, half addrspace(3)* %1113, align 2
  %1114 = mul i32 %30, 16
  %1115 = add i32 %33, %1114
  %1116 = mul i32 %1115, 2
  %1117 = add i32 1, %1116
  %1118 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1117
  store half %1089, half addrspace(3)* %1118, align 2
  %1119 = icmp ult i32 %15, 9
  br i1 %1119, label %1455, label %1120

; <label>:1120                                    ; preds = %1073
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  br i1 %418, label %1121, label %1193

; <label>:1121                                    ; preds = %1120
  %1122 = load half, half addrspace(3)* %1103, align 2
  %1123 = load half, half addrspace(3)* %1108, align 2
  %1124 = load half, half addrspace(3)* %1113, align 2
  %1125 = load half, half addrspace(3)* %1118, align 2
  %1126 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %1127 = and i32 %1126, -4
  %1128 = or i32 %1127, 1
  %1129 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1122, i32 %1128)  ; WaveReadLaneAt(value,lane)
  %1130 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1123, i32 %1128)  ; WaveReadLaneAt(value,lane)
  %1131 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1124, i32 %1128)  ; WaveReadLaneAt(value,lane)
  %1132 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1125, i32 %1128)  ; WaveReadLaneAt(value,lane)
  %1133 = or i32 %1127, 2
  %1134 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1122, i32 %1133)  ; WaveReadLaneAt(value,lane)
  %1135 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1123, i32 %1133)  ; WaveReadLaneAt(value,lane)
  %1136 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1124, i32 %1133)  ; WaveReadLaneAt(value,lane)
  %1137 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1125, i32 %1133)  ; WaveReadLaneAt(value,lane)
  %1138 = or i32 %1126, 3
  %1139 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1122, i32 %1138)  ; WaveReadLaneAt(value,lane)
  %1140 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1123, i32 %1138)  ; WaveReadLaneAt(value,lane)
  %1141 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1124, i32 %1138)  ; WaveReadLaneAt(value,lane)
  %1142 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1125, i32 %1138)  ; WaveReadLaneAt(value,lane)
  %1143 = fadd fast half %1129, %1122
  %1144 = fadd fast half %1130, %1123
  %1145 = fadd fast half %1131, %1124
  %1146 = fadd fast half %1132, %1125
  %1147 = fadd fast half %1143, %1134
  %1148 = fadd fast half %1144, %1135
  %1149 = fadd fast half %1145, %1136
  %1150 = fadd fast half %1146, %1137
  %1151 = fadd fast half %1147, %1139
  %1152 = fadd fast half %1148, %1140
  %1153 = fadd fast half %1149, %1141
  %1154 = fadd fast half %1150, %1142
  %1155 = fmul fast half %1151, 0xH3400
  %1156 = fmul fast half %1152, 0xH3400
  %1157 = fmul fast half %1153, 0xH3400
  %1158 = fmul fast half %1154, 0xH3400
  br i1 %286, label %1159, label %1193

; <label>:1159                                    ; preds = %1121
  %1160 = lshr i32 %30, 1
  %1161 = lshr i32 %33, 1
  %1162 = and i32 %23, 1
  %1163 = add nuw nsw i32 %30, %1162
  %1164 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %1165 = extractvalue %dx.types.CBufRet.i32 %1164, 0
  %1166 = add i32 %1165, 9
  %1167 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %1166, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %1168 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1167, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %1169 = fpext half %1155 to float
  %1170 = fpext half %1156 to float
  %1171 = fpext half %1157 to float
  %1172 = fpext half %1158 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %1168, i32 %1160, i32 %1161, i32 %5, float %1169, float %1170, float %1171, float %1172, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %1173 = mul i32 %1163, 16
  %1174 = add i32 %33, %1173
  %1175 = mul i32 %1174, 2
  %1176 = add i32 0, %1175
  %1177 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1176
  store half %1155, half addrspace(3)* %1177, align 2
  %1178 = mul i32 %1163, 16
  %1179 = add i32 %33, %1178
  %1180 = mul i32 %1179, 2
  %1181 = add i32 1, %1180
  %1182 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1181
  store half %1156, half addrspace(3)* %1182, align 2
  %1183 = mul i32 %1163, 16
  %1184 = add i32 %33, %1183
  %1185 = mul i32 %1184, 2
  %1186 = add i32 0, %1185
  %1187 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1186
  store half %1157, half addrspace(3)* %1187, align 2
  %1188 = mul i32 %1163, 16
  %1189 = add i32 %33, %1188
  %1190 = mul i32 %1189, 2
  %1191 = add i32 1, %1190
  %1192 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1191
  store half %1158, half addrspace(3)* %1192, align 2
  br label %1193

; <label>:1193                                    ; preds = %1159, %1121, %1120
  %1194 = icmp ult i32 %15, 10
  br i1 %1194, label %1455, label %1195

; <label>:1195                                    ; preds = %1193
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  br i1 %518, label %1196, label %1289

; <label>:1196                                    ; preds = %1195
  %1197 = and i32 %21, 1
  %1198 = or i32 %34, %1197
  %1199 = mul i32 %1198, 16
  %1200 = add i32 %35, %1199
  %1201 = mul i32 %1200, 2
  %1202 = add i32 0, %1201
  %1203 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1202
  %1204 = load half, half addrspace(3)* %1203, align 2
  %1205 = mul i32 %1198, 16
  %1206 = add i32 %35, %1205
  %1207 = mul i32 %1206, 2
  %1208 = add i32 1, %1207
  %1209 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1208
  %1210 = load half, half addrspace(3)* %1209, align 2
  %1211 = mul i32 %1198, 16
  %1212 = add i32 %35, %1211
  %1213 = mul i32 %1212, 2
  %1214 = add i32 0, %1213
  %1215 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1214
  %1216 = load half, half addrspace(3)* %1215, align 2
  %1217 = mul i32 %1198, 16
  %1218 = add i32 %35, %1217
  %1219 = mul i32 %1218, 2
  %1220 = add i32 1, %1219
  %1221 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1220
  %1222 = load half, half addrspace(3)* %1221, align 2
  %1223 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %1224 = and i32 %1223, -4
  %1225 = or i32 %1224, 1
  %1226 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1204, i32 %1225)  ; WaveReadLaneAt(value,lane)
  %1227 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1210, i32 %1225)  ; WaveReadLaneAt(value,lane)
  %1228 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1216, i32 %1225)  ; WaveReadLaneAt(value,lane)
  %1229 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1222, i32 %1225)  ; WaveReadLaneAt(value,lane)
  %1230 = or i32 %1224, 2
  %1231 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1204, i32 %1230)  ; WaveReadLaneAt(value,lane)
  %1232 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1210, i32 %1230)  ; WaveReadLaneAt(value,lane)
  %1233 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1216, i32 %1230)  ; WaveReadLaneAt(value,lane)
  %1234 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1222, i32 %1230)  ; WaveReadLaneAt(value,lane)
  %1235 = or i32 %1223, 3
  %1236 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1204, i32 %1235)  ; WaveReadLaneAt(value,lane)
  %1237 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1210, i32 %1235)  ; WaveReadLaneAt(value,lane)
  %1238 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1216, i32 %1235)  ; WaveReadLaneAt(value,lane)
  %1239 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1222, i32 %1235)  ; WaveReadLaneAt(value,lane)
  %1240 = fadd fast half %1226, %1204
  %1241 = fadd fast half %1227, %1210
  %1242 = fadd fast half %1228, %1216
  %1243 = fadd fast half %1229, %1222
  %1244 = fadd fast half %1240, %1231
  %1245 = fadd fast half %1241, %1232
  %1246 = fadd fast half %1242, %1233
  %1247 = fadd fast half %1243, %1234
  %1248 = fadd fast half %1244, %1236
  %1249 = fadd fast half %1245, %1237
  %1250 = fadd fast half %1246, %1238
  %1251 = fadd fast half %1247, %1239
  %1252 = fmul fast half %1248, 0xH3400
  %1253 = fmul fast half %1249, 0xH3400
  %1254 = fmul fast half %1250, 0xH3400
  %1255 = fmul fast half %1251, 0xH3400
  br i1 %286, label %1256, label %1289

; <label>:1256                                    ; preds = %1196
  %1257 = lshr i32 %30, 1
  %1258 = lshr i32 %33, 1
  %1259 = add nuw nsw i32 %34, %1258
  %1260 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %1261 = extractvalue %dx.types.CBufRet.i32 %1260, 0
  %1262 = add i32 %1261, 10
  %1263 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %1262, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %1264 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1263, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %1265 = fpext half %1252 to float
  %1266 = fpext half %1253 to float
  %1267 = fpext half %1254 to float
  %1268 = fpext half %1255 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %1264, i32 %1257, i32 %1258, i32 %5, float %1265, float %1266, float %1267, float %1268, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %1269 = mul i32 %1259, 16
  %1270 = add i32 %35, %1269
  %1271 = mul i32 %1270, 2
  %1272 = add i32 0, %1271
  %1273 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1272
  store half %1252, half addrspace(3)* %1273, align 2
  %1274 = mul i32 %1259, 16
  %1275 = add i32 %35, %1274
  %1276 = mul i32 %1275, 2
  %1277 = add i32 1, %1276
  %1278 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1277
  store half %1253, half addrspace(3)* %1278, align 2
  %1279 = mul i32 %1259, 16
  %1280 = add i32 %35, %1279
  %1281 = mul i32 %1280, 2
  %1282 = add i32 0, %1281
  %1283 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1282
  store half %1254, half addrspace(3)* %1283, align 2
  %1284 = mul i32 %1259, 16
  %1285 = add i32 %35, %1284
  %1286 = mul i32 %1285, 2
  %1287 = add i32 1, %1286
  %1288 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1287
  store half %1255, half addrspace(3)* %1288, align 2
  br label %1289

; <label>:1289                                    ; preds = %1256, %1196, %1195
  %1290 = icmp ult i32 %15, 11
  br i1 %1290, label %1455, label %1291

; <label>:1291                                    ; preds = %1289
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  br i1 %619, label %1292, label %1384

; <label>:1292                                    ; preds = %1291
  %1293 = add nuw nsw i32 %802, %33
  %1294 = mul i32 %1293, 16
  %1295 = add i32 %803, %1294
  %1296 = mul i32 %1295, 2
  %1297 = add i32 0, %1296
  %1298 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1297
  %1299 = load half, half addrspace(3)* %1298, align 2
  %1300 = mul i32 %1293, 16
  %1301 = add i32 %803, %1300
  %1302 = mul i32 %1301, 2
  %1303 = add i32 1, %1302
  %1304 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1303
  %1305 = load half, half addrspace(3)* %1304, align 2
  %1306 = mul i32 %1293, 16
  %1307 = add i32 %803, %1306
  %1308 = mul i32 %1307, 2
  %1309 = add i32 0, %1308
  %1310 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1309
  %1311 = load half, half addrspace(3)* %1310, align 2
  %1312 = mul i32 %1293, 16
  %1313 = add i32 %803, %1312
  %1314 = mul i32 %1313, 2
  %1315 = add i32 1, %1314
  %1316 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1315
  %1317 = load half, half addrspace(3)* %1316, align 2
  %1318 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %1319 = and i32 %1318, -4
  %1320 = or i32 %1319, 1
  %1321 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1299, i32 %1320)  ; WaveReadLaneAt(value,lane)
  %1322 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1305, i32 %1320)  ; WaveReadLaneAt(value,lane)
  %1323 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1311, i32 %1320)  ; WaveReadLaneAt(value,lane)
  %1324 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1317, i32 %1320)  ; WaveReadLaneAt(value,lane)
  %1325 = or i32 %1319, 2
  %1326 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1299, i32 %1325)  ; WaveReadLaneAt(value,lane)
  %1327 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1305, i32 %1325)  ; WaveReadLaneAt(value,lane)
  %1328 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1311, i32 %1325)  ; WaveReadLaneAt(value,lane)
  %1329 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1317, i32 %1325)  ; WaveReadLaneAt(value,lane)
  %1330 = or i32 %1318, 3
  %1331 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1299, i32 %1330)  ; WaveReadLaneAt(value,lane)
  %1332 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1305, i32 %1330)  ; WaveReadLaneAt(value,lane)
  %1333 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1311, i32 %1330)  ; WaveReadLaneAt(value,lane)
  %1334 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1317, i32 %1330)  ; WaveReadLaneAt(value,lane)
  %1335 = fadd fast half %1321, %1299
  %1336 = fadd fast half %1322, %1305
  %1337 = fadd fast half %1323, %1311
  %1338 = fadd fast half %1324, %1317
  %1339 = fadd fast half %1335, %1326
  %1340 = fadd fast half %1336, %1327
  %1341 = fadd fast half %1337, %1328
  %1342 = fadd fast half %1338, %1329
  %1343 = fadd fast half %1339, %1331
  %1344 = fadd fast half %1340, %1332
  %1345 = fadd fast half %1341, %1333
  %1346 = fadd fast half %1342, %1334
  %1347 = fmul fast half %1343, 0xH3400
  %1348 = fmul fast half %1344, 0xH3400
  %1349 = fmul fast half %1345, 0xH3400
  %1350 = fmul fast half %1346, 0xH3400
  br i1 %286, label %1351, label %1384

; <label>:1351                                    ; preds = %1292
  %1352 = lshr i32 %30, 1
  %1353 = lshr i32 %33, 1
  %1354 = add nuw nsw i32 %1352, %33
  %1355 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %1356 = extractvalue %dx.types.CBufRet.i32 %1355, 0
  %1357 = add i32 %1356, 11
  %1358 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %1357, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %1359 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1358, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %1360 = fpext half %1347 to float
  %1361 = fpext half %1348 to float
  %1362 = fpext half %1349 to float
  %1363 = fpext half %1350 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %1359, i32 %1352, i32 %1353, i32 %5, float %1360, float %1361, float %1362, float %1363, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %1364 = mul i32 %1354, 16
  %1365 = add i32 0, %1364
  %1366 = mul i32 %1365, 2
  %1367 = add i32 0, %1366
  %1368 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1367
  store half %1347, half addrspace(3)* %1368, align 2
  %1369 = mul i32 %1354, 16
  %1370 = add i32 0, %1369
  %1371 = mul i32 %1370, 2
  %1372 = add i32 1, %1371
  %1373 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1372
  store half %1348, half addrspace(3)* %1373, align 2
  %1374 = mul i32 %1354, 16
  %1375 = add i32 0, %1374
  %1376 = mul i32 %1375, 2
  %1377 = add i32 0, %1376
  %1378 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1377
  store half %1349, half addrspace(3)* %1378, align 2
  %1379 = mul i32 %1354, 16
  %1380 = add i32 0, %1379
  %1381 = mul i32 %1380, 2
  %1382 = add i32 1, %1381
  %1383 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1382
  store half %1350, half addrspace(3)* %1383, align 2
  br label %1384

; <label>:1384                                    ; preds = %1351, %1292, %1291
  %1385 = icmp ult i32 %15, 12
  br i1 %1385, label %1455, label %1386

; <label>:1386                                    ; preds = %1384
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  br i1 %721, label %1387, label %1455

; <label>:1387                                    ; preds = %1386
  %1388 = mul i32 %6, 16
  %1389 = add i32 0, %1388
  %1390 = mul i32 %1389, 2
  %1391 = add i32 0, %1390
  %1392 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1391
  %1393 = load half, half addrspace(3)* %1392, align 2
  %1394 = mul i32 %6, 16
  %1395 = add i32 0, %1394
  %1396 = mul i32 %1395, 2
  %1397 = add i32 1, %1396
  %1398 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1397
  %1399 = load half, half addrspace(3)* %1398, align 2
  %1400 = mul i32 %6, 16
  %1401 = add i32 0, %1400
  %1402 = mul i32 %1401, 2
  %1403 = add i32 0, %1402
  %1404 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1403
  %1405 = load half, half addrspace(3)* %1404, align 2
  %1406 = mul i32 %6, 16
  %1407 = add i32 0, %1406
  %1408 = mul i32 %1407, 2
  %1409 = add i32 1, %1408
  %1410 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1409
  %1411 = load half, half addrspace(3)* %1410, align 2
  %1412 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %1413 = and i32 %1412, -4
  %1414 = or i32 %1413, 1
  %1415 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1393, i32 %1414)  ; WaveReadLaneAt(value,lane)
  %1416 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1399, i32 %1414)  ; WaveReadLaneAt(value,lane)
  %1417 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1405, i32 %1414)  ; WaveReadLaneAt(value,lane)
  %1418 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1411, i32 %1414)  ; WaveReadLaneAt(value,lane)
  %1419 = or i32 %1413, 2
  %1420 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1393, i32 %1419)  ; WaveReadLaneAt(value,lane)
  %1421 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1399, i32 %1419)  ; WaveReadLaneAt(value,lane)
  %1422 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1405, i32 %1419)  ; WaveReadLaneAt(value,lane)
  %1423 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1411, i32 %1419)  ; WaveReadLaneAt(value,lane)
  %1424 = or i32 %1412, 3
  %1425 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1393, i32 %1424)  ; WaveReadLaneAt(value,lane)
  %1426 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1399, i32 %1424)  ; WaveReadLaneAt(value,lane)
  %1427 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1405, i32 %1424)  ; WaveReadLaneAt(value,lane)
  %1428 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1411, i32 %1424)  ; WaveReadLaneAt(value,lane)
  br i1 %286, label %1429, label %1455

; <label>:1429                                    ; preds = %1387
  %1430 = fadd fast half %1418, %1411
  %1431 = fadd fast half %1430, %1423
  %1432 = fadd fast half %1431, %1428
  %1433 = fmul fast half %1432, 0xH3400
  %1434 = fadd fast half %1417, %1405
  %1435 = fadd fast half %1434, %1422
  %1436 = fadd fast half %1435, %1427
  %1437 = fmul fast half %1436, 0xH3400
  %1438 = fadd fast half %1416, %1399
  %1439 = fadd fast half %1438, %1421
  %1440 = fadd fast half %1439, %1426
  %1441 = fmul fast half %1440, 0xH3400
  %1442 = fadd fast half %1415, %1393
  %1443 = fadd fast half %1442, %1420
  %1444 = fadd fast half %1443, %1425
  %1445 = fmul fast half %1444, 0xH3400
  %1446 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %1447 = extractvalue %dx.types.CBufRet.i32 %1446, 0
  %1448 = add i32 %1447, 12
  %1449 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %1448, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %1450 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1449, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %1451 = fpext half %1445 to float
  %1452 = fpext half %1441 to float
  %1453 = fpext half %1437 to float
  %1454 = fpext half %1433 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %1450, i32 0, i32 0, i32 %5, float %1451, float %1452, float %1453, float %1454, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  br label %1455

; <label>:1455                                    ; preds = %1429, %1387, %1386, %1384, %1289, %1193, %1073, %800, %796, %789, %718, %616, %515, %415, %0
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.groupId.i32(i32, i32) #0

; Function Attrs: nounwind readnone
declare i32 @dx.op.flattenedThreadIdInGroup.i32(i32) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #1

; Function Attrs: noduplicate nounwind
declare void @dx.op.barrier(i32, i32) #2

; Function Attrs: nounwind
declare i32 @dx.op.atomicBinOp.i32(i32, %dx.types.Handle, i32, i32, i32, i32, i32) #3

; Function Attrs: nounwind
declare void @dx.op.rawBufferStore.i32(i32, %dx.types.Handle, i32, i32, i32, i32, i32, i32, i8, i32) #3

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #1

; Function Attrs: nounwind
declare void @dx.op.textureStore.f32(i32, %dx.types.Handle, i32, i32, i32, float, float, float, float, i8) #3

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32, %dx.types.Handle, i32, i32, i32, i32, i32, i32, i32) #1

; Function Attrs: nounwind readonly
declare i32 @dx.op.waveGetLaneIndex(i32) #1

; Function Attrs: nounwind
declare half @dx.op.waveReadLaneAt.f16(i32, half, i32) #3

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind readonly }
attributes #2 = { noduplicate nounwind }
attributes #3 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.entryPoints = !{!6}

!0 = !{!"dxcoob 1.8.2407.7 (416fab6b5)"}
!1 = !{i32 1, i32 8}
!2 = !{!"cs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 20, null}
!6 = !{void ()* @downsample_cs, !"downsample_cs", null, !3, !7}
!7 = !{i32 0, i64 11820081440, i32 4, !8}
!8 = !{i32 256, i32 1, i32 1}
!9 = !{!10, !10, i64 0}
!10 = !{!"int", !11, i64 0}
!11 = !{!"omnipotent char", !12, i64 0}
!12 = !{!"Simple C/C++ TBAA"}
*/
auto Shaders::spd_downsample_cs() const -> Span<const std::byte> {
    return Span(_data).subspan(17988, 14080);
}

#undef texture_data

} // namespace fb::baked::kitchen
