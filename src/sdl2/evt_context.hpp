/**
 * @file 		evt_context.hpp is part of wesnoth-ng
 * @author		Justin Zane Chudgar, <justin@justinzane.com>
 * @date		Dec 11, 2013
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

#ifndef EVT_CONTEXT_HPP_
#define EVT_CONTEXT_HPP_

#include "evt_handler.hpp"
#include <tr1/unordered_set>
#include "stdlib.h"

/** @class evt_context
 * @details event_context objects control the handler objects that SDL events are
 * sent to. When an event_context is created, it will become the current event context.
 * event_context objects MUST be created in LIFO ordering in relation to each other,
 * and in relation to handler objects. That is, all event_context objects should be
 * created as automatic/stack variables.
 *
 * handler objects need not be created as automatic variables (e.g. you could put them
 * in a vector) however you must guarantee that handler objects are destroyed before
 * their context is destroyed
 * @note This documentation is copied verbatim from mainline wesnoth and is **not**
 * completely relevant to wesnoth-ng.
 */
class evt_context {
    public:
        evt_context();
        virtual ~evt_context();

        void add_handler(evt_handler& hndlr) { handlers_.insert(hndlr); }

        void remove_handler(evt_handler& hndlr) { handlers_.erase(hndlr); }

        evt_handler& cycle_focus() {
            for (auto hndlr : handlers_) {
                if (hndlr->requires_event_focus()) { return hndlr; }
            }
            return nullptr;
        }

        void set_focus(const evt_handler& hndlr) {
            if (i != handlers.end() && ( * *i).requires_event_focus()) {
                focused_handler = int(i - handlers.begin());
            }
        }

        evt_handler& focused_handler {nullptr};

        void delete_handler_index(size_t handler);

    private:
        std::tr1::unordered_set<evt_handler&> handlers_ { new std::tr1::unordered_set<evt_handler&>() };

};

#endif /* EVT_CONTEXT_HPP_ */
