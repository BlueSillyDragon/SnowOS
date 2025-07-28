#include <cstdint>
#include <limine.h>
#include <inc/utils/helpers.hpp>
#include <inc/klibc/string.hpp>
#include <inc/io/kprintf.hpp>
#include <inc/io/serial.hpp>
#include <inc/mm/pmm.hpp>
#include <inc/mm/paging.hpp>
#include <inc/mm/vmm.hpp>
#include <inc/sys/spinlock.hpp>
#include <inc/sys/panic.hpp>

uint64_t totalPages = 0;

constexpr uint64_t uAcpiMapBase = 0xffff'a000'0000'0000;
constexpr uint64_t uAcpiMapLimit = 0xffff'a000'ffff'f000;

extern uint64_t _kernelCodeStart;
extern uint64_t _kernelCodeEnd;
extern uint64_t _kernelRodataStart;
extern uint64_t _kernelRodataEnd;
extern uint64_t _kernelVirtualEnd;

void initVmm(limine_memmap_response *memoryMap, limine_executable_address_response *kernelAddr)
{
    uint64_t hhdm = getHhdm();
    kprintf(VMM, "Initializing VMM...\n");
    
    initPagemap(PAGING_LEVEL_4);

    kprintf(VMM, "Mapping higher-half direct map...\n");

    for(uint64_t i = 0; i < memoryMap->entry_count; i++)
    {
        if (memoryMap->entries[i]->type == LIMINE_MEMMAP_BAD_MEMORY ||
            memoryMap->entries[i]->type == LIMINE_MEMMAP_RESERVED ||
            memoryMap->entries[i]->type == LIMINE_MEMMAP_ACPI_NVS) {
            continue;
        }

        uint64_t flags = 0x3;

        if (memoryMap->entries[i]->type == LIMINE_MEMMAP_FRAMEBUFFER) {
            flags |= ptePwt;
        }
        
        mapPages(memoryMap->entries[i]->base + hhdm, memoryMap->entries[i]->base, flags, memoryMap->entries[i]->length);
    }

    kprintf(VMM, "Remapping Yuki...\n");

    for (uint64_t i = 0; (kernelAddr->virtual_base + i) < (uint64_t)&_kernelVirtualEnd; i += 0x1000) {
        if (i >= (uint64_t)&_kernelCodeStart && i <= (uint64_t)&_kernelRodataEnd) {
            mapPage(kernelAddr->virtual_base + i, kernelAddr->physical_base + i, ptePresent);
        } else {
            mapPage(kernelAddr->virtual_base + i, kernelAddr->physical_base + i, ptePresent | pteWrite | pteNx);
        }
    }

    setCr3();

    kprintf(VMM, "VMM Initialized!\n");
}

void *vmmMapPhys(uint64_t physicalAddr, size_t length) {
    // First, round down the physical address
    uint64_t alignedPA = (physicalAddr & ~0xfff);

    // Now round up the length
    length += (physicalAddr - alignedPA);
    length = (length & ~0xfff) + 0x2000;

    uint64_t virtualAddr = (uAcpiMapBase + (totalPages * 0x1000));
    
    mapPages(virtualAddr, alignedPA, 0x3, length);

    totalPages += (length / 0x1000) - 1;

    return (void *)(virtualAddr + (physicalAddr - alignedPA));
}

void vmmUnmapVirt(void *virtualAddr, size_t length) {
    // First, round down the physical address
    uint64_t alignedVA = (uint64_t)virtualAddr & ~0xfff;

    length += ((uint64_t)virtualAddr - alignedVA);
    length = (length & ~0xfff) + 0x1000;

    unmapPages(alignedVA, length);
}