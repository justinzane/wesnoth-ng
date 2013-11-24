/**
 * @file    src/sdl_utils.hpp
 * @project The Battle for Wesnoth NG - https://github.com/justinzane/wesnoth-ng
 * @brief   Utility functionality for the SDL2 library.
 * @authors David White <dave@whitevine.net>, Justin Zane Chudgar <justin@justinzane.com>
 * @copyright Copyright (C) 2003 - 2013 by @authors
 * @section License GPLv2
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY. See the COPYING file for more details.
 * @note For SDL2, **ALL** surfaces will be in ARGB8888 format. Please use the
 * macros "BPP, RMASK, GMASK, BMASK, AMASK" as the last 5 parameters to
 * SDL_CreateSurface.
 * @note For SDL2, which has a very elegant C API, almost all wrapper functions
 * have been removed. The reason for this change is that the existing wrappers were
 * poorly and inconsistently named and often thoroughly unsuitable for SDL2.
 * @note For SDL2 and Wesnoth-NG, the goals is to abandon the BlitSurface paradigm and move to
 * providing the Renderer with Textures (in GPU RAM) and manipulating them through GL-based
 * functionality as provided by SDL2.
 */

#ifndef SDL_UTILS_INCLUDED
#define SDL_UTILS_INCLUDED

#include "defs.hpp"
#include "floating_point_emulation.hpp"
#include "global.hpp"
#include "scoped_resource.hpp"
#include "util.hpp"
#include "video.hpp"
#include "arch_dep/neon.hpp"

#include <boost/math/constants/constants.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <SDL2/SDL.h>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iosfwd>
#include <iostream>
#include <map>
#include <math.h>
#include <stdint.h>
#include <string>
#include <tuple>

#define SDL_UTILS_ROTATE_CROP 201
#define SDL_UTILS_ROTATE_GROW 202

/** @name Event Handling Functionality @{ ---------------------------------------------------*/

SDLKey sdl_keysym_from_name(std::string const &keyname);

/** @} end Event Handling Functionality------------------------------------------------------*/

/** @name Geometric Utilities @{ ------------------------------------------------------------*/

/** @brief Returns true if the point defined by x,y is in rect. */
inline bool is_point_in_rect(int x, int y, const SDL_Rect* rect) {
    return x >= rect->x && y >= rect->y && x < rect->x + rect->w && y < rect>y + rect->h;
}

/** @brief Returns true if the rects intersect. */
inline bool rects_overlap(const SDL_Rect* rect1, const SDL_Rect* rect2) {
    return (rect1->x < rect2->x + rect2->w &&
            rect2->x < rect1->x + rect1->w &&
            rect1->y < rect2->y + rect2->h &&
            rect2->y < rect1->y + rect1->h);
}

/** @brief If the provided rectangles intersect, the rectangle defined by the
 * intersection is returned; else a {0,0,0,0} rect is returned. */
inline SDL_Rect rects_intersect(SDL_Rect const &rect1, SDL_Rect const &rect2) {
    SDL_Rect res;
    res.x = std::max<int>(rect1.x, rect2.x);
    res.y = std::max<int>(rect1.y, rect2.y);
    int w = std::min<int>(rect1.x + rect1.w, rect2.x + rect2.w) - res.x;
    int h = std::min<int>(rect1.y + rect1.h, rect2.y + rect2.h) - res.y;
    if (w <= 0 || h <= 0) { SDL_Rect er {0,0,0,0}; return er; }
    res.w = w;
    res.h = h;
    return res;
}

/** @brief Provides the minimal area rect that encloses both arguments.
 * @todo WRITEME this function needs to be coded. */
inline SDL_Rect union_rects(SDL_Rect const &rect1, SDL_Rect const &rect2) {
    if (rect1.w == 0 || rect1.h == 0) return rect2;
    if (rect2.w == 0 || rect2.h == 0) return rect1;
    SDL_Rect res;
    res.x = std::min<int>(rect1.x, rect2.x);
    res.y = std::min<int>(rect1.y, rect2.y);
    res.w = std::max<int>(rect1.x + rect1.w, rect2.x + rect2.w) - res.x;
    res.h = std::max<int>(rect1.y + rect1.h, rect2.y + rect2.h) - res.y;
    return res;
}

