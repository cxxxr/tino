#!/bin/sh

if [ $# -ne 2 ]; then
    echo "usage $0 EFI_FILE KERNEL_FILE"
    exit
fi

EFI_FILE=$1
KERNEL_FILE=$2
FAT_DISK='userland/fat_disk'

if [ ! -e userland/fat_disk ]; then
    echo "$FAT_DISK does not exist"
    exit
fi

qemu-img create -f raw disk.img 200M
mkfs.fat -n 'HOGEHOGE' -s 2 -f 2 -R 32 -F 32 disk.img
mkdir -p mnt
sudo mount -o loop disk.img mnt
sudo mkdir -p mnt/EFI/BOOT
sudo cp $EFI_FILE mnt/EFI/BOOT/BOOTX64.EFI
sudo cp $KERNEL_FILE mnt/
sudo cp $FAT_DISK mnt/
sudo umount mnt

qemu-system-x86_64 \
    -m 1G \
    -drive if=pflash,format=raw,readonly,file=OVMF_CODE.fd \
    -drive if=pflash,format=raw,file=OVMF_VARS.fd \
    -drive if=ide,index=0,media=disk,format=raw,file=disk.img \
    -serial stdio
    #-monitor stdio
    #-device nec-usb-xhci,id=xhci \
    #-device usb-mouse -device usb-kbd \

#qemu-system-x86_64 \
#    -drive if=pflash,file=OVMF_CODE.fd \
#    -drive if=pflash,file=OVMF_VARS.fd \
#    -hda disk.img
