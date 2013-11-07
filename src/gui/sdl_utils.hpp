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

#ifndef SDL_UTILS_INCLUDED
#define SDL_UTILS_INCLUDED

#include "../scoped_resource.hpp"
#include "../util.hpp"

#include <stddef.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_stdinc.h>

#include <map>
#include <ostream>
#include <string>

//older versions of SDL don't define the
//mouse wheel macros, so define them ourselves
//if necessary.
//#ifndef SDL_BUTTON_WHEELUP
//#define SDL_BUTTON_WHEELUP 4
//#endif
//
//#ifndef SDL_BUTTON_WHEELDOWN
//#define SDL_BUTTON_WHEELDOWN 5
//#endif
//
//#ifndef SDL_BUTTON_WHEELLEFT
//#define SDL_BUTTON_WHEELLEFT 6
//#endif
//
//#ifndef SDL_BUTTON_WHEELRIGHT
//#define SDL_BUTTON_WHEELRIGHT 7
//#endif

namespace {
const SDL_Rect empty_rect = { 0, 0, 0, 0 };
}

#define SDL_UTILS_ROTATE_CROP 201
#define SDL_UTILS_ROTATE_GROW 202

/// @todo Refactor me to work with SDL2
//SDL_keysym sdl_keysym_from_name(std::string const &keyname);

bool point_in_rect(int x, int y, const SDL_Rect& rect);
bool rects_overlap(const SDL_Rect& rect1, const SDL_Rect& rect2);
SDL_Rect intersect_rects(SDL_Rect const &rect1, SDL_Rect const &rect2);
SDL_Rect union_rects(const SDL_Rect &rect1, const SDL_Rect &rect2);

/**
 *  Creates an empty SDL_Rect.
 *
 *  Since SDL_Rect doesn't have a constructor it's not possible to create it as
 *  a temporary for a function parameter. This functions overcomes this limit.
 */
SDL_Rect create_rect(const int x, const int y, const int w, const int h);

struct surface {
    private:
        static void sdl_add_ref(SDL_Surface *surf) {
            if (surf != NULL) ++surf->refcount;
        }

        struct free_sdl_surface {
                void operator()(SDL_Surface *surf) const {
                    if (surf != NULL) SDL_FreeSurface(surf);
                }
        };

        typedef util::scoped_resource<SDL_Surface*, free_sdl_surface> scoped_sdl_surface;
    public:
        surface() :
            surface_(NULL) {
        }

        surface(SDL_Surface *surf) :
            surface_(surf) {
        }

        surface(const surface& o) :
            surface_(o.surface_.get()) {
            sdl_add_ref(surface_.get());
        }

        void assign(const surface& o) {
            SDL_Surface *surf = o.surface_.get();
            sdl_add_ref(surf);  // need to be done before assign to avoid corruption on "a=a;"
            surface_.assign(surf);
        }

        surface& operator=(const surface& o) {
            assign(o);
            return *this;
        }

        operator SDL_Surface*() const {
            return surface_.get();
        }

        SDL_Surface* get() const {
            return surface_.get();
        }

        SDL_Surface* operator->() const {
            return surface_.get();
        }

        void assign(SDL_Surface* surf) {
            surface_.assign(surf);
        }

        bool null() const {
            return surface_.get() == NULL;
        }

    private:
        scoped_sdl_surface surface_;
};

bool operator<(const surface& a, const surface& b);

inline void sdl_blit(const surface& src,
                     SDL_Rect* src_rect,
                     surface& dst,
                     SDL_Rect* dst_rect) {
    SDL_BlitSurface(src, src_rect, dst, dst_rect);
}

inline void sdl_fill_rect(surface& dst, SDL_Rect* dst_rect, const Uint32 color) {
    SDL_FillRect(dst, dst_rect, color);
}

/**
 * @brief Check that the surface is neutral bpp 32.
 * @note The surface may have an empty alpha channel.
 * @param surf                    The surface to test.
 * @returns                       The status @c true if neutral, @c false if not.
 */
bool is_neutral(const surface& surf);

/**
 * @brief Checks if a surface is an ARGB 32bpp image.
 * @param surf
 * @return true/false
 */
bool is_ARGB8888(const surface& surf);

