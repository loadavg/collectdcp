/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#include "ui_structure.h"

#include <iostream>
#include <gtkmm/menu.h>
#include <gtkmm/menuitem.h>
#include <glibmm/fileutils.h>
#include <glibmm/markup.h>

namespace ui_structure
{

Glib::RefPtr<Gtk::Builder> get_resource(std::string what) {
    using namespace std;

    // Load the GtkBuilder file and instantiate its widgets:
    auto builder = Gtk::Builder::create();
    /*
    try {
        builder->add_from_file("../resources/" + what + ".glade");
    }
    catch(const Glib::FileError& ex) {
        cerr << "FileError: " << ex.what() << endl;
    }
    catch(const Glib::MarkupError& ex) {
        cerr << "MarkupError: " << ex.what() << endl;
    }
    catch(const Gtk::BuilderError& ex) {
        cerr << "BuilderError: " << ex.what() << endl;
    }
    */
    get_resource(what, builder);
    return builder;
}

bool get_resource(std::string what, Glib::RefPtr<Gtk::Builder> &builder) {
    using namespace std;

    // Load the GtkBuilder file and instantiate its widgets:
    builder = Gtk::Builder::create();
    try {
        builder->add_from_file(get_resource_path(what, "glade"));
    }
    catch(const Glib::FileError& ex) {
        cerr << "FileError: " << ex.what() << endl;
        return false;
    }
    catch(const Glib::MarkupError& ex) {
        cerr << "MarkupError: " << ex.what() << endl;
        return false;
    }
    catch(const Gtk::BuilderError& ex) {
        cerr << "BuilderError: " << ex.what() << endl;
        return false;
    }

    return true;
}

Gtk::Widget* locate_by_name(Gtk::Widget* w, std::string name, int deep) {
    using namespace std;

    if (deep < 1000)
        cout << string(deep * 2, ' ') << (w->get_name() == name) << ' ' << w->get_name() << endl;

    Gtk::Widget* W = 0;
    if (auto C = dynamic_cast<Gtk::Container*>(w))
        for (auto c: C->get_children())
            if (auto T = locate_by_name(c, name, deep + 1))
                W = T;
    if (auto M = dynamic_cast<Gtk::MenuItem*>(w))
        if (M->has_submenu())
            if (auto T = locate_by_name(M->get_submenu(), name, deep + 1))
                W = T;

    return w->get_name() == name ? w : W;
}

std::string get_resource_path(std::string what, std::string ext) {
    return "../resources/" + what + "." + ext;
}

}
