# project collectd_edit
#
# hand coded Makefile to build on CentOS 7 without an advanced IDE like QtCreator
#
# ing. Carlo Capelli
# Brescia 2015
# Copyright 2015 Sputnik7
#

TARGET_FOLDER=../build-collectdcp

all: $(TARGET_FOLDER)/collectdcp

$(TARGET_FOLDER)/%.o: %.cpp
	if ! [ -d $(TARGET_FOLDER) ] ; then mkdir $(TARGET_FOLDER) ; fi
	g++ -c -std=c++11 $< `pkg-config --cflags gtkmm-3.0` -o $@

$(TARGET_FOLDER)/collectdcp:\
	$(TARGET_FOLDER)/add_plugin_block.o\
	$(TARGET_FOLDER)/app_window.o\
	$(TARGET_FOLDER)/attr_helper.o\
        $(TARGET_FOLDER)/collectdcp.o\
	$(TARGET_FOLDER)/dlg_commands.o\
	$(TARGET_FOLDER)/editor_window.o\
	$(TARGET_FOLDER)/main.o\
	$(TARGET_FOLDER)/message_box.o\
	$(TARGET_FOLDER)/model.o\
        $(TARGET_FOLDER)/os_info.o\
	$(TARGET_FOLDER)/parse_conf.o\
	$(TARGET_FOLDER)/plugin_form.o\
	$(TARGET_FOLDER)/process_run.o\
	$(TARGET_FOLDER)/range.o\
	$(TARGET_FOLDER)/settings.o\
	$(TARGET_FOLDER)/test_glade.o\
	$(TARGET_FOLDER)/test_parse.o\
        $(TARGET_FOLDER)/test_win.o\
	$(TARGET_FOLDER)/ui_structure.o\
	$(TARGET_FOLDER)/view_ast.o
	g++ $^ -o $@ `pkg-config --libs gtkmm-3.0`
