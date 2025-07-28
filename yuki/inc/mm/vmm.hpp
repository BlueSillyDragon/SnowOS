#pragma once

#include <cstdint>
#include <limine.h>
#include <inc/mm/paging.hpp>

uint64_t createPte(uint64_t physicalAddr, uint64_t flags);
void initVmm(limine_memmap_response *memoryMap, limine_executable_address_response *kernelAddr);
void *vmmMapPhys(uint64_t physicalAddr, uint64_t length);
void vmmUnmapVirt(void *virtualAddr, uint64_t length);