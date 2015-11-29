/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#ifndef MODEL_H
#define MODEL_H

#include "parse_conf.h"

/**
 *   collectd editor model data
 */
namespace model
{
    using namespace parse_conf;

    /**
     * @brief set_root
     *  set the root folder, defaults to /etc/collectd
     * @param root
     */
    void set_root(string root);

    /**
     * @brief conf_load_file
     *  load symbolic file and parse it to Ast
     *  throws on first error
     * @return
     *  structured info
    AST conf_load_file(string conf);
     */

    /**
     * @brief base_folder
     *  get the base directory of .conf files
     * @return
     */
    string base_folder();

    /**
     * @brief conf_file
     *  apply folder naming convention to
     *  get full path from a symbolic (like filters) file name
     * @return
     *  full path
     */
    string conf_file(string);

    /**
     * @brief ast_locate_cursor
     *  find path of elements matching Cursor
     */
    typedef vector<const RANGE*> nesting_path_t;
    nesting_path_t ast_locate_cursor(const AST &ast, RANGE::pos cursor);
}

#endif // MODEL_H
