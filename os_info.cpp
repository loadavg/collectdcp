/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "os_info.h"
#include "var_assign.h"
#include "process_run.h"
#include <sstream>

os_info::os_info() {

    using namespace std;
    process_run p("cat /etc/os-release");
    istringstream s(p.result);
    string line;
    while (getline(s, line)) {
        var_assign kv(line);
        if (kv) {
            #define KV(k) if (kv.key == #k) k = kv.value;
            KV(NAME)
            KV(VERSION)
            KV(ID)
            KV(ID_LIKE)
            KV(PRETTY_NAME)
            KV(VERSION_ID)
            KV(HOME_URL)
            KV(BUG_REPORT_URL)
        }
    }
}
