/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef GLOB_PATH_PATTERN_H
#define GLOB_PATH_PATTERN_H

#include <vector>
#include <fileuty.h>

/**
 * @brief The glob_path_pattern struct
 *  use POSIX file name expansion to get all matched files by pattern
 */
struct glob_path_pattern : std::vector<fileuty> {
    glob_path_pattern(std::string pattern);
};

#endif // GLOB_PATH_PATTERN_H