/** @brief Provides the vector of rects that compose the true geometric union
 * of both arguments.
 * @todo WRITEME this function needs to be coded. */
std::vector<SDL_Rect> rects_union(const SDL_Rect* rect1, const SDL_Rect* rect2);

/** @brief SDL_Rect equality operator */
inline bool operator==(const SDL_Rect* a, const SDL_Rect* b) {
    return a->x == b->x && a->y == b->y && a->w == b->w && a->h == b->h;
}

/** @brief SDL_Rect inequality operator */
inline bool operator!=(const SDL_Rect* a, const SDL_Rect* b) {
    return !operator==(a, b);
}

/** @brief SDL_Rect equality operator */
inline bool operator==(const SDL_Rect a, const SDL_Rect b) {
    return a.x == b.x && a.y == b.y && a.w == b.w && a.h == b.h;
}

/** @brief SDL_Rect inequality operator */
inline bool operator!=(const SDL_Rect a, const SDL_Rect b) {
    return !operator==(a, b);
}

/** @} end Geometric Utilities --------------------------------------------------------------*/

/** @name Color Utilities @{ ----------------------------------------------------------------*/

/** @brief SDL_Color equality operator */
inline bool operator==(const SDL_Color* a, const SDL_Color* b) {
    return a->r == b->r && a->g == b->g && a->b == b->b && a->a == b->a;
}

