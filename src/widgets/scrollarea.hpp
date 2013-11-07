/*
 Copyright (C) 2004 - 2013 by Guillaume Melquiond <guillaume.melquiond@gmail.com>
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

#ifndef SCROLLAREA_HPP_INCLUDED
#define SCROLLAREA_HPP_INCLUDED

#include "scrollbar.hpp"

#include "../events.hpp"
#include "../gui/video.hpp"

#include <SDL2/SDL.h>

#include <cstdbool>

namespace gui {

class scrollarea: public widget {
    public:
        /**
         * @brief Create a zone with automatic handling of scrollbar.
         * @param video     the display
         * @param auto_join ??? TODO WRITEME
         */
        scrollarea(ui_window &video, bool auto_join = true);

        /**
         * @brief hide/show the scrollbar
         * @param value true --> hide, false --> show
         */
        virtual void hide(bool value = true) __attribute__((deprecated("Use hide().")));

    protected:
        /**
         * @brief TODO WRITEME
         */
        virtual handler_vector handler_members();

        /**
         * @brief TODO WRITEME
         */
        virtual void update_location(SDL_Rect const &rect);

        /**
         * @brief Handler for SDL_Events
         * @param event
         * @note Currently ignores all but SDL_MouseWheelEvents
         */
        virtual void handle_event(const SDL_Event& event);

        /**
         * @brief TODO WRITEME
         */
        virtual void process_event();

        /**
         * @brief TODO WRITEME
         */
        virtual void scroll(unsigned int pos) = 0;

        /**
         * @brief TODO WRITEME
         */
        virtual void set_inner_location(SDL_Rect const &rect) = 0;

        /**
         * @brief TODO WRITEME
         */
        SDL_Rect inner_location() const;

        /**
         * @brief TODO WRITEME
         */
        unsigned scrollbar_width() const;

        /**
         * @brief TODO WRITEME
         */
        unsigned get_position() const;

        /**
         * @brief TODO WRITEME
         */
        unsigned get_max_position() const;

        /**
         * @brief TODO WRITEME
         */
        void set_position(unsigned pos);

        /**
         * @brief TODO WRITEME
         */
        void adjust_position(unsigned pos);

        /**
         * @brief TODO WRITEME
         */
        void move_position(int dep);

        /**
         * @brief TODO WRITEME
         */
        void set_shown_size(unsigned h);

        /**
         * @brief TODO WRITEME
         */
        void set_full_size(unsigned h);

        /**
         * @brief TODO WRITEME
         */
        void set_scroll_rate(unsigned r);

        /**
         * @brief TODO WRITEME
         */
        bool has_scrollbar() const;

    private:
        scrollbar scrollbar_;   /**< TODO WRITEME */
        int old_position_;      /**< TODO WRITEME */
        bool recursive_;        /**< TODO WRITEME */
        bool shown_scrollbar_;  /**< true if widget hidden, false if shown */
        unsigned shown_size_;   /**< TODO WRITEME */
        unsigned full_size_;    /**< TODO WRITEME */
        void test_scrollbar();  /**< TODO WRITEME */
};

}  // end namespace gui

#endif
