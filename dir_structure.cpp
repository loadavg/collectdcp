/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "dir_structure.h"

#include <ftw.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>

static dir_structure *p_str = nullptr;
static int store_info(const char *fpath, const struct stat *, int , struct FTW *) {
    p_str->push_back(fileuty(fpath));
    return 0;
}

dir_structure::dir_structure(std::string root) {
    p_str = this;
    int rc = nftw(root.c_str(), store_info, 20, 0);
    p_str = nullptr;
    if (rc)
        throw std::invalid_argument(std::strerror(errno));
}
