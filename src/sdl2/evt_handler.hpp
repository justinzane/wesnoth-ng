/**
 * @file 		evt_mgr.hpp is part of wesnoth-ng
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
#ifndef evt_handler_HPP_
#define evt_handler_HPP_

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <functional>
#include <vector>
#include <assert.h>

/**
 * @class evt_mgr
 * @details
 * Any classes that derive from this class will automatically receive sdl events
 * through the handle function for their lifetime, while the event context they
 * were created in is active.
 * @note an event_context object must be initialized before a evt_mgr object the
 * evt_mgr is destroyed.
 * @note this class was stripped from events.[ch]pp and modified to fit SDL2
 */

class evt_handler {
    public:
        /**@brief Constructor */
        evt_handler() {};

        /** Destructor */
        virtual ~evt_handler();

        /** WRITEME */
        virtual void handle_event(const SDL_Event* evt) {}

        /** WRITEME */
        void process_event(const SDL_Event* evt)
            __attribute__((deprecated("Use handle_event."))){
            this->handle_event(evt);
        }

        /** WRITEME */
        virtual bool requires_event_focus() const {
            return hndlr_requires_focus;
        }

        /** WRITEME */
        virtual void process_help_string(int /*mousex*/, int /*mousey*/) {}

        /** WRITEME */
        virtual void process_tooltip_string(int /*mousex*/, int /*mousey*/) {}

    private:
        bool hndlr_requires_focus {false};

};

#endif /* evt_handler_HPP_ */
