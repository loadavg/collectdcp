/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#include "rex_var_assign.h"
#include <regex>
#include <iostream>
using namespace std;

rex_var_assign::rex_var_assign(string line)
{
    smatch cap;
    //regex ex("([A-Za-z0-9_]*)=(.*)");
    if (regex_match(line, cap, regex("([A-Za-z0-9_]+)=(.+)"))) {
        for (auto m : cap) {
            cout << m << endl;
        }
    }
}
