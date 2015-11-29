/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#include "add_plugin_block.h"
#include "ui_structure.h"
#include <gtkmm/dialog.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treeview.h>

add_plugin_block::add_plugin_block(Gtk::Dialog::BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &refBuilder)
    : Gtk::Dialog(cobject)
{
    if (auto ts = Glib::RefPtr<Gtk::TreeStore>::cast_dynamic(refBuilder->get_object("add_plugin_treestore"))) {

    }
}
