#include <inc/sys/panic.hpp>
#include <inc/utils/mmio.hpp>

uint64_t mmioRead(uint64_t address, size_t dataSize) {
    switch (dataSize) {
        case sizeof(uint8_t):
            uint8_t value8;
            asm volatile ("mov %1, %0" : "=q"(value8) : "m"(*reinterpret_cast<volatile uint8_t *>(address)) : "memory");
            return value8;
        case sizeof(uint16_t):
            uint16_t value16;
            asm volatile ("mov %1, %0" : "=q"(value16) : "m"(*reinterpret_cast<volatile uint16_t *>(address)) : "memory");
            return value16;
        case sizeof(uint32_t):
            uint32_t value32;
            asm volatile ("mov %1, %0" : "=q"(value32) : "m"(*reinterpret_cast<volatile uint32_t *>(address)) : "memory");
            return value32;
        case sizeof(uint64_t):
            uint64_t value64;
            asm volatile ("mov %1, %0" : "=q"(value64) : "m"(*reinterpret_cast<volatile uint64_t *>(address)) : "memory");
            return value64;
        default:
            kernelPanic("Invalid size passed to mmioRead()");
    }
    return 0;
}

void mmioWrite(uint64_t address, uint64_t data, size_t dataSize) {
    switch (dataSize) {
        case sizeof(uint8_t):
            asm volatile ("mov %0, %1" :: "q"(data), "m"(*reinterpret_cast<volatile uint8_t *>(address)) : "memory");
			break;
        case sizeof(uint16_t):
            asm volatile ("mov %0, %1" :: "q"(data), "m"(*reinterpret_cast<volatile uint16_t *>(address)) : "memory");
			break;
        case sizeof(uint32_t):
            asm volatile ("mov %0, %1" :: "q"(data), "m"(*reinterpret_cast<volatile uint32_t *>(address)) : "memory");
			break;
        case sizeof(uint64_t):
            asm volatile ("mov %0, %1" :: "q"(data), "m"(*reinterpret_cast<volatile uint64_t *>(address)) : "memory");
			break;
        default:
            kernelPanic("Invalid size passed to mmioWrite()");
    }
}