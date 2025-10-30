/**
Snow Operating System
Copyright (c) BlueSillyDragon 2025
 
File: ke/string.hpp

Description:
This file contains some function prototypes
and defines for the logger

Author:
BlueSillyDragon
October 30th 2025
**/

#pragma once

#define ANSI_RESET         "\033[0m"
#define ANSI_BLACK         "\033[30m"
#define ANSI_RED           "\033[31m"
#define ANSI_GREEN         "\033[32m"
#define ANSI_YELLOW        "\033[33m"
#define ANSI_BLUE          "\033[34m"
#define ANSI_MAGENTA       "\033[35m"
#define ANSI_CYAN          "\033[36m"
#define ANSI_WHITE         "\033[37m"

// Bright Foreground Colors
#define ANSI_BRIGHT_BLACK   "\033[90m"
#define ANSI_BRIGHT_RED     "\033[91m"
#define ANSI_BRIGHT_GREEN   "\033[92m"
#define ANSI_BRIGHT_YELLOW  "\033[93m"
#define ANSI_BRIGHT_BLUE    "\033[94m"
#define ANSI_BRIGHT_MAGENTA "\033[95m"
#define ANSI_BRIGHT_CYAN    "\033[96m"
#define ANSI_BRIGHT_WHITE   "\033[97m"

enum class LOG_TYPE {None, KeLog, HalLog};

void KePrint(LOG_TYPE LogType, const char* String, ...);