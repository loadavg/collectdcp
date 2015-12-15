/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#include "join.h"
#include "settings.h"
#include "ui_structure.h"

#include <gtkmmconfig.h>
#include <fstream>

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
    if (GTKMM_MAJOR_VERSION >= 3 && GTKMM_MINOR_VERSION >= 10) {
        Glib::KeyFile kf;
        for (auto s: servers) {
            auto n = s.first;
            v.push_back(n);
            kf.set_string(n, "address", s.second.address);
            kf.set_string(n, "alias", s.second.alias);
            kf.set_string(n, "folder", s.second.folder);
        }
        kf.set_string_list("servers", "name", v);
        kf.save_to_file(store_path());
    }
    else {
        // CentOS 7 have GTKMM_MINOR_VERSION == 8, missing save_to_file
        std::ofstream kf(store_path());
        for (auto s: servers) {
            auto n = s.first;
            kf << '[' << n << ']' << std::endl;
            v.push_back(n);
            kf << "address" << '=' << s.second.address << std::endl;
            kf << "alias" << '=' << s.second.alias << std::endl;
            kf << "folder" << '=' << s.second.folder << std::endl;
        }
        kf << '[' << join(v) << ']' << std::endl;
    }
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
