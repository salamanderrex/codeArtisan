; ModuleID = '/home/parallels/Desktop/cse231-proj1/logs/x=\*y/x=\*y.pointer.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
entry:
  %hiddenValue = alloca i32, align 4
  %y_other = alloca i32*, align 8
  %x = alloca i32*, align 8
  %y = alloca i32**, align 8
  store i32* %hiddenValue, i32** %y_other, align 8
  store i32** %y_other, i32*** %y, align 8
  %0 = load i32*** %y, align 8
  %1 = load i32** %0, align 8
  store i32* %1, i32** %x, align 8
  ret i32 0
}

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.4 (tags/RELEASE_34/final)"}
