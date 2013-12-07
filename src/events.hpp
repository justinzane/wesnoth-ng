/**
 * @file src/events.hpp
 * @project The Battle for Wesnoth NG - https://github.com/justinzane/wesnoth-ng
 * @brief 
 * @authors 
 * @copyright Copyright (C) 2003 - 2013 by David White <dave@whitevine.net>
 Part of the Battle for Wesnoth Project http://www.wesnoth.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY.

 See the COPYING file for more details.
 */

#ifndef EVENTS_HPP_INCLUDED
#define EVENTS_HPP_INCLUDED

#include "SDL2/SDL.h"
#include <vector>

namespace events {

void focus_handler(const handler* ptr);
void cycle_focus();

bool has_focus(const handler* ptr, const SDL_Event* event);

//event_context objects control the handler objects that SDL events are sent
//to. When an event_context is created, it will become the current event context.
//event_context objects MUST be created in LIFO ordering in relation to each other,
//and in relation to handler objects. That is, all event_context objects should be
//created as automatic/stack variables.
//
//handler objects need not be created as automatic variables (e.g. you could put
//them in a vector) however you must guarantee that handler objects are destroyed
//before their context is destroyed
struct event_context {
        event_context();
        ~event_context();
};

//causes events to be dispatched to all handler objects.
void pump();

struct pump_info {
        pump_info() :
            resize_dimensions(), ticks_(0) {
        }
        std::pair<int, int> resize_dimensions;
        int ticks(unsigned *refresh_counter = nullptr, unsigned refresh_rate = 1);
    private:
        int ticks_;  //0 if not calculated
};

class pump_monitor {
//pump_monitors receive notification after an events::pump() occurs
    public:
        pump_monitor();
        virtual ~pump_monitor();
        virtual void process(pump_info& info) = 0;
};

void raise_process_event();
void raise_draw_event();
void raise_volatile_draw_event();
void raise_volatile_undraw_event();
void raise_help_string_event(int mousex, int mousey);

/**
 * Is the event an input event?
 *
 * @returns                       Whether or not the event is an input event.
 */
bool is_input(const SDL_Event& event);

/** Discards all input events. */
void discard_input();

}

#if ! SDL_VERSION_ATLEAST(2,0,0)

/**
 * Removes events from the queue.
 *
 * This emulates the function available in SDL 2.0.
 *
 * @param type                    The type of event to flush.
 */
void SDL_FlushEvent(Uint32 type);

#endif

#endif
