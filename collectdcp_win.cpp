/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "collectdcp_win.h"
#include "ui_structure.h"
#include "message_box.h"
#include "file2string.h"

#include "plugin_to_store.h"
#include "ns_all.h"

#include <gtkmm/grid.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treeselection.h>

#include <iostream>

collectdcp_win* collectdcp_win::setup(RefPtr<Application> app) {
    collectdcp_win* window = 0;
    RefPtr<Builder> builder;
    if (get_resource("collectdcp", builder)) {
        builder->get_widget_derived("collectdcp_window", window);
        if (window)
            window->app = app;
    }
    return window;
}

collectdcp_win::collectdcp_win(BaseObjectType *cobject, const RefPtr<Builder>& refBuilder)
    : Window(cobject)
{
    auto text = file2string(get_resource_path("plugins", "template"));
    plugins_defaults = new AST(text);

    ScrolledWindow *sw = 0;
    refBuilder->get_widget("scrolledwindow1", sw);
    if (!sw)
        throw runtime_error("missing scrolledwindow1");

    RefPtr<Builder> builder;
    if (!get_resource("generated/main", builder))
        throw runtime_error("missing generated/main");
    Grid* grid_main = 0;
    builder->get_widget("grid_main", grid_main);
    if (!grid_main)
        throw runtime_error("missing grid_main");
    sw->add(*grid_main);

    auto b = get_resource("add_plugin_treeview");
    auto ts = RefPtr<TreeStore>::cast_dynamic(b->get_object("add_plugin_treestore"));

    plugins_view = instance_widget<TreeView>(b, "add_plugin_treeview");
    //tv->signal_row_activated().connect(sigc::mem_fun(this, &collectdcp_win::on_row_activated));
    plugins_view->signal_cursor_changed().connect(sigc::mem_fun(this, &collectdcp_win::on_cursor_changed));

    auto host_plugins_tv = instance_widget<ScrolledWindow>(refBuilder, "host_plugins_tv");
    plugins_view->reparent(*host_plugins_tv);

    auto tem_map = new plugins_t(plugins_defaults);
    auto in_view = new plugins_t(plugins_defaults);

    plugin_to_store p2s {plugins_defaults->text, ts};
    for (auto e: *tem_map)
        p2s.add_plugin(*e.second.back(), *in_view);

    host_plugin_prop = instance_widget<ScrolledWindow>(refBuilder, "host_plugin_prop");
    plugin_description = instance_widget<Label>(refBuilder, "plugin_description");
}
void collectdcp_win::on_row_activated(const TreeModel::Path& , TreeViewColumn* ) {
    cout << "on_row_activated" << endl;
}
void collectdcp_win::on_cursor_changed() {
    auto sel = plugins_view->get_selection();
    auto it = sel->get_selected(); //add_destroy_notify_callback();
    string p;
    it->get_value(0, p);
    cout << "on_cursor_changed " << p << endl;
    if (!it->parent()) {
        host_plugin_prop->remove();
        RefPtr<Builder> builder;
        if (get_resource("generated/" + p, builder)) {
            Grid* grid = 0;
            builder->get_widget(p, grid);
            if (grid)
                host_plugin_prop->add(*grid);
            Label *desc = 0;
            builder->get_widget("label_" + p, desc);
            if (desc)
                plugin_description->set_text(desc->get_text());
        }
        else
            message_box("no resource available for plugin " + p);
    }
}
