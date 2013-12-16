/**
 * @file src/persist_mgr.hpp
 * @project The Battle for Wesnoth NG - https://github.com/justinzane/wesnoth-ng
 * @brief 
 * @authors 
 * @copyright Copyright (C) 2010 - 2013 by Jody Northup
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef PERSIST_mgr_HPP_INCLUDED
#define PERSIST_mgr_HPP_INCLUDED

#include "persist_context.hpp"

class persist_mgr {
	protected:
		typedef std::map<std::string,persist_context *> context_map;

		bool in_transaction_;
		context_map contexts_;
	public:
		bool start_transaction();
		bool end_transaction();
		bool cancel_transaction();

		persist_mgr() : in_transaction_(false),contexts_() {}
		virtual ~persist_mgr() {
			cancel_transaction();
			for (context_map::iterator i = contexts_.begin(); i != contexts_.end(); ++i)
				delete (i->second);
		}
		persist_context &get_context(const std::string &ns);
};

#endif
