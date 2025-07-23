#include <inc/utils/helpers.hpp>

void hcf() {
    for (;;) {
        __asm__ __volatile__ (" hlt ");
    }
}

void disableInterrupts() {
    __asm__ __volatile__ (" cli ");
}

void enableInterrupts() {
    __asm__ __volatile__ (" sti ");
}

void wrmsr(uint32_t msr, uint64_t value) {
    uint32_t lo = value & 0xffffffff;
    uint32_t hi = value >> 32;
    asm volatile("wrmsr" ::"c"(msr), "a"(lo), "d"(hi) : "memory");
}

uint64_t rdmsr(uint32_t msr) {
    uint64_t lo, hi;
    asm volatile("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr));
    return (hi << 32U) | lo;
}