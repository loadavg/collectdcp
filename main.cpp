/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include <iostream>

#include "model.h"
#include "test_win.h"
#include "collectdcp.h"
#include "app_window.h"
#include "test_parse.h"
#include "test_glade.h"
#include "ui_structure.h"
#include "editor_window.h"

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
    int rc = 1;
    try {

        model::check_root(argc, argv);
        auto app = Gtk::Application::create(argc, argv, "org.loadavg.collectdCP");

        /*
        test_win* w = 0;
        Glib::RefPtr<Gtk::Builder> builder;
        //if (ui_structure::get_resource("collectdcp", builder)) {
        if (ui_structure::get_resource("editor", builder)) {
            //builder->get_widget_derived("collectdcp_window", w);
            builder->get_widget_derived("window1", w);
            w->show_all();
            rc = app->run(*w, argc, argv);
            delete w;
        }
        */

        /*
        if (auto w = collectdcp::setup(app)) {
            w->show_all();
            rc = app->run(*w, argc, argv);
            delete w;
        }
        */

        /*
        if (!ui_structure::instance_ui_widget("collectdcp", "collectdcp", [&](Gtk::Widget *widget) {
            auto w = dynamic_cast<Gtk::Window*>(widget);
            rc = app->run(*w, argc, argv);
            return true;
        }))
            rc = 2;
        */

        /*
        app_window w;
        w.show_all();
        rc = app->run(w, argc, argv);
        */

        /*
        if (auto w = editor_window::setup(app)) {
            w->show_all();
            rc = app->run(*w, argc, argv);
            delete w;
        }
        */

        /*
        test_win* window = 0;
        Glib::RefPtr<Gtk::Builder> builder;
        if (ui_structure::get_resource("collectdcp", builder)) {
            builder->get_widget_derived("collectdcp_window", window);
            if (window) {
                window->show_all();
                rc = app->run(*window, argc, argv);
                //window->app = app;
            }
        }
        */
        /*
        collectdcp_win* window = 0;
        Glib::RefPtr<Gtk::Builder> builder;
        if (ui_structure::get_resource("collectdcp", builder)) {
            builder->get_widget_derived("collectdcp_window", window);
            if (window) {
                window->show_all();
                rc = app->run(*window, argc, argv);
                //window->app = app;
            }
        }
        */

        /*
        if (auto w = collectdcp_win::setup(app)) {
            w->show_all();
            rc = app->run(*w, argc, argv);
            delete w;
        }
        */
        if (auto w = collectdcp_app::setup(app)) {
            w->show_all();
            rc = app->run(*w, argc, argv);
            delete w;
        }
    }
    catch(std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    catch(Glib::Exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return rc;
}
