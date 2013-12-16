/**
 * @file 		wesnoth_error.hpp is part of wesnoth-ng
 * @author		Justin Zane Chudgar, <justin@justinzane.com>
 * @date		Dec 15, 2013
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

#ifndef WESNOTH_ERROR_HPP_
#define WESNOTH_ERROR_HPP_

#include <string>
#include <exception>

/** @class wesnoth_error TODO: WRITEME */
class wesnoth_error:
    public std::exception {
    public:
        wesnoth_error(){};
        wesnoth_error(const char* msg) { msg_ = (char*)msg; }
        wesnoth_error(const std::string msg) { msg_ = msg.c_str(); }
        virtual ~wesnoth_error(){};
    private:
        char* msg_ {"Unspecified Wesnoth Error".c_str()};
};

#endif /* WESNOTH_ERROR_HPP_ */
