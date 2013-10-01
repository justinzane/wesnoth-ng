/**
 * @file 		jz_items.hpp is part of Project wesnoth-old
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
 * @brief		Items are objects that are contained either by a tile or by a unit and provide
 * potentially altered functionality to the units that hold them. Examples are "magic weapons"
 * in campaigns like LotI and HttT, "magic armor", gold, gems, etc.
 * @details		TODO This class is just a dummy at the moment.
 */
#ifndef JZ_ITEM_HPP_
#define JZ_ITEM_HPP_

#include <boost/uuid/uuid.hpp>

namespace jz {

/**  @type jz::items TODO: WRITEME */
class item {
    public:
        item() { item::uuid_ = boost::uuids::basic_random_generator()(); }
        virtual ~item();

        enum effect_type_t {
            UNIT_ATTACK     = 0x0001,
            UNIT_DEFEND     = 0x0002,
            UNIT_RESIST     = 0x0004,
            UNIT_MOVE       = 0x0008,
            UNIT_DISP       = 0x0010,
            UNIT_ABIL       = 0x0020,
            TERR_LOGIC      = 0x0100,
            TERR_DISP       = 0x0200,
            TOD_LOGIC       = 0x0400,
            TOD_DISPLAY     = 0x1000,
            BOGUS           =-0x0001
        };

        /** Adds an effect type to an item.
         * @return true if successful, false if not. */
        bool add_effect_type(const effect_type_t et);
        /** Removes an effect type from an item.
         * @return true if successful, false if not. */
        bool remove_effect_type(const effect_type_t et);
        /** Checks if item has an effect type.
         * @return true if it does , false if not.*/
        bool has_effect_type(const effect_type_t et) const;

        /** Get this item's univ. unique id.
         * @return the uuid as a boost::uuid::uuid object */
        const boost::uuids::uuid& get_uuid() const { return uuid_; }

    private:
        boost::uuids::uuid uuid_;
        int effect_types_; /**< bitmap of effect_type_t */

};

} /* namespace jz */

#endif /* JZ_ITEMS_HPP_ */
