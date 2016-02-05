/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "join.h"
#include "ns_all.h"
#include "ast_loader.h"
#include "file2string.h"
#include "message_box.h"
#include "ui_structure.h"
#include "dir_structure.h"
#include "collectdcp_win.h"
#include "plugin_to_store.h"
#include "glob_path_pattern.h"

#include <gtkmm/grid.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/searchentry.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/treeselection.h>
#include <gtkmm/scrolledwindow.h>

#include <iostream>

collectdcp_win* collectdcp_win::setup(RefPtr<Application> app) {
    auto w = instance_derived<collectdcp_win>("collectdcp_win", "collectdcp_win");
    w->app = app;
    return w;
}

void ast_to_grid(const AST *ast, Grid *g) {
    entries_t entries(ast);

    for (int nrows = 0; ; ++nrows) {
        auto c = g->get_child_at(1, nrows);
        if (c == 0)
            break;

        auto n = c->get_name();
        auto p = entries.find(n);
        if (p != entries.end()) {
            if (auto en = dynamic_cast<Entry*>(c)) {
                auto r = *p->second.back();
                if (r.type == KEY_VALUES_t) {
                    string vs = r[VALUES_l](ast->text);
                    en->set_text(vs);
                }
                continue;
            }
            /*
            if (auto cbt = dynamic_cast<ComboBoxText*>(c)) {
                continue;
            }
            if (auto yn = dynamic_cast<CheckButton*>(c)) {
                continue;
            }*/
        }
    }
}

collectdcp_win::collectdcp_win(BaseObjectType *cobject, const RefPtr<Builder>& refBuilder)
    : Window(cobject)
{
    notebook = instance_widget<Notebook>(refBuilder, "notebook1");
    //main_config = ast_loader(model::entry_symbol()).ast;

    auto text = file2string(get_resource_path("plugins", "template"));
    plugins_defaults = new AST(text);

    CATCH_SHOW([this] { load_css(); });

    auto sw = instance_widget<ScrolledWindow>(refBuilder, "scrolledwindow1");
    auto grid_main = instance_widget<Grid>(get_resource("generated/main"), "grid_main");
    sw->add(*grid_main);

    //ast_to_grid(main_config, grid_main);

    auto b = get_resource("add_plugin_treeview");
    //auto ts = RefPtr<TreeStore>::cast_dynamic(b->get_object("add_plugin_treestore"));

    plugins_view = instance_widget<TreeView>(b, "add_plugin_treeview");
    plugins_view->signal_cursor_changed().connect(sigc::mem_fun(this, &collectdcp_win::on_cursor_changed));

    auto host_plugins_tv = instance_widget<ScrolledWindow>(refBuilder, "host_plugins_tv");
    plugins_view->reparent(*host_plugins_tv);

    /*
    auto tem_map = new plugins_t(plugins_defaults);
    auto in_view = new plugins_t(main_config);

    plugin_to_store p2s {plugins_defaults->text, ts};
    for (auto e: *tem_map)
        p2s.add_plugin(*e.second.back(), *in_view);
    */

    host_plugin_prop = instance_widget<ScrolledWindow>(refBuilder, "host_plugin_prop");
    plugin_description = instance_widget<TextView>(refBuilder, "plugin_description");

    handle_includes();
}

inline bool commented(const RANGE::path_t &p) {
    for (auto e : p)
        if (e->type == COMMENT_t)
            return true;
    return false;
}

