#!/bin/sh -x

if [ `uname` == 'Darwin' ]; then
    rm fat_disk.img
    qemu-img create -f raw fat_disk.img 128M
    mkfs.fat -s 2 -f 2 -R 32 -F 32 fat_disk.img

    mkdir -p mnt
    hdiutil attach -mountpoint mnt/ fat_disk.img
    cp hello/* mnt/
    hdiutil detach mnt/
    rmdir mnt
else
    rm fat_disk.img
    dd if=/dev/zero of=fat_disk.img bs=1M count=1
    mkfs.fat -s 2 -f 2 -R 32 -F 32 fat_disk.img

    mkdir mnt
    sudo mount -o loop fat_disk.img mnt
    sudo cp hello/* mnt/
    sudo umount mnt
    rmdir mnt
fi
