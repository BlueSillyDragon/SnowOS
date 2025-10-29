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