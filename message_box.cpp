/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#include "message_box.h"
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
