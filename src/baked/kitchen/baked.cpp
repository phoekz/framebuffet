#include "baked.hpp"

namespace fb::baked::kitchen {

#define texture_data(rp, sp, off, sz)                                                   \
    TextureData {                                                                       \
        .row_pitch = rp, .slice_pitch = sp, .data = transmuted_span<std::byte>(off, sz) \
    }

auto Assets::load() -> void {
    // hash: 799fc360204416196536a93c9eff68ae
    ZoneScoped;
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
    // hash: c341ef8bae9f9a40b1a9fde91351e2fe
    ZoneScoped;
    _data = read_whole_file("fb_kitchen_shaders.bin");
    FB_ASSERT(_data.size() == 32124);
}

// shader_hash: 1a56b389138880d61871707f8f50b695
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
; shader debug name: 1a56b389138880d61871707f8f50b695.pdb
; shader hash: 1a56b389138880d61871707f8f50b695
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
!7 = !{[7 x i32] [i32 5, i32 10, i32 1023, i32 0, i32 0, i32 0, i32 0]}
!8 = !{void ()* @draw_vs, !"draw_vs", !9, !4, !22}
!9 = !{!10, !15, null}
!10 = !{!11, !14}
!11 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !12, i8 0, i32 1, i8 1, i32 0, i8 0, !13}
!12 = !{i32 0}
!13 = !{i32 3, i32 1}
!14 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !12, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!15 = !{!16, !18, !19}
!16 = !{i32 0, !"SV_Position", i8 9, i8 3, !12, i8 4, i32 1, i8 4, i32 0, i8 0, !17}
!17 = !{i32 3, i32 15}
!18 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !12, i8 2, i32 1, i8 4, i32 1, i8 0, !17}
!19 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !20, i8 2, i32 1, i8 2, i32 2, i8 0, !21}
!20 = !{i32 1}
!21 = !{i32 3, i32 3}
!22 = !{i32 0, i64 1082130688}
*/
auto Shaders::gui_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(0, 5376);
}

// shader_hash: f56162255ee90a472ca6e4af5aec7f22
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
; shader debug name: f56162255ee90a472ca6e4af5aec7f22.pdb
; shader hash: f56162255ee90a472ca6e4af5aec7f22
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
!7 = !{[12 x i32] [i32 10, i32 4, i32 0, i32 0, i32 0, i32 0, i32 1, i32 2, i32 4, i32 8, i32 15, i32 15]}
!8 = !{void ()* @draw_ps, !"draw_ps", !9, !4, !20}
!9 = !{!10, !18, null}
!10 = !{!11, !13, !15}
!11 = !{i32 0, !"SV_Position", i8 9, i8 3, !12, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!12 = !{i32 0}
!13 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !12, i8 2, i32 1, i8 4, i32 1, i8 0, !14}
!14 = !{i32 3, i32 15}
!15 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !16, i8 2, i32 1, i8 2, i32 2, i8 0, !17}
!16 = !{i32 1}
!17 = !{i32 3, i32 3}
!18 = !{!19}
!19 = !{i32 0, !"SV_Target", i8 9, i8 16, !12, i8 0, i32 1, i8 4, i32 0, i8 0, !14}
!20 = !{i32 0, i64 3229614336}
*/
auto Shaders::gui_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(5376, 4436);
}

// shader_hash: a1ad702e1d13ee5a5a0cd09356116e1f
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
; shader debug name: a1ad702e1d13ee5a5a0cd09356116e1f.pdb
; shader hash: a1ad702e1d13ee5a5a0cd09356116e1f
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
!6 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 8, null}
!7 = !{[7 x i32] [i32 5, i32 8, i32 255, i32 0, i32 0, i32 0, i32 0]}
!8 = !{void ()* @draw_vs, !"draw_vs", !9, !4, !19}
!9 = !{!10, !15, null}
!10 = !{!11, !14}
!11 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !12, i8 0, i32 1, i8 1, i32 0, i8 0, !13}
!12 = !{i32 0}
!13 = !{i32 3, i32 1}
!14 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !12, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!15 = !{!16, !18}
!16 = !{i32 0, !"SV_Position", i8 9, i8 3, !12, i8 4, i32 1, i8 4, i32 0, i8 0, !17}
!17 = !{i32 3, i32 15}
!18 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !12, i8 2, i32 1, i8 4, i32 1, i8 0, !17}
!19 = !{i32 0, i64 1082130688}
*/
auto Shaders::debug_draw_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(9812, 5140);
}

// shader_hash: fd99ba74a11c0d4a0da57f344f133451
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
; shader debug name: fd99ba74a11c0d4a0da57f344f133451.pdb
; shader hash: fd99ba74a11c0d4a0da57f344f133451
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
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.viewIdState = !{!4}
!dx.entryPoints = !{!5}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 7}
!2 = !{i32 1, i32 8}
!3 = !{!"ps", i32 6, i32 7}
!4 = !{[10 x i32] [i32 8, i32 4, i32 0, i32 0, i32 0, i32 0, i32 1, i32 2, i32 4, i32 8]}
!5 = !{void ()* @draw_ps, !"draw_ps", !6, null, !14}
!6 = !{!7, !12, null}
!7 = !{!8, !10}
!8 = !{i32 0, !"SV_Position", i8 9, i8 3, !9, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!9 = !{i32 0}
!10 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !9, i8 2, i32 1, i8 4, i32 1, i8 0, !11}
!11 = !{i32 3, i32 15}
!12 = !{!13}
!13 = !{i32 0, !"SV_Target", i8 9, i8 16, !9, i8 0, i32 1, i8 4, i32 0, i8 0, !11}
!14 = !{i32 0, i64 8388864}
*/
auto Shaders::debug_draw_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(14952, 3076);
}

// shader_hash: accf9e643c113fcb4e7d227c723bc66f
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 1533
// temp_array_count: 2048
// dynamic_flow_control_count: 30
// array_instruction_count: 76
// float_instruction_count: 449
// int_instruction_count: 347
// uint_instruction_count: 122
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
; shader debug name: accf9e643c113fcb4e7d227c723bc66f.pdb
; shader hash: accf9e643c113fcb4e7d227c723bc66f
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
  %102 = add i32 %101, %35
  %103 = or i32 %41, 16
  %104 = add i32 %103, %33
  %105 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %106 = extractvalue %dx.types.CBufRet.i32 %105, 2
  %107 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %106, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %108 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %107, %dx.types.ResourceProperties { i32 7, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2DArray<4xF32>
  %109 = uitofp i32 %102 to float
  %110 = fmul fast float %19, %109
  %111 = fadd fast float %110, %19
  %112 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %108, %dx.types.Handle %48, float %53, float %111, float %55, float undef, i32 0, i32 0, i32 undef, float 0.000000e+00)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %113 = extractvalue %dx.types.ResRet.f32 %112, 0
  %114 = extractvalue %dx.types.ResRet.f32 %112, 1
  %115 = extractvalue %dx.types.ResRet.f32 %112, 2
  %116 = extractvalue %dx.types.ResRet.f32 %112, 3
  %117 = fptrunc float %113 to half
  %118 = fptrunc float %114 to half
  %119 = fptrunc float %115 to half
  %120 = fptrunc float %116 to half
  %121 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %122 = extractvalue %dx.types.CBufRet.i32 %121, 0
  %123 = add i32 %122, 1
  %124 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %123, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %125 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %124, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %126 = fpext half %117 to float
  %127 = fpext half %118 to float
  %128 = fpext half %119 to float
  %129 = fpext half %120 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %125, i32 %42, i32 %104, i32 %5, float %126, float %127, float %128, float %129, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %130 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %131 = extractvalue %dx.types.CBufRet.i32 %130, 2
  %132 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %131, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %133 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %132, %dx.types.ResourceProperties { i32 7, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2DArray<4xF32>
  %134 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %133, %dx.types.Handle %48, float %82, float %111, float %55, float undef, i32 0, i32 0, i32 undef, float 0.000000e+00)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %135 = extractvalue %dx.types.ResRet.f32 %134, 0
  %136 = extractvalue %dx.types.ResRet.f32 %134, 1
  %137 = extractvalue %dx.types.ResRet.f32 %134, 2
  %138 = extractvalue %dx.types.ResRet.f32 %134, 3
  %139 = fptrunc float %135 to half
  %140 = fptrunc float %136 to half
  %141 = fptrunc float %137 to half
  %142 = fptrunc float %138 to half
  %143 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %144 = extractvalue %dx.types.CBufRet.i32 %143, 0
  %145 = add i32 %144, 1
  %146 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %145, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %147 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %146, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %148 = fpext half %139 to float
  %149 = fpext half %140 to float
  %150 = fpext half %141 to float
  %151 = fpext half %142 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %147, i32 %75, i32 %104, i32 %5, float %148, float %149, float %150, float %151, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %152 = icmp ult i32 %15, 2
  br i1 %152, label %1456, label %153

