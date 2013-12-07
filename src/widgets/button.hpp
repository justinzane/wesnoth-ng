/**
 * @file src/widgets/button.hpp
 * @project The Battle for Wesnoth NG - https://github.com/justinzane/wesnoth-ng
 * @brief 
 * @authors 
 * @copyright Copyright (C) 2003 - 2013 by David White <dave@whitevine.net>
 Part of the Battle for Wesnoth Project http://www.wesnoth.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY.

 See the COPYING file for more details.
 */
#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED

#include "widget.hpp"

#include "../exceptions.hpp"

namespace gui {

enum class button_type_t {
    TYPE_PRESS, TYPE_CHECK, TYPE_TURBO, TYPE_IMAGE, TYPE_RADIO
} button_type;

enum class button_space_consumption_t {
    DEFAULT_SPACE, MINIMUM_SPACE
} button_space_consumption ;

enum class button_state_t {
    UNINIT, NORMAL, ACTIVE, PRESSED, PRESSED_ACTIVE, TOUCHED_NORMAL, TOUCHED_PRESSED
} button_state;

class button: public widget {
    public:
        struct error: public game::error {
                error() :
                    game::error("GUI1 button error") {
                }
        };

        button(CVideo& video,
               const std::string& label,
               button_type_t type = button_type_t::TYPE_PRESS,
               std::string button_image = "",
               button_space_consumption_t spacing = button_space_consumption_t::DEFAULT_SPACE,
               const bool auto_join = true,
               std::string overlay_image = "");

        /** Default implementation, but defined out-of-line for efficiency reasons. */
        virtual ~button();
        void set_check(bool check);
        void set_active(bool active);bool checked() const;

        void set_label(const std::string& val);
        void set_image(const std::string& image_file_base);
        void set_overlay(const std::string& image_file_base);

        bool pressed();bool hit(int x, int y) const;
        virtual void enable(bool new_val = true);
        void release();

    protected:
        virtual void handle_event(const SDL_Event& event);
        virtual void mouse_motion(const SDL_MouseMotionEvent& event);
        virtual void mouse_down(const SDL_MouseButtonEvent& event);
        virtual void mouse_up(const SDL_MouseButtonEvent& event);
        virtual void draw_contents();

        button_type_t type_;

    private:

        void load_images();

        void calculate_size();

        std::string label_ {"woops"};
        SDL_Surface* image_ {nullptr};
        SDL_Surface* pressedImage_ {nullptr};
        SDL_Surface* activeImage_ {nullptr};
        SDL_Surface* pressedActiveImage_ {nullptr};
        SDL_Surface* touchedImage_ {nullptr};
        SDL_Surface* disabledImage_ {nullptr};
        SDL_Surface* pressedDisabledImage_ {nullptr};
        SDL_Surface* overlayImage_ {nullptr};
        SDL_Surface* overlayPressedImage_ {nullptr};
        SDL_Surface* overlayPressedDisabledImage_ {nullptr};
        SDL_Surface* overlayDisabledImage_ {nullptr};
        SDL_Surface* overlayActiveImage_ {nullptr};
        SDL_Rect textRect_ {0,0,0,0};

        button_state_t state_ {button_state_t::NORMAL};

        bool pressed_ {false};

        button_space_consumption_t spacing_ {button_space_consumption_t::DEFAULT_SPACE};

        int base_height_ {0};
        int base_width_  {0};

        std::string button_image_name_ {"oopsie"};
        std::string button_overlay_image_name_ {"daisy"};

};
//end class button

}

#endif
