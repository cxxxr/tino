#!/bin/sh -x

if [ $# -ne 1 ]; then
    echo "usage $0 EFI_FILE"
    exit
fi

EFI_FILE=$1

qemu-img create -f raw disk.img 200M
mkfs.fat -n 'HOGEHOGE' -s 2 -f 2 -R 32 -F 32 disk.img
mkdir -p mnt
sudo mount -o loop disk.img mnt
sudo mkdir -p mnt/EFI/BOOT
sudo cp $EFI_FILE mnt/EFI/BOOT/BOOTX64.EFI
sudo umount mnt

qemu-system-x86_64 \
    -drive if=pflash,file=OVMF_CODE.fd \
    -drive if=pflash,file=OVMF_VARS.fd \
    -hda disk.img
