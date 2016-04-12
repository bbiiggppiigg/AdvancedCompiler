; ModuleID = 'test.ll'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  %d = alloca i32, align 4
  %e = alloca i32, align 4
  %f = alloca i32, align 4
  store i32 0, i32* %1
  store i32 5, i32* %a, align 4
  store i32 6, i32* %b, align 4
  %2 = load i32, i32* %a, align 4
  %3 = load i32, i32* %b, align 4
  %4 = add nsw i32 %2, %3
  store i32 %4, i32* %c, align 4
  %5 = load i32, i32* %a, align 4
  %6 = load i32, i32* %b, align 4
  %7 = add nsw i32 %5, %6
  store i32 %7, i32* %d, align 4
  %8 = load i32, i32* %c, align 4
  %9 = load i32, i32* %c, align 4
  %10 = add nsw i32 %8, %9
  store i32 %10, i32* %e, align 4
  store i32 7, i32* %d, align 4
  %11 = load i32, i32* %d, align 4
  %12 = load i32, i32* %d, align 4
  %13 = add nsw i32 %11, %12
  store i32 %13, i32* %f, align 4
  %14 = load i32, i32* %c, align 4
  %15 = load i32, i32* %d, align 4
  %16 = icmp ne i32 %14, %15
  br i1 %16, label %17, label %18

; <label>:17                                      ; preds = %0
  store i32 1, i32* %1
  br label %19

; <label>:18                                      ; preds = %0
  store i32 0, i32* %1
  br label %19

; <label>:19                                      ; preds = %18, %17
  %20 = load i32, i32* %1
  ret i32 %20
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"Ubuntu clang version 3.7.0-2ubuntu4 (tags/RELEASE_370/final) (based on LLVM 3.7.0)"}
