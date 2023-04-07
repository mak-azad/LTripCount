#!bin/bash
cc -c rtlib.c
clang -g -flegacy-pass-manager -Xclang -load -Xclang build/skeleton/libSkeletonPass.so -c test.c
cc test.o rtlib.o
./a.out