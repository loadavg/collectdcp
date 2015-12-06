/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#include "attr_helper.h"

namespace attr_helper {

void apply_attribute(RefPtr<edit_text_buf> buf, ccp attr, const RANGE &r) {
    auto    I = buf->get_iter_at_offset(r.begin),
            J = buf->get_iter_at_offset(r.end);
    buf->apply_tag_by_name(attr, I, J);
}

void prepare_attributes(RefPtr<edit_text_buf> buf) {

    auto TAG = [buf](ccp tag, attr_func attrs) {
        return attrs(buf->create_tag(tag));
    };

    #define A1(f1, a1) [](TT tt) { f1(tt, a1); }
    #define A2(f1, a1, f2, a2) [](TT tt) { f1(tt, a1); f2(tt, a2); }

    TAG("xml_open", A1(style, Pango::Style::STYLE_ITALIC));
    TAG("xml_close", A2(style, Pango::Style::STYLE_ITALIC, strikethrough, true));
    TAG("key", A1(weight, Pango::Weight::WEIGHT_BOLD));
    TAG("number", A1(foreground, "darkgreen"));
    TAG("unquoted", A1(foreground, "blue4"));
    TAG("quoted", A1(foreground, "blue"));
    TAG("boolean", A1(foreground, "brown"));
    TAG("comment", A1(background, "lightgrey"));
    TAG("monospace", A1(family, "Monospace"));
    TAG("error", A2(strikethrough, true, foreground, "red"));

    #undef A1
    #undef A2
}
}
