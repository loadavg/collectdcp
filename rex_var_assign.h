/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef REX_VAR_ASSIGN_H
#define REX_VAR_ASSIGN_H

#include <string>
#include <utility>

/**
 * @brief The rex_var_assign class
 *  parse line assignment resulting from (for instance) $ cat /etc/os-release
 */
class rex_var_assign
{
public:

    /// parse the file line
    rex_var_assign(std::string line);

    std::string key;
    std::string value;
    bool value_is_quoted;
};

#endif // REX_VAR_ASSIGN_H
