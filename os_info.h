/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef OS_INFO_H
#define OS_INFO_H

#include <string>

/**
 * @brief The os_info class
 *  get info about a Linux distro
 *  capturing from $ cat /etc/os-release
 */
class os_info {
public:
    os_info();

    std::string
        NAME,
        VERSION,
        ID,
        ID_LIKE,
        PRETTY_NAME,
        VERSION_ID,
        HOME_URL,
        BUG_REPORT_URL;
};

#endif // OS_INFO_H
