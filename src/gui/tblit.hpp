/**
 * @file 		tblit.hpp is part of Project wesnoth-ng
 * @author		justin
 * @date		Nov 6, 2013
 * @copyright   Copyright justin, Nov 6, 2013
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
#ifndef TBLIT_HPP_
#define TBLIT_HPP_

#include "drawing_buffer_key.hpp"
#include "sdl_utils.hpp"
#include "tdrawing_layer.hpp"

#include "../board/map_location.hpp"

#include <vector>
#include <SDL2/SDL_rect.h>

namespace gui {

/** Helper structure for rendering the terrains. */
class tblit {
    public:
        /**
         * @brief TODO WRITEME
         * @param layer
         * @param loc
         * @param x
         * @param y
         * @param surf
         * @param clip
         */
        tblit(const tdrawing_layer layer,
              const map_location& loc,
              const int x,
              const int y,
              const surface& surf,
              const SDL_Rect& clip) :
            x_(x), y_(y), surf_list_(1, surf), clip_(clip), key_(loc, layer) {}

        /**
         * @brief TODO WRITEME
         * @param layer
         * @param loc
         * @param x
         * @param y
         * @param surf
         * @param clip
         */
        tblit(const tdrawing_layer layer,
              const map_location& loc,
              const int x,
              const int y,
              const std::vector<surface>& surf,
              const SDL_Rect& clip) :
            x_(x), y_(y), surf_list_(surf), clip_(clip), key_(loc, layer) {}

        virtual ~tblit();

        int x() const { return x_; }
        int y() const { return y_; }
        const std::vector<surface> &surf() const { return surf_list_; }
        const SDL_Rect &clip() const { return clip_; }
        bool operator<(const tblit &rhs) const { return key_ < rhs.key_; }

    private:
        int x_; /**< x screen coordinate to render at. */
        int y_; /**< y screen coordinate to render at. */
        std::vector<surface> surf_list_; /**< surface(s) to render. */
        SDL_Rect clip_; /**<
         * The clipping area of the source if
         * omitted the entire source is used.
         */
        drawing_buffer_key key_;
};

} /* namespace gui */

#endif /* TBLIT_HPP_ */
