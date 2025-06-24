#include <cstdarg>
#include <flanterm.h>
#include <inc/io/terminal.hpp>
#include <inc/io/kprintf.hpp>
#include <inc/klibc/string.hpp>
#include <inc/io/serial.hpp>

struct flanterm_context *ftCtx;

#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_SMALL_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 0

// Compile nanoprintf in this translation unit.
#define NANOPRINTF_IMPLEMENTATION
#include <inc/io/nanoprintf.hpp>

char buf[512];
int idx = 0;

void setFtCtx(struct flanterm_context *flantermCtx) {
    ftCtx = flantermCtx;
}

void putchar(int ch, void *ctx) {
    char c = ch;
    buf[idx] = c;
    idx++;
    writeSerial(c);
}

void kprintf(INFO info, const char *string, ...)
{
    std::va_list arg;
    va_start(arg, string);

    if (info != NONE) {
        switch (info) {
            case YUKI:
                flanterm_write(ftCtx, "[ " ANSI_BLUE "Yuki" ANSI_RESET " ] ", strlen("[ " ANSI_BLUE "Yuki" ANSI_RESET " ] "));
                stringToSerial("[ " ANSI_BLUE "Yuki" ANSI_RESET " ] ");
                break;
            case OK:
                flanterm_write(ftCtx, "[ " ANSI_BRIGHT_GREEN "OK" ANSI_RESET " ] ", strlen("[ " ANSI_BRIGHT_GREEN "OK" ANSI_RESET " ] "));
                stringToSerial("[ " ANSI_BRIGHT_GREEN "OK" ANSI_RESET " ] ");
                break;
            case ERROR:
                flanterm_write(ftCtx, "[ " ANSI_BRIGHT_RED "Error" ANSI_RESET " ] ", strlen("[ " ANSI_BRIGHT_RED "Error" ANSI_RESET " ] "));
                stringToSerial("[ " ANSI_BRIGHT_RED "Error" ANSI_RESET " ] ");
                break;
            case PMM:
                flanterm_write(ftCtx, "[ " ANSI_BRIGHT_BLUE "Pmm" ANSI_RESET " ] ", strlen("[ " ANSI_BRIGHT_BLUE "Pmm" ANSI_RESET " ] "));
                stringToSerial("[ " ANSI_BRIGHT_BLUE "Pmm" ANSI_RESET " ] ");
                break;
            case VMM:
                flanterm_write(ftCtx, "[ " ANSI_BRIGHT_MAGENTA "Vmm" ANSI_RESET " ] ", strlen("[ " ANSI_BRIGHT_MAGENTA "Vmm" ANSI_RESET " ] "));
                stringToSerial("[ " ANSI_BRIGHT_MAGENTA "Vmm" ANSI_RESET " ] ");
                break;
            case SLAB:
                flanterm_write(ftCtx, "[ " ANSI_YELLOW "Slab" ANSI_RESET " ] ", strlen("[ " ANSI_YELLOW "Slab" ANSI_RESET " ] "));
                stringToSerial("[ " ANSI_YELLOW "Slab" ANSI_RESET " ] ");
                break;
            case SCHEDULER:
                flanterm_write(ftCtx, "[ " ANSI_BRIGHT_YELLOW "Scheduler" ANSI_RESET " ] ", strlen("[ " ANSI_BRIGHT_YELLOW "Scheduler" ANSI_RESET " ] "));
                stringToSerial("[ " ANSI_BRIGHT_YELLOW "Scheduler" ANSI_RESET " ] ");
                break;
        }
    }

    npf_vpprintf(putchar, nullptr, string, arg);

    flanterm_write(ftCtx, buf, strlen(buf));
    
    va_end(arg);
    // Make sure to clear the buffer
    memset(buf, 0, sizeof(buf));
    idx = 0;
}

void kvprintf(INFO info, const char *string, std::va_list arg)
{

    if (info != NONE) {
        switch (info) {
            case YUKI:
                flanterm_write(ftCtx, "[ \033[34mYuki\033[0m ] ", sizeof("[ \033[34mYuki\033[0m ] "));
                stringToSerial("[ \033[34mYuki\033[0m ] ");
                break;
            case OK:
                flanterm_write(ftCtx, "[ \033[92mOK\033[0m ] ", sizeof("[ \033[92mOK\033[0m ] "));
                stringToSerial("[ \033[92mOK\033[0m ] ");
                break;
            case ERROR:
                flanterm_write(ftCtx, "[ \033[91mError\033[0m ] ", sizeof("[ \033[91mError\033[0m ] "));
                stringToSerial("[ \033[91mError\033[0m ] ");
                break;
            case PMM:
                flanterm_write(ftCtx, "[ \033[94mPmm\033[0m ] ", sizeof("[ \033[94mPmm\033[0m ] "));
                stringToSerial("[ \033[94mPmm\033[0m ] ");
                break;
            case VMM:
                flanterm_write(ftCtx, "[ \033[95mVmm\033[0m ] ", sizeof("[ \033[95mVmm\033[0m ] "));
                stringToSerial("[ \033[95mVmm\033[0m ] ");
                break;
            case SLAB:
                flanterm_write(ftCtx, "[ \033[33mSlab\033[0m ] ", sizeof("[ \033[33mSlab\033[0m ] "));
                stringToSerial("[ \033[33mSlab\033[0m ] ");
                break;
            case SCHEDULER:
                flanterm_write(ftCtx, "[ \033[93mScheduler\033[0m ] ", sizeof("[ \033[93mScheduler\033[0m ] "));
                stringToSerial("[ \033[93mScheduler\033[0m ] ");
                break;
        }
    }

    npf_vpprintf(putchar, nullptr, string, arg);

    flanterm_write(ftCtx, buf, strlen(buf));
    
    // Make sure to clear the buffer
    memset(buf, 0, sizeof(buf));
    idx = 0;
}