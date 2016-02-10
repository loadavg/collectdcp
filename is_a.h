/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef IS_A_H
#define IS_A_H

#include <gtkmm/container.h>
#include <glibmm/object.h>

template <typename C>
C* is_a(Glib::Object *object) { return dynamic_cast<C*>(object); }

template <typename C>
const C* is_a(const Glib::Object *object) { return dynamic_cast<const C*>(object); }

template <typename C>
C* find_parent(Gtk::Widget *child) {
    for (auto p = child->get_parent(); p; p = p->get_parent())
        if (auto e = is_a<C>(p))
            return e;
    return 0;
}

#endif // IS_A_H
