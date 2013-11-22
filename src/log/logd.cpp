/**
 * @file 		logd.cpp is part of Project wesnoth-ng
 * @author		justin
 * @date		Nov 21, 2013
 * @copyright   Copyright justin, Nov 21, 2013
 * @section		<license> License
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

#include "logd.hpp"
#include <msgpack.hpp>
#include <zmq.h>
#include <cstdbool>
#include <iostream>
#include <string>

namespace logd {

bool should_quit() { return quit_logd_; }

void do_quit() { quit_logd_ = false; }

void write(log_level& lev, log_domain& dom, std::string& msg) {
    // @todo allow configuration of output streams
    if (lev == log_level::ERROR || lev == log_level::WARN) {
        std::cerr << level2name(lev) << domain2name(dom) << msg << "\n";
    } else {
        std::cout << level2name(lev) << domain2name(dom) << msg << "\n";
    }
}

int main(int argc, char* argv[]) {
    // @todo handle arguments, read config

    // Setup ZMQ
    int linger = ZMQ_SOCK_LINGER;
    zmq_sock_.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
    zmq_sock_.bind(LOG_PULL_ADDR);

    // Main loop ------------------------------------------------------------------------------
    while (!quit_logd_) {
        zmq::message_t zmsg;
        zmq_sock_.recv(&zmsg, 0);
        msgpack::unpacked unpckd;
        msgpack::unpack(&unpckd, reinterpret_cast<char*>(zmsg.data()), zmsg.size());
        msgpack::object obj = unpckd.get();
        std::string log_msg;
        obj.convert(&log_msg);
        //;
    }

} //end main

} //end namespace logd
