#pragma once

#include <cstdint>

typedef struct
{
    std::uint16_t limit;
    std::uint64_t base;
} __attribute__((packed)) idtr_t;

typedef struct
{
    std::uint16_t isr_low;
    std::uint16_t seg_select;
    std::uint8_t ist;
    std::uint8_t attributes;
    std::uint16_t isr_mid;
    std::uint32_t isr_high;
    std::uint32_t reserved;
} __attribute__((packed)) idt_entry_t;

void initIdt();
void loadIdt();