/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "trim.h"
#include "join.h"
#include "ns_all.h"
#include "os_info.h"
#include "ast_loader.h"
#include "process_run.h"
#include "file2string.h"
#include "message_box.h"
#include "dlg_commands.h"
#include "ui_structure.h"
#include "dir_structure.h"
#include "collectdcp_app.h"
#include "plugin_to_store.h"
#include "glob_path_pattern.h"

#include <glibmm.h>
#include <gtkmm/treestore.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/searchentry.h>
#include <gtkmm/treeselection.h>

#include <iostream>

collectdcp_app* collectdcp_app::setup(RefPtr<Application> app) {
    auto w = instance_derived<collectdcp_app>("collectdcp", "collectdcp");
    w->app = app;
    return w;
}

collectdcp_app::collectdcp_app(BaseObjectType *cobject, const RefPtr<Builder>& refBuilder)
    : Window(cobject)
{
    //CATCH_SHOW([this] { load_css(); });

    notebook = instance_widget<Notebook>(refBuilder, "notebook1");

    auto text = file2string(get_resource_path("plugins", "template"));
    plugins_defaults = new AST(text);

    if (auto sw = instance_widget<ScrolledWindow>(refBuilder, "scrolledwindow1"))
        if ((global_options = instance_widget<Grid>(get_resource("generated/main"), "grid_main")) != 0)
            sw->add(*global_options);

    host_plugin_prop = instance_widget<ScrolledWindow>(refBuilder, "host_plugin_prop");
    plugin_description = instance_widget<TextView>(refBuilder, "plugin_description");

    logging = instance_widget<TextView>(refBuilder, "logging");

    if (auto ts = instance_widget<SearchEntry>(refBuilder, "te_search"))
        ts->signal_activate().connect(sigc::mem_fun(*this, &collectdcp_app::on_search));

    handle_includes();

    setup_actions(refBuilder);
    setup_plugins_treeview(refBuilder);
    setup_system_interface(refBuilder);

    if (auto v = find_view(model::entry_symbol()))
        if (global_options)
            ast_to_grid(v->get_AST(), dynamic_cast<Grid*>(global_options));

    log_message("done setup");
}

void collectdcp_app::setup_system_interface(const RefPtr<Builder>& refBuilder) {
    start = instance_widget<Button>(refBuilder, "btn_start");
    stop = instance_widget<Button>(refBuilder, "btn_stop");

    password = instance_widget<Entry>(refBuilder, "sudo_password");

    start->signal_clicked().connect(sigc::mem_fun(this, &collectdcp_app::on_start));
    stop->signal_clicked().connect(sigc::mem_fun(this, &collectdcp_app::on_stop));
    schedule_status_check();

    auto info = os_info::info();
    if (info->is_ubuntu()) {
        cmd_start = "/etc/init.d/collectd start";
        cmd_stop = "/etc/init.d/collectd stop";
        cmd_status = "service collectd status";
        cmd_status_running = "* collectd is running";
    }
    else if (info->is_centos()) {
        cmd_start = "systemctl start collectd";
        cmd_stop = "systemctl stop collectd";
        cmd_status = "systemctl status collectd";
        cmd_status_running = "Active: active (running)";
    }
    else
        message_box(prints("unknown system '%s'", info->ID.c_str()));
}

void collectdcp_app::log_message(std::string msg) {
    if (logging) {
        auto b = logging->get_buffer();
        b->insert(b->end(), msg + "\n");
    }
}

void collectdcp_app::setup_plugins_treeview(const RefPtr<Builder>& refBuilder) {
    if (auto v = find_view(model::entry_symbol())) {
        auto ts = RefPtr<TreeStore>::cast_dynamic(refBuilder->get_object("plugin_treestore"));
        plugins_view = instance_widget<TreeView>(refBuilder, "plugin_treeview");
        plugins_view->signal_cursor_changed().connect(sigc::mem_fun(this, &collectdcp_app::on_cursor_changed));

        //auto host_plugins_tv = instance_widget<ScrolledWindow>(refBuilder, "host_plugins_tv");
        //plugins_view->reparent(*host_plugins_tv);

        auto tem_map = new plugins_t(plugins_defaults);
        auto in_view = new plugins_t(v->get_AST());

        plugin_to_store p2s {plugins_defaults->text, ts};
        for (auto e: *tem_map)
            p2s.add_plugin(*e.second.back(), *in_view);
    }
}

