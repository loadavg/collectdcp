/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "prints.h"
#include "fileuty.h"
#include "view_ast.h"
#include "ast_loader.h"
#include "process_run.h"
#include "attr_helper.h"
#include "file2string.h"
#include "message_box.h"
#include "depth_first.h"
#include "ui_structure.h"
#include "editor_window.h"

#include <fstream>
#include <iostream>

#include <glibmm/fileutils.h>
#include <glibmm/stringutils.h>

using namespace attr_helper;
using namespace std;

view_ast::view_ast(string conf, string path) :
    conf(conf)
{
    RefPtr<edit_text_buf> buf = edit_text_buf::create();
    prepare_attributes(buf);
    set_buffer(buf);

    ast_loader loader(conf, path);
    ast = loader.ast;

    #ifdef USE_SOURCEVIEW
    RefPtr<Gsv::UndoManager> um = buf->get_undo_manager();
    um->begin_not_undoable_action();
    #endif
    if (ast) {
        buf->set_text(ast->text);

        edit_text_buf::iterator b, e;
        buf->get_bounds(b, e);
        buf->apply_tag_by_name("monospace", b, e),

        apply_AST_attributes(buf);
    }
    else
        buf->set_text(loader.text);

    #ifdef USE_SOURCEVIEW
    um->end_not_undoable_action();
    set_show_line_numbers(true);
    #endif

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

void view_ast::on_search(string text) {
    /*
    g_log("view_ast", G_LOG_LEVEL_DEBUG, "on_search(%s)", text.c_str());
    RefPtr<Buffer> b = get_source_buffer();
    GtkSourceSearchContext *sc = gtk_source_search_context_new(b->gobj(), 0);
    GtkSourceSearchSettings	*ss = gtk_source_search_context_get_settings(sc);
    gtk_source_search_settings_set_search_text(ss, text.c_str());
    RefPtr<edit_text_buf::Mark> mark = b->get_insert();
    edit_text_buf::iterator iter = b->get_iter_at_mark(mark);
    GtkTextIter I,J;
    gboolean found = gtk_source_search_context_forward(sc, iter.gobj(), &I, &J);
    if (found) {
        g_log("view_ast", G_LOG_LEVEL_DEBUG, "on_search(%s)", text.c_str());
    }
    */
    string feedback;
    RefPtr<edit_text_buf> buf = buffer();
    RefPtr<edit_text_buf::Mark> cur = buf->get_insert();
    edit_text_buf::iterator pos = buf->get_iter_at_mark(cur), I,J;
    if ((++pos).forward_search(text, TEXT_SEARCH_CASE_INSENSITIVE, I,J)) {
        buf->move_mark(cur, I);
        buf->select_range(I, J);
        scroll_to(cur);
        feedback = prints("found '%s'", text.c_str());
    }
    else
        feedback = prints("not found '%s'", text.c_str());
    app_window()->status_message(feedback);
}

void view_ast::add_plugin(string text) {
    RefPtr<edit_text_buf> buf = buffer();
    buf->insert_at_cursor(text);
    reparse_buffer();
}

void view_ast::action_status(std::string action, bool on_off) const {
    app_window()->action_status(action, on_off);
}

void view_ast::save()
{
    if (!is_dirty()) {
        message_box(prints("nothing to save from %s", conf.c_str()));
        return;
    }

    auto text = buffer()->get_text();
    auto path = model::conf_file(conf);
    string error;

    string pwd = app_window()->password()->get_text();
    if (!pwd.empty()) {
        string tmp;
        int fd = Glib::file_open_tmp(tmp);
        string::size_type c = write(fd, text.c_str(), text.length());
        close(fd);
        if (c == text.length()) {
            try {
                process_run p(prints("cp %s %s", tmp.c_str(), path.c_str()), pwd);
                error = p.result;
            }
            catch(exception &e) {
                error = e.what();
            }
        }
    }
    else {
        ofstream s(path);
        if (s)
            s << text;
        else
            error = Glib::strerror(errno);
    }
    if (error.empty()) {
        app_window()->status_message(prints("saved '%s' in '%s'", conf.c_str(), path.c_str()));

        reparse_buffer();
        set_dirty(false);
    }
    else
        message_box(prints("cannot save '%s' in '%s'\n%s", conf.c_str(), path.c_str(), error.c_str()));
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
    RefPtr<edit_text_buf> b = buffer();
    RefPtr<edit_text_buf::Mark> mark = b->get_insert();
    edit_text_buf::iterator iter = b->get_iter_at_mark(mark);
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

void view_ast::apply_AST_attributes(RefPtr<edit_text_buf> buf) {

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
            break;
        case quoted_v:
            apply_attribute(buf, "quoted", r);
            break;
        case integer_v:
        case floating_v:
        case xinteger_v:
        case octals_v:
            apply_attribute(buf, "number", r);
            break;
        case boolean_v:
            apply_attribute(buf, "boolean", r);
            break;
        case unquoted_v:
            apply_attribute(buf, "unquoted", r);
            break;

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

typedef RefPtr<edit_text_buf> pBuffer;

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
                    comment_lines(buffer(), rp->begin, rp->end);
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
                    uncomment_lines(buffer(), rp->begin, rp->end);
                    reparse_buffer();
                }
                return true;
            }
        }
    }
    return false;
}
void view_ast::reparse_buffer() {
    auto buf = buffer();
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
