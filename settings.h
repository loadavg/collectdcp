/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <glibmm/keyfile.h>
#include <string>
#include <vector>
#include <map>

/**
 * @brief The settings class
 *  store information about configured servers
 */
class settings {
public:
    settings();
    ~settings();

    /**
     * @brief initialize
     *  setup some defaults - assume localhost and collectd installed
     */
    void initialize();

    struct server {
        std::string name;
        std::string address;
        std::string alias;
        std::string folder;
    };

    typedef std::map<std::string, server> t_servers;
    t_servers servers;

    std::string store_path() const;
};

#endif // SETTINGS_H
