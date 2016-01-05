/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef ADD_PLUGIN_BLOCK_H
#define ADD_PLUGIN_BLOCK_H

#include <gtkmm/dialog.h>
#include <gtkmm/builder.h>
#include <gtkmm/treestore.h>
#include "view_ast.h"

/**
 * @brief The add_plugin_block class
 *  insert a block from template
 */
class add_plugin_block : public Gtk::Dialog
{
public:
    add_plugin_block(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);
    ~add_plugin_block();

    /**
     * @brief set_editor
     *  associate a view (with its plugins template) for editing in this interface
     * @param editor
     *  editor requiring its content to be modified
     */
    void set_editor(view_ast *editor);

protected:

    view_ast *editor = 0;
    Glib::RefPtr<Gtk::TreeStore> treestore;

    struct plugin_to_store;
    friend struct plugin_to_store;

    model::plugins_t *tem_map = 0;
    model::plugins_t *in_view = 0;

    /**
     * @brief on_add_plugin
     *  insert selected plugin block in view editor
     */
    void on_add_plugin();

    /**
     * expected columns from .glade definition
     */
    enum { c_name, c_used, c_summary, c_args };
};

#endif // ADD_PLUGIN_BLOCK_H
