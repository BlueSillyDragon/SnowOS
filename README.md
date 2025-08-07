# SnowOS

SnowOS is a microkernel-based hobby operating system built from scratch in C++ targeted at the x86 architecture.

> [!WARNING]
> I'm currently taking a break from this project to read up on OS theory so I at least have some idea of what I'm doing 
> I'll be rewriting the kernel when I come back (I want to do things in a different order), so I would steer away from making PRs to the source code, as they most likely will be overwritten when the rewrite comes along (if you do insist on making a PR to the source, I will try to perserve it through the rewrite)

## How to Build and Run SnowOS

### Step 1: Clone repository

If you don't already have the source code, you'll have to clone it using the following commands:
```
git clone https://github.com/BlueSillyDragon/SnowOS.git
cd SnowOS
```

### Step 2: Install Dependencies

You'll need to install the needed tools to be able to build SnowOS if you don't have them installed already.
```
sudo apt install cmake clang ninja-build lld llvm nasm
```
Also install `qemu-system-x86-64` and `ovmf` (it could be `edk2-ovmf` for some distros) for running SnowOS, you can skip this if you're going to use a different VM though (steps for Virtual Box are given near the bottom)

### Step 3: Run get-deps

You'll need to get Yuki's dependencies by running get-deps
```
cd yuki
```
Then run get-deps
```
./get-deps
```
This will get C and C++ freestanding headers, as well as limine.h for the kernel

### Step 4: Run CMake
Ensure this is in the root of the project directory, if not, run:
```
cd ..
```
You'll need to create a build dir  call it whatever you want, I'll call mine build here
```
mkdir build
```
cd into there, and then run the command for CMake below
```
cd build
cmake .. -GNinja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
```
Now you just have to run ninja
```
ninja
```

### Step 5: Run SnowOS

Now all you have to do is build the image file and run SnowOS under QEMU (or your VM of choice, just remember that you need to configure the VM settings to use UEFI).

Simply cd back into the root folder, and run image.sh
```
cd ../
./image.sh
```
This should generate an image file called SnowOS.img, then you can just run it with QEMU: 
```
qemu-system-x86_64 --enable-kvm -M q35 -m 4G -bios /usr/share/ovmf/OVMF.fd -serial stdio -hda SnowOS.img
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

## Technologies
Uses tinyubsan by rdmsr, Limine is the bootloader (will eventually swap back in with SnowBoot once I'm more confident in SnowBoot's abilities lol), Flanterm for a terminal emulator, and uACPI for ACPI stuff.

## License
SnowOS is under an GPLv3 License.

BlueSillyDragon (c) 2023, 2025
