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

/**
 * @file
 *
 * map_display and display: classes which take care of
 * displaying the map and game-data on the screen.
 *
 * The display is divided into two main sections:
 * - the game area, which displays the tiles of the game board, and units on them,
 * - and the side bar, which appears on the right hand side.
 * The side bar display is divided into three sections:
 * - the minimap, which is displayed at the top right
 * - the game status, which includes the day/night image,
 *   the turn number, information about the current side,
 *   and information about the hex currently moused over (highlighted)
 * - the unit status, which displays an image and stats
 *   for the current unit.
 */

#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

#include "drawing_buffer_key.hpp"
#include "font.hpp"
#include "sdl_utils.hpp"
#include "tblit.hpp"
#include "tdrawing_layer.hpp"
#include "theme.hpp"
#include "video.hpp"

//#include "widgets/button.hpp"
//#include "widgets/slider.hpp"

#include "../animated.hpp"
#include "../board/map.hpp"
#include "../board/map_label.hpp"
#include "../board/map_location.hpp"
#include "../builder.hpp"
#include "../config.hpp"
#include "../generic_event.hpp"
#include "../image.hpp"
#include "../overlay.hpp"
#include "../team.hpp"
#include "../time_of_day.hpp"
#include "../unit.hpp"
#include "../unit_map.hpp"
#include "../util.hpp"
//#include "../whiteboard/arrow.hpp"
#include "../widgets/button.hpp"
#include "../widgets/slider.hpp"
//#include "key.hpp"

#include <boost/function/function_fwd.hpp>
#include <boost/function/function_template.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/smart_ptr/scoped_ptr.hpp>
#include <stddef.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_stdinc.h>

#include <algorithm>
#include <cstdbool>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

//class config;
//class terrain_builder;
//struct time_of_day;
//class map_labels;
//class arrow;
//class gamemap;

class display {
    public:
        /**
         * @brief Constructor
         * @param units     Container of <unit,location>
         * @param video     Wesnoth's main window.
         * @param map       The game board.
         * @param t         Vector of teams.
         * @param theme_cfg Selected display theme.
         * @param level     ??? Difficulty level ???
         */
        display(unit_map* units,
                ui_window& video,
                const gamemap* map,
                const std::vector<team>* t,
                const config& theme_cfg,
                const config& level);

        /** Virtual destructor */
        virtual ~display();

        /** @todo WTF? Public constructor and singleton? */
        static display* get_singleton() { return singleton_; }

        /** @todo WRITEME */
        bool show_everything() const { return !viewpoint_; }

        /** @todo WRITEME */
        const std::vector<team>& get_teams() const { return *teams_; }

        /** The playing team is the team whose turn it is. */
        size_t playing_team() const { return activeTeam_; }

        /** @todo WRITEME */
        bool team_valid() const { return currentTeam_ < teams_->size(); }

        /** The viewing team is the team currently viewing the game. */
        size_t viewing_team() const { return currentTeam_; }

        int viewing_side() const { return currentTeam_ + 1; }

        /**
         * @brief Sets the team controlled by the player using the computer;
         * data from this team will be displayed in the game status.
         * @param team      handle of the team
         * @param observe   WRITEME
         */
        void set_team(size_t team, bool observe = false);

        /** @brief set_playing_team sets the team whose turn it currently is */
        void set_playing_team(size_t team);

        /** Cancels all the exclusive draw requests. */
        void clear_exclusive_draws() { exclusive_unit_draw_requests_.clear(); }

        /** @brief TODO WRITEME */
        unit_map& get_units() { return *units_; }

        /** @brief TODO WRITEME */
        const unit_map& get_const_units() const { return *units_; }

        /**
         * @brief Allows a unit to request to be the only one drawn in its hex. Useful for
         * situations where multiple units (one real, multiple temporary) can end up stacked,
         * such as with the whiteboard.
         * @param loc The location of the unit requesting exclusivity.
         * @param unit The unit requesting exclusivity.
         * @return false if there's already an exclusive draw request for this location.
         */
        bool add_exclusive_draw(const map_location& loc, unit& unit);

        /**
         * Cancels an exclusive draw request.
         * @return The id of the unit whose exclusive draw request was canceled, or else
         *         the empty string if there was no exclusive draw request for this location.
         */
        std::string remove_exclusive_draw(const map_location& loc);

