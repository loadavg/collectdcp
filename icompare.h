/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef ICOMPARE
#define ICOMPARE

#include <string>
#include <locale>

/*
#include <algorithm>
inline bool icompare_pred(unsigned char a, unsigned char b) {
    return std::tolower(a) == std::tolower(b);
}
inline bool icompare(std::string const& a, std::string const& b) {
    return std::lexicographical_compare(a.begin(), a.end(),
                                        b.begin(), b.end(), icompare_pred);
}
*/
inline int icompare(std::string const& a, std::string const& b) {
    std::string::const_iterator
            p = a.begin(), P = a.end(),
            q = b.begin(), Q = b.end();
    while (p != P && q != Q)
        if (int c = std::tolower(*p++) - std::tolower(*q++))
            return c;
    return p != P ? +1 : q != Q ? -1 : 0;
}
struct iless : std::less<std::string> {
    bool operator()(const std::string& lhs, const std::string& rhs) const {
        return icompare(lhs, rhs) < 0;
    }
};

#endif // ICOMPARE
