/**
 * @file src/loadscreen.hpp
 * @project The Battle for Wesnoth NG - https://github.com/justinzane/wesnoth-ng
 * @brief 
 * @authors 
 * @copyright Copyright (C) 2005 - 2013 by Joeri Melis <joeri_melis@hotmail.com>
 Part of the Battle for Wesnoth Project http://www.wesnoth.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY.

 See the COPYING file for more details.
 */

/** @file */

#ifndef JM_LOADSCREEN_HPP
#define JM_LOADSCREEN_HPP

class CVideo;

#include "filesystem.hpp"
#include "font.hpp"
#include "gettext.hpp"
#include "image.hpp"
#include "marked-up_text.hpp"
#include "sdl2/rndr_mgr.hpp"
#include "sdl2/loadscreen_mgr.hpp"
#include "sdl_utils.hpp"
#include "log/log.hpp"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <cassert>

class loadscreen {
    public:
        /** @brief Preferred constructor */
        explicit loadscreen(const int percent = 0);

        // Keep default copy constructor

        // Keep default copy assignment

        /** @brief Destructor, dumps the counter values to stderr */
        ~loadscreen() { dump_counters(); }

        /** @brief Starts the stage with identifier id. */
        static void start_stage(char const *id);

        /** @brief Increments the current stage for the progress bar. */
        static void increment_progress();

        /** @brief Function to draw a blank screen. */
        void clear_screen();

        /**@brief A global loadscreen instance that can be used to avoid
         * passing it on to functions that are many levels deep. */
        static loadscreen* global_loadscreen;

    private:
        /** @brief Displays a load progress bar. */
        void draw_screen(const std::string &text);

        // Prohibit default constructor
        loadscreen() = delete;

        // Data members
        SDL_Rect textarea_ {0,0,0,0};
        SDL_Surface* logo_surface_ {nullptr};
        bool logo_drawn_ {false};
        int pby_offset_ {0};
        int prcnt_ {0};

        void dump_counters() const;
};

#endif
