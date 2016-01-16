/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "ns_all.h"
#include "settings.h"
#include "test_glade.h"
#include "plugin_form.h"
#include "file2string.h"
#include "ui_structure.h"

#include <gtkmm/window.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/application.h>

void test_load(int argc, char **argv) {
    auto app = Application::create(argc, argv, "org.loadavg.collectd.test");
    auto b = get_resource("collectdcp");
    auto w = instance_widget<Window>(b, "window1");
    w->show_all();
    app->run(*w, argc, argv);
    exit(0);
}

struct collectdcp : Gtk::Window {
    collectdcp(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
        : Gtk::Window(cobject) {
        statusbar = instance_widget<Gtk::Statusbar>(refBuilder, "statusbar1");
    }
    Gtk::Statusbar *statusbar;
};

void test_plugin_form(int argc, char **argv) {
    auto app = Application::create(argc, argv, "org.loadavg.collectd.test");
    try {
        settings s;
        cout << s.servers.begin()->first << endl;
        auto text = file2string(get_resource_path("plugins", "template"));
        auto ast_template = new AST(text);
        //auto b = get_resource("test_glade");
        //auto w = instance_widget<Window>(b, "collectdcp");
        //w->show_all();
        instance_ui_object<collectdcp>("test_glade", "collectdcp", [&](collectdcp *p) {
            return app->run(*p, argc, argv);
        } );
        delete ast_template;
    } catch (exception &e) {
        message_box(e.what());
    } catch (Glib::Exception &e) {
        message_box(e.what());
    }

    exit(0);
}

void test_glade(int argc, char **argv) {
    test_plugin_form(argc, argv);
}
