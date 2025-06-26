#pragma once

#include <cstdint>
#include <inc/sys/tss.hpp>

#define KERNEL_CS 0x00af9b000000ffff
#define KERNEL_DS 0x00af93000000ffff

#define USER_CS 0x00affb000000ffff
#define USER_DS 0x00aff3000000ffff

typedef struct
{
    std::uint16_t limit;
    std::uint64_t base;
} __attribute__((packed)) gdtr_t;

typedef struct
{
    std::uint64_t null_segment;
    std::uint64_t kernel_code;
    std::uint64_t kernel_data;
    std::uint64_t user_code;
    std::uint64_t user_data;
    TssDescriptor tssDesc;
} __attribute__((packed)) gdt_t;

void initGdt();
void setTss(TssDescriptor tssDesc);