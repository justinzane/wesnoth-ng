/**
 * @file 		drawing_buffer_key.hpp is part of Project wesnoth-ng
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
#ifndef DRAWING_BUFFER_KEY_HPP_
#define DRAWING_BUFFER_KEY_HPP_

#include "tdrawing_layer.hpp"

#include "../board/map_location.hpp"

namespace gui {
/**
 * In order to render a hex properly it needs to be rendered per row. On
 * this row several layers need to be drawn at the same time. Mainly the
 * unit and the background terrain. This is needed since both can spill
 * in the next hex. The foreground terrain needs to be drawn before to
 * avoid decapitation a unit.
 *
 * In other words:
 * for every layer
 *   for every row (starting from the top)
 *     for every hex in the row
 *       ...
 *
 * this is modified to:
 * for every layer group
 *   for every row (starting from the top)
 *     for every layer in the group
 *       for every hex in the row
 *         ...
 *
 * * Surfaces are rendered per level in a map.
 * * Per level the items are rendered per location these locations are
 *   stored in the drawing order required for units.
 * * every location has a vector with surfaces, each with its own screen
 *   coordinate to render at.
 * * every vector element has a vector with surfaces to render.
 */
class drawing_buffer_key {
    public:
        /**
         * Constructor
         * @param loc
         * @param layer
         */
        drawing_buffer_key(const map_location &loc, tdrawing_layer layer);

        /** Destructor */
        virtual ~drawing_buffer_key();

        /** TODO WRTIEME */
        bool operator<(const drawing_buffer_key &rhs) const {
            return key_ < rhs.key_;
        }

    private:
        unsigned int key_;
        static const tdrawing_layer layer_groups[];
        static const unsigned int max_layer_group;

};

} /* namespace gui */

#endif /* DRAWING_BUFFER_KEY_HPP_ */
