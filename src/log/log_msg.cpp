/**
 * @file 		log_msg.cpp is part of wesnoth-ng
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
#include <log/log_msg.hpp>

/** @namespace logd */
namespace logd {
log_msg::log_msg() {
    lev_.lev = log_level::BATSHIT;
    dom_.dom = log_domain::MISC;
    msg_ = "";
};

log_msg::log_msg(log_level lev, log_domain dom, std::string msg) {
    lev_.lev = lev;
    dom_.dom = dom;
    msg_ = msg;
};

log_level log_msg::level() { return lev_.lev; }
log_domain log_msg::domain() { return dom_.dom; }
std::string log_msg::message() { return msg_; }

} /* namespace logd */
