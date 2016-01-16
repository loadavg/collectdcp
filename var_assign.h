/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef VAR_ASSIGN_H
#define VAR_ASSIGN_H

#include "trim.h"

/**
 * @brief The var_assign struct
 *  parse line assignment resulting from (for instance) $ cat /etc/os-release
 */
struct var_assign {

    var_assign(std::string line, char k = '=', char q = '"') {
        auto p = line.find(k);
        if (p != std::string::npos) {
            key = trim(line.substr(0, p));
            value = trim(line.substr(p + 1));
            value_is_quoted = value.size() > 2 && value.front() == q && value.back() == q;
            if (value_is_quoted)
                value = value.substr(1, value.size() - 2);
        }
    }

    std::string key;
    std::string value;
    bool value_is_quoted = false;

    operator bool() const { return key.size(); }
};

#endif // VAR_ASSIGN_H
