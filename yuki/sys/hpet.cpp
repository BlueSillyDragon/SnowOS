#include <cstdint>
#include <inc/io/kprintf.hpp>
#include <inc/mm/vmm.hpp>
#include <inc/utils/mmio.hpp>
#include <uacpi/uacpi.h>
#include <uacpi/tables.h>
#include <uacpi/acpi.h>
#include <inc/sys/hpet.hpp>

#define HPET_COUNTER 0xf0
#define HPET_COUNTER_CLK_PERIOD_SHIFT 32

uint64_t hpetBase;

uint64_t hpetFrequency;

void enableHpet() {
    kprintf(YUKI, "Starting HPET timer...\n");

    uacpi_table table;

    if (uacpi_table_find_by_signature(ACPI_HPET_SIGNATURE, &table) != UACPI_STATUS_OK) {
        kprintf(ERROR, "HPET not supported by CPU!");
        __asm__ volatile (" hlt ");
    }
    acpi_hpet *hpet = reinterpret_cast<acpi_hpet *>(table.ptr);

    kprintf(YUKI, "Physical Address of HPET at 0x%lx\n", hpet->address.address);

    hpetBase = (uint64_t)vmmMapPhys(hpet->address.address, 0x1000);

    uint64_t gcir = mmioRead(hpetBase, sizeof(uint64_t));

    hpetFrequency = 1'000'000'000'000'000ull / (gcir >> HPET_COUNTER_CLK_PERIOD_SHIFT);

    kprintf(YUKI, "Frequency of HPET is | FS: %lu NS: %lu\n", hpetFrequency, hpetFreqNs());

    mmioWrite(hpetBase + 0x10, 0x1, sizeof(uint8_t));
}

uint64_t hpetFreqNs() {
    return hpetFrequency / 1'000'000ull;
}

uint64_t readHpet() {
    uint64_t count = mmioRead(hpetBase + HPET_COUNTER, sizeof(uint64_t));
    return count;
}

void hpetCalibrate(uint64_t ms) {
    const uint64_t ticks = (ms * hpetFrequency) / 1'000;

    uint64_t startCount = readHpet();
    uint64_t currentCount = startCount;

    while (currentCount < startCount + ticks) {
        currentCount = readHpet();
    }
}