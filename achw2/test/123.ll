; ModuleID = 'test.c'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nounwind uwtable
define i32 @main() #0 {	
		%retval = alloca i32, align 4
		%a = alloca i32 ,align 4
		%b = alloca i32 , align 4
		%c = alloca i32 , align 4
		store i32 0, i32* %retval
		store i32 30, i32* %a, align 4
		store i32 40, i32* %b, align 4
		%1 = load i32, i32* %a, align 4
		%2 = load i32, i32* %b, align 4
		%add = add nsw i32 %1, %2
		%add1 = add nsw i32 %add, 10
		store i32 %add1, i32* %c, align 4
		%3 = load i32, i32* %a, align 4
		%4 = load i32, i32* %b, align 4
		%add2 = add nsw i32 %3, %4
		%5 = load i32, i32* %c, align 4
		%mul = mul nsw i32 %add2, %5
		ret i32 0
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"Ubuntu clang version 3.7.0-2ubuntu4 (tags/RELEASE_370/final) (based on LLVM 3.7.0)"}
