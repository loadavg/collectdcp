/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#include "model.h"
#include "attr_helper.h"
#include "process_run.h"
#include "message_box.h"
#include "dlg_commands.h"
#include "ui_structure.h"

dlg_commands::dlg_commands(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
    : Gtk::Dialog(cobject)
{
    using namespace ui_structure;

    tb_output = instance_widget<Gtk::TextView>(refBuilder, "tb_output");
    if (auto buf = tb_output->get_buffer())
        if (auto tag = buf->create_tag("monospace"))
            tag->property_family() = "Monospace";

    password = instance_widget<Gtk::Entry>(refBuilder, "txt_sysadm_pwd");

    auto btn = [&](std::string name, void (dlg_commands::*fun)()) {
        instance_widget<Gtk::Button>(refBuilder, name)->signal_clicked().connect(sigc::mem_fun(this, fun));
    };
    btn("btn_validate", &dlg_commands::on_validate);
    btn("btn_stats",    &dlg_commands::on_stats);
    btn("btn_install",  &dlg_commands::on_install);
    btn("btn_start",    &dlg_commands::on_start);
    btn("btn_stop",     &dlg_commands::on_stop);
}

void dlg_commands::issue_command(std::string cmd) {
    //auto p = model::conf_file("collectd");
    try {
        process_run P(cmd, password->get_text());
        if (tb_output) {
            auto b = tb_output->get_buffer();
            b->set_text(P.result);
            set_monospace();
        }
    }
    catch(std::exception &e) {
        message_box(e.what());
    }
}

void dlg_commands::on_validate() {
    issue_command("collectd -t -C " + model::conf_file("collectd"));
}

void dlg_commands::on_stats() {
    issue_command("service --status-all");
}
void dlg_commands::on_install() {
    message_box("tbd");
}

void dlg_commands::on_start() {
    issue_command("/etc/init.d/collectd start");
}

void dlg_commands::on_stop() {
    issue_command("/etc/init.d/collectd stop");
}

void dlg_commands::set_monospace() {
    auto buf = tb_output->get_buffer();
    Gtk::TextIter I, J;
    buf->get_bounds(I, J);
    buf->apply_tag_by_name("monospace", I, J);
}
