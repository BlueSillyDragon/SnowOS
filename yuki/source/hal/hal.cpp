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
#include <flanterm.h>
#include <flanterm_backends/fb.h>
#include <hal/hal.hpp>
#include <ke/print.hpp>

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

void HalInit(limine_framebuffer* Framebuffer)
{
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