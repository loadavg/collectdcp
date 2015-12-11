/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#include "join.h"
#include "prints.h"
#include "message_box.h"
#include "dlg_commands.h"
#include "ui_structure.h"
#include "editor_window.h"
#include "add_plugin_block.h"

#include <iostream>
#include <gtkmm/toolbutton.h>
#include <gtkmm/searchentry.h>
#include <gtkmm/aboutdialog.h>
#include <gtkmm/scrolledwindow.h>
#include <glibmm.h>

using namespace std;
using namespace Glib;
using namespace Gtk;
using namespace ui_structure;

editor_window* editor_window::setup(RefPtr<Application> app) {
    editor_window* window = 0;
    RefPtr<Builder> builder;
    if (get_resource("editor", builder)) {
        builder->get_widget_derived("window1", window);
        if (window)
            window->app = app;
    }
    return window;
}

editor_window::editor_window(BaseObjectType *cobject, const RefPtr<Builder>& builder)
    : Window(cobject)
{
    // install idle time handler
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &editor_window::on_timer), 100);

    auto menuitem_prepare = [&](string action, void(editor_window::*callback)(), bool sensitive = true) {
        MenuItem* mn = 0;
        builder->get_widget(action, mn);
        if (mn) {
            mn->signal_activate().connect(sigc::mem_fun(*this, callback));
            mn->set_sensitive(sensitive);
        }
    };
    auto toolitem_prepare = [&](string action, void(editor_window::*callback)(), bool sensitive = true) {
        ToolButton *tb = 0;
        builder->get_widget(action, tb);
        if (tb) {
            tb->signal_clicked().connect(sigc::mem_fun(*this, callback));
            tb->set_sensitive(sensitive);
        }
    };

    auto action_prepare = [&](string action, void (editor_window::*callback)(), bool sensitive = true) {
        menuitem_prepare("mn_" + action, callback, sensitive);
        toolitem_prepare("tb_" + action, callback, sensitive);
    };

    action_prepare("file_save", &editor_window::on_file_save);

    action_prepare("edit_cut", &editor_window::on_edit_cut);
    action_prepare("edit_copy", &editor_window::on_edit_copy);
    action_prepare("edit_paste", &editor_window::on_edit_paste);
    action_prepare("edit_delete", &editor_window::on_edit_delete);
    action_prepare("edit_undo", &editor_window::on_edit_undo);
    action_prepare("edit_redo", &editor_window::on_edit_redo);

    action_prepare("add_block", &editor_window::on_add_block);
    action_prepare("disable_block", &editor_window::on_disable_block);
    action_prepare("enable_block", &editor_window::on_enable_block);

    action_prepare("commands", &editor_window::on_commands);

    menuitem_prepare("mn_help_about", &editor_window::on_help_about);
    menuitem_prepare("mn_file_quit", &editor_window::on_file_quit);

    for (auto e: conf_editable()) {
        ScrolledWindow *s = 0;
        builder->get_widget(e, s);
        if (s) {
            if (auto v = new view_ast(e)) {
                s->add(*v);
                continue;
            }
        }
    }

    // web scrapped template based insertion
    find_view("collectd")->set_template("plugins");

    is_a<SearchEntry>(ui_structure::locate_by_name(this, "te_search"))->
            signal_activate().connect(sigc::mem_fun(*this, &editor_window::on_search));
}

/**
 * @brief editor_window::on_delete_event
 *  check for dirties before quitting application
 * @return
 *  acknowledge quitting if no dirties
 */
bool editor_window::on_delete_event(GdkEventAny *)
{
    return !check_quit();
}

/**
 * @brief editor_window::on_timer
 *  idle time processing polling: OK for update UI actions availability
 * @return
 */
bool editor_window::on_timer()
{
    if (auto w = current_view()) {
        action_status("enable_block", w->enable_block(false));
        action_status("disable_block", w->disable_block(false));
        action_status("file_save", w->is_dirty());

#ifdef USE_SOURCEVIEW
        RefPtr<const Gsv::UndoManager> um = w->get_source_buffer()->get_undo_manager();
        action_status("edit_undo", um->can_undo());
        action_status("edit_redo", um->can_redo());
#endif

        action_status("add_block", w->ast_template != 0);
    }

    return true;
}

bool editor_window::on_idle()
{
    return true;
}

/**
 * @brief get_main_iitem
 *  these calls resolve the *hardcoded* application structure
 */
Widget* editor_window::get_main_iitem(int index) {
    auto c0 = get_children();
    auto c1 = is_a<Container>(c0[0])->get_children();
    return c1[index];
}

