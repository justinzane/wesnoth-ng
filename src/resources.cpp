/**
 * @file src/resources.cpp
 * @project The Battle for Wesnoth NG - https://github.com/justinzane/wesnoth-ng
 * @brief 
 * @authors 
 * @copyright Copyright (C) 2009 - 2013 by Guillaume Melquiond <guillaume.melquiond@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#include "resources.hpp"
#include <cstddef>

namespace resources
{
	game_config_mgr  *config_mgr = nullptr;
	play_controller      *controller = nullptr;
	game_data            *gamedata = nullptr;
	gamemap              *game_map = nullptr;
	LuaKernel            *lua_kernel = nullptr;
	persist_mgr      *persist = nullptr;
	game_display         *screen = nullptr;
	soundsource::mgr *soundsources = nullptr;
	game_state           *state_of_game = nullptr;
	std::vector<team>    *teams = nullptr;
	::tod_mgr        *tod_mgr = nullptr;
	pathfind::mgr    *tunnels = nullptr;
	actions::undo_list   *undo_stack = nullptr;
	unit_map             *units = nullptr;
	wb::mgr          *whiteboard = nullptr;
}
