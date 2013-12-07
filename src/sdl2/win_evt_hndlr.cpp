/**
 * @file 		win_evt_hndlr.cpp is part of wesnoth-ng
 * @author		Justin Zane Chudgar, <justin@justinzane.com>
 * @date		Dec 6, 2013
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

#include "win_evt_hndlr.hpp"
#include "sdl2_rndr_mgr.hpp"
#include "preferences.hpp"
#include <stdio.h>

void handle(SDL_Event* evt) {
    if (evt->type == SDL_WINDOWEVENT) {
        SDL_WindowEvent* wevt = (SDL_WindowEvent*)evt;
        switch (evt->window.event) {
            case SDL_WINDOWEVENT_SHOWN: break;
            case SDL_WINDOWEVENT_HIDDEN: break;
            case SDL_WINDOWEVENT_EXPOSED: break;
            case SDL_WINDOWEVENT_MOVED: break;
            case SDL_WINDOWEVENT_RESIZED:
                handle_window_resized(wevt); break;
            case SDL_WINDOWEVENT_MINIMIZED: break;
            case SDL_WINDOWEVENT_MAXIMIZED: break;
            case SDL_WINDOWEVENT_RESTORED: break;
            case SDL_WINDOWEVENT_ENTER: break;
            case SDL_WINDOWEVENT_LEAVE: break;
            case SDL_WINDOWEVENT_FOCUS_GAINED: break;
            case SDL_WINDOWEVENT_FOCUS_LOST: break;
            case SDL_WINDOWEVENT_CLOSE: break;
            default:
                fprintf(stdout, "Window %d got unknown event %d\n", evt->window.windowID,
                        evt->window.event);
                break;
        }
    }
}

void handle_window_resized(SDL_WindowEvent* wevt) {
    sdl2_rndr_mgr rm = sdl2_rndr_mgr::get_rndr_mgr();
    if (wevt->data1 >= preferences::min_allowed_width() &&
        wevt->data2 >= preferences::min_allowed_height()) {
        preferences::set_resolution(rm.get_window_size());
    }
}
