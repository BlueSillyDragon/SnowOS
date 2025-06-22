#pragma once

#include <cstdint>
#include <limine.h>

typedef struct 
{
    uint64_t levels;
    uint64_t topLevel;
} pagemap_t;

uint64_t createPte(uint64_t physicalAddr, uint64_t flags);
void initVmm(limine_memmap_response *memoryMap, limine_kernel_address_response *kernelAddr, std::uint64_t hhdm);
void mapPage(uint64_t virtualAddr, uint64_t physicalAddr, uint64_t flags);
void mapPages(uint64_t virtualStart, uint64_t physicalStart, uint64_t flags, uint64_t count);
void unmapPage(uint64_t virtualAddr);
void unmapPages(uint64_t virtualStart, uint64_t count);
void remapPage(uint64_t virtualAddr);

void *vmmMapPhys(uint64_t physicalAddr, uint64_t length);
void vmmUnmapVirt(void *virtualAddr, uint64_t length);