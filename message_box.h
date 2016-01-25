/*
 * project collectd_edit
 * ing. Carlo Capelli
 * Brescia 2015
 * Copyright (c) 2015,2016 Sputnik7
 * License MIT
 */

#ifndef MESSAGE_BOX_H
#define MESSAGE_BOX_H

#include <string>
#include <functional>

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

/**
 * @brief CATCH_SHOW
 *  catch errors from f() and show in box
 * @param f
 *  applicative procedure
 * @return
 *  true if no errors to show
 */
bool CATCH_SHOW(std::function<void()> f);

#endif // MESSAGE_BOX_H
