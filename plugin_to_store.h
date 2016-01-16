/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef PLUGIN_TO_STORE_H
#define PLUGIN_TO_STORE_H

#include "model.h"
#include <gtkmm/treestore.h>

/**
 * @brief The plugin_to_store struct
 *  transfer a structured configuration block (a plugin)
 *  to treestore/treeview
 */
struct plugin_to_store
{
    typedef Gtk::TreeStore::iterator Node;

    model::kstring text;
    Glib::RefPtr<Gtk::TreeStore> treestore;

    void add_plugin(const RANGE &r, const model::plugins_t &in_view);
    void append(const RANGE &s, Node parent);
};

#endif // PLUGIN_TO_STORE_H
