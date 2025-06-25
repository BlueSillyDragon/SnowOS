#include <cstdint>
#include <inc/sys/panic.hpp>
#include <inc/io/kprintf.hpp>

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
extern "C" void interruptHandler(void);
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
    kprintf(NONE, "\nTick!\n");
    return;
}

extern "C" void irqHandler() {
    kprintf(NONE, "\nIRQ Occured!\n");
    return;
}