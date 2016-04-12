#!/bin/sh

make -C ../build #-fPIC
#clang -emit-llvm -S a.c -o a.ll
#opt  -load ../build/MyCSE/libMyCSE.so -myMagicCSE -S a.ll -o a_mycse.opt.ll
#opt  -adce -S a.ll -o a_adce.opt.ll 
#echo "Comparing MyDCE with no DCE"
#diff a_mydce.opt.ll a.ll
#echo "Comparing MyDCE with ADCE"

#diff a_mydce.opt.ll a_adce.opt.ll
#clang a.opt.ll -o a
#diff a2.opt.ll a.opt.ll