surface make_neutral_surface(const surface &surf);

/**
 * @brief returns a surface in RGBA 32bpp format from existing surface
 * @param surf  source
 * @return  rgba8888 surface
 */
surface make_RGBA8888(const surface &surf);

/**
 * @brief returns a surface in ARGB 32bpp format from existing surface
 * @param surf  source
 * @return  argb8888 surface
 */
surface make_RGBA8888(const surface &surf);

/**
 * @brief TODO: WRITEME
 * @param w
 * @param h
 * @return
 */
surface create_neutral_surface(int w, int h);

/**
 * @brief TODO: WRITEME
 * @param surf
 * @return
 */
surface create_optimized_surface(const surface &surf);

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
surface resize_surface(const surface& surf,
                       const unsigned h,
                       const unsigned w,
                       const bool optimize);

/** @brief wrapper for resize_surface.*/
surface stretch_surface_horizontal(const surface& surf,
                                   const unsigned w,
                                   const bool optimize = true);

/** @brief wrapper for resize_surface.*/
surface stretch_surface_vertical(const surface& surf,
                                 const unsigned h,
                                 const bool optimize = true);

/** @brief wrapper for resize_surface.*/
surface scale_surface(const surface &surf, int w, int h, bool optimize = true);

/** @brief wrapper for resize_surface.*/
surface scale_surface_sharp(const surface& surf, int w, int h, bool optimize = true);

/** Tile a surface
 * @param surf               The source surface.
 * @param w                  The width of the resulting surface.
 * @param h                  The height of the resulting surface.
 * @param optimize           Should the return surface be RLE optimized
 * @return                   A surface containing the tiled version of the source.
 * @retval 0                 Returned upon error
 * @retval surf              Returned if w == surf->w and h == surf->h
 *                           note this ignores the optimize flag.
 */
surface tile_surface(const surface &surf, int w, int h, bool optimize = true);

/**
 * @brief TODO: WRITEME
 * @param surf
 * @param r
 * @param g
 * @param b
 * @param optimize
 * @return
 */
surface adjust_surface_color(const surface &surf,
                             int r,
                             int g,
                             int b,
                             bool optimize = true);

/**
 * @brief Downsample RGBA to K.
 * @return Greyscale image as surface.
 */
surface greyscale_image(const surface &surf,    /**< Color image to be downsampled to grey. */
                        bool optimize = true);  /**< Return RLE optimized surface. */

/**
 * @brief create an heavy shadow of the image, by blurring, increasing alpha and darkening
 */
