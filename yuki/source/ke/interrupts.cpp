#include "hal/hal.hpp"
#include <cstdint>

__attribute__((noreturn)) extern "C" void KeInterruptHandler()
{
    HalPrintString("Oopsies!\n");
    for (;;)
    {
        __asm__ volatile (" hlt ");
    }
}