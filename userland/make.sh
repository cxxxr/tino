#!/bin/sh -x

rm fat_disk
dd if=/dev/zero of=fat_disk bs=1M count=1
mkfs.fat -s 2 -f 2 -R 32 -F 32 fat_disk

mkdir mnt
sudo mount -o loop fat_disk mnt
sudo cp hello/* mnt/
sudo umount mnt
rmdir mnt
