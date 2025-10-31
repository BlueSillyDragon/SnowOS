/**
Snow Operating System
Copyright (c) BlueSillyDragon 2025
 
File: ke/kernel.cpp

Description:
This file is the core source file of
the Ke module of Yuki

Author:
BlueSillyDragon
October 28th 2025
**/

#include <cstdint>
#include <limine.h>
#include <hal/hal.hpp>
#include <ke/print.hpp>

#define YUKI_VERSION_MAJOR 0
#define YUKI_VERSION_MINOR 1
#define YUKI_VERSION_PATCH 0

// MSVC puts global constructors in a section .CRT$XCU that is ordered between .CRT$XCA and
// .CRT$XCZ.
// This is taken from managarm, thank you :3
__declspec(allocate(".CRT$XCA")) const void *crt_xct = nullptr;
__declspec(allocate(".CRT$XCZ")) const void *crt_xcz = nullptr;

extern "C" void KeRunConstructors() {
	using InitializerPtr = void (*)();
	uintptr_t begin = reinterpret_cast<uintptr_t>(&crt_xct);
	uintptr_t end = reinterpret_cast<uintptr_t>(&crt_xcz);
	for (uintptr_t it = begin + sizeof(void *); it < end; it += sizeof(void *)) {
		auto *p = reinterpret_cast<InitializerPtr *>(it);
		(*p)();
	}
}

// The following will be our kernel's entry point.
// If renaming kmain() to something else, make sure to change the
// linker script accordingly.

extern "C" void KeMain(void* SnowBootInfo)
{
    KeRunConstructors();

    HalInit();
    HalPrintString("Snow Operating System (c) 2025 BlueSillyDragon\n");
    KePrint(LOG_TYPE::None, "Yuki Kernel Version %d.%d.%d\n", YUKI_VERSION_MAJOR, YUKI_VERSION_MINOR, YUKI_VERSION_PATCH);
    KePrint(LOG_TYPE::None, "Booted by: ");
    
    if (SnowBootInfo == nullptr)
    {
        KePrint(LOG_TYPE::None, "Limine\n\n");
    }
    else
    {
        KePrint(LOG_TYPE::None, "SnowBoot\n");
    }

    HalInitCpu();

    KePrint(LOG_TYPE::KeLog, "This is a test! :3\n");
    KePrint(LOG_TYPE::KeLog, "Test: %d, %x, %c\n", 13, 0xcafebabe, 's');

    // Cause an exception
    __asm__ volatile ("mov $0xcafebabe, %rcx; mov $0xdeadbeef, %rdx; xor %rax, %rax; xor %rbx, %rbx; div %rbx");

    // We're done, just hang...
    HalInitCpu();
}