/**
 * @file src/tools/exploder_cutter.hpp
 * @project The Battle for Wesnoth NG - https://github.com/justinzane/wesnoth-ng
 * @brief 
 * @authors 
 * @copyright Copyright (C) 2004 - 2013 by Philippe Plantier <ayin@anathas.org>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef EXPLODER_CUTTER_HPP_INCLUDED
#define EXPLODER_CUTTER_HPP_INCLUDED

#include "../sdl_utils.hpp"
#include "../config.hpp"
#include "../sdl_utils.hpp"
#include "exploder_utils.hpp"

class cutter
{
public:
	struct mask
	{
		mask()
			: name()
			, image(nullptr)
			, filename()
			, shift()
			, cut()
		{
		}

		std::string name;
		SDL_Surface image;
		std::string filename;

		exploder_point shift;
		exploder_rect cut;
	};
	typedef std::map<std::string, mask> mask_map;
	struct positioned_surface {
		positioned_surface()
			: name()
			, pos()
			, image(nullptr)
			, mask()
		{
		};

		std::string name;
		exploder_point pos;
		SDL_Surface image;

		cutter::mask mask;
	};
	typedef std::multimap<std::string, positioned_surface> surface_map;

	cutter();

	const config load_config(const std::string& filename);
	void load_masks(const config& conf);
	surface_map cut_surface(SDL_Surface surf, const config& conf);

	void set_verbose(bool value);
private:
	std::string find_configuration(const std::string &file);
	void add_sub_image(const SDL_Surface &surf, surface_map &map, const config* config);

	mask_map masks_;

	bool verbose_;
};

#endif

