#pragma once

#include <cstdint>
#include <cstddef>

uint64_t mmioRead(uint64_t address, size_t dataSize);
void mmioWrite(uint64_t address, uint64_t data, size_t dataSize);