/**
 * @file 		unit_ability_list.cpp is part of Project wesnoth-old
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
 * @brief		TODO WRITEME
 * @details		TODO WRITEME
 */
#include "unit_ability_list.hpp"
#include "../map_location.hpp"
#include <boost/foreach.hpp>

#include <string>
#include <utility>
#include <vector>


std::pair<int,map_location> unit_ability_list::highest(const std::string& key, int def) const
{
    if ( cfgs_.empty() ) {
        return std::make_pair(def, map_location());
    }
    // The returned location is the best non-cumulative one, if any,
    // the best absolute cumulative one otherwise.
    map_location best_loc;
    bool only_cumulative = true;
    int abs_max = 0;
    int flat = 0;
    int stack = 0;
    BOOST_FOREACH(unit_ability const &p, cfgs_)
    {
        int value = (*p.first)[key].to_int(def);
        if ((*p.first)["cumulative"].to_bool()) {
            stack += value;
            if (value < 0) value = -value;
            if (only_cumulative && value >= abs_max) {
                abs_max = value;
                best_loc = p.second;
            }
        } else if (only_cumulative || value > flat) {
            only_cumulative = false;
            flat = value;
            best_loc = p.second;
        }
    }
    return std::make_pair(flat + stack, best_loc);
}

std::pair<int,map_location> unit_ability_list::lowest(const std::string& key, int def) const
{
    if ( cfgs_.empty() ) {
        return std::make_pair(def, map_location());
    }
    // The returned location is the best non-cumulative one, if any,
    // the best absolute cumulative one otherwise.
    map_location best_loc;
    bool only_cumulative = true;
    int abs_max = 0;
    int flat = 0;
    int stack = 0;
    BOOST_FOREACH(unit_ability const &p, cfgs_)
    {
        int value = (*p.first)[key].to_int(def);
        if ((*p.first)["cumulative"].to_bool()) {
            stack += value;
            if (value < 0) value = -value;
            if (only_cumulative && value <= abs_max) {
                abs_max = value;
                best_loc = p.second;
            }
        } else if (only_cumulative || value < flat) {
            only_cumulative = false;
            flat = value;
            best_loc = p.second;
        }
    }
    return std::make_pair(flat + stack, best_loc);
}

