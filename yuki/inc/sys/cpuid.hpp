#pragma once

#include <cstdint>

struct Cpuid {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
};

Cpuid getCpuid(uint32_t eax);