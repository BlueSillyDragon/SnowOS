#pragma once

#include <flanterm.h>

enum INFO {NONE, OK, ERROR, PMM, VMM, SCHEDULER};

void setFtCtx(struct flanterm_context *flantermCtx);
void kprintf(INFO info, const char *string, ...);