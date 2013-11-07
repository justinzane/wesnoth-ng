/**
 * @file 		layer_bits.hpp is part of Project wesnoth-ng
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
#ifndef LAYER_BITS_HPP_
#define LAYER_BITS_HPP_

enum class layer_bits {
    // you may adjust the following when needed:
    MAX_BORDER           = 3,  /**< maximum border. 3 should be safe even if a larger
                                    border is in use somewhere */
    BITS_FOR_LAYER_GROUP = 4,  /**< store x, y, and layer in one 32 bit integer
                                    4 most significant bits == layer group   => 16 */
    BITS_FOR_Y           = 10, /**< 10 second most significant bits == y     => 1024 */
    BITS_FOR_X_PARITY    = 1,  /**< 1 third most significant bit == x parity => 2 */
    BITS_FOR_LAYER       = 8,  /**< 8 fourth most significant bits == layer  => 256 */
    BITS_FOR_X_OVER_2    = 9   /**< 9 least significant bits == x/2 => 512 (really 1024 for x) */
};

#endif /* LAYER_BITS_HPP_ */
