#pragma once

#include <cstdint>

void hcf();
void disableInterrupts();
void enableInterrupts();

void wrmsr(uint32_t msr, uint64_t value);
uint64_t rdmsr(uint32_t msr);

uint64_t getHhdm();
uint64_t getRsdp();