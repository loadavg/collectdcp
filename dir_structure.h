/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef DIR_STRUCTURE_H
#define DIR_STRUCTURE_H

#include <vector>
#include "fileuty.h"

struct dir_structure : std::vector<fileuty> {
    dir_structure(std::string root);
};

#endif // DIR_STRUCTURE_H
