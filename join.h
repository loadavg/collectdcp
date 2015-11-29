/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#ifndef JOIN_H
#define JOIN_H

#include <string>
#include <functional>

template<class C>
inline std::string join(const C& c, char sepc = ',') {
    std::string j;
    for (size_t i = 0; i < c.size(); ++i) {
        if (i > 0)
            j += sepc;
        j += c[i];
    }
    return j;
}


#endif // JOIN_H

