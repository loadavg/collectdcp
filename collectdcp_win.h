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
#include <gtkmm/toolbar.h>
#include <gtkmm/notebook.h>
#include <gtkmm/treeview.h>
#include <gtkmm/textview.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/scrolledwindow.h>

#include "model.h"
#include "view_ast.h"

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

    ///// from editor_window
    /////

    /**
     * @brief on_delete_event
     *  check for modified files before closing
     * @param event
     * @return
     *  acknowledge quitting from all views
     */
    virtual bool on_delete_event(GdkEventAny* event);

    /**
     * @brief action_status
     *  change status to an action - a pair of menu/toolbar commands
     *  lookup action by name, prefixing by mn_ or tb_
     * @param action
     *  action suffix name
     * @param status
     *  enabled/disabled
     */
    void action_status(std::string action, bool status);

    /**
     * @brief status_message
     *  display a formatted message
     */
    void status_message(std::string msg);

    /**
     * @brief password
     *  sudo password required to access protected resources
     * @return
     *  user interface to define password
     */
    Gtk::Entry* password();

protected:

    /**
     * @brief on_timer
     *  idle time processing polling: OK for update UI actions availability
     * @brief on_idle
     *  idle time processing, update UI actions availability
     * @return
     */
    bool on_timer();

    /**
     * @brief on_idle
     *  idle time processing, use only for compute intensive tasks
     * @return
     */
    bool on_idle();

    /**
     * @brief get_main_iitem
     *  access interface item by index
     * @param index
     *  required item
     * @return
     *  item found
     */
    Widget* get_main_iitem(int index);

    /**
     * @brief get_notebook,get_statusbar,get_toolbar
     *  these calls resolve the *hardcoded* application structure
     */
    Gtk::Notebook* get_notebook();
    Gtk::Statusbar* get_statusbar();
    Gtk::Toolbar* get_toolbar();

    /**
     * @brief route_to_view
     *  redispatch action' signal to active view
     * @param sig
     *  action' signal by name
     */
    void route_to_view(std::string sig);

    /**
     * @brief current_view
     *  get current AST view from notebook
     * @return
     *  currently visible view_ast
     */
    view_ast *current_view();

    /**
     * @brief find_view
     *  search by conf symbol
     * @param conf
     * @return
     *  @a view_ast containing @param conf, if found
     */
    view_ast *find_view(std::string conf);

    /**
     * @brief file_view_buf
     *  borrowed from Prolog, where pattern matching was a good deal
     *  to access structured data, but useless now
     */
    typedef std::tuple<std::string, Gtk::TextView*, Glib::RefPtr<Gtk::TextBuffer>> file_view_buf;
    file_view_buf current_file_view_buf();

    /**
     * @brief conf_editable
     *  name all editable .conf files
     * @return
     *  a list of foreseen symbols, without extension or folder
     */
    typedef std::vector<std::string> strings_t;
    static const strings_t& conf_editable();

    /**
     * @brief check_quit
     *  check modified files before quit
     * @return
     *  true if closeable (no dirties or user acknowledge to discard changes)
     */
    bool check_quit();

    void on_file_save();
    void on_edit_copy();
    void on_edit_cut();
    void on_edit_paste();
    void on_edit_delete();
    void on_edit_undo();
    void on_edit_redo();
    void on_disable_block();
    void on_enable_block();
    void on_help_about();
    void on_file_quit();
    void on_commands();
    void on_search();

    /////
    ///// from editor_window

protected:

    void on_cursor_changed();
    void load_css();
    void handle_includes();
    void add_conf_file(std::string symbol, std::string path);

    typedef std::map<std::string, view_ast *> t_ast_views_sym;
    t_ast_views_sym views_by_symbol() const;

    Glib::RefPtr<Gtk::Application> app;
    model::AST *plugins_defaults = 0;

    Gtk::Notebook *notebook = 0;
    Gtk::TreeView *plugins_view = 0;
    Gtk::ScrolledWindow *host_plugin_prop = 0;
    Gtk::TextView *plugin_description = 0;
};

#endif // COLLECTDCP_WIN_H