        /** @brief TODO WRITEME */
        void draw_bar(const std::string& image,
                      int xpos,
                      int ypos,
                      const map_location& loc,
                      size_t height,
                      double filled,
                      const SDL_Color& col,
                      fixed_t alpha);

        /**
         * Check the overlay_map for proper team-specific overlays to be
         * displayed/hidden
         */
        void parse_team_overlays();

        /**
         * Functions to add and remove overlays from locations.
         *
         * An overlay is an image that is displayed on top of the tile.
         * One tile may have multiple overlays.
         */
        void add_overlay(const map_location& loc,
                         const std::string& image,
                         const std::string& halo = "",
                         const std::string& team_name = "",
                         bool visible_under_fog = true);

        /** remove_overlay will remove all overlays on a tile. */
        void remove_overlay(const map_location& loc);

        /** remove_single_overlay will remove a single overlay from a tile */
        void remove_single_overlay(const map_location& loc, const std::string& toDelete);

        /**
         * Updates internals that cache map size. This should be called when the map
         * size has changed.
         */
        void reload_map();

        /** @brief TODO WRITEME */
        void change_map(const gamemap* m);

        /** @brief TODO WRITEME */
        void change_teams(const std::vector<team>* teams);

        /** @brief TODO WRITEME */
        void change_units(unit_map* units);

        /** @brief TODO WRITEME */
        static Uint32 rgb(Uint8 red, Uint8 green, Uint8 blue) {
            return 0xFF000000 | (red << 16) | (green << 8) | blue;
        }

        /** @brief TODO WRITEME */
        static Uint8 red(Uint32 color) {
            return (color & 0x00FF0000) >> 16;
        }

        /** @brief TODO WRITEME */
        static Uint8 green(Uint32 color) {
            return (color & 0x0000FF00) >> 8;
        }

        /** @brief TODO WRITEME */
        static Uint8 blue(Uint32 color) {
            return (color & 0x000000FF);
        }

        /** @brief TODO WRITEME */
        static Uint32 max_rgb(Uint32 first, Uint32 second) {
            return rgb(std::max(red(first), red(second)), std::max(green(first), green(second)),
                       std::max(blue(first), blue(second)));
        }

        /**
         * Add r,g,b from tod_manager to the map
         *
         */
        void update_tod();

        /**
         * Add r,g,b to the colors for all images displayed on the map.
         *
         * Used for special effects like flashes.
         */
        void adjust_color_overlay(int r, int g, int b);

        /** Gets the underlying screen object. */
        ui_window& video() {
            return screen_;
        }

        /** return the screen surface or the surface used for map_screenshot. */
        surface get_screen_surface() {
            return map_screenshot_ ? map_screenshot_surf_ : screen_.getSurface();
        }

        /** @brief TODO WRITEME */
        virtual bool in_game() const {
            return false;
        }

        /** @brief TODO WRITEME */
        virtual bool in_editor() const {
            return false;
        }

        /**
         * the dimensions of the display. x and y are width/height.
         * mapx is the width of the portion of the display which shows the game area.
         * Between mapx and x is the sidebar region.
         */
        int w() const {
            return screen_.getx();
        } /**< width */

        /** @brief TODO WRITEME */
        int h() const {
            return screen_.gety();
        } /**< height */

        /** @brief TODO WRITEME */
        const SDL_Rect& minimap_area() const {
            return theme_.mini_map_location(screen_area());
        }

        /** @brief TODO WRITEME */
        const SDL_Rect& palette_area() const {
            return theme_.palette_location(screen_area());
        }

        /** @brief TODO WRITEME */
        const SDL_Rect& unit_image_area() const {
            return theme_.unit_image_location(screen_area());
        }

        /** @brief TODO WRITEME */
        SDL_Rect screen_area() const {
            return create_rect(0, 0, w(), h());
        }

        /**
         * Returns the maximum area used for the map
         * regardless to resolution and view size
         */
        const SDL_Rect& max_map_area() const;

        /**
         * Returns the area used for the map
         */
        const SDL_Rect& map_area() const;

        /**
         * Returns the available area for a map, this may differ
         * from the above. This area will get the background area
         * applied to it.
         */
        const SDL_Rect& map_outside_area() const {
            return map_screenshot_ ? max_map_area() : theme_.main_map_location(screen_area());
        }

        /** Check if the bbox of the hex at x,y has pixels outside the area rectangle. */
        bool outside_area(const SDL_Rect& area, const int x, const int y) const;

