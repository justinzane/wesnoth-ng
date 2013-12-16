/**
 * @file 		sdl2_color.hpp is part of wesnoth-ng
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
#ifndef SDL2_COLOR_HPP_
#define SDL2_COLOR_HPP_

#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdint.h>
#include <cstdint>
#include <string>
#include <cstdio>

/**  @type sdl2_color TODO: WRITEME */
class sdl2_color {
    public:
        sdl2_color(){};
        sdl2_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a){ c_.r = r; c_.g = g; c_.b = b; c_.a = a; };
        virtual ~sdl2_color();

        std::string get_html_color() {
            char hex[9];
            sprintf(hex, "#%2x%2x%2x%2x", c_.r, c_.g, c_.b, c_.a);
            std::string retval = hex;
            return retval;
        }

        SDL_Color* get_sdl_color() { return &c_; }

        /** @brief SDL_Color equality operator */
        inline bool operator==(const sdl2_color& a) {
            return a.c_.r == c_.r && a.c_.g == c_.g && a.c_.b == c_.b && a.c_.a == c_.a;
        }

        /** @brief SDL_Color inequality operator */
        inline bool operator!=(const sdl2_color& a) {
            return a.c_.r != c_.r || a.c_.g != c_.g || a.c_.b != c_.b || a.c_.a != c_.a;
        }

    private:
        SDL_Color c_ {0, 0, 0, 0};

};

#endif /* SDL2_COLOR_HPP_ */
