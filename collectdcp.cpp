/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "collectdcp.h"

#include "ns_all.h"
using namespace ns_all;

collectdcp* collectdcp::setup(RefPtr<Application> app) {
    collectdcp* window = 0;
    RefPtr<Builder> builder;
    if (get_resource("collectdcp", builder)) {
        builder->get_widget_derived("collectdcp", window);
        //if (window)
            //window->app = app;
    }
    return window;
}

collectdcp::collectdcp(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder) {

}
