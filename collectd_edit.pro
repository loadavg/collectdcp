# project collectd_edit
# ing. Carlo Capelli
# Brescia 2015
#
# Copyright 2015 Sputnik7
#

TEMPLATE = app

CONFIG += console
CONFIG += C++11
CONFIG -= qt
CONFIG -= app_bundle

CONFIG += link_pkgconfig
PKGCONFIG += gtkmm-3.0 gtksourceviewmm-3.0

SOURCES += main.cpp \
    parse_conf.cpp \
    model.cpp \
    view_ast.cpp \
    ui_structure.cpp \
    message_box.cpp \
    add_plugin_block.cpp \
    range.cpp \
    dlg_commands.cpp \
    attr_helper.cpp \
    process_run.cpp \
    editor_window.cpp \
    test_parse.cpp

HEADERS += \
    parse_conf.h \
    model.h \
    file2string.h \
    view_ast.h \
    ui_structure.h \
    message_box.h \
    add_plugin_block.h \
    prints.h \
    icompare.h \
    range.h \
    tokenizer.h \
    join.h \
    dlg_commands.h \
    attr_helper.h \
    process_run.h \
    editor_window.h \
    test_parse.h \
    is_a.h

DISTFILES += \
    doxygen_config
