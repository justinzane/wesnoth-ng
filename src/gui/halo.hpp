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

/** @file */

#ifndef HALO_HPP_INCLUDED
#define HALO_HPP_INCLUDED

class display;

#include "board/map_location.hpp"
#include <opencv2/opencv.hpp>

/**
 * @brief TODO WRITEME
 */
class halo_mgr {
    public:
        halo_mgr(display& disp);
        ~halo_mgr();

    private:
        display* const old;
};

enum halo_orientation_t {
    NORMAL,
    HORIZ_REFLECT, /**< top->bottom */
    VERT_REFLECT,  /**< left->right */
    BOTH_REFLECT   /**< top->bottom and left->right*/
};

const int HALO_INVALID = -666; /**< Invalid handle */

/**
 * @brief Add a haloing effect using image centered on (x,y).
 * @details If the halo is attached to an item, it needs to be hidden if the
 * shroud is active. If it is not attached to an item, the location should
 * be set to -1, -1.
 * @note It will be shown with the fog active.
 * @param x             x-coordiante of halo center
 * @param y             y-coordinate of halo center
 * @param image         the source image for the halo
 * @param loc           the board location to display the halo
 * @param orientation   for non-symettric halos, the orientation to display.
 * Reflection direction refers to the axis of reflection. That is, the top row
 * of pixels of an image will reflect to the bottom with HORIZ_REFLECT.
 * @param infinite      ??? TODO WRITEME
 * @return The handle to the halo object. Returns HALO_INVALID for invalid halos.
 */
int halo_add(int x,
             int y,
             const std::string& image,
             const map_location& loc,
             halo_orientation_t orientation = NORMAL,
             bool infinite = true);

/**
 * @brief Set the position of an existing halo identified by its handle.
 * @param handle
 * @param x
 * @param y
 */
void halo_set_loc(int handle, int x, int y);

/** Remove the halo with the given handle. */
void halo_remove(int handle);

/**
 * @brief Render halos.
 * @details Which halos are rendered is determined by the
 * internal state in the control sets (in halo.cpp). ???
 */
void halo_render();

/**
 * @brief Unrender halos.
 * @details Which halos are unrendered is determined by invalidated_locations and the
 * internal state in the control sets (in halo.cpp). ???
 */
void halo_unrender(std::set<map_location> invalidated_locations);

#endif
