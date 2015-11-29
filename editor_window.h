/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/builder.h>
#include <gtkmm/statusbar.h>

#include <map>
#include <string>
#include <vector>

#include "parse_conf.h"
#include "view_ast.h"

using namespace std;
using namespace Glib;
using namespace Gtk;

typedef vector<string> strings_t;
typedef RANGE::pos CURSOR;
typedef vector<Widget*> parents_t;
typedef map<string, Widget*> named_parents_t;

/**
 * @brief The editor_window class
 *  application main window
 */
class editor_window : public Gtk::Window
{
public:

    /**
     * @brief setup
     *  get resource from editor.glade (Builder format),
     *  build the main toplevel interface, must be named as default:
     * 'window1'. Associate default .conf files to editors.
     * @return
     *  instantiated window1
     */
    static editor_window* setup(RefPtr<Application> app);

    /**
     * @brief editor_window
     *  constructor required by Builder to instantiate from editor.glade
     * @param cobject
     *  gtkmm provided typedef
     * @param refBuilder
     *  actual resource loader
     */
    editor_window(BaseObjectType *cobject, const RefPtr<Builder>& refBuilder);

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
    void action_status(string action, bool status);

    /**
     * @brief status_message
     *  display a formatted message
     */
    void status_message(string msg);

protected:

    RefPtr<Application> app;

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
     * @brief get_notebook,get_statusbar
     *  these calls resolve the *hardcoded* application structure
     */
    Notebook* get_notebook();
    Statusbar* get_statusbar();

    /**
     * @brief route_to_view
     *  redispatch action' signal to active view
     * @param sig
     *  action' signal by name
     */
    void route_to_view(string sig);

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
    view_ast *find_view(string conf);

    /**
     * @brief file_view_buf
     *  borrowed from Prolog, where pattern matching was a good deal
     *  to access structured data, but useless now
     */
    typedef tuple<string, Gsv::View*, RefPtr<Gsv::Buffer>> file_view_buf;
    file_view_buf current_file_view_buf();

    /**
     * @brief conf_editable
     *  name all editable .conf files
     * @return
     *  a list of foreseen symbols, without extension or folder
     */
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

    /**
     * @brief on_add_block
     *  insert a block based on user selection from template
     */
    void on_add_block();
};

#endif // EDITOR_WINDOW_H