        /**
         * Function which returns the width of a hex in pixels,
         * up to where the next hex starts.
         * (i.e. not entirely from tip to tip -- use hex_size()
         * to get the distance from tip to tip)
         */
        int hex_width() const {
            return (zoom_ * 3) / 4;
        }

        /**
         * Function which returns the size of a hex in pixels
         * (from top tip to bottom tip or left edge to right edge).
         */
        int hex_size() const {
            return zoom_;
        }

        /** Returns the current zoom factor. */
        double get_zoom_factor() const {
            return double(zoom_) / double(image::tile_size);
        }

        /**
         * given x,y co-ordinates of an onscreen pixel, will return the
         * location of the hex that this pixel corresponds to.
         * Returns an invalid location if the mouse isn't over any valid location.
         */
        const map_location hex_clicked_on(int x, int y) const;

        /**
         * given x,y co-ordinates of a pixel on the map, will return the
         * location of the hex that this pixel corresponds to.
         * Returns an invalid location if the mouse isn't over any valid location.
         */
        const map_location pixel_position_to_hex(int x, int y) const;

        /**
         * given x,y co-ordinates of the mouse, will return the location of the
         * hex in the minimap that the mouse is currently over, or an invalid
         * location if the mouse isn't over the minimap.
         */
        map_location minimap_location_on(int x, int y);

        /** @brief TODO WRITEME */
        const map_location& selected_hex() const {
            return selectedHex_;
        }

        /** @brief TODO WRITEME */
        const map_location& mouseover_hex() const {
            return mouseoverHex_;
        }

        /** @brief TODO WRITEME */
        virtual void select_hex(map_location hex);

        /** @brief TODO WRITEME */
        virtual void highlight_hex(map_location hex);

        /** Function to invalidate the game status displayed on the sidebar. */
        void invalidate_game_status() {
            invalidateGameStatus_ = true;
        }

        /** Functions to get the on-screen positions of hexes. */
        int get_location_x(const map_location& loc) const;
        int get_location_y(const map_location& loc) const;

        /**
         * Rectangular area of hexes, allowing to decide how the top and bottom
         * edges handles the vertical shift for each parity of the x coordinate
         */
        struct rect_of_hexes {
                int left;
                int right;
                int top[2];  // for even and odd values of x, respectively
                int bottom[2];

                /**  very simple iterator to walk into the rect_of_hexes */
                struct iterator {
                        iterator(const map_location &loc, const rect_of_hexes &rect) :
                            loc_(loc), rect_(rect) {
                        }
                        ;

                        /** increment y first, then when reaching bottom, increment x */
                        iterator& operator++();
                        bool operator==(const iterator &that) const {
                            return that.loc_ == loc_;
                        }
                        bool operator!=(const iterator &that) const {
                            return that.loc_ != loc_;
                        }
                        const map_location& operator*() const {
                            return loc_;
                        }
                        ;

                        typedef std::forward_iterator_tag iterator_category;
                        typedef map_location value_type;
                        typedef int difference_type;
                        typedef const map_location *pointer;
                        typedef const map_location &reference;

                    private:
                        map_location loc_;
                        const rect_of_hexes &rect_;
                };
                typedef iterator const_iterator;

                iterator begin() const;
                iterator end() const;
        };

        /** Return the rectangular area of hexes overlapped by r (r is in screen coordinates) */
        const rect_of_hexes hexes_under_rect(const SDL_Rect& r) const;

        /** Returns the rectangular area of visible hexes */
        const rect_of_hexes get_visible_hexes() const {
            return hexes_under_rect(map_area());
        }
        ;

        /** Returns true if location (x,y) is covered in shroud. */
        bool shrouded(const map_location& loc) const {
            return viewpoint_ && viewpoint_->shrouded(loc);
        }
        /** Returns true if location (x,y) is covered in fog. */
        bool fogged(const map_location& loc) const {
            return viewpoint_ && viewpoint_->fogged(loc);
        }

        /**
         * Determines whether a grid should be overlayed on the game board.
         * (to more clearly show where hexes are)
         */
        void set_grid(const bool grid) {
            grid_ = grid;
        }

        /** Getter for the x,y debug overlay on tiles */
        bool get_draw_coordinates() const {
            return draw_coordinates_;
        }
        /** Setter for the x,y debug overlay on tiles */
        void set_draw_coordinates(bool value) {
            draw_coordinates_ = value;
        }

