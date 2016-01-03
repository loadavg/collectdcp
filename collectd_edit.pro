# project collectd_edit
# ing. Carlo Capelli
# Brescia 2015
#
# Copyright 2015 Sputnik7
#

TEMPLATE = app

CONFIG += console
CONFIG += C++14
CONFIG -= qt
CONFIG -= app_bundle

CONFIG += link_pkgconfig
PKGCONFIG += gtkmm-3.0

DEFINES += USE_SOURCEVIEW
PKGCONFIG += gtksourceviewmm-3.0

DEFINES += RESOURCES_DIR=\\\"resources-3.10\\\"

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
    test_parse.cpp \
    plugin_form.cpp \
    test_glade.cpp \
    settings.cpp

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
    is_a.h \
    depth_first.h \
    edittext.h \
    plugin_form.h \
    test_glade.h \
    ns_all.h \
    settings.h \
    ini_file.h \
    fileuty.h

DISTFILES += \
    Makefile \
    README.md \
    doxygen_config \
    resources-3.8/test_glade.glade \
    resources-3.8/editor.glade \
    resources-3.8/dlg_commands.glade \
    resources-3.8/collectdcp.glade \
    resources-3.8/add_plugin_treeview.glade \
    resources-3.8/about_box.glade \
    resources-3.8/plugins.template \
    resources-3.10/test_glade.glade \
    resources-3.10/collectdcp.glade \
    resources-3.10/editor.glade \
    resources-3.10/dlg_commands.glade \
    resources-3.10/add_plugin_treeview.glade \
    resources-3.10/about_box.glade \
    resources-3.10/plugins.template \
    swipl-plgi/url_page_cache.pl \
    swipl-plgi/parse_conf.pl \
    swipl-plgi/glade_utility.pl \
    swipl-plgi/add_plugin_block.pl \
    swipl-plgi/colors.pl \
    swipl-plgi/dlg_commands.pl \
    swipl-plgi/editapp.pl \
    swipl-plgi/message_box.pl \
    swipl-plgi/model.pl \
    swipl-plgi/scrap_plugins_wiki.pl \
    swipl-plgi/ui_structure.pl \
    swipl-plgi/uty.pl \
    swipl-plgi/view_ast.pl \
    swipl-plgi/ubuntu-14.04-default/collectd.conf.d/thresholds.conf \
    swipl-plgi/ubuntu-14.04-default/collectd.conf.d/filters.conf \
    swipl-plgi/ubuntu-14.04-default/collectd.conf \
    swipl-plgi/ubuntu-14.04-default/collection.conf \
    swipl-plgi/gen_glade.pl \
    resources-3.8/generated/main.glade
