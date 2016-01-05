/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "add_plugin_block.h"
#include "ui_structure.h"
#include <gtkmm/dialog.h>
#include <gtkmm/treeview.h>

#include "is_a.h"
#include "join.h"
#include "icompare.h"
#include "depth_first.h"

#include <map>
#include <iostream>

#include "ns_all.h"
using namespace ns_all;

add_plugin_block::add_plugin_block(BaseObjectType *cobject, const RefPtr<Builder> &refBuilder)
    : Dialog(cobject)
{
    treestore = RefPtr<TreeStore>::cast_dynamic(refBuilder->get_object("add_plugin_treestore"));
    if (treestore) {
        instance_widget<Button>(refBuilder, "btn_add_plugin")
            ->signal_clicked().connect(sigc::mem_fun(this, &add_plugin_block::on_add_plugin));
    }
}
add_plugin_block::~add_plugin_block() {
    delete tem_map;
    delete in_view;
}

void add_plugin_block::on_add_plugin() {
    auto treeview = is_a<TreeView>(locate_by_name(this, "add_plugin_treeview"));
    auto sel = treeview->get_selection();
    auto row = sel->get_selected();

    string v; row->get_value(c_name, v);
    bool in; row->get_value(c_used, in);
    if (!in) {
        auto i = tem_map->find(v);
        const RANGE &r = *(i->second.back());
        string p = r(tem_map->ast->text);
        editor->add_plugin(p);
    }
}

namespace implementation {

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

}

struct add_plugin_block::plugin_to_store {

    typedef TreeStore::iterator Node;

    kstring text;
    RefPtr<TreeStore> treestore;

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
                strings args;
                for (auto a: e[VALUES_l].nesting)
                    args.push_back(a(text));
                c->set_value(3, join(args, ','));
            }
        }
    }

};

void add_plugin_block::set_editor(view_ast *editor) {
    this->editor = editor;
    if (treestore) {
        using namespace implementation;

        /*
        plugins_t tem_map(editor->ast_template);
        plugins_t in_view(editor->get_AST());
        plugin_to_store p2s {tem_map.ast->text, treestore};
        for (auto e: tem_map)
            p2s.add_plugin(*e.second.back(), in_view);
        */
        tem_map = new plugins_t(editor->ast_template);
        in_view = new plugins_t(editor->get_AST());

        plugin_to_store p2s {tem_map->ast->text, treestore};

        for (auto e: *tem_map)
            p2s.add_plugin(*e.second.back(), *in_view);
    }
}
