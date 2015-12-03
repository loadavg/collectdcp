/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#ifndef DLG_COMMANDS_H
#define DLG_COMMANDS_H

#include <gtkmm/entry.h>
#include <gtkmm/dialog.h>
#include <gtkmm/builder.h>
#include <gtkmm/textview.h>

/**
 * @brief The dlg_commands class
 *  run a modal dialog with commonly used commands
 *  defined as buttons, output is captured and shown in text box
 */
class dlg_commands : public Gtk::Dialog
{
public:
    dlg_commands(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);

    Gtk::Entry *password;

private:
    void on_validate();
    void on_stats();
    void on_install();

    void on_start();
    void on_stop();

    void issue_command(std::string cmd);

    Gtk::TextView *tb_output;

    void set_monospace();
};

#endif // DLG_COMMANDS_H
