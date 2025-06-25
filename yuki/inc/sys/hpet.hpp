#pragma once

#include <cstdint>

void enableHpet();
void disableHpet();
uint64_t readHpet();
uint64_t hpetFreqNs();
void hpetSleep(uint64_t ms);