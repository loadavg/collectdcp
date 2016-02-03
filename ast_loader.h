/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef AST_LOADER_H
#define AST_LOADER_H

#include "parse_conf.h"

/**
 * @brief The ast_loader class
 *  get file content with user interface to show errors
 */
struct ast_loader {

    /**
     * @brief ast_loader
     *  load file on construct, warn if errors
     * @param conf
     *  symbolic .conf name (for instance, collectd)
     * @param path
     *  actual path, or empty to get the model::resolution
     */
    ast_loader(std::string conf, std::string path = "");

    std::string text;
    parse_conf::AST *ast = 0;
};

#endif // AST_LOADER_H
