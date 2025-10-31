/**
Snow Operating System
Copyright (c) BlueSillyDragon 2025
 
File: hal/hal.cpp

Description:
This file is the main source file of
Hardware Abstraction Layer module of Yuki

Author:
BlueSillyDragon
October 28th 2025
**/

#include <cstdint>
#include <cstddef>
#include <limine.h>
#include <flanterm.h>
#include <flanterm_backends/fb.h>
#include <hal/hal.hpp>
#include <ke/print.hpp>

// Limine Stuff

// Set the base revision to 4, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

namespace {

__attribute__((used, section(".limine_requests")))
volatile LIMINE_BASE_REVISION(4);

}

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

namespace {

__attribute__((used, section(".limine_requests")))
volatile limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
    .response = nullptr
};

}

// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .cpp file, as seen fit.

namespace {

__attribute__((used, section(".limine_requests_start")))
volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
volatile LIMINE_REQUESTS_END_MARKER;

}

struct flanterm_context* FtCtx;

uint32_t TerminalForeground = 0xFFFFFF;
uint32_t TerminalBackground = 0x23272E;

size_t strlen(const char *string) {
    size_t length = 0;
    for (int i = 0; string[i] != '\0'; i++)
    {
        length++;
    }
    length++;
    return length;
}

extern "C" void ReloadSegments();
extern "C" void* IsrStubTable[];

GDT Gdt {0, KERNEL_CS, KERNEL_DS, USER_CS, USER_DS};
DTR GdtRegister;

IDTENTRY Idt[256];
DTR Idtr;

limine_framebuffer *Framebuffer;

// The following stubs are required by the Itanium C++ ABI (the one we use,
// regardless of the "Itanium" nomenclature).
// Like the memory functions above, these stubs can be moved to a different .cpp file,
// but should not be removed, unless you know what you are doing.
extern "C" {
    int __cxa_atexit(void (*)(void *), void *, void *) { return 0; }
    void __cxa_pure_virtual() { HalHaltCpu(); }
    void *__dso_handle;
}


void HalIdtSetDescriptor(uint8_t Vector, void* Isr, uint8_t Flags) {
    IDTENTRY* Descriptor = &Idt[Vector];

    Descriptor->IsrLow              = (uint64_t)Isr & 0xFFFF;
    Descriptor->SegmentSelector     = 0x08;
    Descriptor->Ist                 = 0;
    Descriptor->Attributes          = Flags;
    Descriptor->IsrMid              = ((uint64_t)Isr >> 16) & 0xFFFF;
    Descriptor->IsrHigh             = ((uint64_t)Isr >> 32) & 0xFFFFFFFF;
    Descriptor->Reserved            = 0;
}

void HalInit()
{
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        HalHaltCpu();
    }

    // Ensure we got a framebuffer.
    if (framebuffer_request.response == nullptr
     || framebuffer_request.response->framebuffer_count < 1) {
        HalHaltCpu();
    }

    // Fetch the first framebuffer.
    Framebuffer = framebuffer_request.response->framebuffers[0];

    FtCtx = flanterm_fb_init(
        NULL,
        NULL,
        reinterpret_cast<uint32_t *>(Framebuffer->address), Framebuffer->width, Framebuffer->height, Framebuffer->pitch,
        Framebuffer->red_mask_size, Framebuffer->red_mask_shift,
        Framebuffer->green_mask_size, Framebuffer->green_mask_shift,
        Framebuffer->blue_mask_size, Framebuffer->blue_mask_shift,
        NULL,
        NULL, NULL,
        &TerminalBackground, &TerminalForeground,
        NULL, NULL,
        NULL, 0, 0, 1,
        0, 0,
        0
    );
}

void HalPrintString(const char* String)
{
    flanterm_write(FtCtx, String, strlen(String));
}

void HalHaltCpu()
{
    for (;;)
    {
        __asm__ volatile ("hlt");
    }
}

void HalInitCpu()
{
    // Setup the GDT
    GdtRegister.Base = reinterpret_cast<uint64_t>(&Gdt);
    GdtRegister.Limit = (sizeof(Gdt) - 1);

    __asm__ volatile ("lgdt %0" :: "m"(GdtRegister));
    ReloadSegments();

    KePrint(LOG_TYPE::HalLog, "GDT Initialized!\n");

    // Setup the IDT
    Idtr.Base = (uint64_t)&Idt;
    Idtr.Limit = (uint16_t)sizeof(IDTENTRY) * 256 - 1;

    for(int i = 0; i < 40; i++)
    {
        HalIdtSetDescriptor(i, IsrStubTable[i], 0x8e);
    }

    __asm__ volatile ("lidt %0" :: "m"(Idtr));
    KePrint(LOG_TYPE::HalLog, "IDT Initialized!\n");
}