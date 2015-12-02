/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#include "prints.h"
#include "view_ast.h"
#include "attr_helper.h"
#include "file2string.h"
#include "message_box.h"
#include "depth_first.h"
#include "ui_structure.h"
#include "editor_window.h"

#include <fstream>
#include <iostream>

using namespace attr_helper;
using namespace std;

view_ast::view_ast(string conf) :
    conf(conf)
{
    RefPtr<Buffer> buf = Buffer::create();
    prepare_attributes(buf);
    set_buffer(buf);

    auto path = model::conf_file(conf);
    auto text = file2string(path);
    try {
        vector<string> errors;
        ast = new model::AST(text, conf, &errors);
        if (!errors.empty())
            message_box(model::AST::format_errors(errors));
    }
    catch(exception &e) {
        message_box(e.what());
    }

    RefPtr<UndoManager> um = buf->get_undo_manager();
    um->begin_not_undoable_action();
    if (ast) {
        buf->set_text(ast->text);

        Buffer::iterator b, e;
        buf->get_bounds(b, e);
        buf->apply_tag_by_name("monospace", b, e),

        apply_AST_attributes(buf);
    }
    else
        buf->set_text(text);
    um->end_not_undoable_action();

    set_show_line_numbers(true);

    buf->property_cursor_position().signal_changed().connect(sigc::mem_fun(this, &view_ast::on_cursor_position_changed));
    buf->signal_changed().connect(sigc::mem_fun(this, &view_ast::on_changed));

    //signal_focus().connect(sigc::mem_fun(this, &view_ast::on_focus)); on_focus is virtual...
}

view_ast::~view_ast() {
    delete ast;
    delete ast_template;
}

editor_window* view_ast::app_window() const {
    for (auto p = get_parent(); p; p = p->get_parent())
        if (auto e = is_a<editor_window>(p))
            return const_cast<editor_window*>(e);
    return 0;
}
void view_ast::action_status(std::string action, bool on_off) const {
    app_window()->action_status(action, on_off);
}

void view_ast::save()
{
    if (is_dirty()) {
        auto text = get_buffer()->get_text();
        auto path = model::conf_file(conf);
        ofstream s(path);
        if (s) {
            s << text;
            app_window()->status_message(prints("saved '%s' in '%s'", conf.c_str(), path.c_str()));

            reparse_buffer();
            set_dirty(false);
        }
        else
            message_box(prints("cannot save '%s' in '%s'", conf.c_str(), path.c_str()));
    }
    else
        message_box(prints("nothing to save from %s", conf.c_str()));
}

void view_ast::on_cursor_position_changed() {
    /*
    if (enable_block(false)) {
        action_status("enable_block", true);
        action_status("disable_block", false);
    }
    if (disable_block(false)) {
        action_status("disable_block", true);
        action_status("enable_block", false);
    }
    */
}

void view_ast::on_changed() {
    set_dirty();
    //action_status("file_save", true);
}
/*
bool view_ast::on_focus(DirectionType dt) {
    cout << "on_focus" << dt << endl;
    if (is_dirty()) {
        action_status("file_save", true);
    }
    else {
        action_status("file_save", false);
    }
    return false;
}
*/

RANGE::pos view_ast::buffer_cursor_offset() {
    RefPtr<Buffer> b = get_source_buffer();
    RefPtr<TextBuffer::Mark> mark = b->get_insert();
    TextBuffer::iterator iter = b->get_iter_at_mark(mark);
    return iter.get_offset();
}

static const string dirty_flag = "* ";
static bool is_dirty_flag(string label) { return label.substr(0, 2) == dirty_flag; }
static string set_dirty_flag(string label) { return dirty_flag + label; }
static string clear_dirty_flag(string label) { return is_dirty_flag(label) ? label.substr(2) : label; }

bool view_ast::is_dirty() {
    return is_dirty_flag(label()->get_text());
}
void view_ast::set_dirty(bool dirty) {
    Label *l = label();
    string t = l->get_text();
    if (dirty) {
        if (!is_dirty_flag(t))
            l->set_text(set_dirty_flag(t));
    }
    else {
        if (is_dirty_flag(t))
            l->set_text(clear_dirty_flag(t));
    }
}

Label* view_ast::label() {
    if (Notebook *n = view_notebook())
        return is_a<Label>(n->get_tab_label(*get_parent()));
    return 0;
}

void view_ast::set_template(string conf_template) {
    auto text = file2string(ui_structure::get_resource_path(conf_template, "template"));
    try {
        this->ast_template = new parse_conf::AST(text);
    } catch (exception &e) {
        message_box(e.what());
    }
}

Notebook* view_ast::view_notebook() {
    Notebook* n = 0;
    auto p = get_parent();
    while (p) {
        if ((n = is_a<Notebook>(p)))
            break;
        p = p->get_parent();
    }
    return n;
}

