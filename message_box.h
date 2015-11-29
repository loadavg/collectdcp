/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright 2015 Sputnik7
 */

#ifndef MESSAGE_BOX_H
#define MESSAGE_BOX_H

#include <string>

/**
 * @brief message_box
 *  display standard modal message box of @param msg
 * @param msg
 *  text to display
 * @return
 *  TBD user choice
 */
enum mode { OK, YES_NO };
bool message_box(std::string msg, mode md = OK);

#endif // MESSAGE_BOX_H
