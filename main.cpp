/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "model.h"
#include "app_window.h"
#include "test_parse.h"
#include "test_glade.h"
#include "editor_window.h"

#include <iostream>

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
        // debugging error handling
        //test_parse();
        //test_glade(argc, argv);

        // for debugging purpose, get first argument to indicate
        //  the collectd folder
        /*
        if (argc > 1) {
            model::set_root(argv[1]);
            argc--;
        }
        else
            model::check_root();
        */
        model::check_root(argc, argv);

        auto app = Gtk::Application::create(argc, argv, "org.loadavg.collectdCP");
        app_window w;
        w.show_all();
        rc = app->run(w, argc, argv);

        /*
        if (auto w = editor_window::setup(app)) {
            w->show_all();
            rc = app->run(*w, argc, argv);
            delete w;
        }
        */

    }
    catch(std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    catch(Glib::Exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return rc;
}
