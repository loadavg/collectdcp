/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#ifndef VIEW_AST_H
#define VIEW_AST_H

#include "model.h"

#include <gtksourceviewmm.h>
#include <gtkmm/notebook.h>

class editor_window;

/**
 * @brief The view_ast class
 *  a decorated AST view/editor
 */
class view_ast : public Gsv::View
{
public:
    view_ast(std::string conf);
    ~view_ast();

    void save();

    Gtk::Notebook* view_notebook();
    Gtk::Label* label();

    const model::AST *get_AST() const { return ast; }
    std::string conf;

    /**
     * @brief set_template
     * @param conf_template
     */
    void set_template(std::string conf_template);

    /**
     * @brief buffer_cursor_offset
     *  get cursor offset for buffer
     */
    RANGE::pos buffer_cursor_offset();

    bool disable_block(bool perform);
    bool enable_block(bool perform);

    bool is_dirty();

    editor_window* app_window() const;

    /**
     * @brief ast_template
     *  holds pertinent sections to insert/validate elements
     */
    model::AST *ast_template = 0;

private:

    /**
     * @brief ast
     *  parsed and decorated .conf text file
     */
    model::AST *ast = 0;

    /**
     * @brief apply_AST_attributes
     *  text areas get colored, depending on their AST role
     *	decorate a 'key value' pair
     *	decorate an XML like tag
     */
    void apply_AST_attributes(Glib::RefPtr<Gsv::Buffer> buf);

    /**
     * @brief on_cursor_position_changed
     *  enable/disable context sensitive commands, depending on cursor position
     */
    void on_cursor_position_changed();

    void on_changed();
    //bool on_focus(Gtk::DirectionType);

    //bool disable_parse;
    void setup_context_commands();

    void set_dirty(bool dirty = true);
    void reparse_buffer();

    void action_status(std::string action, bool on_off) const;
};

#endif // VIEW_AST_H
