#include <cstdint>
#include <inc/io/kprintf.hpp>
#include <inc/sys/idt.hpp>

extern void* isr_stub_table[];

idtr_t idtr;
idt_entry_t idt[256];

void idtSetDescriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];

    descriptor->isr_low        = (uint64_t)isr & 0xFFFF;
    descriptor->seg_select     = 0x08;
    descriptor->ist            = 0;
    descriptor->attributes     = flags;
    descriptor->isr_mid        = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isr_high       = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved       = 0;
}

void initIdt()
{
    idtr.base = (uint64_t)&idt;
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * 256 - 1;

    for(int i = 0; i < 32; i++)
    {
        idtSetDescriptor(i, isr_stub_table[i], 0x8e);
    }
    idtSetDescriptor(32, isr_stub_table[32], 0x8e);
    idtSetDescriptor(0xff, isr_stub_table[39], 0x8e);
    idtSetDescriptor(0x60, isr_stub_table[40], 0x8e);

    __asm__ __volatile__ ("lidt %0" :: "m"(idtr));
    kprintf(OK, "IDT Initialized!\n");
}