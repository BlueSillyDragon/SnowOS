#pragma once

#include <flanterm.h>
#include <limine.h>
#include <cstdint>
#include <cstdarg>

enum INFO_TYPE {PMM, VMM, SCHEDULER};

struct flanterm_context *termInit(limine_framebuffer *framebuffer, uint32_t termFg, uint32_t termBg);