/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "join.h"
#include "plugin_to_store.h"

#include "ns_all.h"

void plugin_to_store::add_plugin(const RANGE &r, const plugins_t &in_view) {
    Node i = treestore->append();
    auto pid = plugin_id(r, text);
    i->set_value(0, pid);
    //i->set_value(1, in_view.find(pid) != in_view.end());
    append(r, i);
}

void plugin_to_store::append(const RANGE &s, Node parent) {
    g_assert(s.type == XML_LIKE_t);
    for (auto e: s[BODY_l].nesting) {
        if (e.type == XML_LIKE_t) {
            Node c = treestore->append(parent->children());
            c->set_value(0, e[HEAD_l][HTAG_l](text));
            append(e, c);

            strings args;
            for (auto a: e[HEAD_l][HARGS_l].nesting)
                args.push_back(a(text));
            auto t = join(args, ',');
            c->set_value(1, t);
        }
        if (e.type == KEY_VALUES_t) {
            Node c = treestore->append(parent->children());
            c->set_value(0, e[KEY_l](text));
            strings args;
            for (auto a: e[VALUES_l].nesting)
                args.push_back(a(text));
            auto t = join(args, ',');
            c->set_value(1, t);
        }
    }
}
