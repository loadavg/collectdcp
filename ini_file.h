/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef INI_FILE_H
#define INI_FILE_H

#include <fstream>
#include <utility>

/**
 * @brief The ini_file struct
 *  CentOS 7 have GTKMM_MINOR_VERSION == 8, missing save_to_file in Glib::KeyFile
 */
struct ini_file {
    ini_file(std::string path) : file(path) {}

    std::string section(std::string section) {
        file << '[' << section << ']' << std::endl;
        return section;
    }

    template <typename V>
    void entry(std::string k, V&& v) {
        file << k << '=' << v << std::endl;
    }

    std::ofstream file;
};

#endif // INI_FILE_H
