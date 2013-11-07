/*
 Copyright (C) 2011 - 2013 by Fabian Mueller
 Part of the Battle for Wesnoth Project http://www.wesnoth.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY.

 See the COPYING file for more details.
 */

//#include "board/map.hpp"
#include "board/map_location.hpp"
//#include "gui/sdl_utils.hpp"

#include <SDL2/SDL_joystick.h>

#include <utility>
#include <vector>

#ifndef JOYSTICK_HPP_
#define JOYSTICK_HPP_

class joystick_manager {

    public:

        joystick_manager();

        ~joystick_manager();

        bool init();
        bool close();

        /**
         * Used for absolute movement of the cursor.
         * @param highlighted_hex will change if the cursor moved.
         * @return true if the highlighted hex changed.
         */
        bool update_highlighted_hex(map_location& highlighted_hex);

        /**
         * Used for relative movement of the cursor.
         * @param highlighted_hex will change if the cursor moved.
         * @param map_location TODO WRITEME
         * @return true if the highlighted hex changed.
         */
        bool update_highlighted_hex(map_location& highlighted_hex,
                                    const map_location& selected_hex);

        /**
         * @return a value in range [-1,+1] representing the gauges of the scroll axes.
         */
        std::pair<double, double> get_scroll_axis_pair();

        /**
         * TODO fendrin
         */
        std::pair<double, double> get_cursor_polar_coordinates();

        /**
         * TODO fendrin
         */
        std::pair<double, double> get_mouse_axis_pair();

        /**
         * TODO fendrin
         */
        double get_thrusta_axis();

        /**
         * TODO fendrin
         */
        double get_thrustb_axis();

        /**
         * TODO fendrin
         */
        double get_angle();

        /**
         * @todo WRITEME Better.
         * @brief Get distance and direction from joystick ???
         * @param joystick_xaxis    ???
         * @param xaxis             ???
         * @param joystick_yaxis    ???
         * @param yaxis             ???
         * @return  distance, direction in unit?, unit?
         */
        std::pair<double, double> get_polar_coordinates(int joystick_xaxis,
                                                        int xaxis,
                                                        int joystick_yaxis,
                                                        int yaxis);

    private:
        /**
         * @enum WRITEME
         * @todo Explain why these are not given values in degrees?
         */
        enum DIRECTION {
                NORTH,      //!< NORTH
                NORTH_EAST, //!< NORTH_EAST
                SOUTH_EAST, //!< SOUTH_EAST
                SOUTH,      //!< SOUTH
                SOUTH_WEST, //!< SOUTH_WEST
                NORTH_WEST, //!< NORTH_WEST
                NDIRECTIONS,//!< NDIRECTIONS
                WEST,       //!< WEST
                EAST        //!< EAST
        };

        /**
         * @brief TODO WRITEME
         * @param joystick_xaxis
         * @param xaxis
         * @param joystick_yaxis
         * @param yaxis
         * @return
         */
        std::pair<int, int> get_axis_pair(int joystick_xaxis,
                                          int xaxis,
                                          int joystick_yaxis,
                                          int yaxis);

        /**
         * @brief TODO WRITEME
         * @param joystick_axis
         * @param axis
         * @return
         */
        int get_axis(int joystick_axis, int axis);

        /**
         * @brief TODO WRITEME
         * @param x_axis
         * @param y_axis
         * @param old_hex
         * @return
         */
        const map_location get_next_hex(int x_axis,
                                        int y_axis,
                                        map_location old_hex);

        /**
         * @brief TODO WRITEME
         * @param loc
         * @param direction
         * @return
         */
        const map_location get_direction(const map_location& loc,
                                         joystick_manager::DIRECTION direction);

        std::vector<SDL_Joystick*> joysticks_;  /**< A list of all available joysticks. */
        int joystick_area_;                     /**< TODO WRITEME */
        int counter_;                           /**< TODO WRITEME */
};

#endif /* JOYSTICK_HPP_ */
