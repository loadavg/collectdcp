/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "join.h"
#include "prints.h"
#include "icompare.h"
#include "tokenizer.h"
#include "parse_conf.h"

#include <ostream>
#include <algorithm>

#include <glibmm.h>

namespace parse_conf {

struct AST::parse : tokenizer {

    /**
     * @brief parse
     *  constructor cache parameters
     * @param s
     *  text to parse
     * @param path
     *  filename for error reporting
     */
    parse(const string& s, const string& path)
        : tokenizer(s), comment(false), path(path) {
    }

    typedef vector<string> errors_t;
    errors_t errors;

    /**
     * @brief element
     *  main parse entry point
     * @return
     */
    RANGE element() {
        bool in_comment = comment;

        skip();
        RANGE elem;

        try {
            if (cc('#')) {
                if (!in_comment) {
                    comment = true;
                    elem = begin(COMMENT_t);
                }
                next();
            }

            if (RANGE x = xml()) {
                if (elem.type == COMMENT_t)
                    end(elem << x);
                else
                    elem = x;
            }
            else if (RANGE kv = key_value()) {
                if (elem.type == COMMENT_t)
                    end(elem << kv);
                else
                    elem = kv;
            }
            else {
                if (!elem.type)
                    elem = begin(SKIP_t);
                while (!eol() && !eof())
                    next();
                if (eol())
                    next();
                end(elem);
            }
        }
        catch(exception &e) {
            if (find(errors.begin(), errors.end(), e.what()) == errors.end())
                errors.push_back(e.what());
            elem = begin(ERROR_t);
            while (!eol() && !eof())
                next();
            if (eol())
                next();
            end(elem);
        }

        if (!in_comment)
            comment = false;

        return elem;
    }

private:

    /**
     * @brief comment
     *  attempt to parse valid structure behind commented lines
     */
    bool comment;

    /**
     * @brief path
     *  error reporting: keep filename around
     */
    string path;

    RANGE xml() {
        lookahead l(this);
        skip();
        RANGE x = begin(XML_LIKE_t);
        if (RANGE h = open_tag()) {
            RANGE body = begin(BODY_l);
            unsigned cline = line() + 1;
            string o = h[HTAG_l](text);
            for ( ; ; ) {
                end(body);
                if (RANGE c = close_tag(o))
                    return l.commit(x << h << body << c);

                skip();
                if (eof())
                    error(prints("block '%s' from line %d unclosed", o.c_str(), cline));

                body << element();
            }
        }
        return RANGE();
    }
    RANGE open_tag() {
        RANGE open, name;
        if (begin('<', open)) {
            if (name = tag()) {
                RANGE arg, args = begin(HARGS_l);
                for ( ; ; ) {
                    skip();
                    if (eof())
                        error("unterminated header");
                    if (pc('>')) {
                        if (!skipline())
                            error("section header not line");
                        end(args);
                        return fixup(open << name << args, _VALUES_);
                    }
                    if (value(arg))
                        args << arg;
                    else
                        error("not a value");
                }
            }
        }
        return RANGE();
    }
    RANGE close_tag(string o) {
        lookahead l(this);
        skip();
        if (comment) {
            if (!pc('#'))
                return RANGE();
            skip();
        }
        if (nc('<') && pc('/'))
            if (RANGE name = tag()) {
                end(name);
                string c = name(text);
                if (icompare(o, c))
                    error(prints("tags '%s','%s' mismatch", o.c_str(), c.c_str()));
                if (pc('>')) {
                    if (!skipline())
                        error("section tag close not line");
                    l.commit();
                    return name;
                }
            }
        return RANGE();
    }

    RANGE key_value() {
        lookahead l(this);
        skip();
        RANGE kv = begin(KEY_VALUES_t);
        if (RANGE k = key()) {
            skip();
            RANGE vs = begin(VALUES_l), v;
            while (value(v)) {
                vs << v;
                skip();
                if (eol()) {
                    next();
                    end(vs);
                    return l.commit(kv << k << vs);
                }
            }
        }
        return RANGE();
    }

