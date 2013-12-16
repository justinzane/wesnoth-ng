/**
 * @file 		sdl2_rect.hpp is part of wesnoth-ng
 * @author		justin
 * @date		Dec 5, 2013
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
#ifndef SDL2_RECT_HPP_
#define SDL2_RECT_HPP_

#include <SDL2/SDL.h>

/**  @type sdl2_rect TODO: WRITEME */
class rectangle {
    public:

        /** Less useful constructor. */
        rectangle(){};

        /** Useful constructor. */
        rectangle(int x, int y, int w, int h){ r_.x = x; r_.y = y; r_.w = w; r_.h = h; }

        /** @brief set the  value. */
        void set_x(int x) { r_.x = x; }

        /** @brief set the  value. */
        void set_y(int y) { r_.y = y; }

        /** @brief set the  value. */
        void set_w(int w) { r_.w = w; }

        /** @brief set the  value. */
        void set_h(int h) { r_.h = h; }

        /** @brief get the x value. */
        int x() { return r_.x; }

        /** @brief get the y value. */
        int y() { return r_.y; }

        /** @brief get the w value. */
        int w() { return r_.w; }

        /** @brief get the h value. */
        int h() { return r_.h; }

        /** @brief Get pointer to the actual SDL_Rect. */
        SDL_Rect* get_rect() { return &r_; }

        /** @brief SDL_Rect equality operator */
        bool operator==(const rectangle& a) {
            return (a.r_.x == r_.x && a.r_.y == r_.y && a.r_.w == r_.w && a.r_.h == r_.h);
        }

        /** @brief SDL_Rect inequality operator */
        bool operator!=(const rectangle& a) {
            return (a.r_.x != r_.x || a.r_.y != r_.y || a.r_.w != r_.w || a.r_.h != r_.h);
        }

        /** Destructor */
        virtual ~rectangle();

    private:
        SDL_Rect r_ {0,0,0,0};
};

#endif /* SDL2_RECT_HPP_ */
