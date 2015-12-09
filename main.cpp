/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#include "model.h"
#include "test_parse.h"
#include "editor_window.h"

#define TEST_GLADE_VERSION
#ifdef TEST_GLADE_VERSION
#include "ui_structure.h"
auto test_glade() {
    auto b = ui_structure::get_resource("collectdcp");
    auto w = ui_structure::instance_widget<Gtk::Window>(b, "window1");
    w->show_all();
    return w;
}
#endif

/**
 * @brief main
 *  application entry point
 *  for debugging purpose, first argument indicates the (fake) collectd folder
 * @param argc
 *  standard argc
 * @param argv
 *  standard argv
 * @return
 *  standard application
 */
int main(int argc, char **argv)
{
    auto app = Gtk::Application::create("org.loadavg.collectd.edit");

    // debugging error handling
    //test_parse();

    #ifdef TEST_GLADE_VERSION
    auto w = test_glade();
    return app->run(*w, argc, argv);
    #endif

    // for debugging purpose, get first argument to indicate
    //  the collectd folder
    if (argc > 1) {
        model::set_root(argv[1]);
        argc--;
    }

    int rc = 1;
    if (auto w = editor_window::setup(app)) {
        w->show_all();
        rc = app->run(*w, argc, argv);
        delete w;
    }

    return rc;
}