        /** Getter for the terrain code debug overlay on tiles */
        bool get_draw_terrain_codes() const {
            return draw_terrain_codes_;
        }
        /** Setter for the terrain code debug overlay on tiles */
        void set_draw_terrain_codes(bool value) {
            draw_terrain_codes_ = value;
        }

        /** Save a (map-)screenshot and return the estimated file size */
        int screenshot(std::string filename, bool map_screenshot = false);

        /** Invalidates entire screen, including all tiles and sidebar. Calls redraw observers. */
        void redraw_everything();

        /** Adds a redraw observer, a function object to be called when redraw_everything is used */
        void add_redraw_observer(boost::function<void(display&)> f);

        /** Clear the redraw observers */
        void clear_redraw_observers();

        /** @brief TODO WRITEME */
        theme& get_theme() { return theme_; }

        /**
         * Retrieves a pointer to a theme UI button.
         *
         * @note The returned pointer may either be NULL, meaning the button
         *       isn't defined by the current theme, or point to a valid
         *       gui::button object. However, the objects retrieved will be
         *       destroyed and recreated by draw() method calls. Do *NOT* store
         *       these pointers for longer than strictly necessary to
         *       accomplish a specific task before the next screen refresh.
         */
        gui::button* find_action_button(const std::string& id);

        /** @brief TODO WRITEME */
        gui::button* find_menu_button(const std::string& id);

        /** @brief TODO WRITEME */
        gui::slider* find_slider(const std::string& id);

        /** @brief TODO WRITEME */
        gui::button::TYPE string_to_button_type(std::string type);

        /** @brief TODO WRITEME */
        void create_buttons();

        /** @brief TODO WRITEME */
        void invalidate_theme() {
            panelsDrawn_ = false;
        }

        /** @brief TODO WRITEME */
        void refresh_report(std::string const &report_name, const config * new_cfg = NULL);

        /** @brief TODO WRITEME */
        void draw_minimap_units();

        /** Function to invalidate all tiles. */
        void invalidate_all();

        /** Function to invalidate a specific tile for redrawing. */
        bool invalidate(const map_location& loc);

        /** @brief TODO WRITEME */
        bool invalidate(const std::set<map_location>& locs);

        /**
         * If this set is partially invalidated, invalidate all its hexes.
         * Returns if any new invalidation was needed
         */
        bool propagate_invalidation(const std::set<map_location>& locs);

        /** invalidate all hexes under the rectangle rect (in screen coordinates) */
        bool invalidate_locations_in_rect(const SDL_Rect& rect);

        /** @brief TODO WRITEME */
        bool invalidate_visible_locations_in_rect(const SDL_Rect& rect);

        /**
         * Function to invalidate animated terrains and units which may have changed.
         */
        void invalidate_animations();

        /**
         * helper function for invalidate_animations
         * returns a list of units to check for invalidation
         */
        virtual std::vector<unit*> get_unit_list_for_invalidation();

        /**
         * Per-location invalidation called by invalidate_animations()
         * Extra game per-location invalidation (village ownership)
         */
        void invalidate_animations_location(const map_location& loc);

        /** @brief TODO WRITEME */
        const gamemap& get_map() const { return *map_; }

        /**
         * mouseover_hex_overlay_ require a prerendered surface
         * and is drawn underneath the mouse's location
         */
        void set_mouseover_hex_overlay(const surface& image) {
            mouseover_hex_overlay_ = image;
        }

        /** @brief TODO WRITEME */
        void clear_mouseover_hex_overlay() {
            mouseover_hex_overlay_ = NULL;
        }

        /**
         * Debug function to toggle the "sunset" mode.
         * The map area become progressively darker,
         * except where hexes are refreshed.
         * delay is the number of frames between each darkening
         * (0 to toggle).
         */
        static void sunset(const size_t delay = 0);

        /** Toggle to continuously redraw the screen. */
        static void toggle_benchmark();

        /**
         * Toggle to debug foreground terrain.
         * Separate background and foreground layer
         * to better spot any error there.
         */
        static void toggle_debug_foreground();

        /** @brief TODO WRITEME */
        terrain_builder& get_builder() { return *builder_; }

        /** @brief TODO WRITEME */
        void flip() __attribute__((deprecated("FLip is deprecated for SDL2")));

        /** Copy the backbuffer to the framebuffer. */
        void update_display();

        /** Rebuild all dynamic terrain. */
        void rebuild_all();

        /** @brief TODO WRITEME */
        const theme::action* action_pressed();

        /** @brief TODO WRITEME */
        const theme::menu* menu_pressed();

