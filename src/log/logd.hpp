/**
 * @file 		logd.hpp is part of Project wesnoth-ng
 * @author		justin
 * @date		Nov 21, 2013
 * @copyright   Copyright justin, Nov 21, 2013
 * @section		License GPLv2
 * > wesnoth-ng is free software: you can redistribute it and/or modify
 * > it under the terms of the GNU General Public License as published by
 * > the Free Software Foundation, either version 3 of the License, or
 * > (at your option) any later version.
 * >
 * > wesnoth-ng is distributed in the hope that it will be useful,
 * > but WITHOUT ANY WARRANTY; without even the implied warranty of
 * > MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * > GNU General Public License for more details.
 * >
 * > You should have received a copy of the GNU General Public License
 * > along with wesnoth-ng.  If not, see <a href="http://www.gnu.org/licenses/">
 * > http://www.gnu.org/licenses/</a>.
 * @brief		TODO WRITEME
 * @details		TODO WRITEME
 */
#ifndef LOGD_HPP_
#define LOGD_HPP_

#ifndef __cplusplus
#define __cplusplus 201103L
#endif
#if __cplusplus <= 201103L
#define __cplusplus 201103L
#endif

#include "log_msg.hpp"
#include "../defs.hpp"
#include <zmq.hpp>
#include <msgpack.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <tuple>

namespace logd {
// Functions ----------------------------------------------------------------------------------
/** @brief converts an element of the log_domain enum into a corresponding string. */
std::string domain2name(log_domain dom);

/** @brief converts an element of the log_level enum into a corresponding string. */
std::string level2name(log_level lev);

/** @brief Called by client loop for exit. */
bool should_quit();

/** @brief Causes client loop to exit.*/
void do_quit();

/** @brief Output received log messages. */
void write(log_msg& msg);

// Variables
bool quit_logd_ {false};
zmq::context_t zmq_cntx_ {1};
zmq::socket_t  zmq_sock_ {zmq_cntx_, ZMQ_PULL};
};

#endif /* LOGD_HPP_ */
