/**
Snow Operating System
Copyright (c) BlueSillyDragon 2025
 
File: ke/interrupts.cpp

Description:
This file contains the interrupt handling
routines for Yuki

Author:
BlueSillyDragon
October 29th 2025
**/

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