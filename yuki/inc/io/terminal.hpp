#pragma once

#include <flanterm.h>
#include <limine.h>
#include <cstdint>
#include <cstdarg>

struct flanterm_context *termInit(limine_framebuffer *framebuffer, uint32_t termFg, uint32_t termBg);