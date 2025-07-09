#include <cstddef>
#include <limine.h>
#include <cstdint>
#include <inc/io/kprintf.hpp>
#include <inc/klibc/string.hpp>
#include <inc/mm/pmm.hpp>
#include <inc/sys/spinlock.hpp>

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
        default:
            return "Unknown Memory Type";
    }
}

struct pmmNode {
    uint64_t start;
    uint64_t length;
    pmmNode *next;
} __attribute__((packed));
pmmNode head = {.next = nullptr};

void initPmm(limine_memmap_response *memoryMap, uint64_t hhdm)
{
    kprintf(PMM, "Building FreeList...\n");

    uint64_t nop = 0;
    pmmNode *currentNode = &head;
    pmmNode *nextNode = nullptr;
    hhdm_offset = hhdm;
    bool endOfMem = true;

    for(uint64_t i = 0; i < memoryMap->entry_count; i++)
    {
        if (memoryMap->entries[i]->type == LIMINE_MEMMAP_USABLE)
        {
            if (memoryMap->entries[i]->base == 0x0000) continue;
            nop += (memoryMap->entries[i]->length / 0x1000);

            // Find the next free area
            for (uint64_t j = i + 1; j < memoryMap->entry_count; j++) {
                if (memoryMap->entries[j]->type == LIMINE_MEMMAP_USABLE) {
                    nextNode = reinterpret_cast<pmmNode *>(memoryMap->entries[j]->base + hhdm);
                    memset(nextNode, 0x0, 0x1000);
                    endOfMem = false;
                    break;
                }
            }

            if (endOfMem) {
                currentNode->next = nullptr;
            } else {
                currentNode->next = nextNode;
                currentNode->start = memoryMap->entries[i]->base;
                currentNode->length = memoryMap->entries[i]->length;
                currentNode = nextNode;
            }
        }
        endOfMem = true;
    }
    if (((nop * 4) / 1024) < 25)   // Not all of RAM is usable, give some account for this
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
    TicketSpinlock::lock();
    if (head.start >= (head.length)) // TODO: This is a temporary fix
    {
        if (head.next == nullptr) {
            kprintf(ERROR, "Out of Memory!\n");
            return 0x0;
        }
        head = *head.next;
    }
    uint64_t returnPage = head.start;
    head.start += 0x1000;
    TicketSpinlock::unlock();

    return returnPage;
}

void pmmFree(uint64_t page)
{
    pmmNode *newNode = reinterpret_cast<pmmNode *>(page + hhdm_offset);
    memset(newNode, 0, 0x1000);
    newNode->next = &head;
    newNode->start = page;
    newNode->length = 0x1000;
    head = *newNode;
    kprintf(PMM, "New freelist entry created!\n");
}