        /**
         * @brief Finds the menu which has a given item in it, and enables or disables it.
         */
        void enable_menu(const std::string& item, bool enable);

        /** @brief TODO WRITEME */
        void set_diagnostic(const std::string& msg);

        /** Delay routines: use these not SDL_Delay (for --nogui). */
        void delay(unsigned int milliseconds) const;

        /**
         * Set/Get whether 'turbo' mode is on.
         * When turbo mode is on, everything moves much faster.
         */
        void set_turbo(const bool turbo) {
            turbo_ = turbo;
        }

        /** @brief TODO WRITEME */
        double turbo_speed() const;

        /** @brief TODO WRITEME */
        void set_turbo_speed(const double speed) {
            turbo_speed_ = speed;
        }

        /** control unit idle animations and their frequency */
        void set_idle_anim(bool ison) {
            idle_anim_ = ison;
        }

        /** @brief TODO WRITEME */
        bool idle_anim() const {
            return idle_anim_;
        }

        /** @brief TODO WRITEME */
        void set_idle_anim_rate(int rate);

        /** @brief TODO WRITEME */
        double idle_anim_rate() const {
            return idle_anim_rate_;
        }

        /** @brief TODO WRITEME */
        void bounds_check_position();

        /** @brief TODO WRITEME */
        void bounds_check_position(int& xpos, int& ypos);

        /**
         * Scrolls the display by xmov,ymov pixels.
         * Invalidation and redrawing will be scheduled.
         * @return true if the map actually moved.
         */
        bool scroll(int xmov, int ymov, bool force = false);

        /**
         * Zooms the display by the specified amount.
         * Negative values zoom out.
         * Note the amount should be a multiple of four,
         * otherwise the images might start to look odd
         * (hex_width() gets rounding errors).
         */
        bool set_zoom(int amount, bool absolute = false);

        /** @brief TODO WRITEME */
        bool zoom_at_max() const;

        /** @brief TODO WRITEME */
        bool zoom_at_min() const;

        /** Sets the zoom amount to the default. */
        void set_default_zoom();

        /** @brief TODO WRITEME */
        bool view_locked() const {
            return view_locked_;
        }

        /** Sets whether the map view is locked (e.g. so the user can't scroll away) */
        void set_view_locked(bool value) {
            view_locked_ = value;
        }

        /**
         * @brief TODO WRITEME
         */
        enum SCROLL_TYPE {
            SCROLL,      //!< SCROLL
            WARP,        //!< WARP
            ONSCREEN,    //!< ONSCREEN
            ONSCREEN_WARP//!< ONSCREEN_WARP
        };

        /**
         * Scroll such that location loc is on-screen.
         * WARP jumps to loc; SCROLL uses scroll speed;
         * ONSCREEN only scrolls if x,y is offscreen
         * force : scroll even if preferences tell us not to,
         * or the view is locked.
         */
        void scroll_to_tile(const map_location& loc,
                            SCROLL_TYPE scroll_type = ONSCREEN,
                            bool check_fogged = true,
                            bool force = true);

        /**
         * Scroll such that location loc1 is on-screen.
         * It will also try to make it such that loc2 is on-screen,
         * but this is not guaranteed. For ONSCREEN scrolls add_spacing
         * sets the desired minimum distance from the border in hexes.
         */
        void scroll_to_tiles(map_location loc1, map_location loc2, SCROLL_TYPE scroll_type =
            ONSCREEN,
                             bool check_fogged = true, double add_spacing = 0.0, bool force =
                                 true);

        /** Scroll to fit as many locations on-screen as possible, starting with the first. */
        void scroll_to_tiles(const std::vector<map_location>::const_iterator & begin,
                             const std::vector<map_location>::const_iterator & end,
                             SCROLL_TYPE scroll_type = ONSCREEN,
                             bool check_fogged = true,
                             bool only_if_possible = false,
                             double add_spacing = 0.0,
                             bool force = true);

        /** Scroll to fit as many locations on-screen as possible, starting with the first. */
        void scroll_to_tiles(const std::vector<map_location>& locs,
                             SCROLL_TYPE scroll_type = ONSCREEN,
                             bool check_fogged = true,
                             bool only_if_possible = false,
                             double add_spacing = 0.0,
                             bool force = true) {
            scroll_to_tiles(locs.begin(), locs.end(), scroll_type, check_fogged,
                            only_if_possible, add_spacing, force);
        }

