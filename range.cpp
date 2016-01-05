/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "range.h"
#include "prints.h"
#include "join.h"
#include <iterator>
#include <algorithm>

using namespace std;

string RANGE::describe() const {
    return prints("%d, [%d .. %d)", type, begin, end);
}

string RANGE::describe(const string &s, int ellipsis) const {
    return describe() + " {" + text(s).substr(0, ellipsis) + "}";
}

string RANGE::text(const string &s) const {
    if (begin >= s.size() || end > s.size())
        error("string out of bounds " + describe(s));
    return s.substr(begin, size());
}

void RANGE::validate(path_t &v) const {

    auto cat = [](path_t &v) {
        vector<string> p;
        transform(v.begin(), v.end(), inserter(p, p.end()), [](const RANGE *r){ return r->describe(); });
        return join(p);
    };

    if (end < begin)
        error("end < begin " + describe() + " {" + cat(v) + "}");

    v.push_back(this);

    for (size_t i = 0; i < nesting.size(); ++i) {
        auto &x = nesting[i];
        x.validate(v);
        if (!contains(x.begin) || x.end > end)
            error("invalid nesting " + describe() + " with " + x.describe() + " {" + cat(v) + "}");
        if (i > 0) {
            auto &y = nesting[i - 1];
            if (y.end > x.begin)
                error("overlapping " + y.describe() + " and " + x.describe() + " {" + cat(v) + "}");
        }
    }

    v.pop_back();
}

const RANGE &RANGE::operator[](size_t p) const {
    if (p >= nesting.size())
        error("bad nesting reference " + describe());
    return nesting[p];
}
