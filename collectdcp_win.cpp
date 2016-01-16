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
    try {
        plugins_defaults = new AST(text);
    } catch (exception &e) {
        message_box(e.what());
    }

    ScrolledWindow *sw = 0;
    refBuilder->get_widget("scrolledwindow1", sw);
    if (sw) {
        RefPtr<Builder> builder;
        if (get_resource("generated/main", builder)) {
            Grid* grid_main = 0;
            builder->get_widget("grid_main", grid_main);
            if (grid_main) {
                sw->add(*grid_main);
                return;
            }
        }

        auto b = get_resource("add_plugin_treeview");
        auto ts = RefPtr<TreeStore>::cast_dynamic(b->get_object("add_plugin_treestore"));
        TreeView *tv;
        //if (treestore) {

        if (1) {
            Grid* grid_main = 0;
            builder->get_widget("grid_main", grid_main);
            if (grid_main) {
                sw->add(*grid_main);
                return;
            }
        }
    }

    message_box("something went wrong\nin application setup");
}