; <label>:153                                     ; preds = %0
  %154 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %155 = and i32 %154, -4
  %156 = or i32 %155, 1
  %157 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %61, i32 %156)  ; WaveReadLaneAt(value,lane)
  %158 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %62, i32 %156)  ; WaveReadLaneAt(value,lane)
  %159 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %63, i32 %156)  ; WaveReadLaneAt(value,lane)
  %160 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %64, i32 %156)  ; WaveReadLaneAt(value,lane)
  %161 = or i32 %155, 2
  %162 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %61, i32 %161)  ; WaveReadLaneAt(value,lane)
  %163 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %62, i32 %161)  ; WaveReadLaneAt(value,lane)
  %164 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %63, i32 %161)  ; WaveReadLaneAt(value,lane)
  %165 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %64, i32 %161)  ; WaveReadLaneAt(value,lane)
  %166 = or i32 %154, 3
  %167 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %61, i32 %166)  ; WaveReadLaneAt(value,lane)
  %168 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %62, i32 %166)  ; WaveReadLaneAt(value,lane)
  %169 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %63, i32 %166)  ; WaveReadLaneAt(value,lane)
  %170 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %64, i32 %166)  ; WaveReadLaneAt(value,lane)
  %171 = fadd fast half %157, %61
  %172 = fadd fast half %158, %62
  %173 = fadd fast half %159, %63
  %174 = fadd fast half %160, %64
  %175 = fadd fast half %171, %162
  %176 = fadd fast half %172, %163
  %177 = fadd fast half %173, %164
  %178 = fadd fast half %174, %165
  %179 = fadd fast half %175, %167
  %180 = fadd fast half %176, %168
  %181 = fadd fast half %177, %169
  %182 = fadd fast half %178, %170
  %183 = fmul fast half %179, 0xH3400
  %184 = fmul fast half %180, 0xH3400
  %185 = fmul fast half %181, 0xH3400
  %186 = fmul fast half %182, 0xH3400
  %187 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %188 = and i32 %187, -4
  %189 = or i32 %188, 1
  %190 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %88, i32 %189)  ; WaveReadLaneAt(value,lane)
  %191 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %89, i32 %189)  ; WaveReadLaneAt(value,lane)
  %192 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %90, i32 %189)  ; WaveReadLaneAt(value,lane)
  %193 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %91, i32 %189)  ; WaveReadLaneAt(value,lane)
  %194 = or i32 %188, 2
  %195 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %88, i32 %194)  ; WaveReadLaneAt(value,lane)
  %196 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %89, i32 %194)  ; WaveReadLaneAt(value,lane)
  %197 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %90, i32 %194)  ; WaveReadLaneAt(value,lane)
  %198 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %91, i32 %194)  ; WaveReadLaneAt(value,lane)
  %199 = or i32 %187, 3
  %200 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %88, i32 %199)  ; WaveReadLaneAt(value,lane)
  %201 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %89, i32 %199)  ; WaveReadLaneAt(value,lane)
  %202 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %90, i32 %199)  ; WaveReadLaneAt(value,lane)
  %203 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %91, i32 %199)  ; WaveReadLaneAt(value,lane)
  %204 = fadd fast half %190, %88
  %205 = fadd fast half %191, %89
  %206 = fadd fast half %192, %90
  %207 = fadd fast half %193, %91
  %208 = fadd fast half %204, %195
  %209 = fadd fast half %205, %196
  %210 = fadd fast half %206, %197
  %211 = fadd fast half %207, %198
  %212 = fadd fast half %208, %200
  %213 = fadd fast half %209, %201
  %214 = fadd fast half %210, %202
  %215 = fadd fast half %211, %203
  %216 = fmul fast half %212, 0xH3400
  %217 = fmul fast half %213, 0xH3400
  %218 = fmul fast half %214, 0xH3400
  %219 = fmul fast half %215, 0xH3400
  %220 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %221 = and i32 %220, -4
  %222 = or i32 %221, 1
  %223 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %117, i32 %222)  ; WaveReadLaneAt(value,lane)
  %224 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %118, i32 %222)  ; WaveReadLaneAt(value,lane)
  %225 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %119, i32 %222)  ; WaveReadLaneAt(value,lane)
  %226 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %120, i32 %222)  ; WaveReadLaneAt(value,lane)
  %227 = or i32 %221, 2
  %228 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %117, i32 %227)  ; WaveReadLaneAt(value,lane)
  %229 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %118, i32 %227)  ; WaveReadLaneAt(value,lane)
  %230 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %119, i32 %227)  ; WaveReadLaneAt(value,lane)
  %231 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %120, i32 %227)  ; WaveReadLaneAt(value,lane)
  %232 = or i32 %220, 3
  %233 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %117, i32 %232)  ; WaveReadLaneAt(value,lane)
  %234 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %118, i32 %232)  ; WaveReadLaneAt(value,lane)
  %235 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %119, i32 %232)  ; WaveReadLaneAt(value,lane)
  %236 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %120, i32 %232)  ; WaveReadLaneAt(value,lane)
  %237 = fadd fast half %223, %117
  %238 = fadd fast half %224, %118
  %239 = fadd fast half %225, %119
  %240 = fadd fast half %226, %120
  %241 = fadd fast half %237, %228
  %242 = fadd fast half %238, %229
  %243 = fadd fast half %239, %230
  %244 = fadd fast half %240, %231
  %245 = fadd fast half %241, %233
  %246 = fadd fast half %242, %234
  %247 = fadd fast half %243, %235
  %248 = fadd fast half %244, %236
  %249 = fmul fast half %245, 0xH3400
  %250 = fmul fast half %246, 0xH3400
  %251 = fmul fast half %247, 0xH3400
  %252 = fmul fast half %248, 0xH3400
  %253 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %254 = and i32 %253, -4
  %255 = or i32 %254, 1
  %256 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %139, i32 %255)  ; WaveReadLaneAt(value,lane)
  %257 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %140, i32 %255)  ; WaveReadLaneAt(value,lane)
  %258 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %141, i32 %255)  ; WaveReadLaneAt(value,lane)
  %259 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %142, i32 %255)  ; WaveReadLaneAt(value,lane)
  %260 = or i32 %254, 2
  %261 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %139, i32 %260)  ; WaveReadLaneAt(value,lane)
  %262 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %140, i32 %260)  ; WaveReadLaneAt(value,lane)
  %263 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %141, i32 %260)  ; WaveReadLaneAt(value,lane)
  %264 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %142, i32 %260)  ; WaveReadLaneAt(value,lane)
  %265 = or i32 %253, 3
  %266 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %139, i32 %265)  ; WaveReadLaneAt(value,lane)
  %267 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %140, i32 %265)  ; WaveReadLaneAt(value,lane)
  %268 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %141, i32 %265)  ; WaveReadLaneAt(value,lane)
  %269 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %142, i32 %265)  ; WaveReadLaneAt(value,lane)
  %270 = fadd fast half %256, %139
  %271 = fadd fast half %257, %140
  %272 = fadd fast half %258, %141
  %273 = fadd fast half %259, %142
  %274 = fadd fast half %270, %261
  %275 = fadd fast half %271, %262
  %276 = fadd fast half %272, %263
  %277 = fadd fast half %273, %264
  %278 = fadd fast half %274, %266
  %279 = fadd fast half %275, %267
  %280 = fadd fast half %276, %268
  %281 = fadd fast half %277, %269
  %282 = fmul fast half %278, 0xH3400
  %283 = fmul fast half %279, 0xH3400
  %284 = fmul fast half %280, 0xH3400
  %285 = fmul fast half %281, 0xH3400
  %286 = and i32 %6, 3
  %287 = icmp eq i32 %286, 0
  br i1 %287, label %288, label %416

; <label>:288                                     ; preds = %153
  %289 = lshr i32 %30, 1
  %290 = lshr i32 %33, 1
  %291 = shl i32 %3, 4
  %292 = shl i32 %4, 4
  %293 = or i32 %289, %291
  %294 = add i32 %290, %292
  %295 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %296 = extractvalue %dx.types.CBufRet.i32 %295, 0
  %297 = add i32 %296, 2
  %298 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %297, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %299 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %298, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %300 = fpext half %183 to float
  %301 = fpext half %184 to float
  %302 = fpext half %185 to float
  %303 = fpext half %186 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %299, i32 %293, i32 %294, i32 %5, float %300, float %301, float %302, float %303, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %304 = mul i32 %289, 16
  %305 = add i32 %290, %304
  %306 = mul i32 %305, 2
  %307 = add i32 0, %306
  %308 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %307
  store half %183, half addrspace(3)* %308, align 2
  %309 = mul i32 %289, 16
  %310 = add i32 %290, %309
  %311 = mul i32 %310, 2
  %312 = add i32 1, %311
  %313 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %312
  store half %184, half addrspace(3)* %313, align 2
  %314 = mul i32 %289, 16
  %315 = add i32 %290, %314
  %316 = mul i32 %315, 2
  %317 = add i32 0, %316
  %318 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %317
  store half %185, half addrspace(3)* %318, align 2
  %319 = mul i32 %289, 16
  %320 = add i32 %290, %319
  %321 = mul i32 %320, 2
  %322 = add i32 1, %321
  %323 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %322
  store half %186, half addrspace(3)* %323, align 2
  %324 = or i32 %293, 8
  %325 = or i32 %289, 8
  %326 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %327 = extractvalue %dx.types.CBufRet.i32 %326, 0
  %328 = add i32 %327, 2
  %329 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %328, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %330 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %329, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %331 = fpext half %216 to float
  %332 = fpext half %217 to float
  %333 = fpext half %218 to float
  %334 = fpext half %219 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %330, i32 %324, i32 %294, i32 %5, float %331, float %332, float %333, float %334, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %335 = mul i32 %325, 16
  %336 = add i32 %290, %335
  %337 = mul i32 %336, 2
  %338 = add i32 0, %337
  %339 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %338
  store half %216, half addrspace(3)* %339, align 2
  %340 = mul i32 %325, 16
  %341 = add i32 %290, %340
  %342 = mul i32 %341, 2
  %343 = add i32 1, %342
  %344 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %343
  store half %217, half addrspace(3)* %344, align 2
  %345 = mul i32 %325, 16
  %346 = add i32 %290, %345
  %347 = mul i32 %346, 2
  %348 = add i32 0, %347
  %349 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %348
  store half %218, half addrspace(3)* %349, align 2
  %350 = mul i32 %325, 16
  %351 = add i32 %290, %350
  %352 = mul i32 %351, 2
  %353 = add i32 1, %352
  %354 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %353
  store half %219, half addrspace(3)* %354, align 2
  %355 = or i32 %292, 8
  %356 = add i32 %355, %290
  %357 = add nuw nsw i32 %290, 8
  %358 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %359 = extractvalue %dx.types.CBufRet.i32 %358, 0
  %360 = add i32 %359, 2
  %361 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %360, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %362 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %361, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %363 = fpext half %249 to float
  %364 = fpext half %250 to float
  %365 = fpext half %251 to float
  %366 = fpext half %252 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %362, i32 %293, i32 %356, i32 %5, float %363, float %364, float %365, float %366, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %367 = mul i32 %289, 16
  %368 = add i32 %357, %367
  %369 = mul i32 %368, 2
  %370 = add i32 0, %369
  %371 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %370
  store half %249, half addrspace(3)* %371, align 2
  %372 = mul i32 %289, 16
  %373 = add i32 %357, %372
  %374 = mul i32 %373, 2
  %375 = add i32 1, %374
  %376 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %375
  store half %250, half addrspace(3)* %376, align 2
  %377 = mul i32 %289, 16
  %378 = add i32 %357, %377
  %379 = mul i32 %378, 2
  %380 = add i32 0, %379
  %381 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %380
  store half %251, half addrspace(3)* %381, align 2
  %382 = mul i32 %289, 16
  %383 = add i32 %357, %382
  %384 = mul i32 %383, 2
  %385 = add i32 1, %384
  %386 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %385
  store half %252, half addrspace(3)* %386, align 2
  %387 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %388 = extractvalue %dx.types.CBufRet.i32 %387, 0
  %389 = add i32 %388, 2
  %390 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %389, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %391 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %390, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %392 = fpext half %282 to float
  %393 = fpext half %283 to float
  %394 = fpext half %284 to float
  %395 = fpext half %285 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %391, i32 %324, i32 %356, i32 %5, float %392, float %393, float %394, float %395, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %396 = mul i32 %325, 16
  %397 = add i32 %357, %396
  %398 = mul i32 %397, 2
  %399 = add i32 0, %398
  %400 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %399
  store half %282, half addrspace(3)* %400, align 2
  %401 = mul i32 %325, 16
  %402 = add i32 %357, %401
  %403 = mul i32 %402, 2
  %404 = add i32 1, %403
  %405 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %404
  store half %283, half addrspace(3)* %405, align 2
  %406 = mul i32 %325, 16
  %407 = add i32 %357, %406
  %408 = mul i32 %407, 2
  %409 = add i32 0, %408
  %410 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %409
  store half %284, half addrspace(3)* %410, align 2
  %411 = mul i32 %325, 16
  %412 = add i32 %357, %411
  %413 = mul i32 %412, 2
  %414 = add i32 1, %413
  %415 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %414
  store half %285, half addrspace(3)* %415, align 2
  br label %416

