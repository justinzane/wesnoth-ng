/*
 Copyright (C) 2003 - 2013 by David White <dave@whitevine.net>
 Part of the Battle for Wesnoth Project http://www.wesnoth.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY.

 See the COPYING file for more details.
 */
#ifndef GAME_CONFIG_H_INCLUDED
#define GAME_CONFIG_H_INCLUDED

class config;
class version_info;
class color_range;

#include "tstring.hpp"

#include <SDL_types.h>

#include <vector>
#include <map>

/** @namespace game_config
 * @brief basic game configuration information is here.
 */
namespace game_config {
// Externally defined plain-old-data variables.------------------------------------------------

// ---- Integer Variables ---------------------------------------------------------------------
extern int base_income;                 /**< TODO WRITEME */
extern int cache_compression_level;     /**< TODO WRITEME */
extern const int gold_carryover_percentage; /**< Default % gold carried over to next scenario. */
extern int kill_experience;             /**< TODO WRITEME */
extern unsigned lobby_network_timer;    /**< TODO WRITEME */
extern unsigned lobby_refresh;          /**< TODO WRITEME */
extern const size_t max_loop;           /**< @brief The maximum number of hexes on a map,
                                             items in an array and iterations in wml loops.
                                             @warning This should not be set to less than the
                                             max map area */
extern int poison_amount;               /**< TODO WRITEME */
extern int rest_heal_amount;            /**< TODO WRITEME */
extern int recall_cost;                 /**< TODO WRITEME */
extern int tile_size;                   /**< TODO WRITEME */
extern int village_income;              /**< TODO WRITEME */
extern int village_support;             /**< TODO WRITEME */

// ---- Text Variables ------------------------------------------------------------------------
extern std::string default_defeat_music;        /**< TODO WRITEME */
extern std::string default_preferences_path;    /**< TODO WRITEME */
extern std::string default_terrain;             /**< TODO WRITEME */
extern std::string default_victory_music;       /**< TODO WRITEME */
extern std::string flag_rgb;                    /**< TODO WRITEME */
extern std::string fog_prefix;                  /**< TODO WRITEME */
extern std::string foot_teleport_enter;         /**< TODO WRITEME */
extern std::string foot_teleport_exit;          /**< TODO WRITEME */
extern std::string lobby_music;                 /**< TODO WRITEME */
extern const std::string observer_team_name;    /**< observer team name for observer team chat */
extern std::string path;                        /**< TODO WRITEME */
extern std::string preferences_dir;             /**< TODO WRITEME */
extern const std::string revision;              /**< TODO WRITEME */
extern std::string shroud_prefix;               /**< TODO WRITEME */
extern const std::string test_version;          /**< TODO WRITEME */
extern std::string title_music;                 /**< TODO WRITEME */
extern const std::string version;               /**< TODO WRITEME */
extern std::string wesnoth_program_dir;         /**< TODO WRITEME */

// ---- Boolean Variables ---------------------------------------------------------------------
extern bool debug;                      /**< TODO WRITEME */
extern bool debug_lua;                  /**< TODO WRITEME */
extern bool disable_autosave;           /**< TODO WRITEME */
extern bool editor;                     /**< TODO WRITEME */
extern bool exit_at_end;                /**< TODO WRITEME */
extern bool ignore_replay_errors;       /**< TODO WRITEME */
extern bool mp_debug;                   /**< TODO WRITEME */
extern bool no_delay;                   /**< TODO WRITEME */

// ---- Double Variables ----------------------------------------------------------------------
extern double hp_bar_scaling;           /**< TODO WRITEME */
extern double xp_bar_scaling;           /**< TODO WRITEME */
extern double hex_brightening;          /**< TODO WRITEME */
extern double hex_semi_brightening;     /**< TODO WRITEME */

// Externally Defined Complex Variables -------------------------------------------------------
/** TODO WRITEME */
const color_range& color_info(const std::string& name);

/** TODO WRITEME */
extern const version_info wesnoth_version;

/** TODO WRITEME */
extern const version_info min_savegame_version;

// ---- Container Variables ----------------------------------------------------------------------
/** TODO WRITEME */
extern std::vector<std::string> foot_speed_prefix;

/** TODO WRITEME */
extern std::vector<Uint32> red_green_scale;

/** TODO WRITEME */
extern std::vector<Uint32> red_green_scale_text;

/** TODO WRITEME */
const std::vector<Uint32>& tc_info(const std::string& name);

/** TODO WRITEME */
extern std::map<std::string, std::vector<Uint32> > team_rgb_colors;

/** TODO WRITEME */
extern std::map<std::string, t_string> team_rgb_name;

/** TODO WRITEME */
extern std::map<std::string, color_range> team_rgb_range;

// Structs and Enums --------------------------------------------------------------------------
/** TODO WRITEME */
struct server_info {
        server_info() :
            name(""), address("") {
        }
        std::string name;
        std::string address; /**< may include ':' followed by port number */
};
/** TODO WRITEME */
extern std::vector<server_info> server_list;


// Functions ----------------------------------------------------------------------------------
/** @brief TODO WRITEME */
inline int kill_xp(int level) { return level ? kill_experience * level : kill_experience / 2; }

/** @brief TODO WRITEME */
void load_config(const config &cfg);

/** @brief TODO WRITEME */
void add_color_info(const config& v);

/** @details Return a color corresponding to the value val red for val=0 to green for val=100,
 * passing by yellow. Colors are defined by [game_config] keys red_green_scale and
 * red_green_scale_text */
Uint32 red_to_green(int val, bool for_text = true);

/** @details Return a color corresponding to the value val white for val=0 to blue for val=100.
 * Colors are defined by [game_config] keys blue_white_scale and blue_white_scale_text. */
Uint32 blue_to_white(int val, bool for_text = true);

} //end namespace game_config

