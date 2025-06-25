#pragma once

#include <cstdint>

uint32_t apicRead(uint32_t registr);
void apicWrite(uint32_t registr, uint32_t value);
void enableIoApic();
void enableLapicTimer();
void disableLvtTimer();
void enableLvtTimer(bool periodic);