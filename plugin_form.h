/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef PLUGIN_FORM_H
#define PLUGIN_FORM_H

#include "model.h"
#include <gtkmm/widget.h>

/**
 * @brief The plugin_form class
 *  translate a nested block to editable form
 *  requires a template and actual data, if any
 */
class plugin_form {
public:
    plugin_form();

    Gtk::Widget* build_form(parse_conf::token tk);

private:

};

#endif // PLUGIN_FORM_H
