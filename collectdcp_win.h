/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef COLLECTDCP_WIN_H
#define COLLECTDCP_WIN_H

#include <gtkmm/window.h>
#include <gtkmm/builder.h>
#include <gtkmm/treeview.h>
#include <gtkmm/textview.h>
#include <gtkmm/scrolledwindow.h>

#include "model.h"

/**
 * @brief The collectdcp_win class
 *  redesigned control panel for whole application
 */
class collectdcp_win : public Gtk::Window
{
public:

    /**
     * @brief setup
     *  instantiate the derived widget
     * @param app
     *  gtk application reference
     * @return
     *  instantiated window
     */
    static collectdcp_win* setup(Glib::RefPtr<Gtk::Application> app);

    /**
     * @brief collectdcp_win
     *  as required by Gtk subclassing when using Gtk::Builder
     * @param cobject
     *  Gtk base object
     * @param refBuilder
     *  resource loader
     */
    collectdcp_win(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);

protected:
    //void on_row_activated(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column);
    void on_cursor_changed();

private:
    Glib::RefPtr<Gtk::Application> app;

    model::AST *main_config = 0;
    model::AST *plugins_defaults = 0;

    Gtk::TreeView *plugins_view = 0;
    Gtk::ScrolledWindow *host_plugin_prop = 0;
    Gtk::TextView *plugin_description = 0;
};

#endif // COLLECTDCP_WIN_H
