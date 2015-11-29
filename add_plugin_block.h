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
#include "view_ast.h"

/**
 * @brief The add_plugin_block class
 *  insert a block from template
 */
class add_plugin_block : public Gtk::Dialog
{
public:
    add_plugin_block(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);
    view_ast *editor = 0;
};

#endif // ADD_PLUGIN_BLOCK_H
