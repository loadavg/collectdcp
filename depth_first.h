/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef DEPTH_FIRST_H
#define DEPTH_FIRST_H

#include "range.h"
#include <functional>

/**
 * @brief The depth_first struct
 *  ease visiting AST in depth first fashion,
 *  while keeping the trail (path from root to node' parent)
 */
struct depth_first : RANGE::path_t {

    /**
     * @brief visit
     *
     * @param node
     *  subtree to visit
     * @param on_node
     *  node callback: must return true if want to go down children
     * @return
     *  count of visited nodes
     */
    int visit(const RANGE &node, std::function<bool(const RANGE &top)> on_node) {
        int c = 1;
        if (on_node(node)) {
            push_back(&node);
            for (auto &child: node.nesting)
                c += visit(child, on_node);
            pop_back();
        }
        return c;
    }
};

#endif // DEPTH_FIRST_H

