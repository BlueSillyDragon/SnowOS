rm -rf iso_root
mkdir -p iso_root
cp -v build/yuki/yuki iso_root/
mkdir -p iso_root/limine
cp -v yuki/limine.conf iso_root/limine/
mkdir -p iso_root/EFI/BOOT
cp -v limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin iso_root/limine/
cp -v limine/BOOTX64.EFI iso_root/EFI/BOOT/
xorriso -as mkisofs -R -r -J -b limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
		-apm-block-size 2048 --efi-boot limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o SnowOS.iso
./limine/limine bios-install SnowOS.iso
