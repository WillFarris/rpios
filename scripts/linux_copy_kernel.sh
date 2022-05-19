mkdir -p boot_mnt && mount $1 boot_mnt && cp kernel8.img boot_mnt/kernel8.img && clear && ls boot_mnt && umount $1 && rmdir boot_mnt && date
