#!/bin/sh
if [ -b /dev/mmcblk2 ]; then
	echo "/dev/mmcblk2 exist."
else
	echo "/dev/mmcblk2 not exist!!!"
    sleep 5
	exit
fi
echo "flashing uboot..."
dd if=/mnt/tfcard/images/u-boot-sunxi-with-spl.bin of=/dev/mmcblk2 bs=1k seek=8
sync
echo "done."
sleep 1
