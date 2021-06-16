#!/bin/sh -x

clang -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti hello.c -o hello.o
ld.lld --entry main -z norelro --image-base 0 --static -o hello hello.o
