/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include <iostream>

#include "split.h"
#include "os_info.h"
#include "var_assign.h"
#include "process_run.h"

static os_info *checked;
os_info* os_info::info() {
    if (!checked)
        checked = new os_info;
    return checked;
}

os_info::os_info() {
    using namespace std;
    process_run p("cat /etc/os-release");
    for (auto line: split(p.result)) {
        if (var_assign kv = line) {
            #define KV(k) if (kv.key == #k) k = kv.value;
            KV(NAME)
            KV(VERSION)
            KV(ID)
            KV(ID_LIKE)
            KV(PRETTY_NAME)
            KV(VERSION_ID)
            KV(HOME_URL)
            KV(BUG_REPORT_URL)
            #undef KV
        }
    }
}

void os_info::dump() {
    using namespace std;
    #define KV(k) << #k << ':' << k << endl
    cout
        KV(NAME)
        KV(VERSION)
        KV(ID)
        KV(ID_LIKE)
        KV(PRETTY_NAME)
        KV(VERSION_ID)
        KV(HOME_URL)
        KV(BUG_REPORT_URL);
    #undef KV
}
