/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef EQUAL_RANGE_H
#define EQUAL_RANGE_H

#include <map>

/**
 *  iterator for multimap::equal_range
 */
template <class mmap> struct equal_range_it {
    typedef std::pair<typename mmap::iterator, typename mmap::iterator> iter;
    typedef typename mmap::mapped_type value_type;

    iter it;

    equal_range_it(iter it) : it(it) {}
    operator bool() const { return it.first != it.second; }
    void operator++() { ++it.first; }
    const value_type& value() const { return it.first->second; }
    const value_type& operator*() const { return it.first->second; }
};

template <class mmap>
inline equal_range_it<mmap> equal_range(mmap &m, typename mmap::key_type key) {
    return equal_range_it<mmap>(m.equal_range(key));
}

#endif // EQUAL_RANGE_H
