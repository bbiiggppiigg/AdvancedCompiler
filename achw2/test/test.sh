#!/bin/sh

make -C ../build #-fPIC
#clang -emit-llvm -S -O0 test.c -o test.ll
#opt -gvn -S test.ll -o test_gvn.opt.ll
opt  -load ../build/MyCSE/libMyCSE.so -myMagicCSE -S test.ll -o test_mycse.opt.ll
#diff test.ll test_mycse.opt.ll
#diff test_mycse.opt.ll test_gvn.opt.ll
#diff 123_mycse.opt.ll 123.ll
#opt  -adce -S a.ll -o a_adce.opt.ll 
#echo "Comparing MyDCE with no DCE"
#diff a_mydce.opt.ll a.ll
#echo "Comparing MyDCE with ADCE"

#diff a_mydce.opt.ll a_adce.opt.ll
#clang a.opt.ll -o a
#diff a2.opt.ll a.opt.ll
