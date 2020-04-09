#!/bin/sh
if [ -b /dev/mmcblk2 ] && [ -b /dev/mmcblk2p1 ]; then
	echo "/dev/mmcblk2 bootfs exist."
else
	echo "/dev/mmcblk2 bootfs not exist!!!"
    sleep 5
	exit
fi
mkfs.fat /dev/mmcblk2p1
mount /dev/mmcblk2p1 /mnt/mmc
tar xvf /mnt/tfcard/images/bootfs.tar -C /mnt/mmc
sync
umount /mnt/mmc
sleep 1