void collectdcp_app::setup_actions(const RefPtr<Builder>& builder) {

    // install idle time handler
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &collectdcp_app::on_timer), 100);

    auto menuitem_prepare = [&](string action, void(collectdcp_app::*callback)(), bool sensitive = true) {
        MenuItem* mn = 0;
        builder->get_widget(action, mn);
        if (mn) {
            mn->signal_activate().connect(sigc::mem_fun(*this, callback));
            mn->set_sensitive(sensitive);
        }
    };
    auto toolitem_prepare = [&](string action, void(collectdcp_app::*callback)(), bool sensitive = true) {
        ToolButton *tb = 0;
        builder->get_widget(action, tb);
        if (tb) {
            tb->signal_clicked().connect(sigc::mem_fun(*this, callback));
            tb->set_sensitive(sensitive);
        }
    };

    auto action_prepare = [&](string action, void (collectdcp_app::*callback)(), bool sensitive = true) {
        menuitem_prepare("mn_" + action, callback, sensitive);
        toolitem_prepare("tb_" + action, callback, sensitive);
    };

    action_prepare("file_save", &collectdcp_app::on_file_save);

    action_prepare("edit_cut", &collectdcp_app::on_edit_cut);
    action_prepare("edit_copy", &collectdcp_app::on_edit_copy);
    action_prepare("edit_paste", &collectdcp_app::on_edit_paste);
    action_prepare("edit_delete", &collectdcp_app::on_edit_delete);
    action_prepare("edit_undo", &collectdcp_app::on_edit_undo);
    action_prepare("edit_redo", &collectdcp_app::on_edit_redo);

    //action_prepare("add_block", &collectdcp_app::on_add_block);
    action_prepare("disable_block", &collectdcp_app::on_disable_block);
    action_prepare("enable_block", &collectdcp_app::on_enable_block);

    action_prepare("commands", &collectdcp_app::on_commands);

    menuitem_prepare("mn_help_about", &collectdcp_app::on_help_about);
    menuitem_prepare("mn_file_quit", &collectdcp_app::on_file_quit);
}

inline bool commented(const RANGE::path_t &p) {
    for (auto e : p)
        if (e->type == COMMENT_t)
            return true;
    return false;
}

void collectdcp_app::handle_includes() {
    auto sym = model::entry_symbol();
    auto file = model::conf_file(sym);
    auto v = add_conf_file(sym, file);
    entries_t main(v->get_AST());   // find_view not available yet

    for (auto e = main.equal_range("include"); e.first != e.second; ++e.first) {
        entries_t::const_iterator i = e.first;
        const RANGE::path_t &p = i->second;
//for (auto x : p)
    //cout << (*x)(main.ast->text) << endl;

        if (commented(p))
            continue;

        const RANGE &r = *p.back();
        if (r.type == KEY_VALUES_t) {
            auto path = r[VALUES_l](main.ast->text);
            fileuty type(path);
            if (type) {
                if (type.is_dir())
                    for (auto p: dir_structure(path))
                        add_conf_file(string(), p.path());
            }
            else for (auto p : glob_path_pattern(path))
                if (p.is_file())
                    add_conf_file(string(), p.path());
        }
        else if (r.type == XML_LIKE_t) {
            auto dir = parse_conf::unquote(r[HEAD_l][HARGS_l][0], main.ast->text);
            auto args = r[BODY_l];
            for (auto a : args.nesting)
                if (a.type == KEY_VALUES_t) {
                    auto pattern = parse_conf::unquote(a[VALUES_l], main.ast->text);
                    auto ppath = dir + '/' + pattern;
                    for (auto p : glob_path_pattern(ppath))
                        if (p.is_file()) {
                            if (p.path() != file)
                                add_conf_file(string(), p.path());
                        }
                        else if (p.is_dir())
                            for (auto n: dir_structure(p.path()))
                                add_conf_file(string(), n.path());
                }
        }
    }
}

