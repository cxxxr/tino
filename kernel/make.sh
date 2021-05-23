#!/bin/sh

clang -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -c main.c
ld.lld --entry KernelMain -z norelro --image-base 0x100000 --static -o kernel.elf main.o
