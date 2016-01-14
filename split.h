/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef SPLIT_H
#define SPLIT_H

#include <vector>
#include <sstream>

/**
 * @brief split
 *  divide a string on character boundaries
 * @param file
 *  string buffer to split
 * @param sep
 *  string delimiter character
 * @return
 *  array of strings
 */
inline std::vector<std::string> split(std::string file, char sep = '\n') {
    std::vector<std::string> v;
    std::istringstream s(file);
    std::string line;
    while (getline(s, line, sep))
        v.push_back(line);
    return v;
}

#endif // SPLIT_H
