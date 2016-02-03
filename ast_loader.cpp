/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "model.h"
#include "fileuty.h"
#include "ast_loader.h"
#include "file2string.h"
#include "message_box.h"

using namespace std;

ast_loader::ast_loader(string conf, string path) {
    if (path.empty())
        path = model::conf_file(conf);
    string text;
    if (fileuty(path)) {
        text = file2string(path);
        try {
            vector<string> errors;
            ast = new model::AST(text, conf, &errors);
            if (!errors.empty())
                message_box(model::AST::format_errors(errors));
        }
        catch(exception &e) {
            message_box(e.what());
        }
    }
}
