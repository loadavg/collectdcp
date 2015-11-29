/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#include <iostream>
#include "model.h"
#include "ui_structure.h"
#include "message_box.h"
#include "file2string.h"
#include "test_parse.h"

using namespace std;

void test_parse()
{
    auto text = file2string(ui_structure::get_resource_path("plugins", "template"));
    try {
        auto ast_template = new parse_conf::AST(text);
        ast_template->dump(cout);
        delete ast_template;
    } catch (exception &e) {
        message_box(e.what());
    }
#if 0
    try {
        parse_conf::AST ast = model::conf_load_file("collectd");
        ast.dump(cout);
    }
    catch(const Glib::Exception &e) {
        std::cerr << e.what();
        exit(1);
    }
    catch(const std::exception &e) {
        std::cerr << e.what();
        exit(1);
    }
#endif
}
