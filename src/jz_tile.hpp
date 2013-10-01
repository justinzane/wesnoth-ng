/**
 * @file 		jz_tile.hpp is part of Project wesnoth-old
 * @author		justin
 * @date		Sep 29, 2013
 * @copyright   Copyright justin, Sep 29, 2013
 * @section		<license> License
 * > wesnoth-old is free software: you can redistribute it and/or modify
 * > it under the terms of the GNU General Public License as published by
 * > the Free Software Foundation, either version 3 of the License, or
 * > (at your option) any later version.
 * >
 * > wesnoth-old is distributed in the hope that it will be useful,
 * > but WITHOUT ANY WARRANTY; without even the implied warranty of
 * > MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * > GNU General Public License for more details.
 * >
 * > You should have received a copy of the GNU General Public License
 * > along with wesnoth-old.  If not, see <a href="http://www.gnu.org/licenses/">
 * > http://www.gnu.org/licenses/</a>.
 * @brief		A tile is the core "container" for Wesnoth.
 * @details		A tile is presented graphically as a hex on a Wesnoth map. That is to say,
 * Wesnoth maps, also called boards, are containers of tiles. In turn, tiles contain:
 *
 * - 0 or 1 units (Elvish Archer, Skeleton, etc.)
 * - 0 or more items (Healing Potion, Sword of Barys, Pile of Gold, etc.)
 * - Exactly 1 logical terrain type (Deep Water, Castle, Impassable, etc.)
 * - Exactly 1 visual terrain type (Green Grass, Brown Grass, Cobblestones, Bricks, Rails, etc.)
 *   This is constrained by the logical terrain type.
 * - 0 or more triggers (player stepping one tile triggers bridge collapse, etc., end of
 *   scenarios, etc.)
 *
 * @note Though it is possible to consider terrain as
 *
 * @note Because tiles are containers for objects critical to visual presentation and game
 * logic, they should be as simple, efficient and understandable as possible. This class was
 * created by justinzane as an attempt to simplify some of the game structures. Any
 * improvements, constructive criticisms, or cash donations are most welcome. :)
 */

#ifndef JZ_TILE_HPP_
#define JZ_TILE_HPP_

#include "unit.hpp"
#include "jz_item.hpp"
#include "terrain.hpp"

namespace jz {

/**  @type jz::tile TODO: WRITEME */
class tile {
    public:
        tile();
        virtual ~tile();

        void add_item(const item& item);
        void remove_item(const item& item);
        bool has_item(const item& item) const;
        const std::vector<item>& get_items() const {
            return items_;
        }

        const terrain_type& get_terr_type() const {
            return terr_type_;
        }

        const unit*& get_unit() const {
            return unit;
        }

        bool add_unit(const unit& unit);
        unit& remove_unit();
        unit& get_unit() const;

    private:
        unit& unit_;
        std::set <item> items_;
        terrain_type terr_type_;
};

} /* namespace jz */

#endif /* JZ_TILE_HPP_ */
