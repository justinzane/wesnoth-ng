/**
 * @file 		sdl2_evt_mgr.cpp is part of wesnoth-ng
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

#include "sdl2_evt_mgr.hpp"

void sdl2_evt_mgr::register_hndlr(SDL_EventType type, const sdl2_evt_hndlr& hndlr) {
    if (hndlr_map_[type].count(hndlr) == 1) {
        throw std::invalid_argument("Handler already registered.");
    }
    hndlr_map_[type].insert(hndlr);
}

void sdl2_evt_mgr::unregister_hndlr(SDL_EventType type, const sdl2_evt_hndlr& hndlr) {
    if (hndlr_map_[type].count(hndlr) == 0) {
        throw std::invalid_argument("Handler not registered.");
    }
    hndlr_map_[type].erase(hndlr);
}

bool sdl2_evt_mgr::is_registered(SDL_EventType type, const sdl2_evt_hndlr& hndlr) {
    if (hndlr_map_[type].count(hndlr) == 0) { return false; } else { return true; }
}


void sdl2_evt_mgr::handle_evt(SDL_Event& evt) {
    if (hndlr_map_[evt.type].empty()) { return; }
    for (auto h : hndlr_map_[evt.type]) {
        h.handle_event(evt);
    }
}

void listen() {
    while (keep_listening_) {
        SDL_Event evt;
        int poll_result = SDL_PollEvent(&evt);
        if (poll_result == 1) {     // If poll_result == 0, no event, move along
            handler(&evt);
        }
    }
}
