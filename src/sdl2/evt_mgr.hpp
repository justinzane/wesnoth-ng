/**
 * @file 		evt_mgr.hpp is part of wesnoth-ng
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

#ifndef evt_mgr_HPP_
#define evt_mgr_HPP_

#if __cplusplus < 201103L
#define __cplusplus 200103L
#endif

//our user-defined double-click event type
#define DOUBLE_CLICK_EVENT          SDL_USEREVENT
#define TIMER_EVENT                (SDL_USEREVENT + 1)
#define HOVER_REMOVE_POPUP_EVENT   (SDL_USEREVENT + 2)
#define DRAW_EVENT                 (SDL_USEREVENT + 3)
#define CLOSE_WINDOW_EVENT         (SDL_USEREVENT + 4)
#define SHOW_HELPTIP_EVENT         (SDL_USEREVENT + 5)

#include "evt_handler.hpp"
#include <SDL2/SDL.h>
#include <exception>
#include <set>
#include <atomic>
#include <unordered_map>

/** @class evt_mgr TODO: WRITEME */
class evt_mgr {
    public:
        /** @brief Get a reference to the global SDL2 Event mgr */
        evt_mgr& get_evt_mgr() { return instance_; }

        /** @brief Register a handler for a specific SDL2 event type.
         * @param type      SDL_Event type
         * @param hndlr     new evt_handler
         * @throws invalid_argument if trying to add a an already registered handler */
        void register_hndlr(SDL_EventType type, const evt_handler& hndlr);

        /**@brief Unegister a handler for a specific SDL2 event type.
         * @param type      SDL_Event type
         * @param hndlr     registered evt_handler
         * @throws invalid_argument if trying to remove a an unregistered handler */
        void unregister_hndlr(SDL_EventType type, const evt_handler& hndlr);

        /** @brief check if event handler is registered */
        bool is_registered(SDL_EventType type, const evt_handler& hndlr);

        /** @brief Dispatch the event to the registered handler(s). */
        void handle_evt(SDL_Event& evt);

        /**@brief Main event handling loop.
         * @details Polls SDL2 for events and dispatches them to handlers. */
        void listen();

        /**@brief Quit the event processing loop. */
        void quit_listening() { keep_listening_ = false; }

    private:
        bool keep_listening_ {true};    /**< When set false, event loop exits. */

        std::unordered_map<SDL_EventType, std::set<evt_handler> > hndlr_map_ {};

        static evt_mgr instance_;  /**< Singleton instance */


        /** Singleton constructor */
        evt_mgr(){};
        /** Singleton copy constructor */
        evt_mgr(evt_mgr const&) = delete;
        /** Singleton copy constructor */
        evt_mgr& operator=(evt_mgr const&) = delete;
        /** Destructor */
        virtual ~evt_mgr();
};

#endif /* evt_mgr_HPP_ */
