/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include <iostream>

#include "model.h"
#include "ui_structure.h"
#include "collectdcp_app.h"

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
