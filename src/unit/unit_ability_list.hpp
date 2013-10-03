/**
 * @file 		unit_ability_list.hpp is part of Project wesnoth-old
 * @author		justin
 * @date		Oct 2, 2013
 * @copyright   Copyright justin, Oct 2, 2013
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
 * @brief		Provides a list of unit abilities accessible as a vector.
 * @details		TODO WRITEME
 */
#ifndef UNIT_ABILITY_LIST_HPP_
#define UNIT_ABILITY_LIST_HPP_

/** The things contained within a unit_ability_list. */
typedef std::pair<const config *, map_location> unit_ability;

/** @brief: TODO: WRITEME */
class unit_ability_list {
    public:
        unit_ability_list() :
            cfgs_() {}

        std::pair<int, map_location> highest(const std::string& key, int def = 0) const;
        std::pair<int, map_location> lowest(const std::string& key, int def = 0) const;

        /** This makes this class usable with BOOST_FOREACH */
        typedef std::vector<unit_ability>::iterator iterator;
        /** This makes this class usable with BOOST_FOREACH */
        typedef std::vector<unit_ability>::const_iterator const_iterator;

        // Iterator access
        iterator begin() { return cfgs_.begin(); }
        const_iterator begin() const { return cfgs_.begin(); }
        iterator end() { return cfgs_.end(); }
        const_iterator end() const { return cfgs_.end(); }
        iterator erase(const iterator & erase_it) { return cfgs_.erase(erase_it); }

        // Vector access:
        bool empty() const { return cfgs_.empty(); }
        unit_ability & front() { return cfgs_.front(); }
        const unit_ability & front() const { return cfgs_.front(); }
        unit_ability & back() { return cfgs_.back(); }
        const unit_ability & back() const { return cfgs_.back(); }
        void push_back(const unit_ability & ability) { cfgs_.push_back(ability); }

    private:
        // Data:
        std::vector<unit_ability> cfgs_;
};

#endif /* UNIT_ABILITY_LIST_HPP_ */
