/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "app_window.h"
#include "ui_structure.h"

app_window::app_window() : start("start") {
    set_title("CollectdCP");
    set_default_size(800, 600);

    notebook.append_page(start, "Configuration");
    add(notebook);
}
