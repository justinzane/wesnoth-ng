/**
 * @file 		jz_items.cpp is part of Project wesnoth-old
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
#include <jz_item.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace jz {

bool item::add_effect_type(const effect_type_t et) {
    if (item::effect_types_ & et) {
        return false; // already have this effect type
    } else {
        item::effect_types_ += et;
        return true;
    }
}

bool item::remove_effect_type(const effect_type_t et) {
    if (item::effect_types_ & et) {
        item::effect_types_ -= et;
        return true;
    } else {
        return false; // can't remove what we do not have
    }
}

bool item::has_effect_type(const effect_type_t et) const {
    return (item::effect_types_ & et);
}


} /* namespace jz */
