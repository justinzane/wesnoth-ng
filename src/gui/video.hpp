/**
 * @file        ui_window.hpp is part of Project wesnoth-ng
 * @author      Justin Zane Chudgar <justin@justinzane.com>
 * @author      David White <dave@whitevine.net>
 * @date        Nov 5, 2013
 * @copyright   Copyright @authors, 2003-2013
 * @section     <license> License GPLv2
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
 * > along with wesnoth-ng.  If not, see <a href="http://www.gnu.org/licenses/">
 * > http://www.gnu.org/licenses/</a>.
 * @brief       TODO WRITEME
 * @note This has been brutally hacked for SDL2.
 * @note Only 32 bits per pixel, ARGB color are supported.
 * @note Unit test code has been removed since it needs to be rewritten.
 */
#ifndef VIDEO_HPP_INCLUDED
#define VIDEO_HPP_INCLUDED

#include "sdl_utils.hpp"

#include "../events.hpp"
#include "../lua_jailbreak_exception.hpp"

#include <boost/noncopyable.hpp>
#include <stddef.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>

#include <atomic>
#include <cstdbool>
#include <memory>
#include <string>

surface display_format_alpha(surface surf);
surface get_video_surface();
SDL_Rect screen_area();

bool non_interactive();

void update_rect(size_t x, size_t y, size_t w, size_t h);
void update_rect(const SDL_Rect& rect);
void update_whole_screen();

/**
 * @brief TODO WRITEME
 */
class ui_window:
    // Why noncopyable?
    private boost::noncopyable {
    public:
        ui_window();
        ~ui_window();

        //functions to get the dimensions of the current video-mode
        int getx() const;
        int gety() const;

        //blits a surface with black as alpha
        void blit_surface(int x,
                          int y,
                          surface surf,
                          SDL_Rect* srcrect = NULL,
                          SDL_Rect* clip_rect = NULL);

        surface& getSurface();

        bool isFullScreen() const {
            return (SDL_GetWindowFlags(ui_window_) && SDL_WINDOW_FULLSCREEN);
        }

        /** @brief TODO WTF? */
        class quit: public tlua_jailbreak_exception {
            public:
                quit() :
                    tlua_jailbreak_exception() {
                }
            private:
                IMPLEMENT_LUA_JAILBREAK_EXCEPTION (quit)
        };

        /**
         * @brief set 'help strings'.
         * @details A 'help string' is like a tooltip, but it appears at the bottom
         * of the screen, so as to not be intrusive. Setting a help string sets what
         * is currently displayed there.
         * @return handle for the string that was set.
         */
        int set_help_string(const std::string& str);

        /**
         * @brief clear 'help strings'.
         * @details A 'help string' is like a tooltip, but it appears at the bottom
         * of the screen, so as to not be intrusive. Setting a help string sets what
         * is currently displayed there.
         * @param handle    handle of the string to clear
         */
        void clear_help_string(int handle);

        /**
         * @brief clear all 'help strings'.
         * @details A 'help string' is like a tooltip, but it appears at the bottom
         * of the screen, so as to not be intrusive. Setting a help string sets what
         * is currently displayed there.
         */
        void clear_all_help_strings();

        /** @brief Prevent the screen being redrawn. */
        inline void lock_updates() { updates_locked_ = true; }

        /** @brief Allow the screen to be redrawn. */
        inline void unlock_updates() { updates_locked_ = false; }

        /** @brief returns true if updates are locked. */
        inline bool update_locked() const { return updates_locked_;}

        /** @brief Prevent the screen being resized. */
        inline void lock_resizes() { updates_locked_ = true; }

        /** @brief Allow the screen to be resized. */
        inline void unlock_resizes() { updates_locked_ = false; }

        /** @brief returns true if resizes are locked. */
        inline bool resizes_locked() const { return updates_locked_;}

    private:
        void initSDL();

        SDL_Window* ui_window_;
        SDL_Renderer* ui_renderer_;
        SDL_Surface* ui_surface_;
        SDL_Texture* ui_texture_;
        int help_string_;
        std::atomic<bool> updates_locked_;
        std::atomic<bool> resizes_locked_;
};

class resize_monitor:
    public events::pump_monitor {
        void process(events::pump_info &info);
};

#endif
