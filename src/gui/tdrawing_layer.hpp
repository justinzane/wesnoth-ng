/**
 * @file 		tdrawing_layer.hpp is part of Project wesnoth-ng
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
#ifndef TDRAWING_LAYER_HPP_
#define TDRAWING_LAYER_HPP_

namespace gui {

/**
 * @brief TODO WRITEME
 * @details The layers to render something on. This value should never be stored it's the
 * internal drawing order and adding removing and reordering the layers should be safe.
 * If needed in WML use the name and map that to the enum value.
 */
enum class tdrawing_layer {
        LAYER_TERRAIN_BG, /**< Layer for the terrain drawn behind the unit. */
        LAYER_GRID_TOP, /**< Top half part of grid image */
        LAYER_MOUSEOVER_OVERLAY, /**< Mouseover overlay used by editor*/
        LAYER_FOOTSTEPS, /**< Footsteps showing path from unit to mouse */
        LAYER_MOUSEOVER_TOP, /**< Top half of image following the mouse */
        LAYER_UNIT_FIRST, /**< Reserve layers to be selected for WML. */
        LAYER_UNIT_BG = LAYER_UNIT_FIRST + 10, /**< Used for the ellipse behind the unit. */
        LAYER_UNIT_DEFAULT = LAYER_UNIT_FIRST + 40, /**< default layer for drawing units */
        LAYER_TERRAIN_FG = LAYER_UNIT_FIRST + 50, /**< Layer for the terrain drawn in front of the unit. */
        LAYER_GRID_BOTTOM, /**< Used for the bottom half part of grid image. Should be under moving units, to avoid masking south move. */
        LAYER_UNIT_MOVE_DEFAULT = LAYER_UNIT_FIRST + 60 /**< default layer for drawing moving units */,
        LAYER_UNIT_FG = LAYER_UNIT_FIRST + 80, /**< Used for the ellipse in front of the unit. */
        LAYER_UNIT_MISSILE_DEFAULT = LAYER_UNIT_FIRST + 90, /**< default layer for missile frames*/
        LAYER_UNIT_LAST = LAYER_UNIT_FIRST + 100,
        LAYER_REACHMAP, /**< "black stripes" on unreachable hexes. */
        LAYER_MOUSEOVER_BOTTOM, /**< Bottom half of image following the mouse */
        LAYER_FOG_SHROUD, /**< Fog and shroud. */
        LAYER_ARROWS, /**< Arrows from the arrows framework. Used for planned moves display. */
        LAYER_ACTIONS_NUMBERING, /**< Move numbering for the whiteboard. */
        LAYER_SELECTED_HEX, /**< Image on the selected unit */
        LAYER_ATTACK_INDICATOR, /**< Layer which holds the attack indicator. */
        LAYER_UNIT_BAR, /**< Unit bars and overlays are drawn on this layer (for testing here). */
        LAYER_MOVE_INFO, /**< Movement info (defense%, etc...). */
        LAYER_LINGER_OVERLAY, /**< The overlay used for the linger mode. */
        LAYER_BORDER, /**< The border of the map. */
        LAYER_LAST_LAYER /**< Don't draw to this layer it's a dummy to size the vector. */
};


} /* namespace gui */

#endif /* TDRAWING_LAYER_HPP_ */
