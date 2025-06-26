#pragma once

#include <cstdint>

struct Tss {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist[7];
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iobp;
}__attribute__((packed));

struct TssDescriptor {
    uint16_t limitLow;
    uint16_t baseLow;
    uint8_t baseMid0;
    uint8_t accessByte;
    uint8_t limitFlags;
    uint8_t baseMid1;
    uint32_t baseHigh;
    uint32_t reserved;
}__attribute__((packed));

void initTss();