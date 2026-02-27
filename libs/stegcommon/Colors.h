//
// Created by Roumite on 26/02/2026.
//

#pragma once


#ifdef _WIN32
#include <windows.h>

inline bool enable_ansi()
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(h, &mode);
    SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    return true;
}

inline const bool ansi_enabled = enable_ansi();

#endif

namespace AnsiColors
{
    constexpr const char* Reset = "\033[0m";
    constexpr const char* Red = "\033[31m";
    constexpr const char* Yellow = "\033[33m";
    constexpr const char* Bold = "\033[1m";
}