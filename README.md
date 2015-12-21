collectdcp
================================

project collectd_edit
================================

>ing. Carlo Capelli

>Brescia 2015

>Copyright 2015 Sputnik7

Build instructions:
-----

Currently, this project is developed in Ubuntu 14.04, using using QtCreator as IDE,
hence if you have QtCreator, just open collectd_edit.pro and build.

A Makefile is provided for CentOS 7,  if you miss an advanced IDE like QtCreator.
It will build the collectdcp executable in a folder:

TARGET_FOLDER=../build-collectdcp

GNOME platform has been installed from standard Debian repo, and this has created several difficulties when porting to CentOS 7.
GLib, Gtk, gtkmm, Glade versions mismatch, and gtksourceviewmmm isn't available.

Since the original plan was to have minimal requirements on target platform, some workarounds has been implemented.
Glade files version have been 'downgraded' from 3.10 to 3.8, using a minimal script that updates just the version element.
The resource folder has been duplicated and renamed to match the different versions.

Advanced components:
-----

To use *GtkSourceView* we need the gtkmm wrapper:

    sudo apt-get install libgtksourceviewmm-3.0-dev

to use *GtkTextView* (and lose some minor features, like undo/redo, parenthesis highlight, lines numbers display)
comment out these two lines in *collectd_edit.pro*

    DEFINES += USE_SOURCEVIEW
    PKGCONFIG += gtksourceviewmm-3.0

