; ModuleID = 'a.c'
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
  %e = alloca i32, align 4
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
  br i1 %11, label %12, label %30

; <label>:12                                      ; preds = %0
  %13 = load i32, i32* %c, align 4
  %14 = zext i32 %13 to i64
  %15 = call i8* @llvm.stacksave()
  store i8* %15, i8** %2
  %16 = alloca i32, i64 %14, align 16
  %17 = call i32 @puts(i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str, i32 0, i32 0))
  %18 = load i32, i32* %c, align 4
  %19 = load i32, i32* %d, align 4
  %20 = icmp eq i32 %18, %19
  br i1 %20, label %21, label %25

; <label>:21                                      ; preds = %12
  %22 = load i32, i32* %a, align 4
  %23 = load i32, i32* %b, align 4
  %24 = call i32 @sum(i32 %22, i32 %23)
  br label %26

; <label>:25                                      ; preds = %12
  br label %26

; <label>:26                                      ; preds = %25, %21
  %27 = phi i32 [ %24, %21 ], [ 0, %25 ]
  %28 = call noalias i8* @malloc(i64 30) #1
  store i32 1, i32* %1
  %29 = load i8*, i8** %2
  call void @llvm.stackrestore(i8* %29)
  br label %36

; <label>:30                                      ; preds = %0
  %31 = call i32 @puts(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.1, i32 0, i32 0))
  %32 = call noalias i8* @malloc(i64 10) #1
  %33 = load i32, i32* %a, align 4
  %34 = load i32, i32* %b, align 4
  %35 = call i32 @sum(i32 %33, i32 %34)
  store i32 0, i32* %1
  br label %36

; <label>:36                                      ; preds = %30, %26
  %37 = load i32, i32* %1
  ret i32 %37
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
