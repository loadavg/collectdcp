/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "collectdcp.h"
#include "ui_structure.h"

//#include "ns_all.h"
//using namespace ns_all;

collectdcp* collectdcp::setup(Glib::RefPtr<Gtk::Application> app) {
    collectdcp* window = 0;
    Glib::RefPtr<Gtk::Builder> builder;
    if (ui_structure::get_resource("editor", builder)) {
        builder->get_widget_derived("window1", window);
    }
    /*
    if (ui_structure::get_resource("collectdcp", builder)) {
        builder->get_widget_derived("window1", window);
    }*/
    return window;
}

collectdcp::collectdcp(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
    : Gtk::Window(cobject)
{

}
