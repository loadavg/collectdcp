/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "model.h"
#include "fileuty.h"
#include "depth_first.h"
#include "file2string.h"
#include <iostream>


namespace model {

using namespace parse_conf;

/**
 * @brief root
 *  adapting to different collectd  folder configuration
 */
static string root = "/etc";

static void set_root(string r) {
    fileuty f(r);
    if (!f)
        throw invalid_argument(r + ":cannot access");
    if (!f.is_dir())
        throw invalid_argument(r + ":not a directory");
    root = r;
}

void check_root(int &argc, char **argv) {
    if (argc >= 3 && string(argv[1]) == "-r") {
        set_root(argv[2]);
        for (int a = 3; a < argc; ++a)
            argv[a] = argv[a + 2];
        argc -= 2;
    }
    else {
        auto r = root + "/collectd";
        if (fileuty(r).is_dir())
            root = r;
    }
}


string base_folder() {
    return root;
}

string conf_file(string conf) {
    if (conf == "collectd")
        return base_folder() + "/" + conf + ".conf";
    return base_folder() + "/collectd.conf.d/" + conf + ".conf";
}

nesting_path_t ast_locate_cursor(const AST &ast, RANGE::pos cursor) {
    //ast.dump(cout);

    const RANGE *r = &ast.elements;
    RANGE::path_t p;
    r->validate(p);

    nesting_path_t path;
    while (r) {
        if (r->contains(cursor)) {
            path.push_back(r);
            const RANGE *n = 0;
            for (auto &t: r->nesting)
                if (t.contains(cursor)) {
                    n = &t;
                    break;
                }
            r = n;
        }
        else
            break;
    }
    return path;
}

plugins_t::plugins_t(const AST *ast) : ast(ast) {
    depth_first df;
    df.visit(ast->elements, [&](const RANGE &r) {
        if (r.type == XML_LIKE_t) {
            auto id = plugin_id(r, ast->text);
            if (!id.empty())
                ((*this)[id] = df).push_back(&r);
            return false;
        }
        return true;
    });
}

template <class S>
inline void store(depth_first &df, S &s, string k, const RANGE &r) {
    RANGE::path_t path = df;
    path.push_back(&r);
    s.emplace(make_pair(k, path));
}
template <class S>
struct indexer : depth_first {
    indexer(S* store) : store(store) {}
    void remember(string k, const RANGE &r) {
        RANGE::path_t path = *this;
        path.push_back(&r);
        store->emplace(make_pair(k, path));
    }
    S* store;
};

entries_t::entries_t(const AST *ast) : ast(ast) {
#if 0
    depth_first df;
    /*
    auto sto = [df, this](string k, const RANGE &r) {
        RANGE::path_t path = df;
        path.push_back(&r);
        emplace(make_pair(k, path));
    };
    */
    df.visit(ast->elements, [&](const RANGE &r) {
        switch (r.type) {
        case COMMENT_t: // ??
            return false;
        case XML_LIKE_t: {
            auto &h = r[HEAD_l][HTAG_l];
            auto tag = unquote(h, ast->text);
            //sto(tag, r);
            store(df, *this, tag, r);
        } break;
        case KEY_VALUES_t: {
            auto k = unquote(r[KEY_l], ast->text);
            //sto(k, r);
            store(df, *this, k, r);
        } break;

        }
        return true;
    });
#endif

    indexer<entries_t> df(this);
    /*
    auto sto = [df, this](string k, const RANGE &r) {
        RANGE::path_t path = df;
        path.push_back(&r);
        emplace(make_pair(k, path));
    };
    */
    df.visit(ast->elements, [&](const RANGE &r) {
        switch (r.type) {
        case COMMENT_t: // ??
            return false;
        case XML_LIKE_t: {
            auto &h = r[HEAD_l][HTAG_l];
            auto tag = unquote(h, ast->text);
            //sto(tag, r);
            df.remember(tag, r);
        } break;
        case KEY_VALUES_t: {
            auto k = unquote(r[KEY_l], ast->text);
            //sto(k, r);
            df.remember(k, r);
        } break;

        }
        return true;
    });
}

terminals_t::terminals_t(const AST *ast, RANGE::path_t &root) : ast(ast) {
    /*
    depth_first df;
    df.visit(*root.back(), [&](const RANGE &r) {
        switch (r.type) {
        case COMMENT_t: // ??
            return false;
        case XML_LIKE_t: {
            auto &h = r[HEAD_l][HTAG_l];
            auto tag = unquote(h, ast->text);
            store(df, *this, tag, r);
        }   break;
        case KEY_VALUES_t: {
            auto k = unquote(r[KEY_l], ast->text);
            store(df, *this, k, r);
        }   break;
        }
        return true;
    });
    */
    indexer<terminals_t> df(this);
    df.visit(*root.back(), [&](const RANGE &r) {
        switch (r.type) {
        case COMMENT_t: // ??
            return false;
        case XML_LIKE_t: {
            auto &h = r[HEAD_l][HTAG_l];
            auto tag = unquote(h, ast->text);
            df.remember(tag, r);
        }   break;
        case KEY_VALUES_t: {
            auto k = unquote(r[KEY_l], ast->text);
            df.remember(k, r);
        }   break;
        }
        return true;
    });

}

string entry_symbol() { return "collectd"; }

}
