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

#include <cstdint>
#include <ke/print.hpp>

typedef struct
{
    uint64_t Rdx;
    uint64_t Rcx;
    uint64_t Rbx;
    uint64_t Rax;
    uint64_t InterruptVector;
    uint64_t ErrorCode;
    uint64_t Rip;
    uint64_t Cs;
    uint64_t Rflags;
    uint64_t Rsp;
} __attribute__((packed)) InterruptFrame;

__attribute__((noreturn)) extern "C" void KeInterruptHandler(InterruptFrame* StackFrame)
{
    KePrint(LOG_TYPE::None, "\n" ANSI_BRIGHT_RED "Kernel Panic!!!\n");
    KePrint(LOG_TYPE::None, "Stack Frame at: 0x%llX\n\n", StackFrame);

    KePrint(LOG_TYPE::None, "Exception: ");

    switch(StackFrame->InterruptVector)
    {
        case (0x0):
            KePrint(LOG_TYPE::None, "Division Error!");
            break;
        case (0x1):
            KePrint(LOG_TYPE::None, "Debug Interrupt!");
            break;
        case (0x2):
            KePrint(LOG_TYPE::None, "Non-maskable Interrupt!");
            break;
        case (0x3):
            KePrint(LOG_TYPE::None, "Breakpoint!");
            break;
        case (0x4):
            KePrint(LOG_TYPE::None, "Overflow!");
            break;
        case (0x5):
            KePrint(LOG_TYPE::None, "Bound Range Exceeded!");
            break;
        case (0x6):
            KePrint(LOG_TYPE::None, "Invalid Opcode!");
            break;
        case (0x7):
            KePrint(LOG_TYPE::None, "Device not Available!");
            break;
        case (0xD):
            KePrint(LOG_TYPE::None, "General Protection Fault!");
            break;
        case (0xE):
            KePrint(LOG_TYPE::None, "Page Fault!");
            break;
    }

    KePrint(LOG_TYPE::None, " Error Code: 0x%llX\nRSP: 0x%llX | RFLAGS: 0x%llX\nCS: 0x%llX | RIP: 0x%llX\n",
    StackFrame->ErrorCode,
    StackFrame->Rsp,
    StackFrame->Rflags,
    StackFrame->Cs,
    StackFrame->Rip);
    KePrint(LOG_TYPE::None, "RAX: 0x%llX | RBX: 0x%llX\nRCX: 0x%llX | RDX: 0x%llX",
    StackFrame->Rax,
    StackFrame->Rbx,
    StackFrame->Rcx,
    StackFrame->Rdx);
    for (;;)
    {
        __asm__ volatile (" hlt ");
    }
}