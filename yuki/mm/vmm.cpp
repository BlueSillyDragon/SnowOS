#include "inc/io/kprintf.hpp"
#include <cstdint>
#include <limine.h>
#include <inc/klibc/string.hpp>
#include <inc/io/serial.hpp>
#include <inc/mm/pmm.hpp>
#include <inc/mm/vmm.hpp>
#include <stdint.h>

constexpr uint64_t ptePresent = 0x1;
constexpr uint64_t pteWrite = 0x2;
constexpr uint64_t pteUser = 0x4;
constexpr uint64_t ptePwt = 0x8;
constexpr uint64_t ptePcd = 0x10;
constexpr uint64_t pteAddress = 0x0000fffffffff000;

#define PML4_ID(virt) (((virt) >> 39) & 0x1FF)
#define PDPT_ID(virt) (((virt) >> 30) & 0x1FF)
#define PD_ID(virt) (((virt) >> 21) & 0x1FF)
#define PT_ID(virt) (((virt) >> 12) & 0x1FF)

pagemap_t pagemap;

uint64_t hhdmOffset;

uint64_t kernelVirt = 0xffffffff80000000;

void initVmm(limine_memmap_response *memoryMap, std::uint64_t hhdm)
{
    kprintf(VMM, "Initializing VMM...\n");
    hhdmOffset = hhdm;
    pagemap.topLevel = pmmAlloc();
    memset(reinterpret_cast<uint64_t *>(pagemap.topLevel + hhdm), 0x0, 0x1000);

    uint64_t oldCr3 = 0;

    __asm__ __volatile__ ("mov %%cr3, %%rax; mov %%rax, %0" : "=a"(oldCr3));

    for (uint64_t i = 0; i < 512; i++)
    {
        if (reinterpret_cast<uint64_t *>(oldCr3 + hhdm)[i] & ptePresent)
        {
            reinterpret_cast<uint64_t *>(pagemap.topLevel + hhdm)[i] = reinterpret_cast<uint64_t *>(oldCr3 + hhdm)[i];
        }
    }

    __asm__ __volatile__ ("mov %0, %%cr3" :: "r"(pagemap.topLevel) : "memory");

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

    uint64_t *pml4 = reinterpret_cast<uint64_t *>(pagemap.topLevel + hhdmOffset);
    uint64_t *pdpt, *pd, *pt;

    if (!(pml4[PML4_ID(virtualAddr)] & ptePresent))
    {
        uint64_t lvlAddress = pmmAlloc();
        memset(reinterpret_cast<uint64_t *>(lvlAddress + hhdmOffset), 0, 0x1000);
        pml4[PML4_ID(virtualAddr)] = createPte(lvlAddress, 0);
    }
    pdpt = reinterpret_cast<uint64_t *>((pml4[PML4_ID(virtualAddr)] & pteAddress) + hhdmOffset);

    if (!(pdpt[PDPT_ID(virtualAddr)] & ptePresent))
    {
        uint64_t lvlAddress = pmmAlloc();
        memset(reinterpret_cast<uint64_t *>(lvlAddress + hhdmOffset), 0, 0x1000);
        pdpt[PDPT_ID(virtualAddr)] = createPte(lvlAddress, 0);
    }
    pd = reinterpret_cast<uint64_t *>((pdpt[PDPT_ID(virtualAddr)] & pteAddress) + hhdmOffset);

    if (!(pd[PD_ID(virtualAddr)] & ptePresent))
    {
        uint64_t lvlAddress = pmmAlloc();
        memset(reinterpret_cast<uint64_t *>(lvlAddress + hhdmOffset), 0, 0x1000);
        pd[PD_ID(virtualAddr)] = createPte(lvlAddress, 0);
    }
    pt = reinterpret_cast<uint64_t *>((pd[PD_ID(virtualAddr)] & pteAddress) + hhdmOffset);
    pt[PT_ID(virtualAddr)] = createPte(physicalAddr, flags);
    kprintf(VMM, "Entry is: 0x%x\n", pt[PT_ID(virtualAddr)]);
}

void mapPages(uint64_t virtualStart, uint64_t physicalStart, uint64_t flags, uint64_t count)
{
    if (virtualStart % 0x1000 != 0 || physicalStart % 0x1000 != 0 || count % 0x1000 != 0)
    {
        //kernTerminal.kerror("Attempted to map multiple virtual addresses or physical addresses or count that was not aligned to 4KB!\n");
        __asm__ volatile (" hlt ");
    }

    for (uint64_t i = 0; i < count; ) {
        mapPage(virtualStart + i, physicalStart + i, flags);
        i += 0x1000;
    }
}