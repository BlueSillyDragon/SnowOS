#pragma once

void enableIoApic();
void enableLapicTimer();
void disableLvtTimer();
void enableLvtTimer(bool periodic);