; <label>:416                                     ; preds = %288, %153
  %417 = icmp ult i32 %15, 3
  br i1 %417, label %1456, label %418

; <label>:418                                     ; preds = %416
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  %419 = icmp ult i32 %6, 256
  br i1 %419, label %420, label %516

; <label>:420                                     ; preds = %418
  %421 = mul i32 %30, 16
  %422 = add i32 %33, %421
  %423 = mul i32 %422, 2
  %424 = add i32 0, %423
  %425 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %424
  %426 = load half, half addrspace(3)* %425, align 2
  %427 = mul i32 %30, 16
  %428 = add i32 %33, %427
  %429 = mul i32 %428, 2
  %430 = add i32 1, %429
  %431 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %430
  %432 = load half, half addrspace(3)* %431, align 2
  %433 = mul i32 %30, 16
  %434 = add i32 %33, %433
  %435 = mul i32 %434, 2
  %436 = add i32 0, %435
  %437 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %436
  %438 = load half, half addrspace(3)* %437, align 2
  %439 = mul i32 %30, 16
  %440 = add i32 %33, %439
  %441 = mul i32 %440, 2
  %442 = add i32 1, %441
  %443 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %442
  %444 = load half, half addrspace(3)* %443, align 2
  %445 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %446 = and i32 %445, -4
  %447 = or i32 %446, 1
  %448 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %426, i32 %447)  ; WaveReadLaneAt(value,lane)
  %449 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %432, i32 %447)  ; WaveReadLaneAt(value,lane)
  %450 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %438, i32 %447)  ; WaveReadLaneAt(value,lane)
  %451 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %444, i32 %447)  ; WaveReadLaneAt(value,lane)
  %452 = or i32 %446, 2
  %453 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %426, i32 %452)  ; WaveReadLaneAt(value,lane)
  %454 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %432, i32 %452)  ; WaveReadLaneAt(value,lane)
  %455 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %438, i32 %452)  ; WaveReadLaneAt(value,lane)
  %456 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %444, i32 %452)  ; WaveReadLaneAt(value,lane)
  %457 = or i32 %445, 3
  %458 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %426, i32 %457)  ; WaveReadLaneAt(value,lane)
  %459 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %432, i32 %457)  ; WaveReadLaneAt(value,lane)
  %460 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %438, i32 %457)  ; WaveReadLaneAt(value,lane)
  %461 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %444, i32 %457)  ; WaveReadLaneAt(value,lane)
  %462 = fadd fast half %448, %426
  %463 = fadd fast half %449, %432
  %464 = fadd fast half %450, %438
  %465 = fadd fast half %451, %444
  %466 = fadd fast half %462, %453
  %467 = fadd fast half %463, %454
  %468 = fadd fast half %464, %455
  %469 = fadd fast half %465, %456
  %470 = fadd fast half %466, %458
  %471 = fadd fast half %467, %459
  %472 = fadd fast half %468, %460
  %473 = fadd fast half %469, %461
  %474 = fmul fast half %470, 0xH3400
  %475 = fmul fast half %471, 0xH3400
  %476 = fmul fast half %472, 0xH3400
  %477 = fmul fast half %473, 0xH3400
  br i1 %287, label %478, label %516

; <label>:478                                     ; preds = %420
  %479 = lshr i32 %30, 1
  %480 = lshr i32 %33, 1
  %481 = shl i32 %3, 3
  %482 = shl i32 %4, 3
  %483 = or i32 %479, %481
  %484 = add i32 %480, %482
  %485 = and i32 %23, 1
  %486 = add nuw nsw i32 %30, %485
  %487 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %488 = extractvalue %dx.types.CBufRet.i32 %487, 0
  %489 = add i32 %488, 3
  %490 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %489, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %491 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %490, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %492 = fpext half %474 to float
  %493 = fpext half %475 to float
  %494 = fpext half %476 to float
  %495 = fpext half %477 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %491, i32 %483, i32 %484, i32 %5, float %492, float %493, float %494, float %495, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %496 = mul i32 %486, 16
  %497 = add i32 %33, %496
  %498 = mul i32 %497, 2
  %499 = add i32 0, %498
  %500 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %499
  store half %474, half addrspace(3)* %500, align 2
  %501 = mul i32 %486, 16
  %502 = add i32 %33, %501
  %503 = mul i32 %502, 2
  %504 = add i32 1, %503
  %505 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %504
  store half %475, half addrspace(3)* %505, align 2
  %506 = mul i32 %486, 16
  %507 = add i32 %33, %506
  %508 = mul i32 %507, 2
  %509 = add i32 0, %508
  %510 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %509
  store half %476, half addrspace(3)* %510, align 2
  %511 = mul i32 %486, 16
  %512 = add i32 %33, %511
  %513 = mul i32 %512, 2
  %514 = add i32 1, %513
  %515 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %514
  store half %477, half addrspace(3)* %515, align 2
  br label %516

; <label>:516                                     ; preds = %478, %420, %418
  %517 = icmp ult i32 %15, 4
  br i1 %517, label %1456, label %518

; <label>:518                                     ; preds = %516
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  %519 = icmp ult i32 %6, 64
  br i1 %519, label %520, label %617

; <label>:520                                     ; preds = %518
  %521 = and i32 %21, 1
  %522 = or i32 %34, %521
  %523 = mul i32 %522, 16
  %524 = add i32 %35, %523
  %525 = mul i32 %524, 2
  %526 = add i32 0, %525
  %527 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %526
  %528 = load half, half addrspace(3)* %527, align 2
  %529 = mul i32 %522, 16
  %530 = add i32 %35, %529
  %531 = mul i32 %530, 2
  %532 = add i32 1, %531
  %533 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %532
  %534 = load half, half addrspace(3)* %533, align 2
  %535 = mul i32 %522, 16
  %536 = add i32 %35, %535
  %537 = mul i32 %536, 2
  %538 = add i32 0, %537
  %539 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %538
  %540 = load half, half addrspace(3)* %539, align 2
  %541 = mul i32 %522, 16
  %542 = add i32 %35, %541
  %543 = mul i32 %542, 2
  %544 = add i32 1, %543
  %545 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %544
  %546 = load half, half addrspace(3)* %545, align 2
  %547 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %548 = and i32 %547, -4
  %549 = or i32 %548, 1
  %550 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %528, i32 %549)  ; WaveReadLaneAt(value,lane)
  %551 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %534, i32 %549)  ; WaveReadLaneAt(value,lane)
  %552 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %540, i32 %549)  ; WaveReadLaneAt(value,lane)
  %553 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %546, i32 %549)  ; WaveReadLaneAt(value,lane)
  %554 = or i32 %548, 2
  %555 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %528, i32 %554)  ; WaveReadLaneAt(value,lane)
  %556 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %534, i32 %554)  ; WaveReadLaneAt(value,lane)
  %557 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %540, i32 %554)  ; WaveReadLaneAt(value,lane)
  %558 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %546, i32 %554)  ; WaveReadLaneAt(value,lane)
  %559 = or i32 %547, 3
  %560 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %528, i32 %559)  ; WaveReadLaneAt(value,lane)
  %561 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %534, i32 %559)  ; WaveReadLaneAt(value,lane)
  %562 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %540, i32 %559)  ; WaveReadLaneAt(value,lane)
  %563 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %546, i32 %559)  ; WaveReadLaneAt(value,lane)
  %564 = fadd fast half %550, %528
  %565 = fadd fast half %551, %534
  %566 = fadd fast half %552, %540
  %567 = fadd fast half %553, %546
  %568 = fadd fast half %564, %555
  %569 = fadd fast half %565, %556
  %570 = fadd fast half %566, %557
  %571 = fadd fast half %567, %558
  %572 = fadd fast half %568, %560
  %573 = fadd fast half %569, %561
  %574 = fadd fast half %570, %562
  %575 = fadd fast half %571, %563
  %576 = fmul fast half %572, 0xH3400
  %577 = fmul fast half %573, 0xH3400
  %578 = fmul fast half %574, 0xH3400
  %579 = fmul fast half %575, 0xH3400
  br i1 %287, label %580, label %617

