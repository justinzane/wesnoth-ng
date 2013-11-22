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

std::string domain2name(log_domain dom) {
    std::string retval;
    switch (dom) {
        case log_domain::AI:
            retval = "AI         "; break;
        case log_domain::ARCH_DEP:
            retval = "Arch. Dep. "; break;
        case log_domain::CMPGN_SRVR:
            retval = "CampaignD  "; break;
        case log_domain::DISP_SRVR:
            retval = "DisplayD   "; break;
        case log_domain::EDITOR:
            retval = "Editor     "; break;
        case log_domain::FORMULA:
            retval = "Formula    "; break;
        case log_domain::EVENTS:
            retval = "GameEvents "; break;
        case log_domain::UI:
            retval = "UI         "; break;
        case log_domain::LUA:
            retval = "Lua        "; break;
        case log_domain::MP:
            retval = "Multiplayer"; break;
        case log_domain::NET:
            retval = "Networking "; break;
        case log_domain::PATHFIND:
            retval = "Pathfind   "; break;
        case log_domain::SCRIPT:
            retval = "Scripting  "; break;
        case log_domain::SERDES:
            retval = "SerDes     "; break;
        case log_domain::SERVER:
            retval = "WesnothD   "; break;
        case log_domain::TEST:
            retval = "Test       "; break;
        case log_domain::CUTTER:
            retval = "Cutter     "; break;
        case log_domain::EXPLODER:
            retval = "Exploder   "; break;
        case log_domain::VALIDATOR:
            retval = "Validator  "; break;
        case log_domain::UNIT:
            retval = "Units      "; break;
        case log_domain::WESMAGE:
            retval = "Wesmage    "; break;
        case log_domain::WHTBRD:
            retval = "Whiteboard "; break;
        case log_domain::WIDGETS:
            retval = "UI/Widgets "; break;
        case log_domain::MISC:
            retval = "Misc       "; break;
        default:
            retval = "Unknown    ";
    }
    return retval;
}

std::string level2name(log_level lev) {
    std::string retval;
    switch (lev) {
        case log_level::ERROR:
            retval = "ERROR  : "; break;
        case log_level::WARN:
            retval = "WARNING: "; break;
        case log_level::INFO:
            retval = "Info   : "; break;
        case log_level::DEBUG:
            retval = "Debug  : "; break;
        case log_level::BATSHIT:
            retval = "XDebug : "; break;
        default:
            retval = "         ";
    }
    return retval;
}

bool should_quit() { return quit_logd_; }

void do_quit() { quit_logd_ = false; }

void write(log_msg& msg) {
    // @todo allow configuration of output streams
    if (msg.level() == log_level::ERROR || msg.level() == log_level::WARN) {
        std::cerr << level2name(msg.level()) << domain2name(msg.domain()) << msg.message() << "\n";
    } else {
        std::cout << level2name(msg.level()) << domain2name(msg.domain()) << msg.message() << "\n";
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
        log_msg msg;
        obj.convert(&msg);
        //;
    }
    return 0;
} //end main

} //end namespace logd