namespace game_config {
/** @namespace game_config::images
 * @brief TODO WRITEME */
namespace images {
extern std::string game_title;                  /**< TODO WRITEME */
extern std::string game_title_background;       /**< TODO WRITEME */
    // orbs and hp/xp bar
extern std::string orb;                         /**< TODO WRITEME */
extern std::string unmoved_orb_color;           /**< TODO WRITEME */
extern std::string partmoved_orb_color;         /**< TODO WRITEME */
extern std::string enemy_orb_color;             /**< TODO WRITEME */
extern std::string ally_orb_color;              /**< TODO WRITEME */
extern std::string moved_orb_color;             /**< TODO WRITEME */
extern std::string energy;                      /**< TODO WRITEME */
    // flags
extern std::string flag;                        /**< TODO WRITEME */
extern std::string flag_icon;                   /**< TODO WRITEME */
    // hex overlay
extern std::string terrain_mask;                /**< TODO WRITEME */
extern std::string grid_top;                    /**< TODO WRITEME */
extern std::string grid_bottom;                 /**< TODO WRITEME */
extern std::string mouseover;                   /**< TODO WRITEME */
extern std::string selected;                    /**< TODO WRITEME */
extern std::string editor_brush;                /**< TODO WRITEME */
extern std::string unreachable;                 /**< TODO WRITEME */
extern std::string linger;                      /**< TODO WRITEME */
    // GUI elements
extern std::string observer;                    /**< TODO WRITEME */
extern std::string tod_bright;                  /**< TODO WRITEME */
extern std::string tod_dark;                    /**< TODO WRITEME */
extern std::string selected_menu;               /**< TODO WRITEME */
extern std::string deselected_menu;             /**< TODO WRITEME */
extern std::string checked_menu;                /**< TODO WRITEME */
extern std::string unchecked_menu;              /**< TODO WRITEME */
extern std::string wml_menu;                    /**< TODO WRITEME */
extern std::string level;                       /**< TODO WRITEME */
extern std::string ellipsis;                    /**< TODO WRITEME */
extern std::string missing;
}  //end namespace game_config::images
} //end namespace game_config

namespace game_config {
/** @namespace game_config::sounds
 * @brief TODO WRITEME */
namespace sounds {
extern const std::string turn_bell;                 /**< TODO WRITEME */
extern const std::string timer_bell;                /**< TODO WRITEME */
extern const std::string receive_message;           /**< TODO WRITEME */
extern const std::string receive_message_highlight; /**< TODO WRITEME */
extern const std::string receive_message_friend;    /**< TODO WRITEME */
extern const std::string receive_message_server;    /**< TODO WRITEME */
extern const std::string user_arrive;               /**< TODO WRITEME */
extern const std::string user_leave;                /**< TODO WRITEME */
extern const std::string game_user_arrive;          /**< TODO WRITEME */
extern const std::string game_user_leave;           /**< TODO WRITEME */
extern const std::string button_press;              /**< TODO WRITEME */
extern const std::string checkbox_release;          /**< TODO WRITEME */
extern const std::string slider_adjust;             /**< TODO WRITEME */
extern const std::string menu_expand;               /**< TODO WRITEME */
extern const std::string menu_contract;             /**< TODO WRITEME */
extern const std::string menu_select;               /**< TODO WRITEME */
} //end namespace game_config::sounds
} //end namespace game_config

#endif
