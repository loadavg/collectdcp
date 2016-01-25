/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "message_box.h"
#include <glibmm/exception.h>
#include <gtkmm/messagedialog.h>

bool message_box(std::string msg, mode md) {
    using namespace Gtk;
    bool use_markup = false;
    MessageType type = MESSAGE_INFO;
    ButtonsType buttons = BUTTONS_OK;
    bool modal = true;
    int expect = RESPONSE_OK;
    if (md == YES_NO) {
        buttons = BUTTONS_YES_NO;
        expect = RESPONSE_YES;
    }
    MessageDialog dlg(msg, use_markup, type, buttons, modal);
    return dlg.run() == expect;
}

bool CATCH_SHOW(std::function<void()> f) {
    std::string msg;
    try {
        f();
        return true;
    }
    catch(std::exception &e) {
        msg = e.what();
    }
    catch(Glib::Exception &e) {
        msg = e.what();
    }
    message_box("exception:" + msg);
    return false;
}
