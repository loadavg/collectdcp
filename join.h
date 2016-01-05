/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef JOIN_H
#define JOIN_H

#include <string>
#include <functional>

template<class C>
inline std::string join(const C& c, std::string sep) {
    std::string ret;
    for (size_t i = 0; i < c.size(); ++i) {
        if (i > 0)
            ret += sep;
        ret += c[i];
    }
    return ret;
}

template<class C>
inline std::string join(const C& c, char sepc = ',') { return join(c, std::string(1, sepc)); }

#endif // JOIN_H
