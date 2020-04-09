#!/bin/sh
if [ -b /dev/mmcblk2 ] && [ -b /dev/mmcblk2p2 ]; then
	echo "/dev/mmcblk2 rootfs exist."
else
	echo "/dev/mmcblk2 rootfs not exist!!!"
    sleep 5
	exit
fi
mkfs.ext4 -F /dev/mmcblk2p2
mount /dev/mmcblk2p2 /mnt/mmc
tar xvf /mnt/tfcard/images/rootfs.tar -C /mnt/mmc
sync
umount /mnt
sleep 1