Notebook* editor_window::get_notebook() {
    return is_a<Notebook>(get_main_iitem(2));
}
Statusbar* editor_window::get_statusbar() {
    return is_a<Statusbar>(get_main_iitem(3));
}
Toolbar* editor_window::get_toolbar() {
    return is_a<Toolbar>(get_main_iitem(1));
}

/**
 * @brief status_message
 *  display a formatted message
 */
void editor_window::status_message(string msg) {
    Statusbar *sb = get_statusbar();
    sb->remove_all_messages();
    sb->push(msg);
}

view_ast *editor_window::current_view() {
    if (auto nb = get_notebook())
        if (auto sc = is_a<Container>(nb->get_nth_page(nb->get_current_page()))) {
            auto ch = sc->get_children();
            if (!ch.empty())
                return is_a<view_ast>(ch[0]);
        }
    return 0;
}

view_ast *editor_window::find_view(string conf) {
    if (auto nb = get_notebook())
        for (int p = 0; p < nb->get_n_pages(); ++p)
            if (auto sc = is_a<Container>(nb->get_nth_page(p)))
                if (auto v = is_a<view_ast>(sc->get_children()[0]))
                    if (v->conf == conf)
                        return v;
    return 0;
}

editor_window::file_view_buf editor_window::current_file_view_buf() {
    auto va = current_view();
    auto sb = va->buffer();
    return file_view_buf(va->label()->get_text(), va, sb);
}

void editor_window::on_file_save() {
    for (auto c: conf_editable()) {
        view_ast *v = find_view(c);
        if (v->is_dirty())
            v->save();
    }
}

void editor_window::route_to_view(string sig) {
    g_signal_emit_by_name(current_view()->gobj(), sig.c_str());
}

void editor_window::on_edit_copy() {
    route_to_view("copy-clipboard");
}
void editor_window::on_edit_cut() {
    route_to_view("cut-clipboard");
}
void editor_window::on_edit_paste() {
    route_to_view("paste-clipboard");
}
void editor_window::on_edit_delete() {
    g_signal_emit_by_name(current_view()->gobj(), "delete-from-cursor", GTK_DELETE_CHARS, gint(1));
}

void editor_window::on_edit_undo() {
    route_to_view("undo");
}
void editor_window::on_edit_redo() {
    route_to_view("redo");
}

void editor_window::on_disable_block() {
    view_ast *v = current_view();
    if (v && v->disable_block(true))
        status_message(prints("block disabled on %s", v->conf.c_str()));
    else
        status_message("cannot disable block");
}
void editor_window::on_enable_block() {
    view_ast *v = current_view();
    if (v && v->enable_block(true))
        status_message(prints("block enabled on %s", v->conf.c_str()));
    else
        status_message("cannot enable block");
}

const editor_window::strings_t& editor_window::conf_editable() {
    static strings_t e = {"collectd", "filters", "thresholds"};
    return e;
}

void editor_window::on_help_about() {
    instance_ui_widget("about_box", "aboutdialog1");
}

bool editor_window::check_quit() {
    strings_t dirties;
    for (auto cf : conf_editable())
        if (auto av = find_view(cf))
            if (av->is_dirty())
                dirties.push_back(cf);
    return dirties.empty() ||
        message_box(prints("[%s] not saved, close anyway ?", join(dirties).c_str()), YES_NO);
}

void editor_window::on_file_quit() {
    if (check_quit()) {
        hide();
        app->quit();
    }
}

void editor_window::on_commands() {
    instance_ui_object<dlg_commands>("dlg_commands", "dlg_commands", [&](dlg_commands* dia) {
        if (dia->password)
            dia->password->set_text(password()->get_text());
        return run_dialog(dia);
    });
}

void editor_window::action_status(string action, bool status) {
    if (auto mn = locate_by_name(this, "mn_"+action))
        mn->set_sensitive(status);
    if (auto tb = locate_by_name(this, "tb_"+action))
        tb->set_sensitive(status);
}

void editor_window::on_add_block() {
    instance_ui_object<add_plugin_block>(
                "add_plugin_treeview", "add_plugin_block", [&](add_plugin_block *dialog) {
        dialog->set_editor(current_view());
        return run_dialog(dialog);
    });
}

Entry* editor_window::password() {
    return is_a<Entry>(locate_by_name(get_toolbar(), "te_password"));
}

void editor_window::on_search() {
    if (view_ast *v = current_view()) {
        string search = is_a<SearchEntry>(ui_structure::locate_by_name(this, "te_search"))->get_text();
        v->on_search(search);
    }
}
