/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#include "add_plugin_block.h"
#include "ui_structure.h"
#include <gtkmm/dialog.h>
#include <gtkmm/treeview.h>

#include "join.h"
#include "icompare.h"
#include "depth_first.h"

#include <map>
#include <iostream>

add_plugin_block::add_plugin_block(Gtk::Dialog::BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &refBuilder)
    : Gtk::Dialog(cobject)
{
    treestore = Glib::RefPtr<Gtk::TreeStore>::cast_dynamic(refBuilder->get_object("add_plugin_treestore"));
}

namespace implementation {

using namespace std;
using namespace model;
using namespace parse_conf;

typedef const string &kstring;

/*
typedef RANGE::path_t path;
typedef map<path, const RANGE *> block_indexing_t;
typedef vector<string> strings;

inline string path_image(const path &p, kstring s) {
    strings S;
    for (auto r: p)
        if (r->type == XML_LIKE_t) {
            S.push_back((*r)[HEAD_l][HTAG_l](s));
        }
    return join(S, '/');
}
struct cmp_nesting_paths : less<path> {
    cmp_nesting_paths(const AST &ast) : ast(ast) {}
    const AST &ast;
    bool operator()(const path& lhs, const path& rhs) const {
        return icompare(path_image(lhs, ast.text), path_image(rhs, ast.text)) < 0;
    }
};
struct df_visit : path {
    df_visit(const model::AST *ast) {
        push_back(&ast->elements);
    }
    operator bool() const { return !empty(); }
    const RANGE *top() const { return back(); }
    void operator++() {
        const RANGE *t = top();
        pop_back();
        for (auto i = t->nesting.rbegin(); i != t->nesting.rend(); ++i)
            push_back(&*i);
    }
};
int visit(path& buffer, const RANGE &node, function<void(const RANGE &top)> on_node) {
    int c = 1;
    on_node(node);
    buffer.push_back(&node);
    for (auto &child: node.nesting)
        c += visit(buffer, child, on_node);
    buffer.pop_back();
    return c;
}
*/

inline string plugin_id(const RANGE &r, kstring t) {
    if (r.type == XML_LIKE_t)
        if (0 == icompare("plugin", r[HEAD_l][HTAG_l](t)))
            if (r[HEAD_l][HARGS_l].nesting.size())
                return r[HEAD_l][HARGS_l][0](t);
    return string();
}

struct plugins_t : std::map<string, RANGE::path_t, iless> {
    const AST *ast;
    plugins_t(const AST *ast) : ast(ast) {
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
};

struct plugin_to_store {

    typedef Gtk::TreeStore Model;
    typedef Model::iterator Node;

    kstring text;
    Glib::RefPtr<Model> treestore;

    void add_plugin(const RANGE &r, const plugins_t &in_view) {
        Node i = treestore->append();
        auto pid = plugin_id(r, text);
        i->set_value(0, pid);
        i->set_value(1, in_view.find(pid) != in_view.end());
        append(r, i);
    }

    void append(const RANGE &s, Node parent) {
        g_assert(s.type == XML_LIKE_t);
        for (auto e: s[BODY_l].nesting) {
            if (e.type == XML_LIKE_t) {
                Node c = treestore->append(parent->children());
                c->set_value(0, e[HEAD_l][HTAG_l](text));
                append(e, c);
            }
            if (e.type == KEY_VALUES_t) {
                Node c = treestore->append(parent->children());
                c->set_value(0, e[KEY_l](text));
                vector<string> args;
                for (auto a: e[VALUES_l].nesting)
                    args.push_back(a(text));
                c->set_value(3, join(args, ','));
            }
        }
    }

};

}

void add_plugin_block::set_editor(view_ast *editor) {
    this->editor = editor;
    if (treestore) {
        using namespace implementation;

        plugins_t tem_map(editor->ast_template);
        plugins_t in_view(editor->get_AST());
        plugin_to_store p2s {tem_map.ast->text, treestore};

        for (auto e: tem_map)
            p2s.add_plugin(*e.second.back(), in_view);
    }
}