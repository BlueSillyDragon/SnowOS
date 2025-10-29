#pragma once

#include <limine.h>

#define KERNEL_CS 0x00af9b000000ffff
#define KERNEL_DS 0x00af93000000ffff

#define USER_CS 0x00affb000000ffff
#define USER_DS 0x00aff3000000ffff

typedef struct
{
    uint16_t Limit;
    uint64_t Base;
} __attribute__((packed)) GDTR;

typedef struct
{
    uint64_t NullSegment;
    uint64_t KernelCode;
    uint64_t KernelData;
    uint64_t UserCode;
    uint64_t UserData;
} __attribute__((packed)) GDT;

void HalInit(limine_framebuffer* Framebuffer);
void HalPrintString(const char* String);
void HalInitGdt();