/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#include "test_glade.h"
#include "plugin_form.h"
#include "ui_structure.h"

#include <gtkmm/window.h>
#include <gtkmm/application.h>

#include "ns_all.h"
#include "file2string.h"

using namespace ns_all;

void test_load(int argc, char **argv) {
    auto app = Application::create("org.loadavg.collectd.test");
    auto b = get_resource("collectdcp");
    auto w = instance_widget<Gtk::Window>(b, "window1");
    w->show_all();
    app->run(*w, argc, argv);
    exit(0);
}

void test_plugin_form(int argc, char **argv) {
    auto app = Application::create("org.loadavg.collectd.test");

    auto text = file2string(get_resource_path("plugins", "template"));
    auto ast_template = new AST(text);
    try {
        ast_template->dump(cout);
        delete ast_template;
    } catch (exception &e) {
        message_box(e.what());
    }
    auto b = ui_structure::get_resource("collectdcp");
    auto w = ui_structure::instance_widget<Gtk::Window>(b, "window1");
    w->show_all();
    app->run(*w, argc, argv);
    exit(0);
}

void test_glade(int argc, char **argv) {
    test_plugin_form(argc, argv);
}
