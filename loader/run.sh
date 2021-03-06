#!/bin/sh

clang -g -target x86_64-pc-win32-coff -mno-red-zone -fno-stack-protector -fshort-wchar -Wall -c loader.c
lld-link /subsystem:efi_application /entry:efi_main /out:loader.efi loader.o

EFI_FILE=loader.efi
KERNEL_FILE=../kernel/kernel.elf
FAT_DISK='../userland/fat_disk.img'

qemu-img create -f raw disk.img 200M
mkfs.fat -s 2 -f 2 -R 32 -F 32 disk.img

mkdir -p mnt

if [ `uname` == 'Darwin' ]; then
    hdiutil attach -mountpoint mnt/ disk.img
    mkdir -p mnt/EFI/BOOT
    cp $EFI_FILE mnt/EFI/BOOT/BOOTX64.EFI
    cp $KERNEL_FILE mnt/
    cp $FAT_DISK mnt/
    hdiutil detach mnt/
else
    sudo mount -o loop disk.img mnt
    sudo mkdir -p mnt/EFI/BOOT
    sudo cp $EFI_FILE mnt/EFI/BOOT/BOOTX64.EFI
    sudo cp $KERNEL_FILE mnt/
    sudo cp $FAT_DISK mnt/
    sudo umount mnt
fi

rmdir mnt

qemu-system-x86_64 \
    -m 1G \
    -drive if=pflash,format=raw,readonly,file=../OVMF_CODE.fd \
    -drive if=pflash,format=raw,file=../OVMF_VARS.fd \
    -drive if=ide,index=0,media=disk,format=raw,file=disk.img \
    -serial stdio
