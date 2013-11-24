/**
 * @file src/video.hpp
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
#ifndef VIDEO_HPP_INCLUDED
#define VIDEO_HPP_INCLUDED

#include "events.hpp"
#include "exceptions.hpp"
#include "font.hpp"
#include "global.hpp"
#include "image.hpp"
#include "log/log.hpp"
#include "lua_jailbreak_exception.hpp"
#include "preferences.hpp"
#include "preferences_display.hpp"
#include "sdl_utils.hpp"
#include <atomic>
#include <boost/utility.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include <map>
#include <algorithm>

SDL_Window* window_;

SDL_Surface display_format_alpha(SDL_Surface surf);

SDL_Surface get_video_surface();

SDL_Rect screen_area();

bool non_interactive();

//which areas of the screen will be updated when the buffer is flipped?
void update_rect(size_t x, size_t y, size_t w, size_t h);
void update_rect(const SDL_Rect* rect);
void update_whole_screen();

class CVideo: private boost::noncopyable {
    public:
        CVideo() { initSDL(); }
        ~CVideo() { SDL_Quit(); }

        /** @brief return the width of the current window. */
        inline int getx() const {
            int x,y;
            SDL_GetWindowSize(window_, &x, &y);
            return x;
        }

        /** @brief return the width of the current window. */
        inline int gety() const {
            int x,y;
            SDL_GetWindowSize(window_, &x, &y);
            return y;
        }

        void blit_surface(int x,
                          int y,
                          SDL_Surface surf,
                          SDL_Rect* srcrect = nullptr,
                          SDL_Rect* clip_rect = nullptr);
        void flip();

        SDL_Surface* getSurface();

        bool isFullScreen() const;

        struct error: public game::error {
                error() :
                    game::error("Video initialization failed") {
                }
        };

        class quit: public tlua_jailbreak_exception {
            public:

                quit() :
                    tlua_jailbreak_exception() {
                }

            private:

                IMPLEMENT_LUA_JAILBREAK_EXCEPTION(quit)
        };

        //functions to set and clear 'help strings'. A 'help string' is like a tooltip, but it appears
        //at the bottom of the screen, so as to not be intrusive. Setting a help string sets what
        //is currently displayed there.
        int set_help_string(const std::string& str);
        void clear_help_string(int handle);
        void clear_all_help_strings();

        //function to stop the screen being redrawn. Anything that happens while
        //the update is locked will be hidden from the user's view.
        //note that this function is re-entrant, meaning that if lock_updates(true)
        //is called twice, lock_updates(false) must be called twice to unlock
        //updates.
        void lock_updates(bool value);
        bool update_locked() const;

    private:
        /** @brief WRITEME */
        void initSDL();

        /** @brief variables for help strings */
        int help_string_ {0};

        /** @brief WRITEME */
        int updatesLocked_ {0};
};

//an object which will lock the display for the duration of its lifetime.
struct update_locker {
        update_locker(CVideo& v, bool lock = true) :
            video(v), unlock(lock) {
            if (lock) {
                video.lock_updates(true);
            }
        }

        ~update_locker() {
            unlock_update();
        }

        void unlock_update() {
            if (unlock) {
                video.lock_updates(false);
                unlock = false;
            }
        }

    private:
        CVideo& video;
        bool unlock;
};

class resize_monitor: public events::pump_monitor {
        void process(events::pump_info &info);
};

//an object which prevents resizing of the screen occurring during
//its lifetime.
struct resize_lock {
        resize_lock();
        ~resize_lock();
};

#endif
