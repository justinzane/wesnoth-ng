/**
 * @file 		sdl2_evt_mgr.hpp is part of wesnoth-ng
 * @author	    Justin Zane Chudgar
 * @date		2013
 * @copyright   Copyright justin, 2013-2014
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
#ifndef SDL2_EVT_MGR_HPP_
#define SDL2_EVT_MGR_HPP_

#include "events.hpp"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <assert.h>

/**
 * @class sdl2_evt_mgr
 * @details
 * Any classes that derive from this class will automatically receive sdl events
 * through the handle function for their lifetime, while the event context they
 * were created in is active.
 * @note an event_context object must be initialized before a sdl2_evt_mgr object the
 * sdl2_evt_mgr is destroyed.
 * @note this class was stripped from events.[ch]pp and modified to fit SDL2
 */
class sdl2_evt_hndlr {
    public:
        /** Constructor */
        sdl2_evt_hndlr();
        sdl2_evt_hndlr(const bool auto_join);

        /** Destructor */
        virtual ~sdl2_evt_hndlr();

        /** WRITEME */
        virtual void handle_event(const SDL_Event& event) = 0;

        /** WRITEME */
        virtual void process_event() {}

        /** WRITEME */
        virtual void draw() {}

        /** WRITEME */
        virtual void volatile_draw() {}

        /** WRITEME */
        virtual void volatile_undraw() {}

        /** WRITEME */
        virtual bool requires_event_focus(const SDL_Event* = nullptr) const {
            return false;
        }

        /** WRITEME */
        virtual void process_help_string(int /*mousex*/, int /*mousey*/) {}

        /** WRITEME */
        virtual void process_tooltip_string(int /*mousex*/, int /*mousey*/) {}

        /** WRITEME */
        virtual void join(); /*joins the current event context*/

        /** WRITEME */
        virtual void leave(); /*leave the event context*/


        /** WRITEME */
        virtual std::vector<sdl2_evt_hndlr*> sdl2_evt_mgr_members() {
            return std::vector<sdl2_evt_hndlr*>();
        }

    private:
        bool has_joined_ {false};
};

#endif /* SDL2_EVT_MGR_HPP_ */
