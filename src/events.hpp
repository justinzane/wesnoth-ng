/**
 * @author David White <dave@whitevine.net>
 * @author Justin Zane Chudgar <justin@justinzane.com>
 * @copyright Copyright (C) 2003 - 2013 by @authors
 * @file events.hpp is Part of the Battle for Wesnoth Project @link http://www.wesnoth.org/
 * @section <license> License GPLv2
 * This program is free software; you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY.
 * See the COPYING file for more details.
 * @note this file has been heavily modified as part of the migration to SDL2
 */

#ifndef EVENTS_HPP_INCLUDED
#define EVENTS_HPP_INCLUDED

//#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_version.h>
#include <cstddef>
#include <vector>

/* Commented out for SDL2
//our user-defined double-click event type
#define DOUBLE_CLICK_EVENT SDL_USEREVENT
#define TIMER_EVENT (SDL_USEREVENT + 1)
#define HOVER_REMOVE_POPUP_EVENT (SDL_USEREVENT + 2)
#define DRAW_EVENT (SDL_USEREVENT + 3)
#define CLOSE_WINDOW_EVENT (SDL_USEREVENT + 4)
#define SHOW_HELPTIP_EVENT (SDL_USEREVENT + 5) */

namespace events {

/**
 * @brief Wesnoth handlers for SDL events.
 * @details Any classes that derive from this class will automatically receive sdl events
 * through the handle function for their lifetime, while the event context they were created
 * in is active.
 * @note an event_context object must be initialized before a handler object can be
 * initialized, and the event_context must be destroyed after the handler is destroyed.
 */
class handler {
    public:
        /** @brief TODO WRITEME */
        virtual void handle_event(const SDL_Event& event) = 0;

        /** @brief TODO WRITEME */
        virtual void process_event() {}

        /** @brief TODO WRITEME */
        virtual void draw() {}

        /** @brief TODO WRITEME */
        virtual void volatile_draw() {}

        /** @brief TODO WRITEME */
        virtual void volatile_undraw() {}

        /** @brief TODO WRITEME */
        virtual bool requires_event_focus(const SDL_Event * = NULL) const { return false; }

        /** @brief TODO WRITEME */
        virtual void process_help_string(int /*mousex*/, int /*mousey*/) {}

        /** @brief TODO WRITEME */
        virtual void process_tooltip_string(int /*mousex*/, int /*mousey*/) {}

        /** @brief joins the current event context */
        virtual void join();

        /** @brief leave the event context */
        virtual void leave();

    protected:
        handler(const bool auto_join = true);
        virtual ~handler();
        virtual std::vector<handler*> handler_members() {
            return std::vector<handler*>();
        }

    private:
        bool has_joined_;
};

/** @brief TODO WRITEME */
void focus_handler(const handler* ptr);

/** @brief TODO WRITEME */
void cycle_focus();

/**
 * @brief TODO WRITEME
 */
bool has_focus(const handler* ptr, const SDL_Event* event);

/**
 * @brief TODO WRITEME
 * @details event_context objects control the handler objects that SDL events are sent
 * to. When an event_context is created, it will become the current event context.
 * event_context objects MUST be created in LIFO ordering in relation to each other,
 * and in relation to handler objects. That is, all event_context objects should be
 * created as automatic/stack variables.
 *
 * handler objects need not be created as automatic variables (e.g. you could put
 * them in a vector) however you must guarantee that handler objects are destroyed
 * before their context is destroyed
 */
struct event_context {
    public:
        event_context();
        ~event_context();
};

/**
 * @brief causes events to be dispatched to all handler objects.
 */
void pump();

struct pump_info {
    public:
        /**
         * @brief TODO WRITEME
         */
        pump_info() :
            resize_dimensions(),
            ticks_(0) {}

        /**
         * @brief TODO WRITEME
         */
        std::pair<int, int> resize_dimensions;

        /**
         * @brief TODO WRITEME
         */
        int ticks(unsigned *refresh_counter = NULL, unsigned refresh_rate = 1);

    private:
        /**
         * @brief TODO WRITEME
         */
        int ticks_;  //0 if not calculated
};

/**
 * @brief pump_monitors receive notification after an events::pump() occurs
 */
class pump_monitor {
    public:
        pump_monitor();
        virtual ~pump_monitor();

        /**
         * @brief todo WRITEME
         * @param info
         */
        virtual void process(pump_info& info) = 0;
};

/** @brief TODO WRITEME */
void raise_process_event();

/** @brief TODO WRITEME */
void raise_draw_event();

/** @brief TODO WRITEME */
void raise_volatile_draw_event();

/** @brief TODO WRITEME */
void raise_volatile_undraw_event();

/** @brief TODO WRITEME */
void raise_help_string_event(int mousex, int mousey);

/**
 * @brief Is the event an input event?
 */
bool is_input(const SDL_Event& event);

/**
 * @brief Discards all input events.
 */
void discard_input();

} // end namespace events

typedef std::vector<events::handler*> handler_vector;

#endif
