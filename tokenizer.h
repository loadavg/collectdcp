/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "range.h"

#include <cctype>
#include <stdexcept>
using namespace std;

/**
 * @brief The tokenizer struct
 *  basic utilities to scan a string buffer
 */
struct tokenizer {

    typedef string::const_iterator iter;

    iter curr, stop;
    const string& text;

    tokenizer(const string& s)
        : curr(s.begin()), stop(s.end()), text(s) {
    }

    /**
     * @brief cc
     *  access current character
     */
    char cc() const { return *curr; }

    /**
     * @brief cc
     *  test current character
     */
    bool cc(char c) const { return cc() == c; }

    /**
     * @brief isblank,csymf,csym,digit,octal,eof,eol
     *  categorize current character
     */
    bool isblank() const { return cc('\r') || std::isblank(cc()); }
    bool csymf() const { return cc('_') || isalpha(cc()); }
    bool csym() const { return cc('_') || isalnum(cc()); }
    bool digit() const { return isdigit(cc()); }
    bool octal() const { return '0' <= cc() && cc() <= '7' ; }
    bool eof() const { return curr == stop; }
    bool eol() const { return cc('\n'); }
    bool skipline() {
        //
        for ( ; ; ) {
            skip();
            if (pc('#'))
                continue;
            if (pc('\n'))
                return true;
            return false;
        }
        //
        skip();
        return pc('\n');
    }

    /**
     * @brief next
     *  advance to next character
     */
    char next() { if (eof()) throw out_of_range("read past EOF");  return *++curr; }
    void skip() { while (isblank()) next(); }
    char nc() { skip(); return cc(); }

    bool pc(char c) { if (cc(c)) { next(); return true; } return false; }
    bool nc(char c) { skip(); return pc(c); }

    /**
     * @brief symbol
     *  a C like symbol sequence
     * @param r
     *  delimiting range
     * @return
     *  true if symbol matched
     */
    bool symbol(RANGE &r) {
        if (csymf()) {
            begin(r);
            next();
            while (csym())
                next();
            return end(r);
        }
        return false;
    }

    /**
     * @brief offset_t
     *  we are interested in tokens' offsets
     */
    typedef string::size_type offset_t;
    offset_t off() const { return curr - text.begin(); }

    /**
     * @brief line
     *  return 0-based line index of @param p
     * @param p
     *  offset in string to get line position
     */
    unsigned line(offset_t p) const {
        unsigned l = 0;
        offset_t x = text.find_first_of('\n');
        while (x != string::npos && x < p) {
            ++l;
            x = text.find_first_of('\n', x + 1);
        }
        return l;
    }

    /**
     * @brief line
     *  return 0-based line index of @m curr
     */
    unsigned line() const { return line(off()); }

    /**
     * @brief line_offset
     *  return 0-based offset of line containing @m curr
     */
    unsigned line_offset() const {
        offset_t x = text.find_first_of('\n');
        while (x != string::npos && x < off())
            x = text.find_first_of('\n', x + 1);
        return x;
    }

    /**
     * @brief column
     *  return 0-based offset wrt @m line begin
     */
    unsigned column() const {
        return off() - line_offset();
    }

    /**
     * @brief begin
     *  check current, if match mark range start point
     */
    bool begin(char c, RANGE &r) {
        skip();
        if (cc(c)) {
            r.begin = off();
            next();
            return true;
        }
        return false;
    }
    void begin(RANGE &r) const { r.begin = off(); }
    RANGE begin(int type) const { RANGE r; r.begin = r.end = off(); r.type = type; return r; }
    bool end(RANGE &r) const { r.end = off(); return true; }

    RANGE& fixup(RANGE &r, int type) const { r.type = type; r.end = off(); return r; }

    static void empty(RANGE &r) { r.begin = r.end = 0; }

    /**
     * @brief The lookahead struct
     *  enable analysis and restore if not matching occurs
     *  if not committed restores tokenizer to entry point
     */
    struct lookahead {
        lookahead(tokenizer *p) : p(p), save(p->curr) { }
        ~lookahead() {
            if (p)
                p->curr = save;
        }
        bool commit() { p = 0; return true; }
        //bool commit(RANGE &r) { auto q = p; p = 0; return q->end(r); }
        RANGE& commit(RANGE &r) { p->end(r); p = 0; return r; }

        tokenizer *p;
        iter save;
    };
};

#endif // TOKENIZER_H
