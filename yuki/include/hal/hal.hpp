/**
Snow Operating System
Copyright (c) BlueSillyDragon 2025
 
File: hal/hal.hpp

Description:
This file contain architecture specific
structures and definitions for Hal

Author:
BlueSillyDragon
October 28th 2025
**/

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
} __attribute__((packed)) DTR; // Descriptor Table Register

typedef struct
{
    uint64_t NullSegment;
    uint64_t KernelCode;
    uint64_t KernelData;
    uint64_t UserCode;
    uint64_t UserData;
} __attribute__((packed)) GDT;

typedef struct
{
    uint16_t IsrLow;
    uint16_t SegmentSelector;
    uint8_t Ist;
    uint8_t Attributes;
    uint16_t IsrMid;
    uint32_t IsrHigh;
    uint32_t Reserved;
} __attribute__((packed)) IDTENTRY;

void HalInit(limine_framebuffer* Framebuffer);
void HalPrintString(const char* String);
void HalInitCpu();