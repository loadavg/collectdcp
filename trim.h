/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef TRIM_H
#define TRIM_H

#include <string>

inline std::string trim(std::string str, std::string blanks = " \n\t\r") {
    str.erase(0, str.find_first_not_of(blanks));       //prefixing spaces
    str.erase(str.find_last_not_of(blanks) + 1);       //surfixing spaces
    return str;
}

#endif // TRIM_H
