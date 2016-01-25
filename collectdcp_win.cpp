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
    auto w = instance_derived<collectdcp_win>("collectdcp", "collectdcp_window");
    w->app = app;
    return w;
}

collectdcp_win::collectdcp_win(BaseObjectType *cobject, const RefPtr<Builder>& refBuilder)
    : Window(cobject)
{
    main_config = new AST(conf_file("collectd"));

    auto text = file2string(get_resource_path("plugins", "template"));
    plugins_defaults = new AST(text);

    auto sw = instance_widget<ScrolledWindow>(refBuilder, "scrolledwindow1");
    auto grid_main = instance_widget<Grid>(get_resource("generated/main"), "grid_main");
    sw->add(*grid_main);

    auto b = get_resource("add_plugin_treeview");
    auto ts = RefPtr<TreeStore>::cast_dynamic(b->get_object("add_plugin_treestore"));

    plugins_view = instance_widget<TreeView>(b, "add_plugin_treeview");
    plugins_view->signal_cursor_changed().connect(sigc::mem_fun(this, &collectdcp_win::on_cursor_changed));

    auto host_plugins_tv = instance_widget<ScrolledWindow>(refBuilder, "host_plugins_tv");
    plugins_view->reparent(*host_plugins_tv);

    auto tem_map = new plugins_t(plugins_defaults);
    auto in_view = new plugins_t(main_config);

    plugin_to_store p2s {plugins_defaults->text, ts};
    for (auto e: *tem_map)
        p2s.add_plugin(*e.second.back(), *in_view);

    host_plugin_prop = instance_widget<ScrolledWindow>(refBuilder, "host_plugin_prop");
    plugin_description = instance_widget<TextView>(refBuilder, "plugin_description");
}

/*
void collectdcp_win::on_row_activated(const TreeModel::Path& , TreeViewColumn* ) {
    cout << "on_row_activated" << endl;
}
*/

void collectdcp_win::on_cursor_changed() {
    auto sel = plugins_view->get_selection();
    auto it = sel->get_selected();
    string p;
    it->get_value(0, p);

    if (!it->parent()) {
        host_plugin_prop->remove();
        plugin_description->get_buffer()->set_text("");
        CATCH_SHOW([&]() {
            auto builder = get_resource("generated/" + p);

            auto grid = instance_widget<Grid>(builder, p);
            host_plugin_prop->add(*grid);

            auto desc = instance_widget<Label>(builder, "label_" + p);
            plugin_description->get_buffer()->set_text(desc->get_text());
        });
    }
}
