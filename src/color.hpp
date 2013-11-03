/**
 * @file 		color.hpp is part of Project wesnoth-ng
 * @author		justin
 * @date		Nov 3, 2013
 * @copyright   Copyright justin, Nov 3, 2013
 * @section		<license> License
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
 * @brief		TODO WRITEME
 * @details		TODO WRITEME
 */
#ifndef COLOR_HPP_
#define COLOR_HPP_

#include <SDL2/SDL.h>
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <stdio.h>

namespace ng {

class color {
    public:
        virtual color();
        virtual ~color();

        /**
         * @brief converts from CSS "#RRGGBB" notation to a color. Alpha is set to opaque.
         * @param csscolor the string to convert
         * @return the color
         */
        color from_ARGB8888(const Uint8 A, const Uint8 R, const Uint8 G, const Uint8 B);

        /**
         * @brief converts from CSS "#RRGGBB" notation to a color. Alpha is set to opaque.
         * @param csscolor the string to convert
         * @return the color
         */
        color from_ARGB8888(const Uint32 ARGB);

        /**
         * @brief converts from CSS "#RRGGBB" notation to a color. Alpha is set to opaque.
         * @param csscolor the string to convert
         * @return the color
         */
        color from_SDL(SDL_Color c);

        /**
         * @brief converts from CSS "#RRGGBB" notation to a color. Alpha is set to opaque.
         * @param csscolor the string to convert
         * @return the color
         */
        color from_CSS(std::string csscolor);

        SDL_Color to_SDL();
        Uint32 to_ARGB8888();
        std::string to_CSS();

    private:
        float a, r, g, b, Lstar, astar, bstar, alphastar;
};

//const color NORMAL_COLOR = from_ARGB8888(0xDD,0xDD,0xDD,0);
//const color GRAY_COLOR   = from_ARGB8888(0x77,0x77,0x77,0);
//const color LOBBY_COLOR  = (0xBB,0xBB,0xBB,0);
//const color GOOD_COLOR   = (0x00,0xFF,0x00,0);
//const color BAD_COLOR    = (0xFF,0x00,0x00,0);
//const color BLACK_COLOR  = (0x00,0x00,0x00,0);
//const color YELLOW_COLOR = (0xFF,0xFF,0x00,0);
//const color BUTTON_COLOR = (0xBC,0xB0,0x88,0);
//const color PETRIFIED_COLOR = (0xA0,0xA0,0xA0,0);
//const color TITLE_COLOR  = (0xBC,0xB0,0x88,0);
//const color LABEL_COLOR  = (0x6B,0x8C,0xFF,0);
//const color BIGMAP_COLOR = (0xFF,0xFF,0xFF,0};
//const color DISABLED_COLOR = inverse(PETRIFIED_COLOR);



} /* namespace ng */

#endif /* COLOR_HPP_ */
