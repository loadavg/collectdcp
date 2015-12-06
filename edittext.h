/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#ifndef EDITTEXT_H
#define EDITTEXT_H

#ifdef USE_SOURCEVIEW
    #include <gtksourceviewmm.h>
    #include <gtksourceviewmm/undomanager.h>
    typedef Gsv::View edit_text_view;
    typedef Gsv::Buffer edit_text_buf;
#else
    #include <gtkmm/textview.h>
    typedef Gtk::TextView edit_text_view;
    typedef Gtk::TextBuffer edit_text_buf;
#endif

#endif // EDITTEXT_H

