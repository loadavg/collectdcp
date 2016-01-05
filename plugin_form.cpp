/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "plugin_form.h"
#include "depth_first.h"
#include <gtkmm/box.h>
#include <gtkmm/grid.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>

plugin_form::plugin_form() {

}

using namespace Gtk;
using namespace parse_conf;

Widget* plugin_form::build_form(parse_conf::token tk) {
    auto ret = new Grid();
    int row = 0;
    depth_first df;
    df.visit(tk.first, [&](const RANGE &s) {
        if (s.type == KEY_VALUES_t) {
            auto label = new Label(s[KEY_l](tk.second));
            ret->attach(*label, 0, row, 1, 1);
            auto value = new Entry;
            value->set_text(s[VALUES_l](tk.second));
            ret->attach(*value, 1, row, 1, 1);
            ++row;
            return false;
        }
        return true;
    });
    return ret;
}
