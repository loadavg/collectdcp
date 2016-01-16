/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef COLLECTDCP_WIN_H
#define COLLECTDCP_WIN_H

#include <gtkmm/window.h>
#include <gtkmm/builder.h>

#include "model.h"

class collectdcp_win : public Gtk::Window
{
public:
    static collectdcp_win* setup(Glib::RefPtr<Gtk::Application> app);
    collectdcp_win(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);

private:
    Glib::RefPtr<Gtk::Application> app;
    model::AST *plugins_defaults = 0;
};

#endif // COLLECTDCP_WIN_H
