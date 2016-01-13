/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef APP_WINDOW_H
#define APP_WINDOW_H

#include <gtkmm/grid.h>
#include <gtkmm/entry.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/notebook.h>
#include <gtkmm/textview.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/searchentry.h>
#include <gtkmm/scrolledwindow.h>

/**
 * @brief The app_window class
 */
class app_window : public Gtk::Window {

public:

    /// setup layout
    app_window();

protected:

    /// this application is basically a set of configuration pages
    Gtk::Notebook notebook;

    /// first page - server info
    Gtk::Grid server_info; // layout
    Gtk::Entry sudo_password;
    Gtk::Button start;
    Gtk::Label status;
    Gtk::ScrolledWindow log_frame;
    Gtk::TextView log_view;

    /// plugins selection page + plugin info
    Gtk::SearchEntry locate_entry;
    Gtk::ScrolledWindow host_plugins;
    Gtk::TreeView plugins_config;
};

#endif // APP_WINDOW_H
