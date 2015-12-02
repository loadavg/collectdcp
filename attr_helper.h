/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#ifndef ATTR_HELPER_H
#define ATTR_HELPER_H

#include <gtksourceviewmm.h>
#include "range.h"

namespace attr_helper {

    using namespace Glib;
    using namespace Gtk;
    using namespace Gsv;

    typedef const char* ccp;
    typedef RefPtr<TextTag> TT;
    typedef std::function<void(TT)> attr_func;

    auto style = [](TT tt, Pango::Style style) { tt->property_style() = style; return tt; };
    auto strikethrough = [](TT tt, bool yn) { tt->property_strikethrough() = yn; return tt; };
    auto weight = [](TT tt, Pango::Weight w) { tt->property_weight() = w; return tt; };
    auto foreground = [](TT tt, ccp c) { tt->property_foreground() = c; return tt; };
    auto background = [](TT tt, ccp c) { tt->property_background() = c; return tt; };
    auto family = [](TT tt, ccp f) { tt->property_family() = f; return tt; };

    /**
     * @brief apply_attribute
     *  apply named @param attr to @param buf
     * @param buf
     * @param attr
     * @param r
     */
    void apply_attribute(RefPtr<Buffer> buf, const char *attr, const RANGE &r);

    /**
     * @brief prepare_attributes
     *  specialized attributes setup for view_ast tagging
     * @param buf
     *  text buffer to bind
     */
    void prepare_attributes(RefPtr<Buffer> buf);
}

#endif // ATTR_HELPER_H
