#include "inc/sys/hpet.hpp"
#include <cstdint>
#include <inc/utils/helpers.hpp>
#include <inc/klibc/string.hpp>
#include <inc/io/kprintf.hpp>
#include <inc/sys/panic.hpp>
#include <inc/mm/vmm.hpp>
#include <inc/mm/slab.hpp>
#include <uacpi/uacpi.h>
#include <uacpi/tables.h>
#include <uacpi/acpi.h>
#include <inc/utils/mmio.hpp>
#include <inc/sys/cpuid.hpp>
#include <inc/sys/apic.hpp>
#include <inc/sys/spinlock.hpp>

// IoApic Defines

#define IOAPIC_ID_REG 0x00
#define IOAPIC_VERSION_REG 0x01
#define IOAPIC_ARB_REG 0x02
#define IOAPIC_REDIRECTION_TABLE(n)   (0x10 + 2 * n) // Add 1 for the upper 32 bits

// Lapic Register Defines

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
constexpr uint32_t Ia32ApicBaseMask = 0xffff'f000;

bool x2Apic;

uintptr_t lapicMmio;
uint64_t ticksPerNs;

Cpuid cpuid;

uint32_t toX2Apic(const uint32_t registr) {
    return (registr >> 4) + 0x800;
}

uint32_t apicRead(uint32_t registr) {
    if (x2Apic) {
        uint32_t ret = rdmsr(toX2Apic(registr));
        return ret;
    } else {
        uint32_t ret = mmioRead(lapicMmio + registr, sizeof(uint32_t));
        return ret;
    }
    return 0;
}

void apicWrite(uint32_t registr, uint32_t value) {
    if (x2Apic) {
        wrmsr(toX2Apic(registr), value);
    } else {
        mmioWrite(lapicMmio + registr, value, sizeof(uint32_t));
    }
}

uint32_t readIoApicReg(uint64_t ioapicBase, uint8_t offset) {
    mmioWrite(ioapicBase, offset, sizeof(uint8_t));
    return mmioRead(ioapicBase + 0x10, sizeof(uint32_t));
}

void writeIoApicReg(uint64_t ioapicBase, uint8_t offset, uint32_t data) {
    mmioWrite(ioapicBase, offset, sizeof(uint8_t));
    mmioWrite(ioapicBase + 0x10, data, sizeof(uint32_t));
}

void enableIoApic() {
    kprintf(YUKI, "Configuring the IOAPIC...\n");

    uacpi_table table;
    if (uacpi_table_find_by_signature(ACPI_MADT_SIGNATURE, &table) != UACPI_STATUS_OK) {
        kernelPanic("SnowOS is running on a none IOAPIC enabled machine!");
    }

    acpi_madt *madtPtr = reinterpret_cast<acpi_madt *>(table.ptr);
    acpi_madt *madt = reinterpret_cast<acpi_madt *>(kmalloc(madtPtr->hdr.length));
    memcpy(madt, madtPtr, madtPtr->hdr.length);

    uintptr_t madtStart = (uint64_t)madt->entries;
    uintptr_t madtEnd = (uint64_t)madt + madt->hdr.length;
    acpi_entry_hdr *currentMadt = reinterpret_cast<acpi_entry_hdr *>(madtStart);

    acpi_madt_ioapic *ioapic;

    uint64_t ioapicCount = 0;

    for (uintptr_t entry = madtStart; entry < madtEnd; entry += currentMadt->length, currentMadt = reinterpret_cast<acpi_entry_hdr *>(entry)) {
        switch (currentMadt->type) {
            case ACPI_MADT_ENTRY_TYPE_IOAPIC:
                ioapic = reinterpret_cast<acpi_madt_ioapic *>(entry);
                ioapicCount++;
                break;
            default:
                break;
        }
    }

    uint64_t ioapicBase = (uint64_t)vmmMapPhys(ioapic->address, 0x1000);

    kprintf(YUKI, "%d IOAPIC(s) located\n", ioapicCount);

    uint8_t apicVer = readIoApicReg(ioapicBase, IOAPIC_VERSION_REG);

    kprintf(YUKI, "IOAPIC version 0x%x\n", apicVer);

    hcf();
}

void setupInterval() {
    uint64_t frequency = 0;
    if (getCpuid(0x15).ecx != 0) {
        kprintf(YUKI, "Getting Crystal Clock frequency...\n");
        frequency = getCpuid(0x15).ecx;
        kprintf(YUKI, "Frequency is: %lu\n", frequency);
    }

    else {
        disableLvtTimer();
        apicWrite(LAPIC_DCR, 0b1011); // Div by 1

        apicWrite(LAPIC_LVT_ICR, 0xFFFFFFFF);

        uint32_t startCount = apicRead(LAPIC_LVT_CCR);
        hpetSleep(50);
        uint32_t endCount = apicRead(LAPIC_LVT_CCR);

        apicWrite(LAPIC_LVT_ICR, 0);
        uint64_t ticks = startCount - endCount;
        ticksPerNs = ticks / (50 * 1'000'000);

        kprintf(YUKI, "The LAPIC ticked %d times in 50 ms\n", ticks);
        kprintf(YUKI, "LAPIC Ticks per NS %lu\n", ticksPerNs);
    }
}

uint64_t nsToTicks(uint64_t ns) {
    return ns * ticksPerNs;
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

    lapicMmio = (uintptr_t)vmmMapPhys(lapicBasePhys, 0x1000);

    apicWrite(LAPIC_TPR, 0);
    apicWrite(LAPIC_SPURIOUS_IVT_REG, (1 << 8) | 0xff);

    setupInterval();

    apicWrite(LAPIC_LVT_ICR, nsToTicks(50'000'000));
    enableLvtTimer(true);
}

void disableLvtTimer() {
    apicWrite(LAPIC_LVT_TMR, 0);
}

void enableLvtTimer(bool periodic) {
    if (periodic) {
        apicWrite(LAPIC_LVT_TMR, (1 << 17) | 56);
    } else {
        apicWrite(LAPIC_LVT_TMR, 56);
    }
}