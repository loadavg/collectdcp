/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#ifndef PROCESS_RUN_H
#define PROCESS_RUN_H

#include <string>

/**
 * @brief The process_run class
 *  execute command and capture output
 */
class process_run
{
public:
    process_run(std::string cmd, std::string pwd, bool add_stderr = true);
    std::string result;
};

#endif // PROCESS_RUN_H
