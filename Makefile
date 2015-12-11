# project collectd_edit
# ing. Carlo Capelli
# Brescia 2015
#
# Copyright 2015 Sputnik7
#

.cpp.o:
        g++ -c -std=c++11 $< `pkg-config --cflags gtkmm-3.0`

collectdcp:\
        add_plugin_block.o\
        attr_helper.o\
        dlg_commands.o\
        editor_window.o\
        main.o\
        message_box.o\
        model.o\
        parse_conf.o\
        plugin_form.o\
        process_run.o\
        range.o\
        test_glade.o\
        test_parse.o\
        ui_structure.o\
        view_ast.o
        g++ $^ -o $@ `pkg-config --libs gtkmm-3.0`

add_plugin_block.o: add_plugin_block.cpp

attr_helper.o: attr_helper.cpp

dlg_commands.o: dlg_commands.cpp

editor_window.o: editor_window.cpp

main.o: main.cpp

message_box.o: message_box.cpp

model.o: model.cpp

parse_conf.o: parse_conf.cpp

process_run.o: process_run.cpp

range.o: range.cpp

test_parse.o: test_parse.cpp

ui_structure.o: ui_structure.cpp

view_ast.o: view_ast.cpp
