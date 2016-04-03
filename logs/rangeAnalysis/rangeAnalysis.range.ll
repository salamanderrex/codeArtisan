; ModuleID = '/home/parallels/Desktop/cse231-proj1/logs/rangeAnalysis/rangeAnalysis.range.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  %d = alloca i32, align 4
  %e = alloca i32, align 4
  %f = alloca i32, align 4
  %g = alloca i32, align 4
  %h = alloca i32, align 4
  store i32 0, i32* %retval
  store i32 5, i32* %a, align 4
  %0 = load i32* %a, align 4
  %add = add nsw i32 %0, 50
  store i32 %add, i32* %b, align 4
  %1 = load i32* %b, align 4
  %mul = mul nsw i32 %1, 50
  store i32 %mul, i32* %c, align 4
  %2 = load i32* %b, align 4
  %div = sdiv i32 %2, 25
  store i32 %div, i32* %d, align 4
  %3 = load i32* %b, align 4
  %rem = srem i32 %3, 25
  store i32 %rem, i32* %e, align 4
  %4 = load i32* %b, align 4
  %sub = sub nsw i32 %4, 50
  store i32 %sub, i32* %f, align 4
  %5 = load i32* %b, align 4
  %shr = ashr i32 %5, 1
  store i32 %shr, i32* %g, align 4
  %6 = load i32* %b, align 4
  %shl = shl i32 %6, 1
  store i32 %shl, i32* %h, align 4
  %7 = load i32* %a, align 4
  ret i32 %7
}

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.4 (tags/RELEASE_34/final)"}
