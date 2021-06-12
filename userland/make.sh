#!/bin/sh -x

dd if=/dev/zero of=fat_disk bs=1M count=1
mkfs.fat -n 'FILESYSTEM' -s 2 -f 2 -R 32 -F 32 fat_disk

mkdir mnt
sudo mount -o loop fat_disk mnt
sudo cp -r hello mnt/hello
sudo umount mnt
rmdir mnt
