/**
 * @file 		text_block.hpp is part of wesnoth-ng
 * @author		Justin Zane Chudgar, <justin@justinzane.com>
 * @date		Dec 13, 2013
 * @copyright   Copyright justin, 2013
 * @section		License GPLv3
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
 * > along with wesnoth-ng.  If not, see http://www.gnu.org/licenses/ .
 * @brief		TODO WRITEME
 * @details		TODO WRITEME
 */

#ifndef TEXT_BLOCK_HPP_
#define TEXT_BLOCK_HPP_

#include "sdl2_rect.hpp"
#include "text_style.hpp"
#include "img.hpp"
#include <string>
#include <map>

/**@class text_block
 * @brief A block of text for rendering to an image. */
class text_block {
    public:
        text_block() {}
        ~text_block() {}

        /**@defgroup txt_content Text Content Manipulation ************************************/

        /**@ingroup txt_content @brief */
        std::string& get_text();

        /**@ingroup txt_content @brief */
        std::u16string& get_text16();

        /**@ingroup txt_content @brief */
        std::u32string& get_text32();

        /**@ingroup txt_content @brief */
        std::wstring& get_textw();

        /**@ingroup txt_content @brief Replaces any existing text with given utf8 text.
         * @param text      The utf8 text to use.
         * @param style     The text style for the characters. */
        void set_text(const std::string& text, const text_style& style);

        /**@ingroup txt_content @brief Replaces any existing text with given utf16 text.
         * @param text      The utf16 text to use.
         * @param style     The text style for the characters. */
        void set_text(const std::u16string& text, const text_style& style);

        /**@ingroup txt_content @brief Replaces any existing text with given utf32 text.
         * @param text      The utf32 text to use.
         * @param style     The text style for the characters. */
        void set_text(const std::u32string& text, const text_style& style);

        /**@ingroup txt_content @brief Replaces any existing text with given wide text.
         * @param text      The wide text to use.
         * @param style     The text style for the characters. */
        void set_text(const std::wstring& text, const text_style& style);

        /**@ingroup txt_content @brief Appends utf8 text.
         * @param text      The utf8 text to insert.
         * @param style     The text style for the inserted characters. */
        void append_text(const std::string& text, const text_style& style);

        /**@ingroup txt_content @brief Appends utf16 text.
         * @param text      The utf16 text to insert.
         * @param style     The text style for the inserted characters. */
        void append_text(const std::u16string& text, const text_style& style);

        /**@ingroup txt_content @brief Appends utf32 text.
         * @param text      The utf32 text to insert.
         * @param style     The text style for the inserted characters. */
        void append_text(const std::u32string& text, const text_style& style);

        /**@ingroup txt_content @brief Appends wide text.
         * @param text      The wide text to insert.
         * @param style     The text style for the inserted characters. */
        void append_text(const std::wstring& text, const text_style& style);

        /**@ingroup txt_content @brief Inserts utf8 text.
         * @param offset    The position, in characters, at which to insert the text.
         * @param text      The utf8 text to insert.
         * @param style     The text style for the inserted characters. */
        void insert_text(const Uint32 offset, const std::string& text, const text_style& style);

        /**@ingroup txt_content @brief Deletes text.
         * @param offset   The position, in characters, at which to begin deleting.
         * @param length   The length, in characters, to delete. */
        void delete_text(const Uint32 offset, const Uint32 length);

        /**@defgroup txt_img Text Image Functionality *****************************************/

        /**@ingroup txt_img @brief number of characters in text. */
        Uint32 get_text_length();

        /**@ingroup txt_img @brief The number of pixels wide the rendered text is. */
        Uint32 rendered_width();

        /**@ingroup txt_img @brief The number of pixels high the rendered text is. */
        Uint32 rendered_height();

        /**@ingroup txt_img @brief The rectangle [0,0,w,h] defined by the extent of the
         * rendered text. */
        rectangle rendered_rect();

        /**@ingroup txt_img @brief Check if current text image will fit in given rectangle. */
        bool will_fit(rectangle area);

        /**@ingroup txt_img @brief Get the text rendered as an image. */
        img as_image();

        /**@defgroup txt_cursor Text Cursor Functionality *************************************/

        /**@ingroup txt_cursor @brief get the current cursor position. */
        Uint32 get_cursor_pos() { return cursor_pos_; }

        /**@ingroup txt_cursor @brief set the current cursor position. */
        void set_cursor_pos(Uint32 pos) {
            if (this->is_valid_cursor_pos(pos)) {
                cursor_pos_ = pos;
            } else {
                throw std::invalid_argument("Invalid cursor position provided.");
            }
        }

        /**@ingroup txt_cursor @brief Is argument a valid cursor position. */
        bool is_valid_cursor_pos(Uint32 pos) {
            if (pos <= this->get_text_length()) { return true; } else { return false; }
        }

    private:
        std::vector<std::pair<std::string, text_style> > block_ {"", new text_style()};
        Uint32 cursor_pos_ {0};

};

#endif /* TEXT_BLOCK_HPP_ */
