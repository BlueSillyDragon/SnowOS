#pragma once

#include <cstdint>

int newProcess(void *(*startFunction) (void *));
void initScheduler(uint64_t hhdm);