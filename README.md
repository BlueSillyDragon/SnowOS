# SnowOS

SnowOS is a hobby operating system built from scratch in C++ that is heavily inspired by Windows NT, though is not a clone and still aims to do some of it's own things.

## How to Build and Run SnowOS

### Step 1: Clone repository

If you don't already have the source code, you'll have to clone it using the following commands:
```
git clone https://github.com/BlueSillyDragon/SnowOS.git
cd SnowOS
git submodule init
git submodule update
```

### Step 2: Install Dependencies

You'll need to install the needed tools to be able to build SnowOS if you don't have them installed already.
```
sudo apt install meson clang ninja-build lld llvm nasm
```
Also install `qemu-system-x86-64` and `ovmf` (it could be `edk2-ovmf` for some distros) for running SnowOS, you can skip this if you're going to use a different VM though (steps for Virtual Box are given near the bottom)

### Step 3: Run get-deps

You'll need to get SnowOS's dependencies by running get-deps
```
cd common
```
Then run get-deps
```
./get-deps
```
This will get the C and C++ freestanding headers.

### Step 4: Run Meson
Ensure this is in the root of the project directory, and run the following command:

```
meson setup --cross-file x86_64-pe.cross-file build
```
This will setup Meson and create the build directory, now run:
```
meson compile -C build
```

### Step 5: Run SnowOS

Now all you have to do is build the image file and run SnowOS under QEMU (or your VM of choice, just remember that you need to configure the VM settings to use UEFI).

Before running image.sh, make sure to build Limine like so:
```
make -C Limine
```

Now you can just run image.sh:
```
./image.sh
```
This should generate an image file called SnowOS.img, which you can run with QEMU: 
```
qemu-system-x86_64 --enable-kvm -M q35 -m 1G -bios /usr/share/ovmf/OVMF.fd -serial stdio -hda SnowOS.img
```
Now when you run SnowOS, you should be booted into the kernel!

### Running in Virtual Box

If you want to run SnowOS in Virtual Box, a few extra steps are required. First, run iso.sh to create an ISO for SnowOS
```
./iso.sh
```
Next create a new Virtual Machine in VBox (you can easily find out how to do this on the internet, so I won't explain here)

Now you'll need to enable a few features, namely HPET and X2APIC. This is done via VBoxManage
```
VBoxManage modifyvm name-of-vm --hpet on
VBoxManage modifyvm name-of-vm --x2apic on
```
Also be sure to enable UEFI (this can be done via the VBox GUI though), and now SnowOS should run as intended.

## Credit
The Limine Bootloader, as well as flanterm, is maintained by mintsuki.

## License
SnowOS is under an BSD 3-Clause License.

BlueSillyDragon (c) 2025