; <label>:580                                     ; preds = %520
  %581 = lshr i32 %30, 1
  %582 = lshr i32 %33, 1
  %583 = shl i32 %3, 2
  %584 = shl i32 %4, 2
  %585 = add i32 %581, %583
  %586 = add i32 %582, %584
  %587 = add nuw nsw i32 %34, %582
  %588 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %589 = extractvalue %dx.types.CBufRet.i32 %588, 0
  %590 = add i32 %589, 4
  %591 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %590, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %592 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %591, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %593 = fpext half %576 to float
  %594 = fpext half %577 to float
  %595 = fpext half %578 to float
  %596 = fpext half %579 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %592, i32 %585, i32 %586, i32 %5, float %593, float %594, float %595, float %596, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %597 = mul i32 %587, 16
  %598 = add i32 %35, %597
  %599 = mul i32 %598, 2
  %600 = add i32 0, %599
  %601 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %600
  store half %576, half addrspace(3)* %601, align 2
  %602 = mul i32 %587, 16
  %603 = add i32 %35, %602
  %604 = mul i32 %603, 2
  %605 = add i32 1, %604
  %606 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %605
  store half %577, half addrspace(3)* %606, align 2
  %607 = mul i32 %587, 16
  %608 = add i32 %35, %607
  %609 = mul i32 %608, 2
  %610 = add i32 0, %609
  %611 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %610
  store half %578, half addrspace(3)* %611, align 2
  %612 = mul i32 %587, 16
  %613 = add i32 %35, %612
  %614 = mul i32 %613, 2
  %615 = add i32 1, %614
  %616 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %615
  store half %579, half addrspace(3)* %616, align 2
  br label %617

; <label>:617                                     ; preds = %580, %520, %518
  %618 = icmp ult i32 %15, 5
  br i1 %618, label %1456, label %619

; <label>:619                                     ; preds = %617
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  %620 = icmp ult i32 %6, 16
  br i1 %620, label %621, label %719

; <label>:621                                     ; preds = %619
  %622 = shl nuw nsw i32 %30, 2
  %623 = add nuw nsw i32 %622, %33
  %624 = shl nuw nsw i32 %33, 2
  %625 = mul i32 %623, 16
  %626 = add i32 %624, %625
  %627 = mul i32 %626, 2
  %628 = add i32 0, %627
  %629 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %628
  %630 = load half, half addrspace(3)* %629, align 2
  %631 = mul i32 %623, 16
  %632 = add i32 %624, %631
  %633 = mul i32 %632, 2
  %634 = add i32 1, %633
  %635 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %634
  %636 = load half, half addrspace(3)* %635, align 2
  %637 = mul i32 %623, 16
  %638 = add i32 %624, %637
  %639 = mul i32 %638, 2
  %640 = add i32 0, %639
  %641 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %640
  %642 = load half, half addrspace(3)* %641, align 2
  %643 = mul i32 %623, 16
  %644 = add i32 %624, %643
  %645 = mul i32 %644, 2
  %646 = add i32 1, %645
  %647 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %646
  %648 = load half, half addrspace(3)* %647, align 2
  %649 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %650 = and i32 %649, -4
  %651 = or i32 %650, 1
  %652 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %630, i32 %651)  ; WaveReadLaneAt(value,lane)
  %653 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %636, i32 %651)  ; WaveReadLaneAt(value,lane)
  %654 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %642, i32 %651)  ; WaveReadLaneAt(value,lane)
  %655 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %648, i32 %651)  ; WaveReadLaneAt(value,lane)
  %656 = or i32 %650, 2
  %657 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %630, i32 %656)  ; WaveReadLaneAt(value,lane)
  %658 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %636, i32 %656)  ; WaveReadLaneAt(value,lane)
  %659 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %642, i32 %656)  ; WaveReadLaneAt(value,lane)
  %660 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %648, i32 %656)  ; WaveReadLaneAt(value,lane)
  %661 = or i32 %649, 3
  %662 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %630, i32 %661)  ; WaveReadLaneAt(value,lane)
  %663 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %636, i32 %661)  ; WaveReadLaneAt(value,lane)
  %664 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %642, i32 %661)  ; WaveReadLaneAt(value,lane)
  %665 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %648, i32 %661)  ; WaveReadLaneAt(value,lane)
  %666 = fadd fast half %652, %630
  %667 = fadd fast half %653, %636
  %668 = fadd fast half %654, %642
  %669 = fadd fast half %655, %648
  %670 = fadd fast half %666, %657
  %671 = fadd fast half %667, %658
  %672 = fadd fast half %668, %659
  %673 = fadd fast half %669, %660
  %674 = fadd fast half %670, %662
  %675 = fadd fast half %671, %663
  %676 = fadd fast half %672, %664
  %677 = fadd fast half %673, %665
  %678 = fmul fast half %674, 0xH3400
  %679 = fmul fast half %675, 0xH3400
  %680 = fmul fast half %676, 0xH3400
  %681 = fmul fast half %677, 0xH3400
  br i1 %287, label %682, label %719

; <label>:682                                     ; preds = %621
  %683 = lshr i32 %30, 1
  %684 = lshr i32 %33, 1
  %685 = shl i32 %3, 1
  %686 = shl i32 %4, 1
  %687 = add i32 %683, %685
  %688 = add i32 %684, %686
  %689 = add nuw nsw i32 %683, %33
  %690 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %691 = extractvalue %dx.types.CBufRet.i32 %690, 0
  %692 = add i32 %691, 5
  %693 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %692, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %694 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %693, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %695 = fpext half %678 to float
  %696 = fpext half %679 to float
  %697 = fpext half %680 to float
  %698 = fpext half %681 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %694, i32 %687, i32 %688, i32 %5, float %695, float %696, float %697, float %698, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %699 = mul i32 %689, 16
  %700 = add i32 0, %699
  %701 = mul i32 %700, 2
  %702 = add i32 0, %701
  %703 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %702
  store half %678, half addrspace(3)* %703, align 2
  %704 = mul i32 %689, 16
  %705 = add i32 0, %704
  %706 = mul i32 %705, 2
  %707 = add i32 1, %706
  %708 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %707
  store half %679, half addrspace(3)* %708, align 2
  %709 = mul i32 %689, 16
  %710 = add i32 0, %709
  %711 = mul i32 %710, 2
  %712 = add i32 0, %711
  %713 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %712
  store half %680, half addrspace(3)* %713, align 2
  %714 = mul i32 %689, 16
  %715 = add i32 0, %714
  %716 = mul i32 %715, 2
  %717 = add i32 1, %716
  %718 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %717
  store half %681, half addrspace(3)* %718, align 2
  br label %719

; <label>:719                                     ; preds = %682, %621, %619
  %720 = icmp ult i32 %15, 6
  br i1 %720, label %1456, label %721

; <label>:721                                     ; preds = %719
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  %722 = icmp ult i32 %6, 4
  br i1 %722, label %723, label %790

; <label>:723                                     ; preds = %721
  %724 = mul i32 %6, 16
  %725 = add i32 0, %724
  %726 = mul i32 %725, 2
  %727 = add i32 0, %726
  %728 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %727
  %729 = load half, half addrspace(3)* %728, align 2
  %730 = mul i32 %6, 16
  %731 = add i32 0, %730
  %732 = mul i32 %731, 2
  %733 = add i32 1, %732
  %734 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %733
  %735 = load half, half addrspace(3)* %734, align 2
  %736 = mul i32 %6, 16
  %737 = add i32 0, %736
  %738 = mul i32 %737, 2
  %739 = add i32 0, %738
  %740 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %739
  %741 = load half, half addrspace(3)* %740, align 2
  %742 = mul i32 %6, 16
  %743 = add i32 0, %742
  %744 = mul i32 %743, 2
  %745 = add i32 1, %744
  %746 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %745
  %747 = load half, half addrspace(3)* %746, align 2
  %748 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %749 = and i32 %748, -4
  %750 = or i32 %749, 1
  %751 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %729, i32 %750)  ; WaveReadLaneAt(value,lane)
  %752 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %735, i32 %750)  ; WaveReadLaneAt(value,lane)
  %753 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %741, i32 %750)  ; WaveReadLaneAt(value,lane)
  %754 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %747, i32 %750)  ; WaveReadLaneAt(value,lane)
  %755 = or i32 %749, 2
  %756 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %729, i32 %755)  ; WaveReadLaneAt(value,lane)
  %757 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %735, i32 %755)  ; WaveReadLaneAt(value,lane)
  %758 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %741, i32 %755)  ; WaveReadLaneAt(value,lane)
  %759 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %747, i32 %755)  ; WaveReadLaneAt(value,lane)
  %760 = or i32 %748, 3
  %761 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %729, i32 %760)  ; WaveReadLaneAt(value,lane)
  %762 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %735, i32 %760)  ; WaveReadLaneAt(value,lane)
  %763 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %741, i32 %760)  ; WaveReadLaneAt(value,lane)
  %764 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %747, i32 %760)  ; WaveReadLaneAt(value,lane)
  br i1 %287, label %765, label %790

