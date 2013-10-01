/**
 * @file 		jz_tile.cpp is part of Project wesnoth-old
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
 * @brief		TODO WRITEME
 * @details		TODO WRITEME
 */
#include <jz_tile.hpp>

namespace jz {

tile::tile() {
    tile::items_ = new std::set();
    tile::terr_type_ = NULL;
    tile::unit_ = NULL;
}

tile::~tile() {
    // TODO Auto-generated destructor stub
}

bool tile::add_unit(const unit& unit) {
    if (tile::unit_ == NULL) {
        tile::unit_ = unit;
        return true;
    } else {
        return false;
    }
}

unit& tile::remove_unit(void) {
    if (tile::unit_ == NULL) {
        return NULL; // can't remove what is not here.
    } else {
        unit& temp = tile::unit_;
        tile::unit_ = NULL;
        return temp;
    }
}

} /* namespace jz */
