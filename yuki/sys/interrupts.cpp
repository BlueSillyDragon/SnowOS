#include <cstdint>
#include <inc/sys/panic.hpp>
#include <inc/io/kprintf.hpp>
#include <inc/sys/apic.hpp>
#include <inc/sys/spinlock.hpp>

char *panicArt = "   ____________    _______________________________\n"
"  /            \\   |                              |\n"
" /              \\  / Whoops! Kernel died...       |\n"
" |    `|    `|  | /_______________________________|\n"
" |     |     |  |\n"
" |         ^    |\n"
" |              |\n"
" |              |\n"
" \\              |\n"
"  \\            /\n"
"   \\          /\n"
"   /        _/\n"
"  /       _/\n"
" /      _/\n"
"/     _/\n"
"\\    /\n"
" \\__/\n";


extern "C" void yield();

extern "C" void kernelPanic(char *errorMsg) {
    kprintf(NONE, "[ " ANSI_RED  "KERNEL PANIC!" ANSI_RESET "] Reason: ");

    if (errorMsg == nullptr) {
        kprintf(NONE, "Unknown error!\n");
    } else {
        kprintf(NONE, "%s\n", errorMsg);
    }

    kprintf(NONE, panicArt);
}

__attribute__((noreturn))
extern "C" void interruptHandler() {
    __asm__ volatile ("cli");

    kernelPanic("Exception Occured!\n");

    __asm__ volatile ("hlt");
}

extern "C" void syscallHandler() {
    kprintf(NONE, "\nA syscall has been invoked!\n");
    return;
}

extern "C" void timerHandler() {
    apicWrite(0xb0, 0);
    yield();
}

extern "C" void irqHandler() {
    kprintf(NONE, "\nIRQ Occured!\n");
    return;
}