; <label>:765                                     ; preds = %723
  %766 = fadd fast half %754, %747
  %767 = fadd fast half %766, %759
  %768 = fadd fast half %767, %764
  %769 = fmul fast half %768, 0xH3400
  %770 = fadd fast half %753, %741
  %771 = fadd fast half %770, %758
  %772 = fadd fast half %771, %763
  %773 = fmul fast half %772, 0xH3400
  %774 = fadd fast half %752, %735
  %775 = fadd fast half %774, %757
  %776 = fadd fast half %775, %762
  %777 = fmul fast half %776, 0xH3400
  %778 = fadd fast half %751, %729
  %779 = fadd fast half %778, %756
  %780 = fadd fast half %779, %761
  %781 = fmul fast half %780, 0xH3400
  %782 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %783 = extractvalue %dx.types.CBufRet.i32 %782, 3
  %784 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %783, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %785 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %784, %dx.types.ResourceProperties { i32 20487, i32 1033 })  ; AnnotateHandle(res,props)  resource: globallycoherent RWTexture2DArray<4xF32>
  %786 = fpext half %781 to float
  %787 = fpext half %777 to float
  %788 = fpext half %773 to float
  %789 = fpext half %769 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %785, i32 %3, i32 %4, i32 %5, float %786, float %787, float %788, float %789, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  br label %790

; <label>:790                                     ; preds = %765, %723, %721
  %791 = icmp ult i32 %15, 7
  br i1 %791, label %1456, label %792

; <label>:792                                     ; preds = %790
  %793 = icmp eq i32 %6, 0
  br i1 %793, label %794, label %797

; <label>:794                                     ; preds = %792
  %795 = shl i32 %5, 2
  %796 = call i32 @dx.op.atomicBinOp.i32(i32 78, %dx.types.Handle %10, i32 0, i32 0, i32 %795, i32 undef, i32 1)  ; AtomicBinOp(handle,atomicOp,offset0,offset1,offset2,newValue)
  store i32 %796, i32 addrspace(3)* @"\01?gs_counter@@3IA", align 4, !tbaa !10
  br label %797

; <label>:797                                     ; preds = %794, %792
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  %798 = load i32, i32 addrspace(3)* @"\01?gs_counter@@3IA", align 4, !tbaa !10
  %799 = add i32 %16, -1
  %800 = icmp eq i32 %798, %799
  br i1 %800, label %801, label %1456

; <label>:801                                     ; preds = %797
  %802 = shl i32 %5, 2
  call void @dx.op.rawBufferStore.i32(i32 140, %dx.types.Handle %10, i32 0, i32 %802, i32 0, i32 undef, i32 undef, i32 undef, i8 1, i32 4)  ; RawBufferStore(uav,index,elementOffset,value0,value1,value2,value3,mask,alignment)
  %803 = shl nuw nsw i32 %30, 2
  %804 = shl nuw nsw i32 %33, 2
  %805 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %806 = extractvalue %dx.types.CBufRet.i32 %805, 3
  %807 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %806, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %808 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %807, %dx.types.ResourceProperties { i32 20487, i32 1033 })  ; AnnotateHandle(res,props)  resource: globallycoherent RWTexture2DArray<4xF32>
  %809 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %808, i32 undef, i32 %803, i32 %804, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %810 = extractvalue %dx.types.ResRet.f32 %809, 0
  %811 = extractvalue %dx.types.ResRet.f32 %809, 1
  %812 = extractvalue %dx.types.ResRet.f32 %809, 2
  %813 = extractvalue %dx.types.ResRet.f32 %809, 3
  %814 = fptrunc float %810 to half
  %815 = fptrunc float %811 to half
  %816 = fptrunc float %812 to half
  %817 = fptrunc float %813 to half
  %818 = or i32 %804, 1
  %819 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %808, i32 undef, i32 %803, i32 %818, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %820 = extractvalue %dx.types.ResRet.f32 %819, 0
  %821 = extractvalue %dx.types.ResRet.f32 %819, 1
  %822 = extractvalue %dx.types.ResRet.f32 %819, 2
  %823 = extractvalue %dx.types.ResRet.f32 %819, 3
  %824 = fptrunc float %820 to half
  %825 = fptrunc float %821 to half
  %826 = fptrunc float %822 to half
  %827 = fptrunc float %823 to half
  %828 = or i32 %803, 1
  %829 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %808, i32 undef, i32 %828, i32 %804, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %830 = extractvalue %dx.types.ResRet.f32 %829, 0
  %831 = extractvalue %dx.types.ResRet.f32 %829, 1
  %832 = extractvalue %dx.types.ResRet.f32 %829, 2
  %833 = extractvalue %dx.types.ResRet.f32 %829, 3
  %834 = fptrunc float %830 to half
  %835 = fptrunc float %831 to half
  %836 = fptrunc float %832 to half
  %837 = fptrunc float %833 to half
  %838 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %808, i32 undef, i32 %828, i32 %818, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %839 = extractvalue %dx.types.ResRet.f32 %838, 0
  %840 = extractvalue %dx.types.ResRet.f32 %838, 1
  %841 = extractvalue %dx.types.ResRet.f32 %838, 2
  %842 = extractvalue %dx.types.ResRet.f32 %838, 3
  %843 = fptrunc float %839 to half
  %844 = fptrunc float %840 to half
  %845 = fptrunc float %841 to half
  %846 = fptrunc float %842 to half
  %847 = fadd fast half %824, %814
  %848 = fadd fast half %825, %815
  %849 = fadd fast half %826, %816
  %850 = fadd fast half %827, %817
  %851 = fadd fast half %847, %834
  %852 = fadd fast half %848, %835
  %853 = fadd fast half %849, %836
  %854 = fadd fast half %850, %837
  %855 = fadd fast half %851, %843
  %856 = fadd fast half %852, %844
  %857 = fadd fast half %853, %845
  %858 = fadd fast half %854, %846
  %859 = fmul fast half %855, 0xH3400
  %860 = fmul fast half %856, 0xH3400
  %861 = fmul fast half %857, 0xH3400
  %862 = fmul fast half %858, 0xH3400
  %863 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %864 = extractvalue %dx.types.CBufRet.i32 %863, 0
  %865 = add i32 %864, 7
  %866 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %865, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %867 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %866, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %868 = fpext half %859 to float
  %869 = fpext half %860 to float
  %870 = fpext half %861 to float
  %871 = fpext half %862 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %867, i32 %34, i32 %35, i32 %5, float %868, float %869, float %870, float %871, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %872 = or i32 %803, 2
  %873 = or i32 %34, 1
  %874 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %875 = extractvalue %dx.types.CBufRet.i32 %874, 3
  %876 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %875, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %877 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %876, %dx.types.ResourceProperties { i32 20487, i32 1033 })  ; AnnotateHandle(res,props)  resource: globallycoherent RWTexture2DArray<4xF32>
  %878 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %877, i32 undef, i32 %872, i32 %804, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %879 = extractvalue %dx.types.ResRet.f32 %878, 0
  %880 = extractvalue %dx.types.ResRet.f32 %878, 1
  %881 = extractvalue %dx.types.ResRet.f32 %878, 2
  %882 = extractvalue %dx.types.ResRet.f32 %878, 3
  %883 = fptrunc float %879 to half
  %884 = fptrunc float %880 to half
  %885 = fptrunc float %881 to half
  %886 = fptrunc float %882 to half
  %887 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %877, i32 undef, i32 %872, i32 %818, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %888 = extractvalue %dx.types.ResRet.f32 %887, 0
  %889 = extractvalue %dx.types.ResRet.f32 %887, 1
  %890 = extractvalue %dx.types.ResRet.f32 %887, 2
  %891 = extractvalue %dx.types.ResRet.f32 %887, 3
  %892 = fptrunc float %888 to half
  %893 = fptrunc float %889 to half
  %894 = fptrunc float %890 to half
  %895 = fptrunc float %891 to half
  %896 = or i32 %803, 3
  %897 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %877, i32 undef, i32 %896, i32 %804, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %898 = extractvalue %dx.types.ResRet.f32 %897, 0
  %899 = extractvalue %dx.types.ResRet.f32 %897, 1
  %900 = extractvalue %dx.types.ResRet.f32 %897, 2
  %901 = extractvalue %dx.types.ResRet.f32 %897, 3
  %902 = fptrunc float %898 to half
  %903 = fptrunc float %899 to half
  %904 = fptrunc float %900 to half
  %905 = fptrunc float %901 to half
  %906 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %877, i32 undef, i32 %896, i32 %818, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %907 = extractvalue %dx.types.ResRet.f32 %906, 0
  %908 = extractvalue %dx.types.ResRet.f32 %906, 1
  %909 = extractvalue %dx.types.ResRet.f32 %906, 2
  %910 = extractvalue %dx.types.ResRet.f32 %906, 3
  %911 = fptrunc float %907 to half
  %912 = fptrunc float %908 to half
  %913 = fptrunc float %909 to half
  %914 = fptrunc float %910 to half
  %915 = fadd fast half %892, %883
  %916 = fadd fast half %893, %884
  %917 = fadd fast half %894, %885
  %918 = fadd fast half %895, %886
  %919 = fadd fast half %915, %902
  %920 = fadd fast half %916, %903
  %921 = fadd fast half %917, %904
  %922 = fadd fast half %918, %905
  %923 = fadd fast half %919, %911
  %924 = fadd fast half %920, %912
  %925 = fadd fast half %921, %913
  %926 = fadd fast half %922, %914
  %927 = fmul fast half %923, 0xH3400
  %928 = fmul fast half %924, 0xH3400
  %929 = fmul fast half %925, 0xH3400
  %930 = fmul fast half %926, 0xH3400
  %931 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %932 = extractvalue %dx.types.CBufRet.i32 %931, 0
  %933 = add i32 %932, 7
  %934 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %933, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %935 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %934, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %936 = fpext half %927 to float
  %937 = fpext half %928 to float
  %938 = fpext half %929 to float
  %939 = fpext half %930 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %935, i32 %873, i32 %35, i32 %5, float %936, float %937, float %938, float %939, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %940 = or i32 %804, 2
  %941 = or i32 %35, 1
  %942 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %943 = extractvalue %dx.types.CBufRet.i32 %942, 3
  %944 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %943, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %945 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %944, %dx.types.ResourceProperties { i32 20487, i32 1033 })  ; AnnotateHandle(res,props)  resource: globallycoherent RWTexture2DArray<4xF32>
  %946 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %945, i32 undef, i32 %803, i32 %940, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %947 = extractvalue %dx.types.ResRet.f32 %946, 0
  %948 = extractvalue %dx.types.ResRet.f32 %946, 1
  %949 = extractvalue %dx.types.ResRet.f32 %946, 2
  %950 = extractvalue %dx.types.ResRet.f32 %946, 3
  %951 = fptrunc float %947 to half
  %952 = fptrunc float %948 to half
  %953 = fptrunc float %949 to half
  %954 = fptrunc float %950 to half
  %955 = or i32 %804, 3
  %956 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %945, i32 undef, i32 %803, i32 %955, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %957 = extractvalue %dx.types.ResRet.f32 %956, 0
  %958 = extractvalue %dx.types.ResRet.f32 %956, 1
  %959 = extractvalue %dx.types.ResRet.f32 %956, 2
  %960 = extractvalue %dx.types.ResRet.f32 %956, 3
  %961 = fptrunc float %957 to half
  %962 = fptrunc float %958 to half
  %963 = fptrunc float %959 to half
  %964 = fptrunc float %960 to half
  %965 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %945, i32 undef, i32 %828, i32 %940, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %966 = extractvalue %dx.types.ResRet.f32 %965, 0
  %967 = extractvalue %dx.types.ResRet.f32 %965, 1
  %968 = extractvalue %dx.types.ResRet.f32 %965, 2
  %969 = extractvalue %dx.types.ResRet.f32 %965, 3
  %970 = fptrunc float %966 to half
  %971 = fptrunc float %967 to half
  %972 = fptrunc float %968 to half
  %973 = fptrunc float %969 to half
  %974 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %945, i32 undef, i32 %828, i32 %955, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %975 = extractvalue %dx.types.ResRet.f32 %974, 0
  %976 = extractvalue %dx.types.ResRet.f32 %974, 1
  %977 = extractvalue %dx.types.ResRet.f32 %974, 2
  %978 = extractvalue %dx.types.ResRet.f32 %974, 3
  %979 = fptrunc float %975 to half
  %980 = fptrunc float %976 to half
  %981 = fptrunc float %977 to half
  %982 = fptrunc float %978 to half
  %983 = fadd fast half %961, %951
  %984 = fadd fast half %962, %952
  %985 = fadd fast half %963, %953
  %986 = fadd fast half %964, %954
  %987 = fadd fast half %983, %970
  %988 = fadd fast half %984, %971
  %989 = fadd fast half %985, %972
  %990 = fadd fast half %986, %973
  %991 = fadd fast half %987, %979
  %992 = fadd fast half %988, %980
  %993 = fadd fast half %989, %981
  %994 = fadd fast half %990, %982
  %995 = fmul fast half %991, 0xH3400
  %996 = fmul fast half %992, 0xH3400
  %997 = fmul fast half %993, 0xH3400
  %998 = fmul fast half %994, 0xH3400
  %999 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %1000 = extractvalue %dx.types.CBufRet.i32 %999, 0
  %1001 = add i32 %1000, 7
  %1002 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %1001, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %1003 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1002, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %1004 = fpext half %995 to float
  %1005 = fpext half %996 to float
  %1006 = fpext half %997 to float
  %1007 = fpext half %998 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %1003, i32 %34, i32 %941, i32 %5, float %1004, float %1005, float %1006, float %1007, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %1008 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %1009 = extractvalue %dx.types.CBufRet.i32 %1008, 3
  %1010 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %1009, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %1011 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1010, %dx.types.ResourceProperties { i32 20487, i32 1033 })  ; AnnotateHandle(res,props)  resource: globallycoherent RWTexture2DArray<4xF32>
  %1012 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %1011, i32 undef, i32 %872, i32 %940, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %1013 = extractvalue %dx.types.ResRet.f32 %1012, 0
  %1014 = extractvalue %dx.types.ResRet.f32 %1012, 1
  %1015 = extractvalue %dx.types.ResRet.f32 %1012, 2
  %1016 = extractvalue %dx.types.ResRet.f32 %1012, 3
  %1017 = fptrunc float %1013 to half
  %1018 = fptrunc float %1014 to half
  %1019 = fptrunc float %1015 to half
  %1020 = fptrunc float %1016 to half
  %1021 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %1011, i32 undef, i32 %872, i32 %955, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %1022 = extractvalue %dx.types.ResRet.f32 %1021, 0
  %1023 = extractvalue %dx.types.ResRet.f32 %1021, 1
  %1024 = extractvalue %dx.types.ResRet.f32 %1021, 2
  %1025 = extractvalue %dx.types.ResRet.f32 %1021, 3
  %1026 = fptrunc float %1022 to half
  %1027 = fptrunc float %1023 to half
  %1028 = fptrunc float %1024 to half
  %1029 = fptrunc float %1025 to half
  %1030 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %1011, i32 undef, i32 %896, i32 %940, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %1031 = extractvalue %dx.types.ResRet.f32 %1030, 0
  %1032 = extractvalue %dx.types.ResRet.f32 %1030, 1
  %1033 = extractvalue %dx.types.ResRet.f32 %1030, 2
  %1034 = extractvalue %dx.types.ResRet.f32 %1030, 3
  %1035 = fptrunc float %1031 to half
  %1036 = fptrunc float %1032 to half
  %1037 = fptrunc float %1033 to half
  %1038 = fptrunc float %1034 to half
  %1039 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %1011, i32 undef, i32 %896, i32 %955, i32 %5, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %1040 = extractvalue %dx.types.ResRet.f32 %1039, 0
  %1041 = extractvalue %dx.types.ResRet.f32 %1039, 1
  %1042 = extractvalue %dx.types.ResRet.f32 %1039, 2
  %1043 = extractvalue %dx.types.ResRet.f32 %1039, 3
  %1044 = fptrunc float %1040 to half
  %1045 = fptrunc float %1041 to half
  %1046 = fptrunc float %1042 to half
  %1047 = fptrunc float %1043 to half
  %1048 = fadd fast half %1026, %1017
  %1049 = fadd fast half %1027, %1018
  %1050 = fadd fast half %1028, %1019
  %1051 = fadd fast half %1029, %1020
  %1052 = fadd fast half %1048, %1035
  %1053 = fadd fast half %1049, %1036
  %1054 = fadd fast half %1050, %1037
  %1055 = fadd fast half %1051, %1038
  %1056 = fadd fast half %1052, %1044
  %1057 = fadd fast half %1053, %1045
  %1058 = fadd fast half %1054, %1046
  %1059 = fadd fast half %1055, %1047
  %1060 = fmul fast half %1056, 0xH3400
  %1061 = fmul fast half %1057, 0xH3400
  %1062 = fmul fast half %1058, 0xH3400
  %1063 = fmul fast half %1059, 0xH3400
  %1064 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %1065 = extractvalue %dx.types.CBufRet.i32 %1064, 0
  %1066 = add i32 %1065, 7
  %1067 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %1066, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %1068 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1067, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %1069 = fpext half %1060 to float
  %1070 = fpext half %1061 to float
  %1071 = fpext half %1062 to float
  %1072 = fpext half %1063 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %1068, i32 %873, i32 %941, i32 %5, float %1069, float %1070, float %1071, float %1072, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %1073 = icmp ult i32 %15, 8
  br i1 %1073, label %1456, label %1074

