/**
 * @file 		text_style.hpp is part of wesnoth-ng
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

#ifndef TEXT_STYLE_HPP_
#define TEXT_STYLE_HPP_

#include "sdl2_color.hpp"
#include <pango/pango.h>
#include <pango/pangocairo.h>
#include <string>

/**@brief "Italic" slant. */
typedef enum class font_slant : std::string {
        NORMAL          = "normal",
        OBLIQUE         = "oblique",
        ITALIC          = "italic"
} font_slant_t;

typedef enum class font_weight {
        ULTRALIGHT  = 1,
        EXTRALIGHT  = 2,
        LIGHT       = 3,
        SEMILIGHT   = 4,
        NORMAL      = 5,
        SEMIBOLD    = 6,
        BOLD        = 7,
        EXTRABOLD   = 8,
        ULTRABOLD   = 9
} font_weight_t;

typedef enum class font_variant : std::string {
        NORMAL = "normal",
        SMALLCAPS = "smallcaps"
} font_variant_t;

typedef enum class font_stretch : std::string {
        ULTRACONDENSED  = "ultracondensed",
        EXTRACONDENSED  = "extracondensed",
        CONDENSED       = "condensed",
        SEMICONDENSED   = "semicondensed",
        NORMAL          = "normal",
        SEMIEXPANDED    = "semiexpanded",
        EXPANDED        = "expanded",
        EXTRAEXPANDED   = "extraexpanded",
        ULTRAEXPANDED   = "ultraexpanded"
} font_stretch_t;

typedef enum class font_underline {
        NONE    = "none",
        SINGLE  = "single",
        DOUBLE  = "double",
        LOW     = "low",
} font_underline_t;

typedef enum class font_gravity {
        SOUTH = "south",
        EAST  = "east",
        NORTH = "north",
        WEST  = "west",
        AUTO  = "auto"
} font_gravity_t;

typedef enum class font_grav_hint {
        NATURAL = "natural",
        STRONG  = "strong",
        LINE    = "line"
} font_grav_hint_t;

/**@brief Escapes the markup characters in a text.
 * @details The markups escaped are the ones used in the pango markup. The special
 * characters are: @verbatim <>'"& @endverbatim The escaping is the same as for HTML.
 * @param text                    The text to escape.
 * @returns                       The escaped text. */
inline std::string escape_markup(const std::string& text);

/**@class text_style
 * @brief Text style based on Pango Markup
 * @see http://www.pygtk.org/docs/pygtk/pango-markup-language.html */
class text_style {
    public:
        /**@brief WRITEME */
        text_style() {}
        /**@brief WRITEME */
        ~text_style() {}

        /**@brief get the <span attribute=value... > start tag with the pango markup. */
        std::string get_markup() {
            std::string mu = "<span ";
            mu += "font_family=\"" + face_ + "\" ";
            mu += "font_size=" + std::to_string(font_size_) + " ";
            mu += "font_style=\"" + slant_ + "\" ";
            mu += "font_weight=" + std::to_string(weight_) + " ";
            mu += "font_variant=\"" + variant_ + "\" ";
            mu += "font_stratch=\"" + stretch_ + "\" ";
            mu += "fgcolor=\"" + color_fore_.get_html_color() + "\" ";
            mu += "bgcolor=\"" + color_back_.get_html_color() + "\" ";
            mu += "underline=\"" + underline_ + "\" ";
            mu += "rise=" + std::to_string(rise_) + " ";
            mu += "strikethrough=\"" + ((strikethru_) ? "true" : "false") + "\" ";
            mu += "fallback=\"" + ((fallback_) ? "true" : "false") + "\" ";
            mu += "letter_spacing=" + std::to_string(kern_) + " ";
            /// @todo finish me
            mu += ">";
            return mu;
        }

        /**@brief WRITEME */
        Uint32 get_size() { return font_size_ * 1000; }
        /**@brief WRITEME */
        void set_size(float size) {
            if (size <= 0.0f) { throw std::invalid_argument("Cannot have font size <= 0.0."); }
            font_size_ = (Uint32)(size / 1000.0f);
        }

        /**@brief WRITEME */
        std::string& get_family() { return face_; }
        /**@brief WRITEME */
        void set_family(std::string family) {
            /// @todo Validate input.
            face_ = family;
        }

        /**@brief WRITEME */
        font_weight& get_weight() {return weight_; }
        /**@brief WRITEME */
        void set_weight(font_weight& weight) { weight_ = weight; }

        /**@brief WRITEME */
        font_slant& get_slant() { return slant_; }
        /**@brief WRITEME */
        void set_slant(font_slant slant) { slant_ = slant; }

        /**@brief WRITEME */
        font_stretch& get_stretch() { return stretch_; }
        /**@brief WRITEME */
        void set_stretch(font_stretch& stretch) { stretch_ = stretch; }

        ///@todo write color setters/getters

        /**@brief WRITEME */
        font_underline& get_underline() { return underline_; }
        /**@brief WRITEME */
        void set_underline(font_underline& underline) { underline_ = underline; }

        /**@brief get the rise above baseline in "em" units. */
        float get_rise() { return rise_ / 10000.0f; }
        /**@brief set the rise above baseline in "em" units. */
        void set_rise(float rise) { rise_ = (int)(rise * 10000.0f); }

        /**@brief get the kerning adjustment (inter-letter spacing) in points. */
        float get_kern_adj() { return kern_ / 1024.0f; }
        /**@brief set the kerning adjustment (inter-letter spacing) in points. */
        void set_kern_adj(float kern) { kern = (Sint32)(1024.0f * kern); }

        font_gravity& get_gravity() { return grav_; }
        void set_gravity(font_gravity& grav) { grav_ = grav; }

        font_grav_hint& get_grav_hint() { return hint_; }
        void set_grav_hint(font_grav_hint& hint) { hint_ = hint; }

//        PangoAlignment get_alignment() { return align_; }
//        void set_alignment(PangoAlignment align) { align_ = align; }

    private:
        Uint32         font_size_  {10000};                /**< font size in thousandths of a point */
        std::string    face_       {"Tengwar Telcontar"};  /**< Font family name such as "normal",
                                                                "sans", "serif" or "monospace". */
        font_weight    weight_     {font_weight::NORMAL};  /**< Bold */
        font_variant   variant_    {font_variant::NORMAL}; /**< Smallcaps */
        font_slant     slant_      {font_slant::NORMAL};   /**< Italic */
        font_stretch   stretch_    {font_stretch::NORMAL}; /**< Kerning */
        sdl2_color     color_fore_ {127,127,127,255};
        sdl2_color     color_back_ {127,127,127,0};
        font_underline underline_  {font_underline::NONE};
        Sint32         rise_       {0};  /**< vertical displacement from baseline, in ten
                                              thousandths of an em. Can be negative for subscript,
                                              positive for superscript. */
        bool           strikethru_ {false};
        bool           fallback_   {true};  /**< Enable fallback to other fonts of characters
                                                 are missing from the current font. */
        Sint32         kern_       {0};  /**< Inter-letter spacing in 1024ths of a point. */
        font_gravity   grav_       {font_gravity::AUTO};
        font_grav_hint hint_       {font_grav_hint::NATURAL};
//        PangoAlignment align_      {PANGO_ALIGN_LEFT};


};

#endif /* TEXT_STYLE_HPP_ */