void collectdcp_win::handle_includes() {
    auto sym = model::entry_symbol();
    auto v = add_conf_file(sym, string());
    entries_t main(v->get_AST());   // find_view not available yet

    for (auto e = main.equal_range("include"); e.first != e.second; ++e.first) {
        entries_t::const_iterator i = e.first;
        const RANGE::path_t &p = i->second;
        if (!commented(p)) {
            const RANGE &r = *p.back();
            // handle_include_statement(r);
            if (r.type == KEY_VALUES_t) {
                auto path = r[VALUES_l](main.ast->text);
                fileuty type(path);
                if (type) {
                    if (type.is_dir()) {
                        for (auto p: dir_structure(path)) {
                            add_conf_file(string(), p);
                        }
                    }
                }
                else {
                    for (auto p : glob_path_pattern(path)) {
                        add_conf_file(string(), p.path());
                    }
                }
            }
            else if (r.type == XML_LIKE_t) {
                auto args = r[HEAD_l][HARGS_l];
                for (auto a : args.nesting) {
                    //auto folder_or_file = token(a, main_config).text();
                    if (a.type == KEY_VALUES_t) {
                        auto pattern = a[VALUES_l](main.ast->text);
                        for (auto p : glob_path_pattern(pattern)) {
                            add_conf_file(string(), p.path());
                        }
                    }
                }
                //auto folder_or_file = (main_config->text);
            }

            //string x = r(main_config->text);
            //cout << x.text() << endl;
        }
    }
}

void collectdcp_win::on_cursor_changed() {
    auto sel = plugins_view->get_selection();
    auto it = sel->get_selected();
    string p;
    it->get_value(0, p);

    if (!it->parent()) {
        host_plugin_prop->remove();
        plugin_description->get_buffer()->set_text("");
        CATCH_SHOW([&]() {
            auto builder = get_resource("generated/" + p);

            auto grid = instance_widget<Grid>(builder, p);
            host_plugin_prop->add(*grid);

            auto desc = instance_widget<Label>(builder, "label_" + p);
            plugin_description->get_buffer()->set_text(desc->get_text());
        });
    }
}

