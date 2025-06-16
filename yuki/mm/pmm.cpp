#include <cstddef>
#include <limine.h>
#include <cstdint>
#include <inc/io/kprintf.hpp>
#include <inc/klibc/string.hpp>
#include <inc/mm/pmm.hpp>

uint64_t hhdm_offset;      // We need this to actually access the page, so that we can retrieve the next page in the free list

char *memTypeToString(std::uint64_t memType)
{
    switch (memType)
    {
        case LIMINE_MEMMAP_RESERVED:
            return "Reserved";
        case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
            return "ACPI Reclaimable Memory";
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
            return "Bootloader Reclaimable Memory";
        case LIMINE_MEMMAP_USABLE:
            return "Free Memory";
        case LIMINE_MEMMAP_KERNEL_AND_MODULES:
            return "Kernel and Modules";
        default:
            return "Unknown Memory Type";
    }
}

struct pmmNode {
    pmmNode *next;
} __attribute__((packed));
pmmNode head = {.next = nullptr};

void initPmm(limine_memmap_response *memoryMap, uint64_t hhdm)
{
    kprintf(PMM, "Building FreeList...\n");

    uint64_t nop = 0;
    pmmNode *currentNode = &head;
    hhdm_offset = hhdm;

    for(uint64_t i = 0; i < memoryMap->entry_count; i++)
    {

        if (memoryMap->entries[i]->type == LIMINE_MEMMAP_USABLE)
        {
            nop += (memoryMap->entries[i]->length / 0x1000);

            // Loop through all pages in memory area, and link them together
            for (uint64_t j = 0; j < memoryMap->entries[i]->length; j += 0x1000)
            {
                pmmNode *nextNode = reinterpret_cast<pmmNode *>(memoryMap->entries[i]->base + j + hhdm);
                memset(nextNode, 0x0, 0x1000);
                currentNode->next = reinterpret_cast<pmmNode *>(nextNode);
                currentNode = nextNode;
            }
        }
    }
    if (((nop * 4) / 1024) < 400)   // Not all of RAM is usable, give some account for this
    {
        kprintf(ERROR, "Please run SnowOS with atleast 512MB of RAM!\n");
        __asm__ volatile (" hlt ");
    }

    kprintf(PMM, "Finished building FreeList!\n");
    kprintf(PMM, "SnowOS has %dGB of memory available\n", ((nop * 4) / 1024) / 1024);
    kprintf(PMM, "Usable Pages: %d\n", nop);
}

uint64_t pmmAlloc()
{
    if (head.next == nullptr)
    {
        kprintf(ERROR, "Out of Memory!\n");
        return 0x0;
    }
    pmmNode *returnPage = head.next;
    head.next = returnPage->next;
    memset(returnPage, 0x0, 0x1000);

    return reinterpret_cast<uint64_t>(returnPage) - hhdm_offset;
}

void pmmFree(uint64_t page)
{
    
}