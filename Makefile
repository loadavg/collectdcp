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
        settings.o\
	test_glade.o\
	test_parse.o\
	ui_structure.o\
	view_ast.o
	g++ $^ -o $@ `pkg-config --libs gtkmm-3.0`
