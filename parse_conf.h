/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef PARSE_CONF_H
#define PARSE_CONF_H

#include "range.h"

#include <utility>
#include <vector>

/**
 * - from collectd configuration file documentation -
 *
 * This config file controls how the system statistics collection daemon
 * collectd behaves. The most significant option is LoadPlugin, which
 * controls which plugins to load. These plugins ultimately define
 * collectd's behavior. If the AutoLoadPlugin option has been enabled,
 * the explicit LoadPlugin lines may be omitted for all plugins with a
 * configuration block, i.e. a <Plugin...> block.
 *
 * The syntax of this config file is similar to the config file of the
 * famous Apache webserver. Each line contains either an option (a key and
 * a list of one or more values) or a section-start or -end. Empty lines
 * and everything after a non-quoted hash-symbol (#) is ignored. Keys are
 * unquoted strings, consisting only of alphanumeric characters and the
 * underscore (_) character. Keys are handled case insensitive by collectd
 * itself and all plugins included with it. Values can either be an
 * unquoted string, a quoted string (enclosed in double-quotes) a number
 * or a boolean expression. Unquoted strings consist of only alphanumeric
 * characters and underscores (_) and do not need to be quoted. Quoted
 * strings are enclosed in double quotes ("). You can use the backslash
 * character (\) to include double quotes as part of the string. Numbers
 * can be specified in decimal and floating point format (using a dot . as
 * decimal separator), hexadecimal when using the 0x prefix and octal with
 * a leading zero (0). Boolean values are either true or false.
 */

namespace parse_conf
{
    using namespace std;

    typedef vector<string> strings;
    typedef const string &kstring;

    /**
     * @brief The ELEMENT_t enum
     *  qualify different parts
     *  - COMMENT_t can be recursive
     *  - XML_LIKE_t *is* usually recursive
     */
    enum ELEMENT_t {
        FILE_t = 100,   /// left low values as counters for fixed size structs
        COMMENT_t,
        XML_LIKE_t,     /// @see XML_LIKE_layout
        KEY_VALUES_t,   /// @see KEY_VALUES_layout
        SKIP_t,
        ERROR_t,        /// error recovery
        _VALUES_
    };

    /**
     * @brief The XML_LIKE_layout enum
     *  we always have a RANGE with 3 elements in nesting, so we fix at once
     */
    enum XML_LIKE_layout {
        HEAD_l, BODY_l, TAIL_l
    };
    enum HEAD_layout {
        HTAG_l, HARGS_l
    };
    enum KEY_VALUES_layout {
        KEY_l, VALUES_l
    };

    /**
     * @brief The VALUE_t enum
     *  actual values: *quoted* and *unquoted* distinction is often blurry
     */
    enum VALUE_t {
        quoted_v = _VALUES_,
        integer_v,
        floating_v,
        xinteger_v,
        octals_v,
        boolean_v,
        unquoted_v
    };

    /**
     * @brief The AST struct
     *
     *  collect structure from .conf text files, preserve and use comments to
     *  enable/disable structured plugin blocks
     *
     *  The key representation is to 'bracket' each construct with a start/end position
     *  to allow syntax highlighting and block processing
     */
    struct AST {

        /**
         * @brief AST
         *  construct the nested structure
         *  will throw std::exception on first error
         * @param text
         *  the .conf text content
         * @param path
         *  the .conf file name - for errors report
         * @param errors
         *  report errors found
         */
        AST(string text, string path = "<?>", strings* errors = 0);

        /**
         * @brief elements
         *  all correctly parsed elements found in text
         */
        RANGE elements;

        /**
         * @brief text
         *  the .conf text buffer
         */
        string text;

        /**
         * @brief dump
         *  recursive display of AST structure and data
         * @param s
         *  stream to display to
         * @param indent
         *  left margin indentation: increment on recursive steps
         */
        void dump(ostream &s, int indent = 0) const;

        struct parse;    /// private implementation

        /**
         * @brief format_errors
         *  make a string representation of all errors found
         * @param errors
         *  all errors so far
         * @param ellipsis
         *  cut too long lists to keep manageable
         * @return
         *  a multiline string mesasge
         */
        static string format_errors(strings errors, size_t ellipsis = 3);
    };

    /**
     * @brief unquote
     *  quote is mandatory around symbols with spaces inside
     *  often also proper symbols are quoted
     * @param r
     *  parsed area containing the identifier
     * @param t
     *  text source of parsed area
     * @return
     *  symbol without quotes (check before rewrite is quote is needed!)
     */
    string unquote(const RANGE &r, kstring t);

    /**
     * @brief plugin_id
     *  get the plugin identifier of a XML_LIKE_t section
     * @param r
     *  parsed area containing the section
     * @param t
     *  text source of parsed area
     * @return
     *  unquoted plugin identifier
     */
    string plugin_id(const RANGE &r, kstring t);

    /**
     * @brief The token struct
     *  define token access functions
     */
    struct token : pair<const RANGE &, kstring> {
        token(const RANGE &r, const AST *ast) : pair<const RANGE &, kstring>(r, ast->text) { }
        string unquote() const { return parse_conf::unquote(first, second); }
        string plugin_id() const { return parse_conf::plugin_id(first, second); }
        string text() const { return first(second); }
    };

}

#endif // PARSE_CONF_H