void view_ast::apply_AST_attributes(RefPtr<Buffer> buf) {

    if (!ast)
        return;

    using namespace parse_conf;

    depth_first df;
    df.visit(ast->elements, [&](const RANGE &r) {

        switch (r.type) {
        case XML_LIKE_t:
            apply_attribute(buf, "xml_open", r[HEAD_l][HTAG_l]);
            apply_attribute(buf, "xml_close", r[TAIL_l]);
            break;
        case COMMENT_t:
            apply_attribute(buf, "comment", r);
            break;
        case KEY_VALUES_t:
            apply_attribute(buf, "key", r[KEY_l]);
            for (auto &v: r[VALUES_l].nesting) {
                switch (v.type) {
                case quoted_v:
                    apply_attribute(buf, "quoted", v);
                    break;
                case integer_v:
                case floating_v:
                case xinteger_v:
                case octals_v:
                    apply_attribute(buf, "number", v);
                    break;
                case boolean_v:
                    apply_attribute(buf, "boolean", v);
                    break;
                case unquoted_v:
                    apply_attribute(buf, "unquoted", v);
                    break;
                }
            }
            return false;
        case ERROR_t:
            apply_attribute(buf, "error", r);
            break;
        }
        return true;
    });
}

typedef RANGE::pos CURSOR;
typedef std::vector<CURSOR> Lines;

Lines get_starts(TextIter Xi, TextIter Yi) {
    Lines lines;
    int C = Xi.compare(Yi);
    while (C < 0) {
        lines.push_back(Xi.get_offset());
        if (!Xi.forward_line())
            break;
        C = Xi.compare(Yi);
    }
    return lines;
}

typedef RefPtr<Buffer> pBuffer;

Lines get_start_lines(pBuffer Buf, CURSOR X, CURSOR Y) {
    TextIter Xi = Buf->get_iter_at_offset(X);
    Xi.set_line_offset(0);
    TextIter Yi = Buf->get_iter_at_offset(Y);
    return get_starts(Xi, Yi);
}

void apply_to_lines(function<void(pBuffer, CURSOR c)> Pred, pBuffer Buf, CURSOR X, CURSOR Y) {
    Lines ls = get_start_lines(Buf, X,Y);
    for (auto p = ls.rbegin(); p != ls.rend(); ++p)
        Pred(Buf, *p);
}

void comment_line(pBuffer Buf, CURSOR Line) {
    TextIter Iter = Buf->get_iter_at_offset(Line);
    Buf->insert(Iter, "#");
}
void uncomment_line(pBuffer Buf, CURSOR Line) {
    TextIter X = Buf->get_iter_at_offset(Line);
    TextIter Y = Buf->get_iter_at_offset(Line + 1);
    Buf->erase(X, Y);
}

void comment_lines(pBuffer Buf, CURSOR X, CURSOR Y) {
    apply_to_lines(comment_line, Buf, X,Y);
}
void uncomment_lines(pBuffer Buf, CURSOR X, CURSOR Y) {
    apply_to_lines(uncomment_line, Buf, X,Y);
}

inline bool actionable_block(const RANGE* rp) {
    return rp->type == parse_conf::XML_LIKE_t || rp->type == parse_conf::KEY_VALUES_t;
}

bool view_ast::disable_block(bool perform) {
    if (ast) {
        for (auto rp : model::ast_locate_cursor(*ast, buffer_cursor_offset())) {
            if (rp->type == parse_conf::COMMENT_t)
                return false;   // already disabled
            if (actionable_block(rp)) {
                if (perform) {
                    comment_lines(get_source_buffer(), rp->begin, rp->end);
                    reparse_buffer();
                }
                return true;
            }
        }
    }
    return false;
}
bool view_ast::enable_block(bool perform) {
    if (ast) {
        bool enabled = true;
        for (auto rp : model::ast_locate_cursor(*ast, buffer_cursor_offset())) {
            if (rp->type == parse_conf::COMMENT_t)
                enabled = false;   // already disabled
            if (!enabled && actionable_block(rp)) {
                if (perform) {
                    uncomment_lines(get_source_buffer(), rp->begin, rp->end);
                    reparse_buffer();
                }
                return true;
            }
        }
    }
    return false;
}
void view_ast::reparse_buffer() {
    auto buf = get_source_buffer();
    auto text = buf->get_text();
    try {
        auto temp = new parse_conf::AST(text, conf);

        TextIter I, J;
        buf->get_bounds(I, J);
        buf->remove_all_tags(I, J);
        buf->apply_tag_by_name("monospace", I, J),

        delete ast;
        ast = temp;

        apply_AST_attributes(buf);
    }
    catch(exception &e) {
        message_box(e.what());
    }
}
