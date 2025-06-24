#include <cstdint>
#include <inc/io/kprintf.hpp>
#include <inc/sys/panic.hpp>
#include <inc/mm/vmm.hpp>
#include <uACPI/include/uacpi/tables.h>
#include <inc/utils/mmio.hpp>
#include <inc/sys/cpuid.hpp>
#include <inc/sys/lapic.hpp>

#define LAPIC_ID_REG 0x20
#define LAPIC_VERSION_REG 0x30
#define LAPIC_TPR 0x80
#define LAPIC_APR 0x90
#define LAPIC_PPR 0xa0
#define LAPIC_EOI_REG 0xb0
#define LAPIC_RRD 0xc0
#define LAPIC_LDR 0xd0
#define LAPIC_DFR 0xe0
#define LAPIC_SPURIOUS_IVT_REG 0xf0
#define LAPIC_ISR 0x100
#define LAPIC_TMR 0x180
#define LAPIC_IRR 0x200
#define LAPIC_ESR 0x280
#define LAPIC_CMCI_REG 0x2f0
#define LAPIC_ICR 0x300
#define LAPIC_LVT_TMR 0x320
#define LAPIC_LVT_TSR 0x330
#define LAPIC_LVT_PMCR 0x340
#define LAPIC_LVT_LINT0 0x350
#define LAPIC_LVT_LINT1 0x360
#define LAPIC_LVT_ERROR_REG 0x370
#define LAPIC_LVT_ICR 0x380
#define LAPIC_LVT_CCR 0x390
#define LAPIC_DCR 0x3e0

constexpr uint32_t Ia32ApicBaseMsr = 0x1b;
constexpr uint64_t Ia32ApicBaseMask = 0xffff'f000;

bool x2Apic;

Cpuid cpuid;

static inline void wrmsr(uint32_t msr, uint64_t value) {
    uint32_t lo = value & 0xffffffff;
    uint32_t hi = value >> 32;
    asm volatile("wrmsr" ::"c"(msr), "a"(lo), "d"(hi) : "memory");
}

static inline uint64_t rdmsr(uint32_t msr) {
    uint64_t lo, hi;
    asm volatile("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr));
    return (hi << 32U) | lo;
}

uint32_t toX2Apic(const uint32_t registr) {
    return (registr >> 4) + 0x800;
}

static inline void apicRead(uint32_t registr, uint32_t value) {
    if (x2Apic) {
        rdmsr(toX2Apic(registr));
    }
}

static inline void apicWrite(uint32_t registr, uint32_t value) {
    if (x2Apic) {
        wrmsr(toX2Apic(registr), value);
    }
}

void setupInterval() {
    if (getCpuid(0x15).ecx != 0) {
        kprintf(YUKI, "Getting Crystal Clock frequency...\n");
    }
}

void enableLapicTimer() {
    // Get the APIC MSR and set the Global Bit
    uint64_t apicBaseMsr = rdmsr(Ia32ApicBaseMsr);
    apicBaseMsr |= (1 << 11);

    cpuid = getCpuid(1);

    if (cpuid.ecx & 1 << 21) {
        kprintf(YUKI, "x2APIC is Available!\n");
        apicBaseMsr |= (1 << 10);
        x2Apic = true;
    } else {
        kprintf(YUKI, "No x2APIC =c, using old version\n");
        x2Apic = false;
    }

    wrmsr(Ia32ApicBaseMsr, apicBaseMsr);

    // Get the LAPIC Base
    uintptr_t lapicBasePhys = apicBaseMsr & Ia32ApicBaseMask;
    uint8_t *lapic = (uint8_t *)vmmMapPhys(lapicBasePhys, 0x1000);

    apicWrite(LAPIC_TPR, 0);
    apicWrite(LAPIC_SPURIOUS_IVT_REG, (1 << 8) | 0xff);

    setupInterval();
}