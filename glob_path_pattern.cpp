/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "glob_path_pattern.h"
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <wordexp.h>

using namespace std;

glob_path_pattern::glob_path_pattern(string pattern) {
    wordexp_t p;
    if (0 == wordexp(pattern.c_str(), &p, 0)) {
        char **w = p.we_wordv;
        for (size_t i = 0; i < p.we_wordc; i++)
            push_back(fileuty(w[i]));
        wordfree(&p);
    }
    else
        throw invalid_argument(strerror(errno));
}
