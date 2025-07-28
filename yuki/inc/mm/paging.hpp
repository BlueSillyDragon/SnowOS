#pragma once

#include <cstdint>

constexpr uint64_t ptePresent = 0x1;
constexpr uint64_t pteWrite = 0x2;
constexpr uint64_t pteUser = 0x4;
constexpr uint64_t ptePwt = 0x8;
constexpr uint64_t ptePcd = 0x10;
constexpr uint64_t pteNx = 0x8000000000000000;
constexpr uint64_t pteAddress = 0x0000fffffffff000;

#define PAGING_LEVEL_4 0
#define PAGING_LEVEL_5 1

typedef struct {
    uint64_t levels;
    uint64_t topLevel;
} Pagemap;

// Initializes the kernel pagemap
void initPagemap(unsigned int pagingLevel);

Pagemap newPagemap();

void mapPage(uint64_t virtualAddr, uint64_t physicalAddr, uint64_t flags);
void mapPages(uint64_t virtualStart, uint64_t physicalStart, uint64_t flags, uint64_t count);
void unmapPage(uint64_t virtualAddr);
void unmapPages(uint64_t virtualStart, uint64_t count);
void remapPage(uint64_t virtualAddr);

extern "C" void setCr3();