        /** Expose the event, so observers can be notified about map scrolling. */
        events::generic_event &scroll_event() const {
            return scroll_event_;
        }

        /** @brief TODO WRITEME */
        events::generic_event& complete_redraw_event() {
            return complete_redraw_event_;
        }

        /** Check if a tile is fully visible on screen. */
        bool tile_fully_on_screen(const map_location& loc);

        /** Checks if location @a loc or one of the adjacent tiles is visible on screen. */
        bool tile_nearly_on_screen(const map_location &loc) const;

        /**
         * Draws invalidated items.
         * If update is true, will also copy the display to the frame buffer.
         * If force is true, will not skip frames, even if running behind.
         * Not virtual, since it gathers common actions. Calls various protected
         * virtuals (further below) to allow specialized behavior in derived classes.
         */
        void draw(bool update = true, bool force = false);

        /** @brief TODO WRITEME */
        map_labels& labels();

        /** @brief TODO WRITEME */
        const map_labels& labels() const;

        /** Announce a message prominently. */
        void announce(const std::string& msg, const SDL_Color& color = font::GOOD_COLOR);

        /**
         * Schedule the minimap for recalculation.
         * Useful if any terrain in the map has changed.
         */
        void recalculate_minimap() {
            minimap_ = NULL;
            redrawMinimap_ = true;
        }

        /**
         * Schedule the minimap to be redrawn.
         * Useful if units have moved about on the map.
         */
        void redraw_minimap() {
            redrawMinimap_ = true;
        }

        /** @brief TODO WRITEME */
        virtual const time_of_day& get_time_of_day(const map_location& loc =
            map_location::null_location) const;

        /** @brief TODO WRITEME */
        virtual bool has_time_area() const {
            return false;
        }

        /** @brief TODO WRITEME */
        void write(config& cfg) const;

        /** Init the flag list and the team colors used by ~TC */
        void init_flags();

    protected:
        enum TERRAIN_TYPE {
            BACKGROUND,
            FOREGROUND
        };

        // protected typedefs  ----------------------------------------------------------------
        /** @todo WRITEME */
        typedef std::map<map_location, std::string> exclusive_unit_draw_requests_t;

        /// map of hexes where only one unit should be drawn, the one identified by the associated id string
        exclusive_unit_draw_requests_t exclusive_unit_draw_requests_;

        // protected functions ----------------------------------------------------------------
        /** Clear the screen contents */
        void clear_screen();

        /**
         * Called near the beginning of each draw() call.
         * Derived classes can use this to add extra actions before redrawing
         * invalidated hexes takes place. No action here by default.
         */
        virtual void pre_draw() { }

        /**
         * Called at the very end of each draw() call.
         * Derived classes can use this to add extra actions after redrawing
         * invalidated hexes takes place. No action here by default.
         */
        virtual void post_draw() { }

        /**
         * Get the clipping rectangle for drawing.
         * Virtual since the editor might use a slightly different approach.
         */
        virtual const SDL_Rect& get_clip_rect();

        /**
         * Only called when there's actual redrawing to do. Loops through
         * invalidated locations and redraws them. Derived classes can override
         * this, possibly to insert pre- or post-processing around a call to the
         * base class's function.
         */
        virtual void draw_invalidated();

        /**
         * Hook for actions to take right after draw() calls drawing_buffer_commit
         * No action here by default.
         */
        virtual void post_commit() {
        }

        /**
         * Redraws a single gamemap location.
         */
        virtual void draw_hex(const map_location& loc);

        /**
         * @returns the image type to be used for the passed hex
         */
        virtual image::TYPE get_image_type(const map_location& loc);

        /**
         * Called near the end of a draw operation, derived classes can use this
         * to render a specific sidebar. Very similar to post_commit.
         */
        virtual void draw_sidebar() { }

        /**
         * Draws the border tile overlay.
         * The routine determines by itself which border it is on
         * and draws an overlay accordingly. The definition of the
         * border is stored in the 'main_map_border' part of the theme.
         *
         * @param loc	the map location of the tile
         * @param xpos	the on-screen pixels x coordinate of the tile
         * @param ypos	the on-screen pixels y coordinate of the tile
         */
        virtual void draw_border(const map_location& loc, const int xpos, const int ypos);

        /** @brief TODO WRITEME */
        void draw_minimap();

        /** @brief TODO WRITEME */
        std::vector<surface> get_terrain_images(const map_location &loc,
                                                const std::string& timeid,
                                                image::TYPE type,
                                                TERRAIN_TYPE terrain_type);

