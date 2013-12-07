/**
 * @file 		sdl2_evt_mgr.cpp is part of wesnoth-ng
 * @author		justin
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
#include "sdl2_evt_hndlr.hpp"

sdl2_evt_hndlr::sdl2_evt_hndlr(const bool auto_join) {
    SDL_EnableKeyRepeat(0,0); // Fixme
    if (auto_join) {
        assert( !event_contexts.empty());
        event_contexts.back().add_sdl2_evt_mgr(this);
        has_joined_ = true;
    }
}

sdl2_evt_hndlr::~sdl2_evt_hndlr() {
    leave();
}

void sdl2_evt_hndlr::join() {
    if (has_joined_) { leave(); }                           // should not be in multiple contexts
    event_contexts.back().add_sdl2_evt_mgr(this);           // join self
    has_joined_ = true;
    sdl2_evt_mgr_vector members = sdl2_evt_mgr_members();   //instruct members to join
    if (!members.empty()) {
        for (auto i : members) { (*i)->join(); }
    }
}

void sdl2_evt_hndlr::leave() {
    sdl2_evt_mgr_vector members = sdl2_evt_mgr_members();
    if (!members.empty()) {
        for (auto i : members) { (*i)->leave(); }
    } else {
        assert(event_contexts.empty() == false);
    }
    for (std::deque<context>::reverse_iterator i = event_contexts.rbegin();
        i != event_contexts.rend(); ++i) {
        if (i->remove_sdl2_evt_mgr(this)) { break; }
    }
    has_joined_ = false;
}

