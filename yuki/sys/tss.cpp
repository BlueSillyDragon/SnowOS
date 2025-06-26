#include <cstdint>
#include <inc/sys/gdt.hpp>
#include <inc/sys/tss.hpp>
#include <inc/io/kprintf.hpp>

Tss tss;
TssDescriptor tssDesc;

extern "C" void loadTss();

void initTss() {
    uint32_t limit = sizeof(Tss); // The top 12 bits are unused
    uint64_t base = (uint64_t)&tss;

    tssDesc.limitLow = limit;
    tssDesc.baseLow = base;
    tssDesc.baseMid0 = (base >> 16);
    tssDesc.accessByte = 0x89;
    tssDesc.limitFlags = 0;
    tssDesc.baseMid1 = (base >> 24);
    tssDesc.baseHigh = (base >> 32);
    tssDesc.reserved = 0;

    setTss(tssDesc);

    loadTss();

    kprintf(OK, "TSS Initialized!\n");
}