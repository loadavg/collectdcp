/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef UI_STRUCTURE_H
#define UI_STRUCTURE_H

#include <gtkmm/dialog.h>
#include <gtkmm/widget.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/builder.h>
#include <string>

#include "message_box.h"
#include "prints.h"
#include "is_a.h"

namespace ui_structure
{

/**
 * @brief get_resource
 *  patch the glade base file name with folder and extension
 *  get the GtkBuilder ready for object retrieval
 */
bool get_resource(std::string what, Glib::RefPtr<Gtk::Builder> &builder);
Glib::RefPtr<Gtk::Builder> get_resource(std::string what);

/**
 * @brief locate_by_name
 *  scan the widget nested structure, looking for a named child
 * @param w
 *  root widget
 * @param name
 *  widget sought
 * @param deep
 *  max nesting deep
 * @return
 *  found widget or null
 */
Gtk::Widget* locate_by_name(Gtk::Widget* w, std::string name, int deep = 1000);

/**
 * @brief get_resource_path
 *  apply directory naming conventions to get a named resource
 * @param what
 *  resource identifier
 * @param ext
 *  resource file extension
 * @return
 *  path of named resource
 */
std::string get_resource_path(std::string what, std::string ext);

/**
 * @brief run_dialog
 *  common code required to run dialog
 * @param dialog
 *  form to run
 * @return
 *  always true
 */
inline bool run_dialog(Gtk::Dialog *dialog) {
    dialog->run();
    dialog->hide();
    return true;
}

/**
 * @brief instance_ui_object
 *  guarded handling of builder retrieval and instancing of subclassed widgets
 * @param resource
 *  UI definition without path and extension (should be .glade of course)
 * @param component
 *  name of component to instantiate
 * @param runner
 *  component behaviour
 * @return
 *  @arg runner outcome if instancing went well, false otherwise
 */
template <class Form>
bool instance_ui_object(std::string resource, std::string component, std::function<bool(Form*)> runner) {
    Form *form = 0;
    Glib::RefPtr<Gtk::Builder> builder;
    if (get_resource(resource, builder)) {
        builder->get_widget_derived(component, form);
        if (form)
            return runner(form);
    }
    message_box(prints("cannot instance '%s' from '%s'", component.c_str(), resource.c_str()));
    return false;
}

/**
 * @brief instance_ui_widget
 *  guarded handling of builder retrieval and instancing of widget, specialized for dialogs
 * @param resource
 *  UI definition without path and extension (should be .glade of course)
 * @param widget_name
 *  name of component to instantiate
 * @param runner
 *  component behaviour
 * @return
 *  @arg runner outcome if instancing went well, false otherwise
 */
inline bool instance_ui_widget(std::string resource, std::string widget_name,
    std::function<bool(Gtk::Widget *widget)> handler = [](Gtk::Widget *widget) {
        if (auto dialog = is_a<Gtk::Dialog>(widget))
            return run_dialog(dialog);
        return false;
    })
{
    Glib::RefPtr<Gtk::Builder> builder;
    if (get_resource(resource, builder)) {
        Gtk::Widget *widget = 0;
        builder->get_widget(widget_name, widget);
        if (widget)
            return handler(widget);
    }
    message_box(prints("cannot instance '%s' from '%s'", widget_name.c_str(), resource.c_str()));
    return false;
}

/**
 * @brief instance_widget
 *  guarded handling of builder retrieval and instancing of widget
 * @param builder
 *  loaded reasource with named widget
 * @param name
 *  widget name
 */
template <class T = Gtk::Widget>
inline T* instance_widget(Glib::RefPtr<Gtk::Builder> builder, std::string name) {
    T *widget = 0;
    builder->get_widget(name, widget);
    if (!widget) {
        std::string msg = "cannot instantiate " + name;
        message_box(msg);
        throw msg;
    }
    return widget;
}

#if 0
/**
 * @brief container_structure
 *  recover the container given structure
 */
bool container_structure(Widget* root, Widget* &);

/**
 * @brief parents
 *  go back in parents relationship
 */
bool parents(Widget* leaf, parents_t&);

/**
 * @brief named_parents
 *  get an assoc list of widgets to names
 */
bool named_parents(RefPtr<Widget>& leaf, named_parents_t&);

/**
 * @brief buffer_cursor_offset
 *  get cursor offset for buffer
 */
bool buffer_cursor_offset(RefPtr<TextBuffer>& buffer, CURSOR &);

/**
 * @brief current_cursor_info
 *  retrieve current notebook page cursor offset
 */
bool current_cursor_info(RefPtr<Window>& toplevel, string file, CURSOR &);
#endif

/**
 * @brief tag_buffer
 *  apply tag by name to entire buffer
 * @param buf
 *  buffer to modify
 * @param tag
 *  tag to apply
 * @return
 *  buffer extent
 */
std::pair<Gtk::TextIter, Gtk::TextIter> tag_buffer(Glib::RefPtr<Gtk::TextBuffer> buf, std::string tag = "monospace");

}

#endif // UI_STRUCTURE_H
