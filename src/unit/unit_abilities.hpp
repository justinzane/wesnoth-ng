/*
   Copyright (C) 2006 - 2013 by Dominic Bolin <dominic.bolin@exong.net>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

/**
 * @file unit_abilties.hpp
 * @details Following is a WML snippet demonstrating a unit's abilities:
 * [abilities]
 *     ...
 * [heals]
 *     value=4
 *     max_value=8
 *     cumulative=no
 *     affect_allies=yes
 *     name= _ "heals"
 *     female_name= _ "female^heals"
 *     name_inactive=null
 *     female_name_inactive=null
 *     description=  _ "Heals: Allows the unit to heal adjacent friendly ..."
 *     description_inactive=null
 *     affect_self=yes
 *  [filter] // SUF
 *      ...
 *  [/filter]
 *  [filter_self] // SUF
 *      ...
 *  [/filter_self]
 *  [filter_adjacent] // SUF
 *      adjacent=n,ne,nw
 *      ...
 *  [/filter_adjacent]
 *  [filter_adjacent_location]
 *      adjacent=n,ne,nw
 *      ...
 *  [/filter_adjacent]
 *  [affect_adjacent]
 *      adjacent=n,ne,nw
 *      [filter] // SUF
 *          ...
 *      [/filter]
 *  [/affect_adjacent]
 *  [affect_adjacent]
 *      adjacent=s,se,sw
 *      [filter] // SUF
 *          ...
 *      [/filter]
 *  [/affect_adjacent]
 * [/heals]
 * ...
 * [/abilities]
 */

#ifndef UNIT_ABILITIES_HPP_INCLUDED
#define UNIT_ABILITIES_HPP_INCLUDED

#include "../map_location.hpp"
#include "unit_ability_list.hpp"

bool filter_base_matches(const config& cfg, int def);

enum value_modifier {NOT_USED,SET,ADD,MUL,DIV};

struct individual_effect
{
	individual_effect() : type(NOT_USED), value(0), ability(NULL),
		loc(map_location::null_location) {};
	void set(value_modifier t, int val, const config *abil,const map_location &l);
	value_modifier type;
	int value;
	const config *ability;
	map_location loc;
};

class ability_effect
{
	public:
		ability_effect(const unit_ability_list& list, int def, bool backstab);

		// Provide read-only access to the effect list:
		typedef std::vector<individual_effect>::const_iterator iterator;
		typedef std::vector<individual_effect>::const_iterator const_iterator;

		int get_composite_value() const
		{ return composite_value_; }
		const_iterator begin() const
		{ return effect_list_.begin(); }
		const_iterator end() const
		{ return effect_list_.end(); }
	private:
		std::vector<individual_effect> effect_list_;
		int composite_value_;
};

#endif

