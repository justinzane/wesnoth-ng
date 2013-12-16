/**
 * @file 		sdl2_loadscreen_mgr.hpp is part of wesnoth-ng
 * @author		Justin Zane Chudgar, <justin@justinzane.com>
 * @date		Dec 7, 2013
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

#ifndef SDL2_LOADSCREEN_MGR_HPP_
#define SDL2_LOADSCREEN_MGR_HPP_

#include "../loadscreen.hpp"
#include "rndr_mgr.hpp"
#include <stdlib.h>
#include <mutex>

/** @class sdl2_loadscreen_mgr TODO: WRITEME */
class sdl2_loadscreen_mgr {
    public:
        sdl2_loadscreen_mgr& get_loadscreen_mgr() {
            static sdl2_loadscreen_mgr instance_;
            return instance_;
        }

    private:
        bool owns_ {false};

        void reset();

        sdl2_loadscreen_mgr(){
            (loadscreen::global_loadscreen == nullptr) ? owns_ = true : owns_ = false;
            if (owns_) {
                loadscreen::global_loadscreen = new loadscreen(screen);
                loadscreen::global_loadscreen->clear_screen();
            }
        }

        sdl2_loadscreen_mgr(const sdl2_loadscreen_mgr& rs) = delete;

        sdl2_loadscreen_mgr& operator=(const sdl2_loadscreen_mgr& rs) = delete;

        ~sdl2_loadscreen_mgr(){};

};

#endif /* SDL2_LOADSCREEN_MGR_HPP_ */
