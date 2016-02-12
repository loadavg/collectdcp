/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef MODEL_H
#define MODEL_H

#include "parse_conf.h"
#include "icompare.h"
#include <map>

/**
 *   collectd editor model data
 */
namespace model
{
    using namespace parse_conf;

    /**
     * @brief check_root
     *  need to adapt to different collectd base folders
     */
    void check_root(int &argc, char **argv);

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

    /**
     * @brief The plugins_t struct
     *  index an AST by plugin name
     *  that is a block like <plugin name> ... </plugin>
     */
    struct plugins_t : map<string, RANGE::path_t, iless> {
        const AST *ast;
        plugins_t(const AST *ast);
    };

    /**
     * @brief The entries_t struct
     *  index all elements by name
     *  we can easily have duplicated entries
     */
    struct entries_t : multimap<string, RANGE::path_t, iless> {
        const AST *ast;
        entries_t(const AST *ast);
    };

    /**
     * @brief The terminals_t struct
     *  index terminals by name
     */
    struct terminals_t : map<string, RANGE::path_t, iless> {
        const AST *ast;
        terminals_t(const AST *ast, RANGE::path_t &root);
    };

    /**
     * @brief entry_symbol
     * @return
     *  normally, "collectd"
     */
    string entry_symbol();
}

#endif // MODEL_H
