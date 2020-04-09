#!/bin/sh
if [ -b /dev/mmcblk2 ]; then
	echo "/dev/mmcblk2 exist."
else
	echo "/dev/mmcblk2 not exist!!!"
    sleep 5
	exit
fi
echo "fdisk partions..."
fdisk /dev/mmcblk2 << EOF
d
1
d
2

n
p
1
2048
+8M
n
p
2
2048



t
1
b
p
w
EOF
sleep 1
partprobe
echo "format partions..."
mkfs.fat /dev/mmcblk2p1
mkfs.ext4 -F /dev/mmcblk2p2
sync
echo "done"
sleep 1