void collectdcp_app::on_cursor_changed() {
    auto sel = plugins_view->get_selection();
    auto it = sel->get_selected();
    string p;
    //it->get_value(0, p);

    TreeIter root = it;
    while (root->parent())
        root = root->parent();
    root->get_value(0, p);

    /*if (!it->parent())*/ {
        plugin_options = 0;
        host_plugin_prop->remove();
        plugin_description->get_buffer()->set_text("");
        CATCH_SHOW([&]() {
            auto builder = get_resource("generated/" + p);

            auto grid = instance_widget<Grid>(builder, p);
            host_plugin_prop->add(*grid);

            auto desc = instance_widget<Label>(builder, "label_" + p);
            plugin_description->get_buffer()->set_text(desc->get_text());

            if (auto v = find_view(model::entry_symbol())) {
                ast_to_grid(v->get_AST(), grid);
                plugin_options = grid;
                //plugin_options->set_name(p);
            }
        });
    }
}

void collectdcp_app::load_css() {
    auto css = CssProvider::create();
    auto path = get_resource_path("button", "css");
    if (!css->load_from_path(path))
        throw runtime_error("Failed to load " + path);

    auto screen = Gdk::Screen::get_default();
    auto ctx = get_style_context();
    ctx->add_provider_for_screen(screen, css, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

view_ast* collectdcp_app::add_conf_file(string symbol, string path) {
    if (symbol.empty()) {
        if (!fileuty(path).is_file())
            throw invalid_argument("not a file:" + path);
        symbol = filename_base(path);
    }
    auto v = new view_ast(symbol, path);
    auto w = new ScrolledWindow;
    w->add(*v);
    auto l = new Label(symbol);
    notebook->append_page(*w, *l);
    return v;
}

///// from collectdcp_app
/////

/**
 * @brief collectdcp_app::on_delete_event
 *  check for dirties before quitting application
 * @return
 *  acknowledge quitting if no dirties
 */
bool collectdcp_app::on_delete_event(GdkEventAny *)
{
    return !check_quit();
}

/**
 * @brief collectdcp_app::on_timer
 *  idle time processing polling: OK for update UI actions availability
 * @return
 */
bool collectdcp_app::on_timer()
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

bool collectdcp_app::on_idle()
{
    return true;
}

/**
 * @brief get_main_iitem
 *  these calls resolve the *hardcoded* application structure
 */
Widget* collectdcp_app::get_main_iitem(int index) {
    auto c0 = get_children();
    auto c1 = is_a<Container>(c0[0])->get_children();
    return c1[index];
}

Notebook* collectdcp_app::get_notebook() {
    return is_a<Notebook>(get_main_iitem(2));
}
Statusbar* collectdcp_app::get_statusbar() {
    return is_a<Statusbar>(get_main_iitem(3));
}
Toolbar* collectdcp_app::get_toolbar() {
    return is_a<Toolbar>(get_main_iitem(1));
}

/**
 * @brief status_message
 *  display a formatted message
 */
void collectdcp_app::status_message(string msg) {
    Statusbar *sb = get_statusbar();
    sb->remove_all_messages();
    sb->push(msg);
}

view_ast *collectdcp_app::current_view() {
    if (auto nb = get_notebook())
        if (auto sc = is_a<Container>(nb->get_nth_page(nb->get_current_page()))) {
            auto ch = sc->get_children();
            if (!ch.empty())
                return is_a<view_ast>(ch[0]);
        }
    return 0;
}

view_ast *collectdcp_app::find_view(string conf) {
    if (auto nb = get_notebook())
        for (int p = 0; p < nb->get_n_pages(); ++p)
            if (auto sc = is_a<Container>(nb->get_nth_page(p)))
                if (auto v = is_a<view_ast>(sc->get_children()[0]))
                    if (v->conf == conf)
                        return v;
    return 0;
}

void collectdcp_app::on_file_save() {
    for (auto c: conf_editable()) {
        view_ast *v = find_view(c);
        if (v->is_dirty())
            v->save();
    }
}

void collectdcp_app::route_to_view(string sig) {
    if (auto v = current_view())
        g_signal_emit_by_name(v->gobj(), sig.c_str());
}

void collectdcp_app::on_edit_copy() {
    route_to_view("copy-clipboard");
}
void collectdcp_app::on_edit_cut() {
    route_to_view("cut-clipboard");
}
void collectdcp_app::on_edit_paste() {
    route_to_view("paste-clipboard");
}
void collectdcp_app::on_edit_delete() {
    g_signal_emit_by_name(current_view()->gobj(), "delete-from-cursor", GTK_DELETE_CHARS, gint(1));
}

void collectdcp_app::on_edit_undo() {
    route_to_view("undo");
}
void collectdcp_app::on_edit_redo() {
    route_to_view("redo");
}

void collectdcp_app::on_disable_block() {
    view_ast *v = current_view();
    if (v && v->disable_block(true))
        status_message(prints("block disabled on %s", v->conf.c_str()));
    else
        status_message("cannot disable block");
}
void collectdcp_app::on_enable_block() {
    view_ast *v = current_view();
    if (v && v->enable_block(true))
        status_message(prints("block enabled on %s", v->conf.c_str()));
    else
        status_message("cannot enable block");
}

collectdcp_app::strings_t collectdcp_app::conf_editable() {
    strings_t e;
    if (auto nb = get_notebook())
        for (int p = 0; p < nb->get_n_pages(); ++p)
            if (auto sc = is_a<Container>(nb->get_nth_page(p)))
                if (auto v = is_a<view_ast>(sc->get_children()[0]))
                    e.push_back(v->conf);
    return e;
}

void collectdcp_app::on_help_about() {
    instance_ui_widget("about_box", "aboutdialog1");
}

bool collectdcp_app::check_quit() {
    strings_t dirties;
    for (auto cf : conf_editable())
        if (auto av = find_view(cf))
            if (av->is_dirty())
                dirties.push_back(cf);
    return dirties.empty() ||
        message_box(prints("[%s] not saved, close anyway ?", join(dirties).c_str()), YES_NO);
}

void collectdcp_app::on_file_quit() {
    if (check_quit()) {
        hide();
        app->quit();
    }
}

void collectdcp_app::on_commands() {
    instance_ui_object<dlg_commands>("dlg_commands", "dlg_commands", [&](dlg_commands* dia) {
        if (dia->password)
            dia->password->set_text(get_password()->get_text());
        return run_dialog(dia);
    });
}

void collectdcp_app::action_status(string action, bool status) {
    if (auto mn = locate_by_name(this, "mn_"+action))
        mn->set_sensitive(status);
    if (auto tb = locate_by_name(this, "tb_"+action))
        tb->set_sensitive(status);
}

Entry* collectdcp_app::get_password() {
    return password; //is_a<Entry>(locate_by_name(get_toolbar(), "te_password"));
}

void collectdcp_app::on_search() {
    if (view_ast *v = current_view()) {
        string search = is_a<SearchEntry>(ui_structure::locate_by_name(this, "te_search"))->get_text();
        v->on_search(search);
    }
}

/////
///// end from collectdcp_app

void collectdcp_app::ast_to_grid(const AST *ast, Grid *g) {
    accept_changes = false;
    entries_t entries(ast);

    for (int nrows = 0; ; ++nrows) {
        auto c = g->get_child_at(1, nrows);
        if (c == 0)
            break;

        // use a shared callback to sync changes received from GUI controls
        auto fun = sigc::mem_fun(this, &collectdcp_app::on_widget_changed);

        // check control type and bind
        auto en = is_a<Entry>(c);
        if (en)
            en->signal_changed().connect(sigc::bind<Widget*>(fun, en));

        auto cbt = is_a<ComboBoxText>(c);
        if (cbt)
            cbt->signal_changed().connect(sigc::bind<Widget*>(fun, cbt));

        auto yn = is_a<CheckButton>(c);
        if (yn)
            yn->signal_clicked().connect(sigc::bind<Widget*>(fun, yn));

        // lookup by name
        auto n = c->get_name();
        auto p = entries.find(n);
        if (p != entries.end()) {
            auto r = *p->second.back();
            if (en) {
                if (r.type == KEY_VALUES_t) {
                    string vs = r[VALUES_l](ast->text);
                    en->set_text(vs);
                }
            }
            else if (cbt) {
                if (r.type == XML_LIKE_t) {
                    for (auto e : r[BODY_l].nesting) {
                        string v = e(ast->text);
                        cbt->append(v);
                    }
                }
            }
            else if (yn) {
                if (r.type == KEY_VALUES_t) {
                    for (auto e : r[VALUES_l].nesting)
                        yn->set_active(ast->get_bool(e));
                }
            }
        }
    }

    accept_changes = true;
}

/**
 * @brief collectdcp_app::service_is_running
 *  check the daemon status
 * @return
 */
/*
bool collectdcp_app::service_is_running() const {
    return start && start->is_sensitive();
}
bool collectdcp_app::stop_service() {
    if (service_is_running()) {
        start->set_sensitive(true);
        stop->set_sensitive(false);
        log_message("service stopped");
        return true;
    }
    return false;
}

bool collectdcp_app::start_service() {
    if (!service_is_running()) {
        start->set_sensitive(false);
        stop->set_sensitive(true);
        log_message("service started");
        return true;
    }
    return false;
}
*/

void collectdcp_app::on_status_check() {
    g_assert(service_status == unknown);
    /*
    switch (service_status) {
    case unknown: {
        process_run P(cmd_status);  // no password for check
        log_message(P.result);
    }   break;
    case running:
    case stopped:
    case error:
        break;
    }
    */
    CATCH_SHOW([this] {
        process_run P(cmd_status);  // no password for check
        log_message(P.result);
        string rc = trim(P.result);

        if (rc.find(cmd_status_running) != string::npos){
            start->set_sensitive(false);
            stop->set_sensitive(true);
            //log_message("collectd is running");
        }
        else {
            start->set_sensitive(true);
            stop->set_sensitive(false);
            //log_message("collectd is NOT running");
        }
    });
}

void collectdcp_app::on_start() {
    CATCH_SHOW([this] {
        process_run P(cmd_start, password->get_text());
        log_message(P.result);
        schedule_status_check();
    });
}

void collectdcp_app::on_stop() {
    CATCH_SHOW([this] {
        process_run P(cmd_stop, password->get_text());
        log_message(P.result);
        schedule_status_check();
    });
}
void collectdcp_app::schedule_status_check(int msec) {
    Glib::signal_timeout().connect_once(sigc::mem_fun(*this, &collectdcp_app::on_status_check), msec);
}

void collectdcp_app::on_widget_changed(Widget* w) {
    if (!accept_changes)
        return;

    // lookup the container of this widget
    auto g = find_parent<Grid>(w);
    if (!g)
        return;

    accept_changes = false;

    string name = w->get_name();
    cout << "on_widget_changed " << name << endl;

    int updated = 0;
    auto updater = [](view_ast *v, Widget *w, RANGE::path_t path) {
        int u = 0;
        if (auto b = is_a<CheckButton>(w))
            u += v->update_value(path, b->get_active());
        if (auto c = is_a<ComboBoxText>(w))
            u += v->update_value(path, c->get_active_text());
        if (auto e = is_a<Entry>(w))
            u += v->update_value(path, e->get_text());
        return u;
    };

    if (g == global_options) {
        if (auto v = find_view(model::entry_symbol())) {
            entries_t entries(v->get_AST());
            for (auto er = entries.equal_range(name); er.first != er.second; ++er.first) {
                updated += updater(v, w, er.first->second);
                break;
            }
        }
    }
    else for (auto ce : conf_editable()) {

        // scan the appropriate data model
        g_assert(g == plugin_options);
        string plugin = plugin_options->get_name();
        auto v = find_view(ce);
        plugins_t entries(v->get_AST());
        for (auto er = entries.equal_range(plugin); er.first != er.second; ++er.first) {
            auto path = er.first->second;
            terminals_t terminals(entries.ast, path);
            for (auto tr = terminals.equal_range(name); tr.first != tr.second; ++tr.first) {
                updated += updater(v, w, tr.first->second);
            }
        }
    }

    accept_changes = true;

    status_message(prints("updated '%s' %d times", name.c_str(), updated));
}