; <label>:1074                                    ; preds = %801
  %1075 = fadd fast half %923, %855
  %1076 = fadd fast half %924, %856
  %1077 = fadd fast half %925, %857
  %1078 = fadd fast half %926, %858
  %1079 = fadd fast half %1075, %991
  %1080 = fadd fast half %1076, %992
  %1081 = fadd fast half %1077, %993
  %1082 = fadd fast half %1078, %994
  %1083 = fadd fast half %1079, %1056
  %1084 = fadd fast half %1080, %1057
  %1085 = fadd fast half %1081, %1058
  %1086 = fadd fast half %1082, %1059
  %1087 = fmul fast half %1083, 0xH2C00
  %1088 = fmul fast half %1084, 0xH2C00
  %1089 = fmul fast half %1085, 0xH2C00
  %1090 = fmul fast half %1086, 0xH2C00
  %1091 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %1092 = extractvalue %dx.types.CBufRet.i32 %1091, 0
  %1093 = add i32 %1092, 8
  %1094 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %1093, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %1095 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1094, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %1096 = fpext half %1087 to float
  %1097 = fpext half %1088 to float
  %1098 = fpext half %1089 to float
  %1099 = fpext half %1090 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %1095, i32 %30, i32 %33, i32 %5, float %1096, float %1097, float %1098, float %1099, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %1100 = mul i32 %30, 16
  %1101 = add i32 %33, %1100
  %1102 = mul i32 %1101, 2
  %1103 = add i32 0, %1102
  %1104 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1103
  store half %1087, half addrspace(3)* %1104, align 2
  %1105 = mul i32 %30, 16
  %1106 = add i32 %33, %1105
  %1107 = mul i32 %1106, 2
  %1108 = add i32 1, %1107
  %1109 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1108
  store half %1088, half addrspace(3)* %1109, align 2
  %1110 = mul i32 %30, 16
  %1111 = add i32 %33, %1110
  %1112 = mul i32 %1111, 2
  %1113 = add i32 0, %1112
  %1114 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1113
  store half %1089, half addrspace(3)* %1114, align 2
  %1115 = mul i32 %30, 16
  %1116 = add i32 %33, %1115
  %1117 = mul i32 %1116, 2
  %1118 = add i32 1, %1117
  %1119 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1118
  store half %1090, half addrspace(3)* %1119, align 2
  %1120 = icmp ult i32 %15, 9
  br i1 %1120, label %1456, label %1121

