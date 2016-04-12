; ModuleID = 'a.ll'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [12 x i8] c"Hello World\00", align 1
@.str.1 = private unnamed_addr constant [10 x i8] c"WTF Wolrd\00", align 1

; Function Attrs: nounwind uwtable
define i32 @sum(i32 %c, i32 %d) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 %c, i32* %1, align 4
  store i32 %d, i32* %2, align 4
  %3 = load i32, i32* %1, align 4
  %4 = load i32, i32* %2, align 4
  %5 = add nsw i32 %3, %4
  ret i32 %5
}

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  %d = alloca i32, align 4
  %2 = alloca i8*
  store i32 0, i32* %1
  store i32 5, i32* %a, align 4
  store i32 6, i32* %b, align 4
  %3 = load i32, i32* %a, align 4
  %4 = load i32, i32* %b, align 4
  %5 = add nsw i32 %3, %4
  store i32 %5, i32* %c, align 4
  %6 = load i32, i32* %a, align 4
  %7 = load i32, i32* %b, align 4
  %8 = add nsw i32 %6, %7
  store i32 %8, i32* %d, align 4
  %9 = load i32, i32* %c, align 4
  %10 = load i32, i32* %d, align 4
  %11 = icmp ne i32 %9, %10
  br i1 %11, label %12, label %26

; <label>:12                                      ; preds = %0
  %13 = call i8* @llvm.stacksave()
  store i8* %13, i8** %2
  %14 = call i32 @puts(i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str, i32 0, i32 0))
  %15 = load i32, i32* %c, align 4
  %16 = load i32, i32* %d, align 4
  %17 = icmp eq i32 %15, %16
  br i1 %17, label %18, label %22

; <label>:18                                      ; preds = %12
  %19 = load i32, i32* %a, align 4
  %20 = load i32, i32* %b, align 4
  %21 = call i32 @sum(i32 %19, i32 %20)
  br label %23

; <label>:22                                      ; preds = %12
  br label %23

; <label>:23                                      ; preds = %22, %18
  %24 = call noalias i8* @malloc(i64 30) #1
  store i32 1, i32* %1
  %25 = load i8*, i8** %2
  call void @llvm.stackrestore(i8* %25)
  br label %32

; <label>:26                                      ; preds = %0
  %27 = call i32 @puts(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.1, i32 0, i32 0))
  %28 = call noalias i8* @malloc(i64 10) #1
  %29 = load i32, i32* %a, align 4
  %30 = load i32, i32* %b, align 4
  %31 = call i32 @sum(i32 %29, i32 %30)
  store i32 0, i32* %1
  br label %32

; <label>:32                                      ; preds = %26, %23
  %33 = load i32, i32* %1
  ret i32 %33
}

; Function Attrs: nounwind
declare i8* @llvm.stacksave() #1

declare i32 @puts(i8*) #2

; Function Attrs: nounwind
declare noalias i8* @malloc(i64) #3

; Function Attrs: nounwind
declare void @llvm.stackrestore(i8*) #1

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }
attributes #2 = { "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"Ubuntu clang version 3.7.0-2ubuntu4 (tags/RELEASE_370/final) (based on LLVM 3.7.0)"}
