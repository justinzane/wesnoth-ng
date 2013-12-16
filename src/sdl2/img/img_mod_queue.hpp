/**
 * @file 		img_mod_queue.hpp is part of wesnoth-ng
 * @author		Justin Zane Chudgar, <justin@justinzane.com>
 * @date		Dec 15, 2013
 * @copyright   Copyright justin, 2013
 * @section		License GPLv3
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
 * > along with wesnoth-ng.  If not, see http://www.gnu.org/licenses/ .
 * @brief		TODO WRITEME
 * @details		TODO WRITEME
 */

#ifndef IMG_MOD_QUEUE_HPP_
#define IMG_MOD_QUEUE_HPP_

#include "sdl2/img/img_mod.hpp"
#include <map>
#include <vector>

/**@class img_mod_queue
 * @brief A modified priority queue used to order image modifications.
 * @details The priorities for this queue are to order modifications by priority(),
 * then by the order they are added to the queue.
 *
 * Invariant for this class: At the beginning and end of each member function call,
 * there are no empty vectors in priorities_.*/
class img_mod_queue {
    public:
        img_mod_queue() {}
        ~img_mod_queue() {}
        bool is_empty() const {
            return priorities_.empty();
        }
        void push(img_mod& mod);
        void pop();
        size_t size() const;
        img_mod& top() const;

    private:
        /// Map from a mod's priority() to the mods having that priority.
        typedef std::map<int, std::vector<img_mod&>, std::greater<int> > map_type;
        /// Map from a mod's priority() to the mods having that priority.
        map_type priorities_;
};

#endif /* IMG_MOD_QUEUE_HPP_ */
