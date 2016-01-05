/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef FILEUTY_H
#define FILEUTY_H

#include <sys/types.h>
#include <sys/stat.h>
#include <string>

struct fileuty {
    fileuty(std::string file) {
        valid = stat(file.c_str(), &info) == 0;
    }
    bool is_dir() const {
        return valid && (info.st_mode & S_IFDIR) == S_IFDIR;
    }
    operator bool() const { return valid; }

private:
    bool valid = false;
    struct stat info;
};

#endif // FILEUTY_H
