/**
 * @file 		log_msg.hpp is part of wesnoth-ng
 * @author		justin
 * @date		Nov 22, 2013
 * @copyright   Copyright justin, 2013
 * @section		License GPLv3
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
 * > along with wesnoth-ng.  If not, see http://www.gnu.org/licenses/ .
 * @brief		TODO WRITEME
 * @details		TODO WRITEME
 */
#ifndef LOG_MSG_HPP_
#define LOG_MSG_HPP_

#ifndef __cplusplus
#define __cplusplus 201103L
#endif
#if __cplusplus <= 201103L
#define __cplusplus 201103L
#endif

#include <stdlib.h>
#include <msgpack.hpp>

/** @namespace logd */
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

// Classes ------------------------------------------------------------------------------------

union log_level_t {
        log_level lev;
        int       levint;
        MSGPACK_DEFINE(levint);
};

union log_domain_t {
        log_domain dom;
        int        domint;
        MSGPACK_DEFINE(domint);
};

/**
 * @class logd::log_msg
 * @brief A log message.
 * @details Clients should instantiate a member of this class when they wish to record a
 * message in the log.
 */
class log_msg {
    public:
        log_msg();
        log_msg(log_level lev, log_domain dom, std::string msg);
        log_level level();
        log_domain domain();
        std::string message();
        ~log_msg();
        MSGPACK_DEFINE(lev_.levint, dom_.domint, msg_);

    private:
        log_msg(int levint, int domint, std::string msg);
        log_level_t  lev_;
        log_domain_t dom_;
        std::string  msg_;

};

} /* namespace logd */

#endif /* LOG_MSG_HPP_ */