; <label>:1121                                    ; preds = %1074
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  br i1 %419, label %1122, label %1194

; <label>:1122                                    ; preds = %1121
  %1123 = load half, half addrspace(3)* %1104, align 2
  %1124 = load half, half addrspace(3)* %1109, align 2
  %1125 = load half, half addrspace(3)* %1114, align 2
  %1126 = load half, half addrspace(3)* %1119, align 2
  %1127 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %1128 = and i32 %1127, -4
  %1129 = or i32 %1128, 1
  %1130 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1123, i32 %1129)  ; WaveReadLaneAt(value,lane)
  %1131 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1124, i32 %1129)  ; WaveReadLaneAt(value,lane)
  %1132 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1125, i32 %1129)  ; WaveReadLaneAt(value,lane)
  %1133 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1126, i32 %1129)  ; WaveReadLaneAt(value,lane)
  %1134 = or i32 %1128, 2
  %1135 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1123, i32 %1134)  ; WaveReadLaneAt(value,lane)
  %1136 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1124, i32 %1134)  ; WaveReadLaneAt(value,lane)
  %1137 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1125, i32 %1134)  ; WaveReadLaneAt(value,lane)
  %1138 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1126, i32 %1134)  ; WaveReadLaneAt(value,lane)
  %1139 = or i32 %1127, 3
  %1140 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1123, i32 %1139)  ; WaveReadLaneAt(value,lane)
  %1141 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1124, i32 %1139)  ; WaveReadLaneAt(value,lane)
  %1142 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1125, i32 %1139)  ; WaveReadLaneAt(value,lane)
  %1143 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1126, i32 %1139)  ; WaveReadLaneAt(value,lane)
  %1144 = fadd fast half %1130, %1123
  %1145 = fadd fast half %1131, %1124
  %1146 = fadd fast half %1132, %1125
  %1147 = fadd fast half %1133, %1126
  %1148 = fadd fast half %1144, %1135
  %1149 = fadd fast half %1145, %1136
  %1150 = fadd fast half %1146, %1137
  %1151 = fadd fast half %1147, %1138
  %1152 = fadd fast half %1148, %1140
  %1153 = fadd fast half %1149, %1141
  %1154 = fadd fast half %1150, %1142
  %1155 = fadd fast half %1151, %1143
  %1156 = fmul fast half %1152, 0xH3400
  %1157 = fmul fast half %1153, 0xH3400
  %1158 = fmul fast half %1154, 0xH3400
  %1159 = fmul fast half %1155, 0xH3400
  br i1 %287, label %1160, label %1194

; <label>:1160                                    ; preds = %1122
  %1161 = lshr i32 %30, 1
  %1162 = lshr i32 %33, 1
  %1163 = and i32 %23, 1
  %1164 = add nuw nsw i32 %30, %1163
  %1165 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %1166 = extractvalue %dx.types.CBufRet.i32 %1165, 0
  %1167 = add i32 %1166, 9
  %1168 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %1167, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %1169 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1168, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %1170 = fpext half %1156 to float
  %1171 = fpext half %1157 to float
  %1172 = fpext half %1158 to float
  %1173 = fpext half %1159 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %1169, i32 %1161, i32 %1162, i32 %5, float %1170, float %1171, float %1172, float %1173, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %1174 = mul i32 %1164, 16
  %1175 = add i32 %33, %1174
  %1176 = mul i32 %1175, 2
  %1177 = add i32 0, %1176
  %1178 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1177
  store half %1156, half addrspace(3)* %1178, align 2
  %1179 = mul i32 %1164, 16
  %1180 = add i32 %33, %1179
  %1181 = mul i32 %1180, 2
  %1182 = add i32 1, %1181
  %1183 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1182
  store half %1157, half addrspace(3)* %1183, align 2
  %1184 = mul i32 %1164, 16
  %1185 = add i32 %33, %1184
  %1186 = mul i32 %1185, 2
  %1187 = add i32 0, %1186
  %1188 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1187
  store half %1158, half addrspace(3)* %1188, align 2
  %1189 = mul i32 %1164, 16
  %1190 = add i32 %33, %1189
  %1191 = mul i32 %1190, 2
  %1192 = add i32 1, %1191
  %1193 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1192
  store half %1159, half addrspace(3)* %1193, align 2
  br label %1194

; <label>:1194                                    ; preds = %1160, %1122, %1121
  %1195 = icmp ult i32 %15, 10
  br i1 %1195, label %1456, label %1196

; <label>:1196                                    ; preds = %1194
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  br i1 %519, label %1197, label %1290

; <label>:1197                                    ; preds = %1196
  %1198 = and i32 %21, 1
  %1199 = or i32 %34, %1198
  %1200 = mul i32 %1199, 16
  %1201 = add i32 %35, %1200
  %1202 = mul i32 %1201, 2
  %1203 = add i32 0, %1202
  %1204 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1203
  %1205 = load half, half addrspace(3)* %1204, align 2
  %1206 = mul i32 %1199, 16
  %1207 = add i32 %35, %1206
  %1208 = mul i32 %1207, 2
  %1209 = add i32 1, %1208
  %1210 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1209
  %1211 = load half, half addrspace(3)* %1210, align 2
  %1212 = mul i32 %1199, 16
  %1213 = add i32 %35, %1212
  %1214 = mul i32 %1213, 2
  %1215 = add i32 0, %1214
  %1216 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1215
  %1217 = load half, half addrspace(3)* %1216, align 2
  %1218 = mul i32 %1199, 16
  %1219 = add i32 %35, %1218
  %1220 = mul i32 %1219, 2
  %1221 = add i32 1, %1220
  %1222 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1221
  %1223 = load half, half addrspace(3)* %1222, align 2
  %1224 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %1225 = and i32 %1224, -4
  %1226 = or i32 %1225, 1
  %1227 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1205, i32 %1226)  ; WaveReadLaneAt(value,lane)
  %1228 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1211, i32 %1226)  ; WaveReadLaneAt(value,lane)
  %1229 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1217, i32 %1226)  ; WaveReadLaneAt(value,lane)
  %1230 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1223, i32 %1226)  ; WaveReadLaneAt(value,lane)
  %1231 = or i32 %1225, 2
  %1232 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1205, i32 %1231)  ; WaveReadLaneAt(value,lane)
  %1233 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1211, i32 %1231)  ; WaveReadLaneAt(value,lane)
  %1234 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1217, i32 %1231)  ; WaveReadLaneAt(value,lane)
  %1235 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1223, i32 %1231)  ; WaveReadLaneAt(value,lane)
  %1236 = or i32 %1224, 3
  %1237 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1205, i32 %1236)  ; WaveReadLaneAt(value,lane)
  %1238 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1211, i32 %1236)  ; WaveReadLaneAt(value,lane)
  %1239 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1217, i32 %1236)  ; WaveReadLaneAt(value,lane)
  %1240 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1223, i32 %1236)  ; WaveReadLaneAt(value,lane)
  %1241 = fadd fast half %1227, %1205
  %1242 = fadd fast half %1228, %1211
  %1243 = fadd fast half %1229, %1217
  %1244 = fadd fast half %1230, %1223
  %1245 = fadd fast half %1241, %1232
  %1246 = fadd fast half %1242, %1233
  %1247 = fadd fast half %1243, %1234
  %1248 = fadd fast half %1244, %1235
  %1249 = fadd fast half %1245, %1237
  %1250 = fadd fast half %1246, %1238
  %1251 = fadd fast half %1247, %1239
  %1252 = fadd fast half %1248, %1240
  %1253 = fmul fast half %1249, 0xH3400
  %1254 = fmul fast half %1250, 0xH3400
  %1255 = fmul fast half %1251, 0xH3400
  %1256 = fmul fast half %1252, 0xH3400
  br i1 %287, label %1257, label %1290

; <label>:1257                                    ; preds = %1197
  %1258 = lshr i32 %30, 1
  %1259 = lshr i32 %33, 1
  %1260 = add nuw nsw i32 %34, %1259
  %1261 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %1262 = extractvalue %dx.types.CBufRet.i32 %1261, 0
  %1263 = add i32 %1262, 10
  %1264 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %1263, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %1265 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1264, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %1266 = fpext half %1253 to float
  %1267 = fpext half %1254 to float
  %1268 = fpext half %1255 to float
  %1269 = fpext half %1256 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %1265, i32 %1258, i32 %1259, i32 %5, float %1266, float %1267, float %1268, float %1269, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %1270 = mul i32 %1260, 16
  %1271 = add i32 %35, %1270
  %1272 = mul i32 %1271, 2
  %1273 = add i32 0, %1272
  %1274 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1273
  store half %1253, half addrspace(3)* %1274, align 2
  %1275 = mul i32 %1260, 16
  %1276 = add i32 %35, %1275
  %1277 = mul i32 %1276, 2
  %1278 = add i32 1, %1277
  %1279 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1278
  store half %1254, half addrspace(3)* %1279, align 2
  %1280 = mul i32 %1260, 16
  %1281 = add i32 %35, %1280
  %1282 = mul i32 %1281, 2
  %1283 = add i32 0, %1282
  %1284 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1283
  store half %1255, half addrspace(3)* %1284, align 2
  %1285 = mul i32 %1260, 16
  %1286 = add i32 %35, %1285
  %1287 = mul i32 %1286, 2
  %1288 = add i32 1, %1287
  %1289 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1288
  store half %1256, half addrspace(3)* %1289, align 2
  br label %1290

; <label>:1290                                    ; preds = %1257, %1197, %1196
  %1291 = icmp ult i32 %15, 11
  br i1 %1291, label %1456, label %1292

; <label>:1292                                    ; preds = %1290
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  br i1 %620, label %1293, label %1385

