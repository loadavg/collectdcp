/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "is_a.h"
#include "join.h"
#include "icompare.h"
#include "depth_first.h"
#include "ui_structure.h"
#include "plugin_to_store.h"
#include "add_plugin_block.h"

#include <gtkmm/dialog.h>
#include <gtkmm/treeview.h>

#include <map>
#include <iostream>

#include "ns_all.h"

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

void add_plugin_block::set_editor(view_ast *editor) {
    this->editor = editor;
    if (treestore) {

        tem_map = new plugins_t(editor->ast_template);
        in_view = new plugins_t(editor->get_AST());

        plugin_to_store p2s {tem_map->ast->text, treestore};

        for (auto e: *tem_map)
            p2s.add_plugin(*e.second.back(), *in_view);
    }
}
