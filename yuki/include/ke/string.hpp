/**
Snow Operating System
Copyright (c) BlueSillyDragon 2025
 
File: ke/string.hpp

Description:
This file contains the function
prototypes for some of the functions from
the string library

Author:
BlueSillyDragon
October 30th 2025
**/

#pragma once

#include <cstddef>

extern "C" void *memcpy(void *__restrict dest, const void *__restrict src, std::size_t n);
extern "C" void *memset(void *s, int c, std::size_t n);
extern "C" void *memmove(void *dest, const void *src, std::size_t n);
extern "C" int memcmp(const void *s1, const void *s2, std::size_t n);
size_t strlen(char *String);