; <label>:1293                                    ; preds = %1292
  %1294 = add nuw nsw i32 %803, %33
  %1295 = mul i32 %1294, 16
  %1296 = add i32 %804, %1295
  %1297 = mul i32 %1296, 2
  %1298 = add i32 0, %1297
  %1299 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1298
  %1300 = load half, half addrspace(3)* %1299, align 2
  %1301 = mul i32 %1294, 16
  %1302 = add i32 %804, %1301
  %1303 = mul i32 %1302, 2
  %1304 = add i32 1, %1303
  %1305 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1304
  %1306 = load half, half addrspace(3)* %1305, align 2
  %1307 = mul i32 %1294, 16
  %1308 = add i32 %804, %1307
  %1309 = mul i32 %1308, 2
  %1310 = add i32 0, %1309
  %1311 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1310
  %1312 = load half, half addrspace(3)* %1311, align 2
  %1313 = mul i32 %1294, 16
  %1314 = add i32 %804, %1313
  %1315 = mul i32 %1314, 2
  %1316 = add i32 1, %1315
  %1317 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1316
  %1318 = load half, half addrspace(3)* %1317, align 2
  %1319 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %1320 = and i32 %1319, -4
  %1321 = or i32 %1320, 1
  %1322 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1300, i32 %1321)  ; WaveReadLaneAt(value,lane)
  %1323 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1306, i32 %1321)  ; WaveReadLaneAt(value,lane)
  %1324 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1312, i32 %1321)  ; WaveReadLaneAt(value,lane)
  %1325 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1318, i32 %1321)  ; WaveReadLaneAt(value,lane)
  %1326 = or i32 %1320, 2
  %1327 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1300, i32 %1326)  ; WaveReadLaneAt(value,lane)
  %1328 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1306, i32 %1326)  ; WaveReadLaneAt(value,lane)
  %1329 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1312, i32 %1326)  ; WaveReadLaneAt(value,lane)
  %1330 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1318, i32 %1326)  ; WaveReadLaneAt(value,lane)
  %1331 = or i32 %1319, 3
  %1332 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1300, i32 %1331)  ; WaveReadLaneAt(value,lane)
  %1333 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1306, i32 %1331)  ; WaveReadLaneAt(value,lane)
  %1334 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1312, i32 %1331)  ; WaveReadLaneAt(value,lane)
  %1335 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1318, i32 %1331)  ; WaveReadLaneAt(value,lane)
  %1336 = fadd fast half %1322, %1300
  %1337 = fadd fast half %1323, %1306
  %1338 = fadd fast half %1324, %1312
  %1339 = fadd fast half %1325, %1318
  %1340 = fadd fast half %1336, %1327
  %1341 = fadd fast half %1337, %1328
  %1342 = fadd fast half %1338, %1329
  %1343 = fadd fast half %1339, %1330
  %1344 = fadd fast half %1340, %1332
  %1345 = fadd fast half %1341, %1333
  %1346 = fadd fast half %1342, %1334
  %1347 = fadd fast half %1343, %1335
  %1348 = fmul fast half %1344, 0xH3400
  %1349 = fmul fast half %1345, 0xH3400
  %1350 = fmul fast half %1346, 0xH3400
  %1351 = fmul fast half %1347, 0xH3400
  br i1 %287, label %1352, label %1385

; <label>:1352                                    ; preds = %1293
  %1353 = lshr i32 %30, 1
  %1354 = lshr i32 %33, 1
  %1355 = add nuw nsw i32 %1353, %33
  %1356 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %1357 = extractvalue %dx.types.CBufRet.i32 %1356, 0
  %1358 = add i32 %1357, 11
  %1359 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %1358, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %1360 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1359, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %1361 = fpext half %1348 to float
  %1362 = fpext half %1349 to float
  %1363 = fpext half %1350 to float
  %1364 = fpext half %1351 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %1360, i32 %1353, i32 %1354, i32 %5, float %1361, float %1362, float %1363, float %1364, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %1365 = mul i32 %1355, 16
  %1366 = add i32 0, %1365
  %1367 = mul i32 %1366, 2
  %1368 = add i32 0, %1367
  %1369 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1368
  store half %1348, half addrspace(3)* %1369, align 2
  %1370 = mul i32 %1355, 16
  %1371 = add i32 0, %1370
  %1372 = mul i32 %1371, 2
  %1373 = add i32 1, %1372
  %1374 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1373
  store half %1349, half addrspace(3)* %1374, align 2
  %1375 = mul i32 %1355, 16
  %1376 = add i32 0, %1375
  %1377 = mul i32 %1376, 2
  %1378 = add i32 0, %1377
  %1379 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1378
  store half %1350, half addrspace(3)* %1379, align 2
  %1380 = mul i32 %1355, 16
  %1381 = add i32 0, %1380
  %1382 = mul i32 %1381, 2
  %1383 = add i32 1, %1382
  %1384 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1383
  store half %1351, half addrspace(3)* %1384, align 2
  br label %1385

; <label>:1385                                    ; preds = %1352, %1293, %1292
  %1386 = icmp ult i32 %15, 12
  br i1 %1386, label %1456, label %1387

; <label>:1387                                    ; preds = %1385
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  br i1 %722, label %1388, label %1456

; <label>:1388                                    ; preds = %1387
  %1389 = mul i32 %6, 16
  %1390 = add i32 0, %1389
  %1391 = mul i32 %1390, 2
  %1392 = add i32 0, %1391
  %1393 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1392
  %1394 = load half, half addrspace(3)* %1393, align 2
  %1395 = mul i32 %6, 16
  %1396 = add i32 0, %1395
  %1397 = mul i32 %1396, 2
  %1398 = add i32 1, %1397
  %1399 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_rg@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1398
  %1400 = load half, half addrspace(3)* %1399, align 2
  %1401 = mul i32 %6, 16
  %1402 = add i32 0, %1401
  %1403 = mul i32 %1402, 2
  %1404 = add i32 0, %1403
  %1405 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1404
  %1406 = load half, half addrspace(3)* %1405, align 2
  %1407 = mul i32 %6, 16
  %1408 = add i32 0, %1407
  %1409 = mul i32 %1408, 2
  %1410 = add i32 1, %1409
  %1411 = getelementptr [512 x half], [512 x half] addrspace(3)* @"\01?gs_tmp_ba@@3PAY0BA@$$CAV?$vector@$f16@$01@@A.v.1dim", i32 0, i32 %1410
  %1412 = load half, half addrspace(3)* %1411, align 2
  %1413 = call i32 @dx.op.waveGetLaneIndex(i32 111)  ; WaveGetLaneIndex()
  %1414 = and i32 %1413, -4
  %1415 = or i32 %1414, 1
  %1416 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1394, i32 %1415)  ; WaveReadLaneAt(value,lane)
  %1417 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1400, i32 %1415)  ; WaveReadLaneAt(value,lane)
  %1418 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1406, i32 %1415)  ; WaveReadLaneAt(value,lane)
  %1419 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1412, i32 %1415)  ; WaveReadLaneAt(value,lane)
  %1420 = or i32 %1414, 2
  %1421 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1394, i32 %1420)  ; WaveReadLaneAt(value,lane)
  %1422 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1400, i32 %1420)  ; WaveReadLaneAt(value,lane)
  %1423 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1406, i32 %1420)  ; WaveReadLaneAt(value,lane)
  %1424 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1412, i32 %1420)  ; WaveReadLaneAt(value,lane)
  %1425 = or i32 %1413, 3
  %1426 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1394, i32 %1425)  ; WaveReadLaneAt(value,lane)
  %1427 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1400, i32 %1425)  ; WaveReadLaneAt(value,lane)
  %1428 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1406, i32 %1425)  ; WaveReadLaneAt(value,lane)
  %1429 = call half @dx.op.waveReadLaneAt.f16(i32 117, half %1412, i32 %1425)  ; WaveReadLaneAt(value,lane)
  br i1 %287, label %1430, label %1456

; <label>:1430                                    ; preds = %1388
  %1431 = fadd fast half %1419, %1412
  %1432 = fadd fast half %1431, %1424
  %1433 = fadd fast half %1432, %1429
  %1434 = fmul fast half %1433, 0xH3400
  %1435 = fadd fast half %1418, %1406
  %1436 = fadd fast half %1435, %1423
  %1437 = fadd fast half %1436, %1428
  %1438 = fmul fast half %1437, 0xH3400
  %1439 = fadd fast half %1417, %1400
  %1440 = fadd fast half %1439, %1422
  %1441 = fadd fast half %1440, %1427
  %1442 = fmul fast half %1441, 0xH3400
  %1443 = fadd fast half %1416, %1394
  %1444 = fadd fast half %1443, %1421
  %1445 = fadd fast half %1444, %1426
  %1446 = fmul fast half %1445, 0xH3400
  %1447 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %1448 = extractvalue %dx.types.CBufRet.i32 %1447, 0
  %1449 = add i32 %1448, 12
  %1450 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %1449, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %1451 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1450, %dx.types.ResourceProperties { i32 4103, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2DArray<4xF32>
  %1452 = fpext half %1446 to float
  %1453 = fpext half %1442 to float
  %1454 = fpext half %1438 to float
  %1455 = fpext half %1434 to float
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %1451, i32 0, i32 0, i32 %5, float %1452, float %1453, float %1454, float %1455, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  br label %1456

; <label>:1456                                    ; preds = %1430, %1388, %1387, %1385, %1290, %1194, %1074, %801, %797, %790, %719, %617, %516, %416, %0
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
!6 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 20, null}
!7 = !{void ()* @downsample_cs, !"downsample_cs", null, !4, !8}
!8 = !{i32 0, i64 11820081440, i32 4, !9}
!9 = !{i32 256, i32 1, i32 1}
!10 = !{!11, !11, i64 0}
!11 = !{!"int", !12, i64 0}
!12 = !{!"omnipotent char", !13, i64 0}
!13 = !{!"Simple C/C++ TBAA"}
*/
auto Shaders::spd_downsample_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(18028, 14096);
}

#undef texture_data

} // namespace fb::baked::kitchen
