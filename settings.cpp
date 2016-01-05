/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "join.h"
#include "settings.h"
#include "ui_structure.h"

#include <gtkmmconfig.h>
#include "ini_file.h"

settings::settings() {
    auto path = store_path();
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0) {
        Glib::KeyFile kf;
        kf.load_from_file(path);
        for (auto f: kf.get_string_list("servers", "name"))
            servers[f] = server{
                    f,
                    kf.get_string(f, "address"),
                    kf.get_string(f, "alias"),
                    kf.get_string(f, "folder")
            };
    }
    if (servers.empty())
        initialize();
}

settings::~settings() {
    std::vector<std::string> v;
    ini_file kf(store_path());
    for (auto s: servers) {
        v.push_back(kf.section(s.first));
        kf.entry("address", s.second.address);
        kf.entry("alias", s.second.alias);
        kf.entry("folder", s.second.folder);
    }
    kf.section("servers");
    kf.entry("name", join(v));
}

/**
 * @brief initialize
 *  setup some defaults - assume localhost and collectd installed
 */
void settings::initialize() {
    g_assert(servers.empty());
    servers["localhost"] = server{"localhost", "127.0.0.1", "computer", "/etc/collectd"};
}

std::string settings::store_path() const {
    return ui_structure::get_resource_path("settings", "ini");
}
