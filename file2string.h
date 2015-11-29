/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#ifndef FILE2STRING_H
#define FILE2STRING_H

#include <fstream>
#include <sstream>
#include <stdexcept>

inline std::string file2string(std::string path) {
    std::ifstream t(path);
    if (!t.is_open())
        throw std::invalid_argument("cannot read " + path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

#endif // FILE2STRING_H