void collectdcp_win::load_css() {
    auto css = CssProvider::create();
    auto path = get_resource_path("button", "css");
    if (!css->load_from_path(path))
        throw runtime_error("Failed to load " + path);

    auto screen = Gdk::Screen::get_default();
    auto ctx = get_style_context();
    ctx->add_provider_for_screen(screen, css, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

view_ast* collectdcp_win::add_conf_file(string symbol, string path) {
    if (symbol.empty()) {
        fileuty f(path);
        if (f) {
            if (f.is_dir())
                throw invalid_argument(path);

            auto filename = [](string path) {
                auto ll = path.find_last_of('/');
                if (ll != string::npos && ll + 1 < path.size())
                    return path.substr(ll + 1);
                return path;
            };
            auto filename_base = [filename](string path) {
                auto ll = path.find_last_of('/');
                if (ll != string::npos && ll < path.size()) {
                    path = path.substr(ll + 1);
                }
                auto file = filename(path);
                auto pp = file.find_last_of('.');
                if (pp != string::npos && pp < file.size())
                    return file.substr(0, string::npos - 1);
                return file;
            };
            symbol = filename_base(f.path());
        }
        else
            throw invalid_argument(path);
    }
    auto v = new view_ast(symbol, path);
    auto w = new ScrolledWindow;
    w->add(*v);
    auto l = new Label(symbol);
    notebook->append_page(*w, *l);
    return v;
}

collectdcp_win::t_ast_views_sym collectdcp_win::views_by_symbol() const {
    t_ast_views_sym m;
    return m;
}

///// from editor_window
/////

/**
 * @brief collectdcp_win::on_delete_event
 *  check for dirties before quitting application
 * @return
 *  acknowledge quitting if no dirties
 */
bool collectdcp_win::on_delete_event(GdkEventAny *)
{
    return !check_quit();
}

/**
 * @brief collectdcp_win::on_timer
 *  idle time processing polling: OK for update UI actions availability
 * @return
 */
bool collectdcp_win::on_timer()
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

bool collectdcp_win::on_idle()
{
    return true;
}

/**
 * @brief get_main_iitem
 *  these calls resolve the *hardcoded* application structure
 */
Widget* collectdcp_win::get_main_iitem(int index) {
    auto c0 = get_children();
    auto c1 = is_a<Container>(c0[0])->get_children();
    return c1[index];
}

Notebook* collectdcp_win::get_notebook() {
    return is_a<Notebook>(get_main_iitem(2));
}
Statusbar* collectdcp_win::get_statusbar() {
    return is_a<Statusbar>(get_main_iitem(3));
}
Toolbar* collectdcp_win::get_toolbar() {
    return is_a<Toolbar>(get_main_iitem(1));
}

/**
 * @brief status_message
 *  display a formatted message
 */
void collectdcp_win::status_message(string msg) {
    Statusbar *sb = get_statusbar();
    sb->remove_all_messages();
    sb->push(msg);
}

view_ast *collectdcp_win::current_view() {
    if (auto nb = get_notebook())
        if (auto sc = is_a<Container>(nb->get_nth_page(nb->get_current_page()))) {
            auto ch = sc->get_children();
            if (!ch.empty())
                return is_a<view_ast>(ch[0]);
        }
    return 0;
}

view_ast *collectdcp_win::find_view(string conf) {
    if (auto nb = get_notebook())
        for (int p = 0; p < nb->get_n_pages(); ++p)
            if (auto sc = is_a<Container>(nb->get_nth_page(p)))
                if (auto v = is_a<view_ast>(sc->get_children()[0]))
                    if (v->conf == conf)
                        return v;
    return 0;
}

collectdcp_win::file_view_buf collectdcp_win::current_file_view_buf() {
    auto va = current_view();
    auto sb = va->buffer();
    return file_view_buf(va->label()->get_text(), va, sb);
}

void collectdcp_win::on_file_save() {
    for (auto c: conf_editable()) {
        view_ast *v = find_view(c);
        if (v->is_dirty())
            v->save();
    }
}

void collectdcp_win::route_to_view(string sig) {
    g_signal_emit_by_name(current_view()->gobj(), sig.c_str());
}

void collectdcp_win::on_edit_copy() {
    route_to_view("copy-clipboard");
}
void collectdcp_win::on_edit_cut() {
    route_to_view("cut-clipboard");
}
void collectdcp_win::on_edit_paste() {
    route_to_view("paste-clipboard");
}
void collectdcp_win::on_edit_delete() {
    g_signal_emit_by_name(current_view()->gobj(), "delete-from-cursor", GTK_DELETE_CHARS, gint(1));
}

void collectdcp_win::on_edit_undo() {
    route_to_view("undo");
}
void collectdcp_win::on_edit_redo() {
    route_to_view("redo");
}

void collectdcp_win::on_disable_block() {
    view_ast *v = current_view();
    if (v && v->disable_block(true))
        status_message(prints("block disabled on %s", v->conf.c_str()));
    else
        status_message("cannot disable block");
}
void collectdcp_win::on_enable_block() {
    view_ast *v = current_view();
    if (v && v->enable_block(true))
        status_message(prints("block enabled on %s", v->conf.c_str()));
    else
        status_message("cannot enable block");
}

const collectdcp_win::strings_t& collectdcp_win::conf_editable() {
    static strings_t e = {"collectd", "filters", "thresholds"};
    return e;
}

void collectdcp_win::on_help_about() {
    instance_ui_widget("about_box", "aboutdialog1");
}

bool collectdcp_win::check_quit() {
    strings_t dirties;
    for (auto cf : conf_editable())
        if (auto av = find_view(cf))
            if (av->is_dirty())
                dirties.push_back(cf);
    return dirties.empty() ||
        message_box(prints("[%s] not saved, close anyway ?", join(dirties).c_str()), YES_NO);
}

void collectdcp_win::on_file_quit() {
    if (check_quit()) {
        hide();
        app->quit();
    }
}

void collectdcp_win::on_commands() {
/*
    instance_ui_object<dlg_commands>("dlg_commands", "dlg_commands", [&](dlg_commands* dia) {
        if (dia->password)
            dia->password->set_text(password()->get_text());
        return run_dialog(dia);
    });
*/
}

void collectdcp_win::action_status(string action, bool status) {
    if (auto mn = locate_by_name(this, "mn_"+action))
        mn->set_sensitive(status);
    if (auto tb = locate_by_name(this, "tb_"+action))
        tb->set_sensitive(status);
}

Entry* collectdcp_win::password() {
    return is_a<Entry>(locate_by_name(get_toolbar(), "te_password"));
}

void collectdcp_win::on_search() {
    if (view_ast *v = current_view()) {
        string search = is_a<SearchEntry>(ui_structure::locate_by_name(this, "te_search"))->get_text();
        v->on_search(search);
    }
}

/////
///// end from editor_window