        /** @brief TODO WRITEME */
        std::vector<surface> get_fog_shroud_images(const map_location& loc,
                                                   image::TYPE image_type);

        /** @brief TODO WRITEME */
        void draw_image_for_report(surface& img, SDL_Rect& rect);

        /** @brief TODO WRITEME */
        void scroll_to_xy(int screenxpos, int screenypos, SCROLL_TYPE scroll_type, bool force =
            true);

        /** @brief TODO WRITEME */
        void fill_images_list(const std::string& prefix, std::vector<std::string>& images);

        /** @brief TODO WRITEME */
        const std::string& get_variant(const std::vector<std::string>& variants,
                                       const map_location &loc) const;

        // protected variables ----------------------------------------------------------------
        unit_map* units_;           /**< TODO WRITEME */
        ui_window& screen_;         /**< TODO WRITEME */
        const gamemap* map_;        /**< TODO WRITEME */
        size_t currentTeam_;        /**< TODO WRITEME */
        const std::vector<team>* teams_;    /**< TODO WRITEME */
        const team *viewpoint_;     /**< TODO WRITEME */
        std::map<surface, SDL_Rect> energy_bar_rects_;  /**< TODO WRITEME */
        int xpos_, ypos_;           /**< TODO WRITEME */
        bool view_locked_;          /**< TODO WRITEME */
        theme theme_;               /**< TODO WRITEME */
        int zoom_;                  /**< TODO WRITEME */
        static int last_zoom_;      /**< TODO WRITEME */
        boost::scoped_ptr<terrain_builder> builder_;    /**< TODO WRITEME */
        surface minimap_;           /**< TODO WRITEME */
        SDL_Rect minimap_location_; /**< TODO WRITEME */
        bool redrawMinimap_;        /**< TODO WRITEME */
        bool redraw_background_;    /**< TODO WRITEME */
        bool invalidateAll_;        /**< TODO WRITEME */
        bool grid_;                 /**< TODO WRITEME */
        int diagnostic_label_;      /**< TODO WRITEME */
        bool panelsDrawn_;          /**< TODO WRITEME */
        double turbo_speed_;        /**< TODO WRITEME */
        bool turbo_;                /**< TODO WRITEME */
        bool invalidateGameStatus_; /**< TODO WRITEME */
        boost::scoped_ptr<map_labels> map_labels_;      /**< TODO WRITEME */
        mutable events::generic_event scroll_event_;    /**< Event raised when the map is
                                                             being scrolled */
        events::generic_event complete_redraw_event_;   /**< notify observers that the screen
                                                             has been redrawn completely atm
                                                             this is used for replay_controller
                                                             to add replay controls to the
                                                             standard theme */
        int nextDraw_;  /**< Holds the tick count for when the next drawing event is scheduled.
                             Drawing shouldn't occur before this time. */
        std::map<std::string, SDL_Rect> reportRects_;   /**< Not set by the initializer */
        std::map<std::string, surface> reportSurfaces_; /**< TODO WRITEME */
        std::map<std::string, config> reports_;         /**< TODO WRITEME */
        std::vector<gui::button> menu_buttons_;         /**< TODO WRITEME */
        std::vector<gui::button> action_buttons_;       /**< TODO WRITEME */
        std::vector<gui::slider> sliders_;              /**< TODO WRITEME */
        std::set<map_location> invalidated_;            /**< TODO WRITEME */
        std::set<map_location> previous_invalidated_;   /**< TODO WRITEME */
        surface mouseover_hex_overlay_;                 /**< TODO WRITEME */
        surface tod_hex_mask1, tod_hex_mask2;       /**< If we're transitioning from one time of
                                                         day to the next, then we will use these
                                                         two masks on top of all hexes when we
                                                         blit. */
        std::vector<std::string> fog_images_;       /**< TODO WRITEME */
        std::vector<std::string> shroud_images_;    /**< TODO WRITEME */
        map_location selectedHex_;      /**< TODO WRITEME */
        map_location mouseoverHex_;     /**< TODO WRITEME */
        // Removed for SDL2
        CKey keys_;
        bool animate_map_;      /**< Local cache for preferences::animate_map, since it is constantly queried. */
        bool local_tod_light_;  /**< Local cache for preferences "local_tod_lighting" */

    private:
        /** @brief TODO WRITEME */
        void read(const config& cfg);

