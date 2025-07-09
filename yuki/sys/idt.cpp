#include <cstdint>
#include <inc/io/kprintf.hpp>
#include <inc/sys/idt.hpp>
#include <inc/sys/spinlock.hpp>

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

    for(int i = 0; i < 40; i++)
    {
        idtSetDescriptor(i, isr_stub_table[i], 0x8e);
    }
    idtSetDescriptor(56, isr_stub_table[39], 0x8e);
    idtSetDescriptor(255, isr_stub_table[40], 0x8e);
    idtSetDescriptor(0x60, isr_stub_table[41], 0x8e);

    loadIdt();
    kprintf(OK, "IDT Initialized!\n");
}

void loadIdt() {
    Spinlock::lock();
   __asm__ __volatile__ ("lidt %0" :: "m"(idtr)); 
   Spinlock::unlock();
}