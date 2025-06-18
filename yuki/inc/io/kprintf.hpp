#pragma once

#include <flanterm.h>

enum INFO {NONE, YUKI, OK, ERROR, PMM, VMM, SLAB, SCHEDULER};

void setFtCtx(struct flanterm_context *flantermCtx);
void kprintf(INFO info, const char *string, ...);