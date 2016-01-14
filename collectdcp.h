/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef COLLECTDCP_H
#define COLLECTDCP_H

#include <gtkmm/window.h>
#include <gtkmm/builder.h>

/**
 * @brief The collectdp class
 *  handle main application window, from glade designer
 */
class collectdcp : public Gtk::Window
{
public:
    static collectdcp* setup(Glib::RefPtr<Gtk::Application> app);
    collectdcp(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);
};

#endif // COLLECTDCP_H
