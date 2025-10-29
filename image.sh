dd if=/dev/zero of=SnowOS.img bs=1048576 count=256
sgdisk SnowOS.img -o --new=1 --typecode=1:ef00 -m 1
./Limine/limine bios-install SnowOS.img
sudo losetup /dev/loop0 SnowOS.img
sudo mkfs.fat -F 32 /dev/loop0
sudo mount /dev/loop0 /mnt
sudo mkdir /mnt/EFI
sudo mkdir /mnt/EFI/BOOT
sudo cp Limine/BOOTX64.EFI /mnt/EFI/BOOT/
sudo cp yuki/limine.conf /mnt/EFI/BOOT/
sudo cp Limine/limine-bios.sys /mnt/EFI/BOOT/
sudo cp build/yuki/yuki.exe /mnt/
sudo umount /mnt
sudo losetup -d /dev/loop0
echo "Finished Generating Image!"
