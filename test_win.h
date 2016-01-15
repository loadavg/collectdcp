#ifndef TEST_WIN_H
#define TEST_WIN_H

#include <gtkmm/window.h>
#include <gtkmm/builder.h>

class test_win : public Gtk::Window
{
public:
    test_win(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);
};

#endif // TEST_WIN_H
