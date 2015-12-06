collectdcp
================================

project collectd_edit
================================

>ing. Carlo Capelli

>Brescia 2015

>Copyright 2015 Sputnik7

build instructions:
-----

to use *GtkSourceView* we need the gtkmm wrapper:

    sudo apt-get install libgtksourceviewmm-3.0-dev

to use *GtkTextView* (and lose some minor features, like undo/redo, parenthesis highlight, lines numbers display)
comment out these two lines in *collectd_edit.pro*

    DEFINES += USE_SOURCEVIEW
    PKGCONFIG += gtksourceviewmm-3.0

