#include <inc/sys/panic.hpp>
#include <inc/utils/helpers.hpp>
#include <inc/io/kprintf.hpp>
#include <inc/sys/apic.hpp>
#include <inc/sys/spinlock.hpp>
#include <inc/sched/scheduler.hpp>

const char *panicArt = "   ____________    _______________________________\n"
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

extern "C" void kernelPanic(const char *errorMsg) {
    kprintf(NONE, "[ " ANSI_RED  "KERNEL PANIC!" ANSI_RESET "] Reason: ");

    if (errorMsg == nullptr) {
        kprintf(NONE, "Unknown error!\n");
    } else {
        kprintf(NONE, "%s\n", errorMsg);
    }

    kprintf(NONE, panicArt);

    hcf();
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
    schedule();
}

extern "C" void irqHandler() {
    kprintf(NONE, "\nIRQ Occured!\n");
    return;
}