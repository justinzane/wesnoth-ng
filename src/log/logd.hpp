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

#include "../defs.hpp"
#include <zmq.hpp>
#include <msgpack.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <tuple>

namespace logd {
// Enums --------------------------------------------------------------------------------------
enum class log_level {
        SILENT,
        ERROR,
        WARN,
        INFO,
        DEBUG,
        BATSHIT
};

enum class log_domain {
        AI,
        ARCH_DEP,
        CMPGN_SRVR,
        DISP_SRVR,
        EDITOR,
        FORMULA,
        EVENTS,
        UI,
        LUA,
        MP,
        NET,
        PATHFIND,
        SCRIPT,
        SERDES,
        SERVER,
        TEST,
        CUTTER,
        EXPLODER,
        VALIDATOR,
        UNIT,
        WESMAGE,
        WHTBRD,
        WIDGETS,
        MSGPACK,
        SDL2,
        ZMQ,
        MISC
};

// Functions ----------------------------------------------------------------------------------
inline std::string domain2name(log_domain& dom) {
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

inline std::string level2name(log_level& lev) {
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

/** @brief Called by client loop for exit. */
bool should_quit();

/** @brief Causes client loop to exit.*/
void do_quit();

/** @brief Output received log messages. */
void write(log_level& lev, log_domain& dom, std::string& msg);

// Variables
bool quit_logd_ {false};
zmq::context_t zmq_cntx_ {1};
zmq::socket_t  zmq_sock_ {zmq_cntx_, ZMQ_PULL};
};

#endif /* LOGD_HPP_ */
