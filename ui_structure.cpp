/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "ui_structure.h"

#include <iostream>
#include <gtkmm/menu.h>
#include <gtkmm/menuitem.h>
#include <glibmm/fileutils.h>
#include <glibmm/markup.h>

namespace ui_structure {

using namespace std;

Glib::RefPtr<Gtk::Builder> get_resource(string what) {
    Glib::RefPtr<Gtk::Builder> builder;
    get_resource(what, builder);
    return builder;
}

bool get_resource(std::string what, Glib::RefPtr<Gtk::Builder> &builder) {

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

Gtk::Widget* locate_by_name(Gtk::Widget* w, string name, int deep) {

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

string get_resource_path(string what, string ext) {
    #ifndef RESOURCES_DIR
    #define RESOURCES_DIR "resources-3.8"
    #endif
    return "../collectdcp/" RESOURCES_DIR "/" + what + "." + ext;
}

pair<Gtk::TextIter, Gtk::TextIter> tag_buffer(Glib::RefPtr<Gtk::TextBuffer> buf, string tag) {
    Gtk::TextIter I, J;
    buf->get_bounds(I, J);
    buf->apply_tag_by_name(tag, I, J);
    return std::make_pair(I, J);
}

}
