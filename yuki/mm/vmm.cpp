#include "inc/sys/panic.hpp"
#include <cstdint>
#include <limine.h>
#include <inc/klibc/string.hpp>
#include <inc/io/kprintf.hpp>
#include <inc/io/serial.hpp>
#include <inc/mm/pmm.hpp>
#include <inc/mm/vmm.hpp>
#include <inc/sys/spinlock.hpp>

constexpr uint64_t ptePresent = 0x1;
constexpr uint64_t pteWrite = 0x2;
constexpr uint64_t pteUser = 0x4;
constexpr uint64_t ptePwt = 0x8;
constexpr uint64_t ptePcd = 0x10;
constexpr uint64_t pteNx = 0x8000000000000000;
constexpr uint64_t pteAddress = 0x0000fffffffff000;

constexpr uint64_t uAcpiMapBase = 0xffff'a000'0000'0000;
constexpr uint64_t uAcpiMapLimit = 0xffff'a000'ffff'f000;

uint64_t totalPages = 0;

#define PML4_ID(virt) (((virt) >> 39) & 0x1FF)
#define PDPT_ID(virt) (((virt) >> 30) & 0x1FF)
#define PD_ID(virt) (((virt) >> 21) & 0x1FF)
#define PT_ID(virt) (((virt) >> 12) & 0x1FF)

extern uint64_t _kernelCodeStart;
extern uint64_t _kernelCodeEnd;
extern uint64_t _kernelRodataStart;
extern uint64_t _kernelRodataEnd;
extern uint64_t _kernelVirtualEnd;

pagemap_t kernelPagemap;

uint64_t hhdmOffset;

uint64_t kernelVirt = 0xffffffff80000000;

void initVmm(limine_memmap_response *memoryMap, limine_executable_address_response *kernelAddr, std::uint64_t hhdm)
{
    kprintf(VMM, "Initializing VMM...\n");
    hhdmOffset = hhdm;
    kernelPagemap.topLevel = pmmAlloc();
    memset(reinterpret_cast<uint64_t *>(kernelPagemap.topLevel + hhdm), 0x0, 0x1000);

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

    __asm__ __volatile__ ("mov %0, %%cr3" :: "r"(kernelPagemap.topLevel) : "memory");

    kprintf(VMM, "VMM Initialized!\n");
}

uint64_t createPte(uint64_t physicalAddr, uint64_t flags)
{
    auto pte = (physicalAddr | ptePresent);

    if (flags & pteWrite)
    {
        pte |= pteWrite;
    }

    if (flags & pteUser)
    {
        pte |= pteUser;
    }

    if (flags & ptePwt)
    {
        pte |= ptePwt;
    }

    if (flags & ptePcd)
    {
        pte |= ptePcd;
    }

    return pte;
}

void mapPage(uint64_t virtualAddr, uint64_t physicalAddr, uint64_t flags)
{
    if (virtualAddr % 0x1000 != 0 || physicalAddr % 0x1000 != 0)
    {
        kprintf(ERROR, "Attempted to map a virtual address or physical address that was not aligned to 4KB!\n");
        __asm__ volatile (" hlt ");
    }

    uint64_t *pml4 = reinterpret_cast<uint64_t *>(kernelPagemap.topLevel + hhdmOffset);
    uint64_t *pdpt, *pd, *pt;

    if (!(pml4[PML4_ID(virtualAddr)] & ptePresent))
    {
        uint64_t lvlAddress = pmmAlloc();
        memset(reinterpret_cast<uint64_t *>(lvlAddress + hhdmOffset), 0, 0x1000);
        pml4[PML4_ID(virtualAddr)] = createPte(lvlAddress, flags);
    }
    pdpt = reinterpret_cast<uint64_t *>((pml4[PML4_ID(virtualAddr)] & pteAddress) + hhdmOffset);

    if (!(pdpt[PDPT_ID(virtualAddr)] & ptePresent))
    {
        uint64_t lvlAddress = pmmAlloc();
        memset(reinterpret_cast<uint64_t *>(lvlAddress + hhdmOffset), 0, 0x1000);
        pdpt[PDPT_ID(virtualAddr)] = createPte(lvlAddress, flags);
    }
    pd = reinterpret_cast<uint64_t *>((pdpt[PDPT_ID(virtualAddr)] & pteAddress) + hhdmOffset);

    if (!(pd[PD_ID(virtualAddr)] & ptePresent))
    {
        uint64_t lvlAddress = pmmAlloc();
        memset(reinterpret_cast<uint64_t *>(lvlAddress + hhdmOffset), 0, 0x1000);
        pd[PD_ID(virtualAddr)] = createPte(lvlAddress, flags);
    }
    pt = reinterpret_cast<uint64_t *>((pd[PD_ID(virtualAddr)] & pteAddress) + hhdmOffset);
    pt[PT_ID(virtualAddr)] = createPte(physicalAddr, flags);
}

void unmapPage(uint64_t virtualAddr) {
    uint64_t *pml4 = reinterpret_cast<uint64_t *>(kernelPagemap.topLevel + hhdmOffset);
    uint64_t *pdpt = reinterpret_cast<uint64_t *>((pml4[PML4_ID(virtualAddr)] & pteAddress) + hhdmOffset);
    uint64_t *pd = reinterpret_cast<uint64_t *>((pdpt[PDPT_ID(virtualAddr)] & pteAddress) + hhdmOffset);
    uint64_t *pt = reinterpret_cast<uint64_t *>((pd[PD_ID(virtualAddr)] & pteAddress) + hhdmOffset);
    pt[PT_ID(virtualAddr)] = 0;
    __asm__ __volatile__ (" invlpg (%0) " :: "a"(virtualAddr));
}

void mapPages(uint64_t virtualStart, uint64_t physicalStart, uint64_t flags, uint64_t count)
{
    if (virtualStart % 0x1000 != 0 || physicalStart % 0x1000 != 0 || count % 0x1000 != 0)
    {
        kernelPanic("Attempted to map multiple virtual addresses or physical addresses or count that was not aligned to 4KB!\n");
    }

    for (uint64_t i = 0; i < count; i += 0x1000) {
        mapPage(virtualStart + i, physicalStart + i, flags);
    }
}

void unmapPages(uint64_t virtualStart, uint64_t count) {
    if (virtualStart % 0x1000 != 0 || count % 0x1000 != 0)
    {
        kernelPanic("Attempted to unmap multiple virtual addresses or count that was not aligned to 4KB!\n");
    }

    for (uint64_t i = 0; i < count; i += 0x1000) {
        unmapPage(virtualStart + i);
    }
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

extern "C" void setCr3() {
    __asm__ __volatile__ ("mov %0, %%cr3" :: "r"(kernelPagemap.topLevel) : "memory");
}