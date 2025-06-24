#include "inc/io/kprintf.hpp"
#include <cstdint>
#include <inc/klibc/string.hpp>
#include <inc/sys/cpuid.hpp>

Cpuid getCpuid(uint32_t eax) {
    Cpuid cpuid;
    __asm__ __volatile__ ("mov %0, %%eax" :: "a"(eax));
    __asm__ __volatile__ ("cpuid");

    __asm__ __volatile__ ("mov %%ebx, %0" : "=b"(cpuid.ebx));
    __asm__ __volatile__ ("mov %%ecx, %0" : "=c"(cpuid.ecx));
    __asm__ __volatile__ ("mov %%edx, %0" : "=d"(cpuid.edx));

    return cpuid;
}