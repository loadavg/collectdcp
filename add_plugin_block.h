/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
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
