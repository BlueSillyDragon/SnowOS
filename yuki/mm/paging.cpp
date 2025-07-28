#include <cstdint>
#include <inc/utils/helpers.hpp>
#include <inc/sys/panic.hpp>
#include <inc/io/kprintf.hpp>
#include <inc/klibc/string.hpp>
#include <inc/mm/pmm.hpp>
#include <inc/mm/paging.hpp>

#define PML4_ID(virt) (((virt) >> 39) & 0x1FF)
#define PDPT_ID(virt) (((virt) >> 30) & 0x1FF)
#define PD_ID(virt) (((virt) >> 21) & 0x1FF)
#define PT_ID(virt) (((virt) >> 12) & 0x1FF)

uint64_t kernelVirt = 0xffffffff80000000;

Pagemap kernelPagemap;

void initPagemap(unsigned int pagingLevel) {
    uint64_t hhdm = getHhdm();

    kernelPagemap.topLevel = pmmAlloc();
    memset(reinterpret_cast<uint64_t *>(kernelPagemap.topLevel + hhdm), 0x0, 0x1000);

    if (pagingLevel == PAGING_LEVEL_5) {
        kernelPagemap.levels = 5;
    } else {
        kernelPagemap.levels = 4;
    }
}

Pagemap newPagemap() {
    Pagemap newPm;

    uint64_t hhdm = getHhdm();

    newPm.topLevel = pmmAlloc();
    memset(reinterpret_cast<uint64_t *>(newPm.topLevel + hhdm), 0x0, 0x1000);

    for (int i = 256; i < 512; i++) {
        reinterpret_cast<uint64_t *>(newPm.topLevel + hhdm)[i] = reinterpret_cast<uint64_t *>(kernelPagemap.topLevel + hhdm)[i];
    }

    return newPm;
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
    uint64_t hhdm = getHhdm();

    if (virtualAddr % 0x1000 != 0 || physicalAddr % 0x1000 != 0)
    {
        kprintf(ERROR, "Attempted to map a virtual address or physical address that was not aligned to 4KB!\n");
        __asm__ volatile (" hlt ");
    }

    uint64_t *pml4 = reinterpret_cast<uint64_t *>(kernelPagemap.topLevel + hhdm);
    uint64_t *pdpt, *pd, *pt;

    if (!(pml4[PML4_ID(virtualAddr)] & ptePresent))
    {
        uint64_t lvlAddress = pmmAlloc();
        memset(reinterpret_cast<uint64_t *>(lvlAddress + hhdm), 0, 0x1000);
        pml4[PML4_ID(virtualAddr)] = createPte(lvlAddress, flags);
    }
    pdpt = reinterpret_cast<uint64_t *>((pml4[PML4_ID(virtualAddr)] & pteAddress) + hhdm);

    if (!(pdpt[PDPT_ID(virtualAddr)] & ptePresent))
    {
        uint64_t lvlAddress = pmmAlloc();
        memset(reinterpret_cast<uint64_t *>(lvlAddress + hhdm), 0, 0x1000);
        pdpt[PDPT_ID(virtualAddr)] = createPte(lvlAddress, flags);
    }
    pd = reinterpret_cast<uint64_t *>((pdpt[PDPT_ID(virtualAddr)] & pteAddress) + hhdm);

    if (!(pd[PD_ID(virtualAddr)] & ptePresent))
    {
        uint64_t lvlAddress = pmmAlloc();
        memset(reinterpret_cast<uint64_t *>(lvlAddress + hhdm), 0, 0x1000);
        pd[PD_ID(virtualAddr)] = createPte(lvlAddress, flags);
    }
    pt = reinterpret_cast<uint64_t *>((pd[PD_ID(virtualAddr)] & pteAddress) + hhdm);
    pt[PT_ID(virtualAddr)] = createPte(physicalAddr, flags);
}

void unmapPage(uint64_t virtualAddr) {
    uint64_t hhdm = getHhdm();

    uint64_t *pml4 = reinterpret_cast<uint64_t *>(kernelPagemap.topLevel + hhdm);
    uint64_t *pdpt = reinterpret_cast<uint64_t *>((pml4[PML4_ID(virtualAddr)] & pteAddress) + hhdm);
    uint64_t *pd = reinterpret_cast<uint64_t *>((pdpt[PDPT_ID(virtualAddr)] & pteAddress) + hhdm);
    uint64_t *pt = reinterpret_cast<uint64_t *>((pd[PD_ID(virtualAddr)] & pteAddress) + hhdm);
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

extern "C" void setCr3() {
    __asm__ __volatile__ ("mov %0, %%cr3" :: "r"(kernelPagemap.topLevel) : "memory");
}