        /**
         * Finds the start and end rows on the energy bar image.
         *
         * White pixels are substituted for the color of the energy.
         */
        const SDL_Rect& calculate_energy_bar(surface surf);

        /**
         * @brief This surface must be freed by the caller
         * TODO WRITEME explain this better
         * @param loc
         * @return
         */
        surface get_flag(const map_location& loc);

        /** Animated flags for each team */
        std::vector<animated<image::locator> > flags_;

    public:
        /**
         * Draw an image at a certain location.
         * x,y: pixel location on screen to draw the image
         * image: the image to draw
         * reverse: if the image should be flipped across the x axis
         * greyscale: used for instance to give the petrified appearance to a unit image
         * alpha: the merging to use with the background
         * blendto: blend to this color using blend_ratio
         * submerged: the amount of the unit out of 1.0 that is submerged
         *            (presumably under water) and thus shouldn't be drawn
         */
        void render_image(int x,
                          int y,
                          const tdrawing_layer drawing_layer,
                          const map_location& loc,
                          surface image,
                          bool hreverse = false,
                          bool greyscale = false,
                          fixed_t alpha = ftofxp(1.0),
                          Uint32 blendto = 0,
                          double blend_ratio = 0,
                          double submerged = 0.0,
                          bool vreverse = false);

        /**
         * Draw text on a hex. (0.5, 0.5) is the center.
         * The font size is adjusted to the zoom factor.
         */
        void draw_text_in_hex(const map_location& loc,
                              const tdrawing_layer layer,
                              const std::string& text,
                              size_t font_size,
                              SDL_Color color,
                              double x_in_hex = 0.5,
                              double y_in_hex = 0.5);

    protected:

        //TODO sort
        size_t activeTeam_;

        typedef std::list<tblit> tdrawing_buffer;
        tdrawing_buffer drawing_buffer_;

    public:

        /**
         * Add an item to the drawing buffer. You need to update screen on affected area
         *
         * @param layer              The layer to draw on.
         * @param loc                The hex the image belongs to, needed for the
         *                           drawing order.
         * @param blit               The structure to blit.
         */
        void drawing_buffer_add(const tdrawing_layer layer,
                                const map_location& loc,
                                int x,
                                int y,
                                const surface& surf,
                                const SDL_Rect &clip = SDL_Rect());

        void drawing_buffer_add(const tdrawing_layer layer,
                                const map_location& loc,
                                int x,
                                int y,
                                const std::vector<surface> &surf,
                                const SDL_Rect &clip = SDL_Rect());

    protected:

        /** Draws the drawing_buffer_ and clears it. */
        void drawing_buffer_commit();

        /** Clears the drawing buffer. */
        void drawing_buffer_clear();

        /** redraw all panels associated with the map display */
        void draw_all_panels();

        /**
         * Initiate a redraw.
         *
         * Invalidate controls and panels when changed after they have been drawn
         * initially. Useful for dynamic theme modification.
         */
        void draw_init();
        void draw_wrap(bool update, bool force);

        /** Used to indicate to drawing functions that we are doing a map screenshot */
        bool map_screenshot_;

    public:
        //operations for the arrow framework

        void add_arrow(arrow&);

        void remove_arrow(arrow&);

        /** Called by arrow objects when they change. You should not need to call this directly. */
        void update_arrow(arrow & a);

    private:

        typedef std::multimap<map_location, overlay> overlay_map;

        overlay_map overlays_;

        /** Handle for the label which displays frames per second. */
        int fps_handle_;
        /** Count work done for the debug info displayed under fps */
        int invalidated_hexes_;
        int drawn_hexes_;

        bool idle_anim_;
        double idle_anim_rate_;

        surface map_screenshot_surf_;

        std::vector<boost::function<void(display&)> > redraw_observers_;

        /** Debug flag - overlay x,y coords on tiles */
        bool draw_coordinates_;
        /** Debug flag - overlay terrain codes on tiles */
        bool draw_terrain_codes_;

        typedef std::list<arrow*> arrows_list_t;
        typedef std::map<map_location, arrows_list_t> arrows_map_t;
        /** Maps the list of arrows for each location */
        arrows_map_t arrows_map_;

        tod_color color_adjust_;

#if defined(__GLIBC__)
        /** Flag for bug #17573 - this is set in the constructor **/
        bool do_reverse_memcpy_workaround_;
#endif

    protected:
        static display * singleton_;
};

#endif