    RANGE key() { RANGE r; return symbol(r) ? r : RANGE(); }
    bool value(RANGE &v) {
        if (quoted(v)) {
            v.type = quoted_v;
            return true;
        }
        if (symbol(v)) {
            v.type = unquoted_v;
            return true;
        }
        return v = number();
    }

    RANGE tag() { RANGE r; return symbol(r) ? r : RANGE(); }

    RANGE number() {
        if (digit()) {
            RANGE r;
            begin(r);
            if (pc('0')) {
                if (octal()) {
                    do next();
                    while (octal());
                    return fixup(r, octals_v);
                }
            }
            while (digit())
                next();
            if (pc('.')) {
                while (digit())
                    next();
                return fixup(r, floating_v);
            }
            return fixup(r, integer_v);
        }
        return RANGE();
    }

    bool quoted(RANGE &r) {
        return delimited('\'', r) || delimited('"', r);
    }
    bool delimited(char delim, RANGE &r) {
        skip();
        if (cc(delim)) {
            begin(r);
            bool esc = false;
            while (!eof()) {
                char n = next();
                if (n == delim) {
                    if (!esc) {
                        next();
                        return end(r);
                    }
                    esc = false;
                }
                else if (n == '\\') {
                    esc = !esc;
                }
                else if (n == '\n') {
                    // handle multiline strings (escaped eol)
                    if (!esc)
                        error(prints("string starting at %d not closed", line(r.begin) + 1));
                    esc = false;
                }
                else
                    esc = false;
            }
            error("string not closed");
        }
        return false;
    }

    /**
     * @brief error
     *  throw on first error, report current file position in line/column offset
     */
    void error(string msg) {
        string y = prints("file %s, line %d: error %s",
                          path.c_str(), line() + 1, msg.c_str());
        throw invalid_argument(y);
    }
};

static string dump_desc(const RANGE &r, const string& t) {
    switch (r.type) {
    case FILE_t:
        return "FILE";
    case XML_LIKE_t:
        return "XML " + r[HEAD_l][HTAG_l](t);
    case KEY_VALUES_t:
        return "KV " + r[KEY_l](t);
    case COMMENT_t:
        return "comm";
    default:
        return "";
    }
}
static void dump_r(const RANGE &r, ostream &s, int l, const string& t) {
    s << string(l, ' ') << r.type << ' ' << r.begin << ' ' << r.end << ' ' << dump_desc(r, t) << endl;
    for (auto &x: r.nesting)
        dump_r(x, s, l + 1, t);
}

void AST::dump(ostream &s, int indent) const {
    dump_r(elements, s, indent, text);
}

/**
 * @brief AST::AST
 *  parsing entry point
 * @param text
 *  .conf file content
 * @param path
 *  file path for error reporting
 */
AST::AST(string text, string path, strings* errors) : text(text) {
    parse p(text, path);
    elements = p.begin(FILE_t);
    while (!p.eof())
        if (RANGE e = p.element())
            elements << e;
    p.end(elements);

    if (p.errors.size()) {
        if (errors)
            *errors = p.errors;
        else
            throw invalid_argument(format_errors(p.errors));
    }
}
string AST::format_errors(strings errors, size_t ellipsis) {
    if (errors.size() > ellipsis) {
        errors.resize(ellipsis);
        return prints("problems found:\n%s\n...", join(errors, '\n').c_str());
    }
    return prints("problems found:\n%s", join(errors, '\n').c_str());
}

string unquote(const RANGE &r, kstring t) {
    string v = r(t);
    if (r.type == quoted_v)
        return v.substr(1, v.length() - 2);
    return v;
}

string plugin_id(const RANGE &r, kstring t) {
    if (r.type == XML_LIKE_t)
        if (0 == icompare("plugin", r[HEAD_l][HTAG_l](t))) {
            auto &h = r[HEAD_l][HARGS_l];
            if (h)
                return unquote(h[0], t);
            g_assert(h.nesting.empty());
        }
    return string();
}

}
