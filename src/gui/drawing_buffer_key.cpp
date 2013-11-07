/**
 * @file 		drawing_buffer_key.cpp is part of Project wesnoth-ng
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
#include "drawing_buffer_key.hpp"
#include "layer_bits.hpp"
#include <stdlib.h>

namespace gui {

drawing_buffer_key::drawing_buffer_key(const map_location &loc, tdrawing_layer layer) :
    key_(0) {
    // max_layer_group + 1 is the last valid entry in layer_groups, but it is always > layer
    // thus the first --g is a given => start with max_layer_groups right away
    unsigned int g = max_layer_group;
    while (layer < layer_groups[g]) {
        --g;
    }

    enum {
            SHIFT_LAYER = layer_bits::BITS_FOR_X_OVER_2,
            SHIFT_X_PARITY = layer_bits::BITS_FOR_LAYER + SHIFT_LAYER,
            SHIFT_Y = layer_bits::BITS_FOR_X_PARITY + SHIFT_X_PARITY,
            SHIFT_LAYER_GROUP = layer_bits::BITS_FOR_Y + SHIFT_Y
    };
    if (SHIFT_LAYER_GROUP + BITS_FOR_LAYER_GROUP != sizeof (key_) * 8) {
        throw std::logic_error(
            "Oops: SHIFT_LAYER_GROUP + BITS_FOR_LAYER_GROUP != sizeof(key_) * 8");
    }
    
    // the parity of x must be more significant than the layer but less significant than y.
    // Thus basically every row is split in two: First the row containing all the odd x
    // then the row containing all the even x. Since thus the least significant bit of x is
    // not required for x ordering anymore it can be shifted out to the right.
    const unsigned int x_parity = static_cast<unsigned int>(loc.x) & 1;
    key_ = (g << SHIFT_LAYER_GROUP)
        | (static_cast<unsigned int>(loc.y + MAX_BORDER) << SHIFT_Y);
    key_ |= (x_parity << SHIFT_X_PARITY);
    key_ |= (static_cast<unsigned int>(layer) << SHIFT_LAYER)
        | static_cast<unsigned int>(loc.x + MAX_BORDER) / 2;
}

} /* namespace gui */
