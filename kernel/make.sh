#!/bin/sh -x

clang -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -c serial.c
clang -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -c frame_buffer.c
clang -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -c gdt.c
clang -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -c paging.c
clang -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -c main.c
nasm -f elf64 -o asmfunc.o asmfunc.asm
ld.lld --entry call_kernel -z norelro --image-base 0x100000 --static -o kernel.elf main.o serial.o frame_buffer.o asmfunc.o gdt.o paging.o
