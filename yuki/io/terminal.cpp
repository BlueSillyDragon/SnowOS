#include <cstdint>
#include <cstdarg>
#include <inc/io/terminal.hpp>
#include <flanterm.h>
#include <flanterm_backends/fb.h>
#include <inc/io/krnl_font.hpp>
#include <inc/io/krnl_colors.hpp>
#include <inc/io/serial.hpp>

struct flanterm_context *termInit(limine_framebuffer *framebuffer, uint32_t termFg, uint32_t termBg) {
    struct flanterm_context *ft_ctx = flanterm_fb_init(
        NULL,
        NULL,
        reinterpret_cast<uint32_t *>(framebuffer->address), framebuffer->width, framebuffer->height, framebuffer->pitch,
        framebuffer->red_mask_size, framebuffer->red_mask_shift,
        framebuffer->green_mask_size, framebuffer->green_mask_shift,
        framebuffer->blue_mask_size, framebuffer->blue_mask_shift,
        NULL,
        NULL, NULL,
        &termBg, &termFg,
        NULL, NULL,
        kernel_font, 8, 16, 1,
        0, 0,
        0
    );
    return ft_ctx;
}