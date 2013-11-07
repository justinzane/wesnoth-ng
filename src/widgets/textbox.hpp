/**
 * @file        gui_window.hpp is part of Project wesnoth-ng
 * @author      Justin Zane Chudgar <justin@justinzane.com>
 * @author      David White <dave@whitevine.net>
 * @date        Nov 6, 2013
 * @copyright   Copyright @authors, 2003-2013
 * @section     <license> License GPLv2
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
 * > along with wesnoth-ng.  If not, see <a href="http://www.gnu.org/licenses/">
 * > http://www.gnu.org/licenses/</a>.
 * @brief       TODO WRITEME
 * @note This has been brutally hacked for SDL2.
 * @note Only 32 bits per pixel, ARGB color are supported.
 * @note Unit test code has been removed since it needs to be rewritten.
 */

#ifndef TEXTBOX_HPP_INCLUDED
#define TEXTBOX_HPP_INCLUDED

#include "scrollarea.hpp"

#include "../gui/font.hpp"
#include "../gui/sdl_utils.hpp"
#include "../gui/video.hpp"
#include "../serialization/string_utils.hpp"

//#include <stddef.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_stdinc.h>

#include <algorithm>
#include <atomic>
#include <cstdbool>
#include <cstdlib>
#include <iterator>
#include <string>
#include <vector>

namespace gui {

class textbox:
    public scrollarea
    {
    public:
        textbox(ui_window &ui_win,
                Uint32 width,
                const std::string& text = "",
                bool editable = true,
                Uint32 max_size = 256,
                double alpha = 0.4,
                double alpha_focus = 0.2,
                const bool auto_join = true);
        virtual ~textbox();

        /** @brief Returns the current textbox text. */
        inline const std::string get_text() const {
            return "";
            /// @todo FIXME
//            const std::string &ret = utils::wstring_to_string(text_);
//            return ret;
        }

        /** @brief Set the current textbox text. */
        inline void set_text(const std::string& text,
                             const SDL_Color& color = font::NORMAL_COLOR)
        {
            /// @todo FIXME
            // utils::string_to_wstring(text);
            if (text_.size() > max_size_) {
                text_.resize(max_size_);
            }
            cursor_ = text_.size();
            text_pos_ = 0;
            selstart_ = -1;
            selend_ = -1;
            set_dirty(true);
            update_text_cache(true, color);
            handle_text_changed(text_);
        }

        /** @brief Add to the end of the current textbox text. */
        void append_text(const std::string& text,
                         bool auto_scroll = false,
                         const SDL_Color& color = font::NORMAL_COLOR);

        /** @brief Clears the current textbox text. */
        inline void clear_text() {
            text_.clear();
            cursor_ = 0;
            cursor_pos_ = 0;
            text_pos_ = 0;
            selstart_ = -1;
            selend_ = -1;
            set_dirty(true);
            update_text_cache(true);
            handle_text_changed(text_);
        }

        /** @brief Allow textbox text to be edited. */
        inline void set_editable() { editable_ = true; }

        /** @brief Prevent textbox text to be edited. */
        inline void set_non_editable() { editable_ = false; }

        /** @brief Check whether textbox text is editable. */
        inline bool editable() const { return editable_; }

        inline void scroll_to_bottom() { set_position(get_max_position()); }

        /** @brief Enable text wrap. */
        inline void set_wrap() {
            if (!wrap_) {
                wrap_ = true;
                update_text_cache(true);
                set_dirty(true);
            }
        }

        /** @brief Disable text wrap. */
        inline void set_no_wrap() {
            if (wrap_) {
                wrap_ = false;
                update_text_cache(true);
                set_dirty(true);
            }
        }

        /** @brief Set which textbox is to be edited. */
        inline void set_edit_target(textbox* target) { edit_target_ = target; }

    protected:
        virtual void draw_contents();
        virtual void set_inner_location(SDL_Rect const &);
        virtual void scroll(unsigned int pos);

    private:
        Uint32 max_size_;       /**< Max length of the text contents. */
        wide_string text_;     /**< The text contents. */
        int cursor_;            /**< The cursor position ??? */
        int selstart_;          /**< Position of start of selection. */
        int selend_;            /**< Position of end of selection. */
        bool grabmouse_;        /**< Should mouse be grabbed. */
        int text_pos_;          /**< @todo WRITEME */
        int cursor_pos_;        /**< @todo WRITEME */
        std::vector<int> char_x_;       /**< @todo WRITEME */
        std::vector<int> char_y_;       /**< @todo WRITEME */
        std::atomic<bool> editable_;    /**< Whether the text contents are editable. */
        bool show_cursor_;      /**< Whether the cursor is displayed. */
        int show_cursor_at_;    /**< @brief records the time the cursor was shown at last.
                                 * @details the cursor should be inverted every 500 ms.
                                 * this will be reset when keyboard input events occur. */
        surface text_image_;    /**< Surface to which text is rendered. */
        bool wrap_;             /**< Should long text wrap in the box. */
        Uint32 line_height_;    /**< @todo WRITEME */
        Uint32 yscroll_;        /**< @todo WRITEME */
        double unfocused_alpha_;    /**< Opacity of unfocused text. */
        double focused_alpha_;  /**< Opacity of focused text. */
        textbox* edit_target_;  /**< Pointer to textbox to be edited. */

        virtual void handle_text_changed(const wide_string&) {}

        void handle_event(const SDL_Event& event, bool was_forwarded);

        inline void handle_event(const SDL_Event& event) { handle_event(event, false); }

        inline void pass_event_to_target(const SDL_Event& event) {
            if (edit_target_ && edit_target_->editable()) {
                edit_target_->handle_event(event, true);
            }
        }
        void draw_cursor(int pos, ui_window &video) const;

        void update_text_cache(bool reset = false, const SDL_Color& color = font::NORMAL_COLOR);

        surface add_text_line(const wide_string& text,
                              const SDL_Color& color = font::NORMAL_COLOR);

        inline bool is_selection() {
            return (selstart_ != -1) && (selend_ != -1) && (selstart_ != selend_);
        }

        inline void erase_selection() {
            if (!is_selection()) { return; }
            wide_string::iterator itor = text_.begin() + std::min(selstart_, selend_);
            text_.erase(itor, itor + abs(selend_ - selstart_));
            cursor_ = std::min(selstart_, selend_);
            selstart_ = selend_ = -1;
        }

        /**
         * @brief make it so that only one textbox object can be receiving events at a time.
         */
        bool requires_event_focus(const SDL_Event *event = NULL) const;

        bool show_scrollbar() const;
};

}

#endif
