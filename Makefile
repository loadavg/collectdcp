# project collectd_edit
#
# hand coded Makefile to build on CentOS 7 without an advanced IDE like QtCreator
#
# ing. Carlo Capelli
# Brescia 2015
# Copyright 2015,2016 Sputnik7
#

TARGET_FOLDER=../build-collectdcp

all: $(TARGET_FOLDER)/collectdcp

$(TARGET_FOLDER)/%.o: %.cpp
	if ! [ -d $(TARGET_FOLDER) ] ; then mkdir $(TARGET_FOLDER) ; fi
	g++ -c -std=c++11 $< `pkg-config --cflags gtkmm-3.0` -o $@

$(TARGET_FOLDER)/collectdcp:\
        $(TARGET_FOLDER)/ast_loader.o\
	$(TARGET_FOLDER)/attr_helper.o\
        $(TARGET_FOLDER)/collectdcp_app.o\
        $(TARGET_FOLDER)/dir_structure.o\
	$(TARGET_FOLDER)/dlg_commands.o\
        $(TARGET_FOLDER)/glob_path_pattern.o\
	$(TARGET_FOLDER)/main.o\
	$(TARGET_FOLDER)/message_box.o\
	$(TARGET_FOLDER)/model.o\
        $(TARGET_FOLDER)/os_info.o\
	$(TARGET_FOLDER)/parse_conf.o\
        $(TARGET_FOLDER)/plugin_to_store.o\
	$(TARGET_FOLDER)/process_run.o\
	$(TARGET_FOLDER)/range.o\
	$(TARGET_FOLDER)/settings.o\
	$(TARGET_FOLDER)/ui_structure.o\
	$(TARGET_FOLDER)/view_ast.o
	g++ $^ -o $@ `pkg-config --libs gtkmm-3.0`

ast_loader.cpp:\
    message_box.h\
    ast_loader.h\
    file2string.h\
    fileuty.h\
    icompare.h\
    model.h\
    parse_conf.h\
    prints.h\
    range.h

attr_helper.cpp:\
    attr_helper.h\
    edittext.h\
    range.h

collectdcp_app.cpp:\
    glob_path_pattern.h\
    ast_loader.h\
    collectdcp_app.h\
    dir_structure.h\
    dlg_commands.h\
    edittext.h\
    equal_range.h\
    file2string.h\
    fileuty.h\
    icompare.h\
    is_a.h\
    join.h\
    message_box.h\
    model.h\
    ns_all.h\
    os_info.h\
    parse_conf.h\
    plugin_to_store.h\
    prints.h\
    process_run.h\
    range.h\
    trim.h\
    ui_structure.h\
    view_ast.h

dir_structure.cpp:\
    dir_structure.h\
    fileuty.h

dlg_commands.cpp:\
    ui_structure.h\
    attr_helper.h\
    dlg_commands.h\
    edittext.h\
    icompare.h\
    is_a.h\
    message_box.h\
    model.h\
    parse_conf.h\
    prints.h\
    process_run.h\
    range.h

glob_path_pattern.cpp:\
    glob_path_pattern.h\
    fileuty.h

main.cpp:\
    collectdcp_app.h\
    edittext.h\
    icompare.h\
    model.h\
    parse_conf.h\
    range.h\
    view_ast.h

message_box.cpp:\
    message_box.h

model.cpp:\
    file2string.h\
    depth_first.h\
    fileuty.h\
    icompare.h\
    model.h\
    parse_conf.h\
    range.h

os_info.cpp:\
    process_run.h\
    os_info.h\
    split.h\
    trim.h\
    var_assign.h

parse_conf.cpp:\
    parse_conf.h\
    icompare.h\
    join.h\
    prints.h\
    range.h\
    tokenizer.h

plugin_to_store.cpp:\
    ns_all.h\
    icompare.h\
    is_a.h\
    join.h\
    message_box.h\
    model.h\
    parse_conf.h\
    plugin_to_store.h\
    prints.h\
    range.h\
    ui_structure.h

process_run.cpp:\
    process_run.h\
    os_info.h\
    prints.h

range.cpp:\
    join.h\
    prints.h\
    range.h

settings.cpp:\
    ini_file.h\
    is_a.h\
    join.h\
    message_box.h\
    prints.h\
    settings.h\
    ui_structure.h

ui_structure.cpp:\
    ui_structure.h\
    is_a.h\
    message_box.h\
    prints.h

view_ast.cpp:\
    collectdcp_app.h\
    ast_loader.h\
    attr_helper.h\
    depth_first.h\
    edittext.h\
    file2string.h\
    fileuty.h\
    icompare.h\
    is_a.h\
    message_box.h\
    model.h\
    parse_conf.h\
    prints.h\
    process_run.h\
    range.h\
    ui_structure.h\
    view_ast.h
