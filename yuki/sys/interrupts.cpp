#include <cstdint>
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
__attribute__((noreturn))
extern "C" void interruptHandler(void);
extern "C" void interruptHandler() {
    __asm__ volatile ("cli");

    kprintf(NONE, "\nKERNEL PANIC!\nA Fatal Error occured and the kernel can no longer continue!\n");
    kprintf(NONE, panicArt);

    __asm__ volatile ("hlt");
}

extern "C" void syscallHandler() {
    kprintf(NONE, "\nA syscall has been invoked!\n");
    return;
}