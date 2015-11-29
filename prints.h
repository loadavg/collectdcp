/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#ifndef PRINTS
#define PRINTS

#include <string>
#include <cstdio>
#include <cstdarg>

/**
 * @brief prints
 *  format string in printf style
 * @param spec
 *  printf specification
 * @return
 *  formatted string
 */
inline std::string prints(const char *spec, ...) {
    std::va_list l;
    va_start(l, spec);
    char buffer[1 << 12];
    std::vsnprintf(buffer, sizeof(buffer), spec, l);
    va_end(l);
    return buffer;
}

#endif // PRINTS

