#!/bin/sh -x

clang -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -mcmodel=large hello.c -c
ld.lld --entry main -z norelro --image-base 0 --static --image-base 0xffff800000000000 -o hello hello.o
