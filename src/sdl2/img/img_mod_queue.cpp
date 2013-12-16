/**
 * @file 		img_mod_queue.cpp is part of wesnoth-ng
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

#include <sdl2/img/img_mod_queue.hpp>

/** Adds @a mod to the queue (unless mod is nullptr). */
void img_mod_queue::push(img_mod& mod) {
    // Null pointers do not get stored. (Shouldn't happen, but just in case.)
    if (mod != nullptr) priorities_[mod->priority()].push_back(mod);
}

/** Removes the top element from the queue */
void img_mod_queue::pop() {
    map_type::iterator top_pair = priorities_.begin();
    std::vector<img_mod *> & top_vector = top_pair->second;

    // Erase the top element.
    top_vector.erase(top_vector.begin());
    if (top_vector.empty())
    // We need to keep the map clean.
        priorities_.erase(top_pair);
}

/** Returns the number of elements in the queue. */
size_t img_mod_queue::size() const {
    size_t count = 0;
    for (auto pair : priorities_) { count += pair.second.size(); }
    return count;
}

/** Returns the top element in the queue . */
img_mod& img_mod_queue::top() const {
    return priorities_.begin()->second.front();
}