surface shadow_image(const surface &surf,   /**< @param surf source image */
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
surface brighten_image(const surface &surf, fixed_t amount, bool optimize = true);

/** Get a portion of the screen.
 *  Send NULL if the portion is outside of the screen.
 *  @param surf              The source surface.
 *  @param rect              The portion of the source surface to copy.
 *  @param optimize_format   Optimize by converting to result to display format.
 *                           Only useful if the source is not the screen and you
 *                           plan to blit the result on screen several times.
 *  @return                  A surface containing the portion of the source.
 *                           No RLE or Alpha bits are set.
 *  @retval 0                if error or the portion is outside of the surface.
 */
surface get_surface_portion(const surface &surf, SDL_Rect &rect, bool optimize_format = false);

surface adjust_surface_alpha(const surface &surf, fixed_t amount, bool optimize = true);
surface adjust_surface_alpha_add(const surface &surf, int amount, bool optimize = true);

/** Applies a mask on a surface. */
surface mask_surface(const surface &surf,
                     const surface &mask,
                     bool* empty_result = NULL,
                     const std::string& filename = std::string());

/** Check if a surface fit into a mask */
bool in_mask_surface(const surface &surf, const surface &mask);

/** Progressively reduce alpha of bottom part of the surface
 *  @param surf              The source surface.
 *  @param depth             The height of the bottom part in pixels
 *  @param alpha_base        The alpha adjustment at the interface
 *  @param alpha_delta       The alpha adjustment reduction rate by pixel depth
 *  @param optimize          Optimize by converting to result to display
 */
surface submerge_alpha(const surface &surf,
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
surface light_surface(const surface &surf, const surface &lightmap, bool optimize = true);

/**
 * @brief Performs a basic box blur on a surface
 * @param surf      source surface to be blurred, RGB or RGBA, not RLE encoded.
 * @param depth     size of the kernel used for blurring - 1, 0 < depth < min(surf->h, surf->w)
 * @param optimize  Whether the new surface should be RLE encoded.
 * @return  the blurred surface.
 */
surface blur_surface(const surface &surf, int depth = 1, bool optimize = true);

/**
 * @brief Performs a basic box blur on a sub-rectangle of a surface.
 * @param surf      source surface to be blurred, RGB or RGBA, not RLE encoded.
 * @param rect      rectangle completely within bounds of surf
 * @param depth     size of the kernel used for blurring - 1, 0 < depth < min(surf->h, surf->w)
 */
void blur_surface(surface& surf, SDL_Rect rect, unsigned depth = 1);

/** @brief wrapper for blur surface. */
surface blur_alpha_surface(const surface &surf, int depth = 1, bool optimize = true)
    __attribute__((deprecated ("Use blur_surface instead of blur_alpha_surface.")));

/** Cuts a rectangle from a surface. */
surface cut_surface(const surface &surf, SDL_Rect const &r);

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
surface blend_surface(const surface &surf,
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
surface rotate_surface(const surface& surf,
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
surface rotate_surface(const surface& surf,
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
surface rotate_any_surface(const surface& surf, float angle, int zoom, int offset,
                           bool optimize = true)
                           __attribute__((deprecated("Use rotate_surface.")));

/**
 * @brief Rotates a surface 180 degrees.
 * @param surf                    The surface to rotate.
 * @param optimize                Should the return surface be RLE optimized.
 * @return                        The rotated surface.
 * @deprecated Use rotate_surface instead.
 */
surface rotate_180_surface(const surface &surf, bool optimize = true)
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
surface rotate_90_surface(const surface &surf, bool clockwise, bool optimize = true)
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
surface mirror_surface(const surface& surf,
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
surface flip_surface(const surface &surf, bool optimize = true)
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
surface flop_surface(const surface &surf, bool optimize = true)
                     __attribute__((deprecated("Use mirror surface instead.")));

/**
 * @brief TODO WRITEME
 * @return
 */
surface create_compatible_surface(const surface &surf, int width = -1, int height = -1);

/**
 * Replacement for sdl_blit.
 *
 * sdl_blit has problems with blitting partly transparent surfaces so
 * this is a replacement. It ignores the SDL_SRCALPHA and SDL_SRCCOLORKEY
 * flags. src and dst will have the SDL_RLEACCEL flag removed.
 * The return value of SDL_BlistSurface is normally ignored so no return value.
 * The rectangles are const and will not be modified.
 *
 * @pre @p src contains a valid canvas.
 * @pre @p dst contains a valid canvas.
 * @pre The caller must make sure the @p src fits on the @p dst.
 *
 * @param src          The surface to blit.
 * @param srcrect      The region of the surface to blit
 * @param dst          The surface to blit on.
 * @param dstrect      The offset to blit the surface on, only x and y are used.
 */
void blit_surface(const surface& src,
                  const SDL_Rect* srcrect,
                  surface& dst,
                  const SDL_Rect* dstrect);

void fill_rect_alpha(SDL_Rect &rect, Uint32 color, Uint8 alpha, surface &target);

SDL_Rect get_non_transparent_portion(const surface &surf);

/** @brief equality operator */
bool operator==(const SDL_Rect& a, const SDL_Rect& b);

/** @brief inequality operator */
bool operator!=(const SDL_Rect& a, const SDL_Rect& b);

/** @brief equality operator */
bool operator==(const SDL_Color& a, const SDL_Color& b);

/** @brief inequality operator */
bool operator!=(const SDL_Color& a, const SDL_Color& b);

/**
 * @brief Invert the given color.
 * @param color SDL color
 * @return the inverse of the given color.
 */
SDL_Color inverse(const SDL_Color& color);

/**
 * @brief convert a 32bit unsigned int to an SDL color, alpha -> unused.
 * @details
 *      SDL_Color.unused = bits 24-31
 *      SDL_Color.r      = bits 16-23
 *      SDL_Color.g      = bits  8-15
 *      SDL_Color.b      = bits  0- 7
 */
SDL_Color get_sdl_color(const Uint32 argb);

/**
 * @brief convert 4 chars unsigned int to an SDL color, alpha -> unused.
 */
SDL_Color get_sdl_color(const unsigned char r,
                        const unsigned char g,
                        const unsigned char b,
                        const unsigned char a = 255);

/**
 * @brief convert a 32bit unsigned in to an SDL color, alpha -> unused.
 * @note values above and below 0-255 are replaced with the closest valid value.
 */
SDL_Color get_sdl_color(const int r, const int g, const int b, const int a = 255);

/**
 * @brief convert a 32bit unsigned in to an SDL color, alpha -> unused.
 * @note values above and below 0-255 are replaced with the closest valid value.
 */
SDL_Color get_sdl_color(const size_t r, const size_t g, const size_t b, const size_t a = 255);

/**
 * @brief convert a 32bit unsigned in to an SDL color, alpha -> unused.
 * @note values above and below 0-255 are replaced with the closest valid value,
 * and values are rounded down
 */
SDL_Color get_sdl_color(const float r,
                        const float g,
                        const float b,
                        const float a = 255.0f);

/**
 * @brief convert a 32bit unsigned in to an SDL color, alpha -> unused.
 * @note values above and below 0-255 are replaced with the closest valid value.
 * and values are rounded down
 */
SDL_Color get_sdl_color(const double r,
                        const double g,
                        const double b,
                        const double a = 255.0);

/**
 * Helper class for pinning SDL surfaces into memory.
 * @note This class should be used only with neutral surfaces, so that
 *       the pointer returned by #pixels is meaningful.
 */
struct surface_lock {
    public:
        surface_lock(surface &surf);
        ~surface_lock();

        Uint32* pixels() {
            return reinterpret_cast<Uint32*>(surface_->pixels);
        }
    private:
        surface& surface_;
        bool locked_;
};

/**
 * Helper class for pinning SDL surfaces into memory.
 * @note This class should be used only with neutral surfaces, so that
 *       the pointer returned by #pixels is meaningful.
 */
struct const_surface_lock {
        const_surface_lock(const surface &surf);
        ~const_surface_lock();

        const Uint32* pixels() const {
            return reinterpret_cast<const Uint32*>(surface_->pixels);
        }
    private:
        const surface& surface_;
        bool locked_;
};

/**
 * Helper methods for setting/getting a single pixel in an image.
 * Lifted from http://sdl.beuc.net/sdl.wiki/Pixel_Access
 * @param surf           The image to get or receive the pixel from.
 * @param surf_lock      The locked surface to make sure the pointers are valid.
 * @x                    The position in the row of the pixel.
 * @y                    The row of the pixel.
 */
void put_pixel(const surface& surf, surface_lock& surf_lock, int x, int y, Uint32 pixel);
Uint32 get_pixel(const surface& surf, const const_surface_lock& surf_lock, int x, int y);

struct surface_restorer {
        surface_restorer();
        surface_restorer(class ui_window* target, const SDL_Rect& rect);
        ~surface_restorer();

        void restore() const;
        void restore(SDL_Rect const &dst) const;
        void update();
        void cancel();

        const SDL_Rect& area() const {
            return rect_;
        }

    private:
        class ui_window* target_;
        SDL_Rect rect_;
        surface surface_;
};

struct clip_rect_setter {
        // if r is NULL, clip to the full size of the surface.
        clip_rect_setter(const surface &surf, const SDL_Rect* r, bool operate = true) :
            surface_(surf), rect_(), operate_(operate) {
            if (operate_) {
                SDL_GetClipRect(surface_, &rect_);
                SDL_SetClipRect(surface_, r);
            }
        }

        ~clip_rect_setter() {
            if (operate_) SDL_SetClipRect(surface_, &rect_);
        }

    private:
        surface surface_;
        SDL_Rect rect_;
        const bool operate_;
};

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
                                 const SDL_Rect& rect,
                                 const SDL_Color& color,
                                 surface target);

std::ostream& operator<<(std::ostream& s, const SDL_Rect& rect);

#endif
