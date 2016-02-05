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
    fileuty(std::string file) : file(file) {
        valid = stat(file.c_str(), &info) == 0;
    }
    bool is_dir() const {
        return valid && (info.st_mode & S_IFDIR) == S_IFDIR;
    }
    bool is_file() const {
        return valid && (info.st_mode & S_IFDIR) == 0;
    }
    operator bool() const { return valid; }
    std::string path() const { return file; }

private:
    std::string file;
    bool valid = false;
    struct stat info;
};

inline std::string filename(std::string path) {
    auto ll = path.find_last_of('/');
    if (ll != std::string::npos)
        return path.substr(ll + 1);
    return path;
}
inline std::string filename_ext(std::string path) {
    auto pd = path.find_last_of('.');
    if (pd != std::string::npos)
        return path.substr(pd + 1);
    return std::string();
}
inline std::string filename_dir(std::string path) {
    auto ps = path.find_last_of('/');
    if (ps != std::string::npos)
        return path.substr(0, ps - 1);
    return std::string();
}
inline std::string filename_base(std::string path) {
    auto file = filename(path);
    auto pp = file.find_last_of('.');
    if (pp != std::string::npos)
        return file.substr(0, pp - 1);
    return std::string();
}

#endif // FILEUTY_H
