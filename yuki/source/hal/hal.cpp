#include <cstdint>
#include <cstddef>
#include <flanterm.h>
#include <flanterm_backends/fb.h>
#include <hal/hal.hpp>

struct flanterm_context* FtCtx;

uint32_t TerminalForeground = 0xFFFFFF;
uint32_t TerminalBackground = 0x23272E;

size_t strlen(const char *string) {
    size_t length = 0;
    for (int i = 0; string[i] != '\0'; i++)
    {
        length++;
    }
    length++;
    return length;
}

extern "C" void ReloadSegments();

GDT Gdt {0, KERNEL_CS, KERNEL_DS, USER_CS, USER_DS};
GDTR GdtRegister;

void HalInit(limine_framebuffer* Framebuffer)
{
    FtCtx = flanterm_fb_init(
        NULL,
        NULL,
        reinterpret_cast<uint32_t *>(Framebuffer->address), Framebuffer->width, Framebuffer->height, Framebuffer->pitch,
        Framebuffer->red_mask_size, Framebuffer->red_mask_shift,
        Framebuffer->green_mask_size, Framebuffer->green_mask_shift,
        Framebuffer->blue_mask_size, Framebuffer->blue_mask_shift,
        NULL,
        NULL, NULL,
        &TerminalBackground, &TerminalForeground,
        NULL, NULL,
        NULL, 0, 0, 1,
        0, 0,
        0
    );
}

void HalPrintString(const char* String)
{
    flanterm_write(FtCtx, String, strlen(String));
}

void HalInitGdt()
{
    GdtRegister.Base = reinterpret_cast<uint64_t>(&Gdt);
    GdtRegister.Limit = (sizeof(Gdt) - 1);

    __asm__ volatile ("lgdt %0" :: "m"(GdtRegister));
    ReloadSegments();

    HalPrintString("[Hal] GDT Initialized!\n");
}