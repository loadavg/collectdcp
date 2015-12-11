/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#include "model.h"
#include "depth_first.h"
#include "file2string.h"
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>

namespace model {

using namespace parse_conf;

static string root = "/etc/collectd";
void set_root(string r) {

    struct stat info;
    if (stat(r.c_str(), &info) != 0)
        throw invalid_argument(r + ":cannot access");
    else if (0 == (info.st_mode & S_IFDIR))
        throw invalid_argument(r + ":not a directory");

    root = r;
}
string base_folder() {
    //return root.empty() ? "/etc/collectd" /* "../collectd" */ : root;
    return root; // "../collectd";
}

string conf_file(string conf) {
    if (conf == "collectd")
        return base_folder()+"/"+conf+".conf";
    return base_folder()+"/collectd.conf.d/"+conf+".conf";
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

}