/** @brief SDL_Color equality operator */
inline bool operator==(const SDL_Color a, const SDL_Color b) {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

/** @brief SDL_Color inequality operator */
inline bool operator!=(const SDL_Color* a, const SDL_Color* b) {
    return !operator==(a, b);
}

/** @brief SDL_Color inequality operator */
inline bool operator!=(const SDL_Color a, const SDL_Color b) {
    return !operator==(a, b);
}

/** @brief Invert the hue of the color in HSV space. */
SDL_Color invert_hue(const SDL_Color* color);

/** @brief Invert the value of the color in HSV space. */
SDL_Color invert_value(const SDL_Color* color);

/**
 * @brief convert a 32bit unsigned int to an SDL color, alpha -> unused.
 * @details
 *      SDL_Color.alpha  = bits 24-31
 *      SDL_Color.r      = bits 16-23
 *      SDL_Color.g      = bits  8-15
 *      SDL_Color.b      = bits  0- 7
 */
inline SDL_Color get_sdl_color(const Uint32 argb) {
    SDL_Color result {(RMASK & argb) >> 16,
                      (GMASK & argb) >>  8,
                      (BMASK & argb),
                      (AMASK & argb) >> 24};
    return result;
}

/** @brief convert 4 chars unsigned int to an SDL ARGB8888 color. */
inline SDL_Color get_sdl_color(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a) {
   SDL_Color result{r, g, b, a};
   return result;
}

/** @brief convert 4 32bit unsigned ints to an SDL color.
 *  @note values above and below 0-255 are replaced with the closest valid value. */
inline SDL_Color get_sdl_color(const int r, const int g, const int b, const int a) {
    SDL_Color result {(r < 0) ? 0 : ((r > 255) ? 255 : (Uint8)r),
                      (g < 0) ? 0 : ((g > 255) ? 255 : (Uint8)g),
                      (b < 0) ? 0 : ((b > 255) ? 255 : (Uint8)b),
                      (a < 0) ? 0 : ((a > 255) ? 255 : (Uint8)a)};
    return result;
}

/** @brief convert 4 size_ts to an SDL color.
 *  @note values above and below 0-255 are replaced with the closest valid value. */
inline SDL_Color get_sdl_color(const size_t r, const size_t g, const size_t b, const size_t a) {
    SDL_Color result {(r > 255) ? 255 : (Uint8)r,
                      (g > 255) ? 255 : (Uint8)g,
                      (b > 255) ? 255 : (Uint8)b,
                      (a > 255) ? 255 : (Uint8)a};
    return result;
}

/** @brief convert 4 floats to an SDL color.
 *  @note values above and below 0-255 are replaced with the closest valid value. */
inline SDL_Color get_sdl_color(const float r, const float g, const float b, const float a) {
    SDL_Color result {(r < 0.0f) ? 0 : ((r > 255.0f) ? 255 : (Uint8)r),
                      (g < 0.0f) ? 0 : ((g > 255.0f) ? 255 : (Uint8)g),
                      (b < 0.0f) ? 0 : ((b > 255.0f) ? 255 : (Uint8)b),
                      (a < 0.0f) ? 0 : ((a > 255.0f) ? 255 : (Uint8)a)};
    return result;
}

/** @brief convert 4 doubles to an SDL color.
 *  @note values above and below 0-255 are replaced with the closest valid value. */
inline SDL_Color get_sdl_color(const double r, const double g, const double b, const double a) {
    SDL_Color result {(r < 0.0) ? 0 : ((r > 255.0) ? 255 : (Uint8)r),
                      (g < 0.0) ? 0 : ((g > 255.0) ? 255 : (Uint8)g),
                      (b < 0.0) ? 0 : ((b > 255.0) ? 255 : (Uint8)b),
                      (a < 0.0) ? 0 : ((a > 255.0) ? 255 : (Uint8)a)};
    return result;
}

/** @} end Color Utilities ------------------------------------------------------------------*/

/** @name Image Utilities @{ ----------------------------------------------------------------*/

/**
 * @brief New opencv based resize function.
 * @details Replaces scale_surface_horizontal, scale_surface_vertical, scale_surface,
 * scale_surface_sharp
 * @param surf      Source surface, non-null.
 * @param h         New height, >0.
 * @param w         New width, >0.
 * @param optimize  Whether to optimize return surface
 * @return          Resized surface.
 */
surface resize_surface(SDL_Surface* surf, const unsigned h, const unsigned w);

/** @brief wrapper for resize_surface.*/
surface stretch_surface_horizontal(SDL_Surface* surf,
                                   const unsigned w) __attribute__((deprecated));

/** @brief wrapper for resize_surface.*/
surface stretch_surface_vertical(SDL_Surface* surf,
                                 const unsigned h) __attribute__((deprecated));

/** @brief wrapper for resize_surface.*/
surface scale_surface(SDL_Surface* surf, int w, int h) __attribute__((deprecated));

/** @brief wrapper for resize_surface.*/
surface scale_surface_sharp(SDL_Surface* surf, int w, int h) __attribute__((deprecated));

/**
 * @brief Tile a surface
 * @param src                The source surface.
 * @param dst                The tiled surface.
 * @param w                  The width of the resulting surface.
 * @param h                  The height of the resulting surface.
 */
void tile_surface(SDL_Surface* src, SDL_Surface* dst);

/**
 * @brief Tile a surface
 * @param src               The source surface.
 * @param w                  The width of the resulting surface.
 * @param h                  The height of the resulting surface.
 * @return                   A surface containing the tiled version of the source.
 */
SDL_Surface tile_surface(SDL_Surface* src, int w, int h);

/**
 * @brief Adjust the hue of a surface by hue_diff degrees (0 <= hue_diff < 360).
 * @cite https://en.wikipedia.org/wiki/HSL_and_HSV
 */
void surface_change_hue(SDL_Surface* surf, float hue_diff);

/**
 * @brief Adjust the value of a surface by val_diff (Blacken: -1 < val_diff < 1 :Whiten).
 * @cite https://en.wikipedia.org/wiki/HSL_and_HSV
 */
void surface_change_val(SDL_Surface* surf, float val_diff);

/**
 * @brief Adjust the saturation of a surface by sat_diff (Grey: -1 < sat_diff < 1 :Vivid).
 * @cite https://en.wikipedia.org/wiki/HSL_and_HSV
 */
void surface_change_sat(SDL_Surface* surf, float sat_diff);

/**
 * @brief create an heavy shadow of the image, by blurring, increasing alpha and darkening
 */
surface shadow_image(const SDL_Surface* surf,   /**< @param surf source image */
                     bool optimize = true); /**< @param optimize whether to optimize the return surface. */

/**
 * Recolors a surface using a map with source and converted palette values.
 * This is most often used for team-coloring.
 *
 * @param surf               The source surface.
 * @param map_rgb            Map of color values, with the keys corresponding to the
 *                           source palette, and the values to the recolored palette.
 * @param optimize           Whether the new surface should be RLE encoded. Only
 *                           useful when the source is not the screen and it is
 *                           going to be used multiple times.
 * @return                   A recolored surface, or a null surface if there are
 *                           problems with the source.
 */
surface recolor_image(surface surf, const std::map<Uint32, Uint32>& map_rgb, bool optimize =
    true);

/**
 * @brief Increases the brightness of an image.
 * @param surf
 * @param amount
 * @param optimize
 * @return
 */
surface brighten_image(const SDL_Surface* surf, fixed_t amount, bool optimize = true);

/** Get a portion of the screen.
 *  Send nullptr if the portion is outside of the screen.
 *  @param surf              The source surface.
 *  @param rect              The portion of the source surface to copy.
 *  @param optimize_format   Optimize by converting to result to display format.
 *                           Only useful if the source is not the screen and you
 *                           plan to blit the result on screen several times.
 *  @return                  A surface containing the portion of the source.
 *                           No RLE or Alpha bits are set.
 *  @retval 0                if error or the portion is outside of the surface.
 */
surface get_surface_portion(const SDL_Surface* surf, SDL_Rect* rect, bool optimize_format = false);

surface adjust_surface_alpha(const SDL_Surface* surf, fixed_t amount, bool optimize = true);
surface adjust_surface_alpha_add(const SDL_Surface* surf, int amount, bool optimize = true);

/** Applies a mask on a surface. */
surface mask_surface(const SDL_Surface* surf,
                     const SDL_Surface* mask,
                     bool* empty_result = nullptr,
                     const std::string& filename = std::string());

/** Check if a surface fit into a mask */
bool in_mask_surface(const SDL_Surface* surf, const SDL_Surface* mask);

/** Progressively reduce alpha of bottom part of the surface
 *  @param surf              The source surface.
 *  @param depth             The height of the bottom part in pixels
 *  @param alpha_base        The alpha adjustment at the interface
 *  @param alpha_delta       The alpha adjustment reduction rate by pixel depth
 *  @param optimize          Optimize by converting to result to display
 */
surface submerge_alpha(const SDL_Surface* surf,
                       int depth,
                       float alpha_base,
                       float alpha_delta,
                       bool optimize = true);

/**
 * Light surf using lightmap
 * @param surf               The source surface.
 * @param lightmap           add/subtract this color to surf
 *                           but RGB values are converted to (X-128)*2
 *                           to cover the full (-256,256) spectrum.
 *                           Should already be neutral
 * @param optimize           Whether the new surface should be RLE encoded.
 */
surface light_surface(const SDL_Surface* surf, const SDL_Surface* lightmap, bool optimize = true);

/**
 * @brief Performs a basic box blur on a surface
 * @param surf      source surface to be blurred, RGB or RGBA, not RLE encoded.
 * @param depth     size of the kernel used for blurring - 1, 0 < depth < min(surf->h, surf->w)
 * @param optimize  Whether the new surface should be RLE encoded.
 * @return  the blurred surface.
 */
surface surface_blur(const SDL_Surface* surf, int depth = 1, bool optimize = true);

/**
 * @brief Performs a basic box blur on a sub-rectangle of a surface.
 * @param surf      source surface to be blurred, RGB or RGBA, not RLE encoded.
 * @param rect      rectangle completely within bounds of surf
 * @param depth     size of the kernel used for blurring - 1, 0 < depth < min(surf->h, surf->w)
 */
void surface_blur(SDL_Surface* surf, SDL_Rect rect, unsigned depth = 1);

/** @brief wrapper for blur surface. */
surface blur_alpha_surface(const SDL_Surface* surf, int depth = 1, bool optimize = true)
    __attribute__((deprecated ("Use blur_surface instead of blur_alpha_surface.")));

/** Cuts a rectangle from a surface. */
surface cut_surface(const SDL_Surface* surf, SDL_Rect const &r);

/**
 * @brief Blends a surface with a color.
 * @details  Every pixel in the surface will be blended with the @p color given. The
 * final color of a pixel is amount * @p color + (1 - amount) * original.
 * @param surf                    The surface to blend.
 * @param amount                  The amount of the new color is determined by
 *                                @p color. Must be a number in the range
 *                                [0, 1].
 * @param color                   The color to blend width, note its alpha
 *                                channel is ignored.
 * @param optimize                Should the return surface be RLE optimized.
 * @return                        The blended surface.
 */
surface blend_surface(const SDL_Surface* surf,
                      const double amount,
                      const Uint32 color,
                      const bool optimize = true);

/**
 * @brief Rotates a surface.
 * @param surf          the source surface
 * @param deg           the amount of rotation in counter-clockwise degrees
 * @param center_x      the x-coordinate of the center of rotation
 * @param center_y      the y-coordinate of the center of rotation
 * @param scale         the resize scaling for the result
 * @param croporgrow    SDL_UTILS_ROTATE_CROP indicates that portions of the image rotated
 *                      outside the original image bounds are cropped
 *                      SDL_UTILS_ROTATE_CROP indicates that the returned surface is enlarged
 *                      to hold the complete rotated image
 * @param optimize      true caused the surface to be RLE optimized
 * @return              the rotated image as a surface.
 */
surface rotate_surface(const SDL_Surface* surf,
                       float deg,
                       float scale,
                       int center_x,
                       int center_y,
                       int croporgrow=SDL_UTILS_ROTATE_CROP,
                       bool optimize=true);

/**
 * @brief Rotates a surface.
 * @param surf          the source surface
 * @param deg           the amount of rotation in counter-clockwise degrees
 * @param center_x      the x-coordinate of the center of rotation
 * @param center_y      the y-coordinate of the center of rotation
 * @param scale         the resize scaling for the result
 * @param croporgrow    SDL_UTILS_ROTATE_CROP indicates that portions of the image rotated
 *                      outside the original image bounds are cropped
 *                      SDL_UTILS_ROTATE_CROP indicates that the returned surface is enlarged
 *                      to hold the complete rotated image
 * @param optimize      true caused the surface to be RLE optimized
 * @return              the rotated image as a surface.
 */
surface rotate_surface(const SDL_Surface* surf,
                       double deg,
                       double scale,
                       int center_x,
                       int center_y,
                       int croporgrow=SDL_UTILS_ROTATE_CROP,
                       bool optimize=true);

/**
 * @brief Rotates a surface by any degrees.
 * @pre @zoom >= @offset          Otherwise @return will have empty pixels.
 * @pre @offset > 0               Otherwise the procedure will not return.
 * @param surf                    The surface to rotate.
 * @param angle                   The angle of rotation.
 * @param zoom                    Which zoom level to use for calculating the result.
 * @param offset                  Pixel offset when scanning the zoomed source.
 * @param optimize                Should the return surface be RLE optimized.
 * @return                        The rotated surface.
 * @deprecated Use rotate_surface instead.
 */
surface rotate_any_surface(const SDL_Surface* surf, float angle, int zoom, int offset,
                           bool optimize = true)
                           __attribute__((deprecated("Use rotate_surface.")));

/**
 * @brief Rotates a surface 180 degrees.
 * @param surf                    The surface to rotate.
 * @param optimize                Should the return surface be RLE optimized.
 * @return                        The rotated surface.
 * @deprecated Use rotate_surface instead.
 */
surface rotate_180_surface(const SDL_Surface* surf, bool optimize = true)
                           __attribute__((deprecated("Use rotate_surface.")));

/**
 * Rotates a surface 90 degrees.
 * @param surf                    The surface to rotate.
 * @param clockwise               Whether the rotation should be clockwise (true)
 *                                or counter-clockwise (false).
 * @param optimize                Should the return surface be RLE optimized.
 * @return                        The rotated surface.
 * @deprecated Use rotate_surface instead.
 */
surface rotate_90_surface(const SDL_Surface* surf, bool clockwise, bool optimize = true)
                          __attribute__((deprecated("Use rotate_surface.")));

typedef enum mirror_axis_t{
    VERT,
    HORIZ,
    NE_SW_DIAG,
    SE_NW_DIAG,
    CUSTOM,
} const mirror_axis;

/**
 * @brief Mirror an image with respect to provided axis.
 * @param surf      Image to be mirrored
 * @param axis      Axis to be mirrored across.
 *                  VERT / HORIZ / NE_SW_DIAG / SE_NW_DIAG / CUSTOM
 *                  If CUSTOM is chosen, two points that define the axis line
 *                  must be provided in following parameters.
 * @param optimize  RLE optimize the returned surface
 * @param axis_x1   Only used with axis=CUSTOM
 * @param axis_x2   Only used with axis=CUSTOM
 * @param axis_y1   Only used with axis=CUSTOM
 * @param axis_y2   Only used with axis=CUSTOM
 * @return  Mirrored image as surface.
 * @note Diagonal and custom mirrors are NOT implemented yet.
 * @todo Implement diag and custom mirrors.
 */
surface mirror_surface(const SDL_Surface* surf,
                       const mirror_axis_t axis,
                       const bool optimize=true,
                       const double axis_x1 = 0.0,
                       const double axis_x2 = 0.0,
                       const double axis_y1 = 0.0,
                       const double axis_y2 = 0.0);

/**
 * @brief Mirror image about vertical axis through center of image.
 * @details
 *     0 1 2  -> 2 1 0
 *     3 4 5     5 4 3
 * @param surf
 * @param optimize
 * @return mirrored image
 */
surface flip_surface(const SDL_Surface* surf, bool optimize = true)
                     __attribute__((deprecated("Use mirror surface instead.")));

/**
 * @brief Mirror image about horizontal axis through center of image.
 * @details
 *     0 1 2  -> 3 4 5
 *     3 4 5     0 1 2
 * @param surf
 * @param optimize
 * @return mirrored image
 */
surface flop_surface(const SDL_Surface* surf, bool optimize = true)
                     __attribute__((deprecated("Use mirror surface instead.")));

SDL_Rect get_non_transparent_portion(const SDL_Surface* surf);

/** @} end Image Utilities ------------------------------------------------------------------*/

/**
 * @brief convert a 32bit unsigned in to an SDL color, alpha -> unused.
 * @note values above and below 0-255 are replaced with the closest valid value.
 * and values are rounded down
 */
SDL_Color get_sdl_color(const double r,
                        const double g,
                        const double b,
                        const double a = 255.0);

void draw_rectangle(int x, int y, int w, int h, Uint32 color, surface tg);

void draw_solid_tinted_rectangle(int x,
                                 int y,
                                 int w,
                                 int h,
                                 int r,
                                 int g,
                                 int b,
                                 double alpha,
                                 surface target);

// blit the image on the center of the rectangle
// and a add a colored background
void draw_centered_on_background(surface surf,
                                 const SDL_Rect* rect,
                                 const SDL_Color* color,
                                 surface target);

std::ostream& operator<<(std::ostream& s, const SDL_Rect* rect);

#endif
