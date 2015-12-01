/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#ifndef RANGE_H
#define RANGE_H

#include <string>
#include <stdexcept>
#include <vector>

/**
 * @brief The RANGE struct
 *  in text position of nested elements
 */
struct RANGE {

    /**
     * @brief pos
     *  a position in string buffer
     */
    typedef std::string::size_type pos;

    /**
     * @brief begin, end
     *  range delimiters
     */
    pos begin = std::string::npos,
        end = std::string::npos;

    /**
     * @brief type
     *  optionally qualify content
     */
    int type = 0;

    /**
     * @brief nesting
     *  keep inner structures
     */
    typedef std::vector<RANGE> nesting_t;
    nesting_t nesting;

    /**
     * @brief operator []
     * @param p
     *  index 0 based of element in nesting
     * @return
     *  nested element at position @param p
     */
    const RANGE &operator[](size_t p) const;

    typedef std::vector<const RANGE *> path_t;

    /**
     * @brief validate
     *  apply sanity check
     * @param parents
     *  keep to perform error reporting
     */
    void validate(path_t& parents) const;

    /**
     * @brief contains
     * @param p
     * @return true if p is contained in range
     */
    bool contains(pos p) const {
        return begin <= p && p < end;
    }

    /**
     * @brief size
     *  note we cannot have empty ranges
     * @return
     *  text dimension
     */
    pos size() const {
        if (end <= begin)
            error();
        return end - begin;
    }

    /**
     * @brief text
     * @return the text found in range from @p s
     */
    std::string text(const std::string &s) const;

    /**
     * @brief operator ()
     *  shorthand for @m text
     */
    std::string operator()(const std::string& s) const {
        return text(s);
    }

    /**
     * @brief describe
     * @return
     *  range and type formatted
     */
    std::string describe() const;

    /**
     * @brief describe
     *  display type,range limits and a subset (up to @param ellipsis) of @param s
     */
    std::string describe(const std::string &s, int ellipsis = 8) const;

    /**
     * @brief error
     *  report invalid range data
     */
    static void error(std::string msg = "invalid range") { throw std::out_of_range("internal error:" + msg); }

    /**
     * @brief operator <<
     *  append @p r to nesting
     */
    RANGE& operator<<(RANGE r) { nesting.push_back(r); return *this; }

    /**
     * @brief operator bool
     *  check if not empty - should be, after initialization
     */
    operator bool() const { return end > begin; }
};

#endif // RANGE_H
