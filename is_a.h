/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#ifndef IS_A_H
#define IS_A_H

#include <glibmm/object.h>

template <typename C>
C* is_a(Glib::Object *object) { return dynamic_cast<C*>(object); }

template <typename C>
const C* is_a(const Glib::Object *object) { return dynamic_cast<const C*>(object); }

#endif // IS_A_H

