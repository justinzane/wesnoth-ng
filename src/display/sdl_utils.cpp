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
 *  @file
 *  Support-routines for the SDL-graphics-library.
 */

#include "global.hpp"

#include "display/sdl_utils.hpp"
#include "display/video.hpp"

#include "floating_point_emulation.hpp"
#include "neon.hpp"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/math/constants/constants.hpp>
#include <math.h>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>

surface_lock::surface_lock(surface &surf) :
    surface_(surf), locked_(false) {
    if (SDL_MUSTLOCK(surface_)) locked_ = SDL_LockSurface(surface_) == 0;
}

surface_lock::~surface_lock() {
    if (locked_) SDL_UnlockSurface(surface_);
}

const_surface_lock::const_surface_lock(const surface &surf) :
    surface_(surf), locked_(false) {
    if (SDL_MUSTLOCK(surface_)) locked_ = SDL_LockSurface(surface_) == 0;
}

const_surface_lock::~const_surface_lock() {
    if (locked_) SDL_UnlockSurface(surface_);
}

SDL_Color int_to_color(const Uint32 rgb) {
    SDL_Color result;
    result.r = (0x00FF0000 & rgb) >> 16;
    result.g = (0x0000FF00 & rgb) >> 8;
    result.b = (0x000000FF & rgb);
    result.unused = 0;
    return result;
}

SDL_Color create_color(const unsigned char red,
                       unsigned char green,
                       unsigned char blue,
                       unsigned char unused) {
    SDL_Color result;
    result.r = red;
    result.g = green;
    result.b = blue;
    result.unused = unused;

    return result;
}

SDLKey sdl_keysym_from_name(std::string const &keyname) {
    static bool initialized = false;
    typedef std::map<std::string const, SDLKey> keysym_map_t;
    static keysym_map_t keysym_map;

    if ( !initialized) {
        for (SDLKey i = SDLK_FIRST; i < SDLK_LAST; i = SDLKey(int(i) + 1)) {
            std::string name = SDL_GetKeyName(i);
            if ( !name.empty()) keysym_map[name] = i;
        }
        initialized = true;
    }

    keysym_map_t::const_iterator it = keysym_map.find(keyname);
    if (it != keysym_map.end())
        return it->second;
    else
        return SDLK_UNKNOWN;
}

bool point_in_rect(int x, int y, const SDL_Rect& rect) {
    return x >= rect.x && y >= rect.y && x < rect.x + rect.w && y < rect.y + rect.h;
}

bool rects_overlap(const SDL_Rect& rect1, const SDL_Rect& rect2) {
    return (rect1.x < rect2.x + rect2.w && rect2.x < rect1.x + rect1.w
            && rect1.y < rect2.y + rect2.h && rect2.y < rect1.y + rect1.h);
}

SDL_Rect intersect_rects(SDL_Rect const &rect1, SDL_Rect const &rect2) {
    SDL_Rect res;
    res.x = std::max<int>(rect1.x, rect2.x);
    res.y = std::max<int>(rect1.y, rect2.y);
    int w = std::min<int>(rect1.x + rect1.w, rect2.x + rect2.w) - res.x;
    int h = std::min<int>(rect1.y + rect1.h, rect2.y + rect2.h) - res.y;
    if (w <= 0 || h <= 0) return empty_rect;
    res.w = w;
    res.h = h;
    return res;
}

SDL_Rect union_rects(SDL_Rect const &rect1, SDL_Rect const &rect2) {
    if (rect1.w == 0 || rect1.h == 0) return rect2;
    if (rect2.w == 0 || rect2.h == 0) return rect1;
    SDL_Rect res;
    res.x = std::min<int>(rect1.x, rect2.x);
    res.y = std::min<int>(rect1.y, rect2.y);
    res.w = std::max<int>(rect1.x + rect1.w, rect2.x + rect2.w) - res.x;
    res.h = std::max<int>(rect1.y + rect1.h, rect2.y + rect2.h) - res.y;
    return res;
}

SDL_Rect create_rect(const int x, const int y, const int w, const int h) {
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    return rect;
}

bool operator<(const surface& a, const surface& b) {
    return a.get() < b.get();
}

bool is_neutral(const surface& surf) {
    return (surf->format->BytesPerPixel == 4 && surf->format->Rmask == 0xFF0000u
            && (surf->format->Amask | 0xFF000000u) == 0xFF000000u);
}

bool is_ARGB8888(const surface& surf) {
    return (surf->format->Aloss == 0 &&
            surf->format->Rloss == 0 &&
            surf->format->Gloss == 0 &&
            surf->format->Bloss == 0 &&
            surf->format->Amask == 0xff000000 &&
            surf->format->Rmask == 0x00ff0000 &&
            surf->format->Gmask == 0x0000ff00 &&
            surf->format->Bmask == 0x000000ff);
}

static SDL_PixelFormat& get_neutral_pixel_format() {
    static bool first_time = true;
    static SDL_PixelFormat format;

    if (first_time) {
        first_time = false;
        surface surf(
            SDL_CreateRGBSurface(SDL_SWSURFACE, 1, 1, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000));
        format = *surf->format;
        format.palette = NULL;
    }

    return format;
}

surface make_neutral_surface(const surface &surf) {
    if (surf == NULL) {
        std::cerr << "null neutral surface...\n";
        return NULL;
    }

    surface const result = SDL_ConvertSurface(surf, &get_neutral_pixel_format(), SDL_SWSURFACE);
    if (result != NULL) {
        SDL_SetAlpha(result, SDL_SRCALPHA, SDL_ALPHA_OPAQUE);
    }

    return result;
}

surface create_neutral_surface(int w, int h) {
    if (w < 0 || h < 0) {
        std::cerr << "error : neutral surface with negative dimensions\n";
        return NULL;
    }

    SDL_PixelFormat format = get_neutral_pixel_format();
    surface result = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, format.BitsPerPixel,
                                          format.Rmask, format.Gmask, format.Bmask,
                                          format.Amask);

    return result;
}

surface create_optimized_surface(const surface &surf) {
    if (surf == NULL) return NULL;

    surface const result = display_format_alpha(surf);
    if (result == surf) {
        std::cerr << "resulting surface is the same as the source!!!\n";
    } else if (result == NULL) {
        return surf;
    }

    SDL_SetAlpha(result, SDL_SRCALPHA | SDL_RLEACCEL, SDL_ALPHA_OPAQUE);

    return result;
}

/** Replacement for multiple existing scaling functions. */
surface resize_surface(const surface& surf,
                       const unsigned h,
                       const unsigned w,
                       const bool optimize) {
    if (surf == NULL) {
        throw std::invalid_argument("Cannot resize an null surface.");
    }
    if ((h == 0) || (w == 0)) {
        throw std::invalid_argument("Cannot resize a surface to 0 area.");
    }
    if ((static_cast<int>(h) == surf->h) && (static_cast<int>(w) == surf->w)) {
        std::cerr << "Warning: Resizing a surface to same size. " <<
            surf->h << "x" << surf->w << " -> " << h << "x" << w << "\n";
        return surf;
    }

    // Make sure both surfaces are always in the "neutral" pixel format
    surface dst(create_neutral_surface(w, h));
    surface src(make_neutral_surface(surf));

    if (src == NULL || dst == NULL) {
        throw std::logic_error("Could not create neutral surfaces. This is a BUG.");
    }

    /** TODO: Conditionally use OpenCL or CUDA if system supports them. */
    { // Extra scoping used for the surface_lock.
        using namespace cv;
        const_surface_lock src_lock(src);
        surface_lock dst_lock(dst);

        //int rows, int cols, int type, void* data, size_t step=AUTO_STEP
        Mat src_mat = Mat(surf->h, surf->w, CV_8UC4, src->pixels, Mat::AUTO_STEP);
        Mat dst_mat = Mat(h, w, CV_8UC4, dst->pixels, Mat::AUTO_STEP);

        resize(src_mat, dst_mat, dst_mat.size(), 0.0, 0.0, INTER_CUBIC);
    }

    return optimize ? create_optimized_surface(dst) : dst;
}

/** @brief wrapper for resize_surface */
surface stretch_surface_horizontal(const surface& surf, const unsigned w, const bool optimize) {
    return resize_surface(surf, surf->h, w, optimize);
}

/** @brief wrapper for resize_surface */
surface stretch_surface_vertical(const surface& surf, const unsigned h, const bool optimize) {
    return resize_surface(surf, h, surf->w, optimize);
}

/** @brief wrapper for resize_surface */
surface scale_surface(const surface &surf, int w, int h, bool optimize) {
    return resize_surface(surf, h, w, optimize);
}

/** @brief wrapper for resize_surface */
surface scale_surface_sharp(const surface& surf, int w, int h, bool optimize) {
    return resize_surface(surf, h, w, optimize);
}

surface blur_surface(const surface &surf, int depth, bool optimize) {
    if (surf == NULL) {
        throw std::invalid_argument("Cannot blur an null surface.");
    }
    if (depth < 1) {
        throw std::invalid_argument("Cannot blur with depth < 1.");
    }
    if (depth >= std::min(surf->w, surf->h)) {
        throw std::invalid_argument("Cannot blur with depth >= min(surf->w, surf->y).");
    }

    // Make sure both surfaces are always in the "neutral" pixel format
    surface dst(create_neutral_surface(surf->w, surf->h));
    surface src(make_neutral_surface(surf));
    if (src == NULL || dst == NULL) {
        throw std::logic_error("Could not create neutral surfaces. This is a BUG.");
    }

    /** TODO: Conditionally use OpenCL or CUDA if system supports them. */
    { // Extra scoping used for the surface_lock.
        using namespace cv;
        const_surface_lock src_lock(src);
        surface_lock dst_lock(dst);

        //int rows, int cols, int type, void* data, size_t step=AUTO_STEP
        Mat src_mat = Mat(surf->h, surf->w, CV_8UC4, src->pixels, Mat::AUTO_STEP);
        Mat dst_mat = Mat(surf->h, surf->w, CV_8UC4, dst->pixels, Mat::AUTO_STEP);

        blur(src_mat, dst_mat, Size((depth + 1), (depth + 1)), Point(-1,-1));
    }

    return optimize ? create_optimized_surface(dst) : dst;
}

void blur_surface(surface& surf, SDL_Rect rect, unsigned depth) {
    if (surf == NULL) { throw std::invalid_argument("Cannot blur an null surface."); }
    if (depth < 1) { throw std::invalid_argument("Cannot blur with depth < 1."); }
    if ((int)depth >= std::min(surf->w, surf->h)) {
        throw std::invalid_argument("Cannot blur with depth >= min(surf->w, surf->y).");
    }
    if (rect.x < 0 || rect.y < 0 || rect.w > surf->w || rect.h > surf->h) {
        throw std::invalid_argument("Cannot blur rect outside surf bounds.");
    }

    // Make sure surface is in the "neutral" pixel format
    //surface src(make_neutral_surface(surf));
    //if (src == NULL) { throw std::logic_error("Could not create neutral surface. This is a BUG."); }

    /** TODO: Conditionally use OpenCL or CUDA if system supports them. */
    { // Extra scoping used for the surface_lock.
        using namespace cv;
        surface_lock surf_lock(surf);

        //int rows, int cols, int type, void* data, size_t step=AUTO_STEP
        Mat src_mat = Mat(surf->h, surf->w, CV_8UC4, surf->pixels, Mat::AUTO_STEP);
        Mat crop_mat = Mat(src_mat, Rect(rect.x, rect.y, rect.w, rect.h));

        blur(crop_mat, crop_mat, Size((depth + 1), (depth + 1)), Point(-1,-1));
    }
}

surface blur_alpha_surface(const surface &surf, int depth, bool optimize) {
    return blur_surface(surf, depth, optimize);
}

surface tile_surface(const surface& surf, int w, int h, bool optimize) {
    if (surf->w == w && surf->h == h) {
        return surf;
    }

    surface dest(create_neutral_surface(w, h));
    surface src(make_neutral_surface(surf));

    if (src == NULL || dest == NULL) {
        std::cerr << "failed to make neutral surface\n";
        return NULL;
    }

    {
        const_surface_lock srclock(src);
        surface_lock destlock(dest);

        const Uint32* srcpixels = srclock.pixels();
        Uint32* destpixels = destlock.pixels();

        const int& sw = src->w;
        const int& sh = src->h;

        const int xoff = (w - sw) / 2;
        const int yoff = (h - sh) / 2;

        for (int i = 0; i < w * h; ++i) {
            int x = ( (i % w) - xoff);
            int y = ( (i / w) - yoff);

            while ( (x += sw) < 0) { /* DO NOTHING */
            }
            while ( (y += sh) < 0) { /* DO NOTHING */
            }

            const int sx = x % sw;
            const int sy = y % sh;

            destpixels[i] = srcpixels[sy * sw + sx];
        }
    }

    return optimize ? create_optimized_surface(dest) : dest;
}

surface adjust_surface_color(const surface &surf, int red, int green, int blue, bool optimize) {
    if (surf == NULL) return NULL;

    if ( (red == 0 && green == 0 && blue == 0))
        return optimize ? create_optimized_surface(surf) : surf;

    surface nsurf(make_neutral_surface(surf));

    if (nsurf == NULL) {
        std::cerr << "failed to make neutral surface\n";
        return NULL;
    }

    {
        surface_lock lock(nsurf);
        Uint32* beg = lock.pixels();
        Uint32* end = beg + nsurf->w * surf->h;

        while (beg != end) {
            Uint8 alpha = ( *beg) >> 24;

            if (alpha) {
                Uint8 r, g, b;
                r = ( *beg) >> 16;
                g = ( *beg) >> 8;
                b = ( *beg) >> 0;

                r = std::max<int>(0, std::min<int>(255, int(r) + red));
                g = std::max<int>(0, std::min<int>(255, int(g) + green));
                b = std::max<int>(0, std::min<int>(255, int(b) + blue));

                *beg = (alpha << 24) + (r << 16) + (g << 8) + b;
            }

            ++beg;
        }
    }

    return optimize ? create_optimized_surface(nsurf) : nsurf;
}

surface greyscale_image(const surface &surf, bool optimize) {
    if (surf == NULL) return NULL;

    surface nsurf(make_neutral_surface(surf));
    if (nsurf == NULL) {
        std::cerr << "failed to make neutral surface\n";
        return NULL;
    }

    {
        surface_lock lock(nsurf);
        Uint32* beg = lock.pixels();
        Uint32* end = beg + nsurf->w * surf->h;

        while (beg != end) {
            Uint8 alpha = ( *beg) >> 24;

            if (alpha) {
                Uint8 r, g, b;
                r = ( *beg) >> 16;
                g = ( *beg) >> 8;
                b = ( *beg);
                //const Uint8 avg = (red+green+blue)/3;

                // Use the correct formula for RGB to grayscale conversion.
                // Ok, this is no big deal :)
                // The correct formula being:
                // gray=0.299red+0.587green+0.114blue
                const Uint8 avg = static_cast<Uint8>( (77 * static_cast<Uint16>(r)
                    + 150 * static_cast<Uint16>(g) + 29 * static_cast<Uint16>(b))
                                                     / 256);

                *beg = (alpha << 24) | (avg << 16) | (avg << 8) | avg;
            }

            ++beg;
        }
    }

    return optimize ? create_optimized_surface(nsurf) : nsurf;
}

surface shadow_image(const surface &surf, bool optimize) {
    if (surf == NULL) return NULL;

    // we blur it, and reuse the neutral surface created by the blur function (optimized = false)
    surface nsurf(blur_surface(surf, 2, false));

    if (nsurf == NULL) {
        std::cerr << "failed to blur the shadow surface\n";
        return NULL;
    }

    {
        surface_lock lock(nsurf);
        Uint32* beg = lock.pixels();
        Uint32* end = beg + nsurf->w * surf->h;

        while (beg != end) {
            Uint8 alpha = ( *beg) >> 24;

            if (alpha) {
                // increase alpha and color in black (RGB=0)
                // with some stupid optimization for handling maximum values
                if (alpha < 255 / 4)
                    *beg = (alpha * 4) << 24;
                else
                    *beg = 0xFF000000;  // we hit the maximum
            }

            ++beg;
        }
    }

    return optimize ? create_optimized_surface(nsurf) : nsurf;
}

surface recolor_image(surface surf, const std::map<Uint32, Uint32>& map_rgb, bool optimize) {
    if (surf == NULL) return NULL;

    if ( !map_rgb.empty()) {
        surface nsurf(make_neutral_surface(surf));
        if (nsurf == NULL) {
            std::cerr << "failed to make neutral surface\n";
            return NULL;
        }

        surface_lock lock(nsurf);
        Uint32* beg = lock.pixels();
        Uint32* end = beg + nsurf->w * surf->h;

        while (beg != end) {
            Uint8 alpha = ( *beg) >> 24;

            if (alpha) {	// don't recolor invisible pixels.
                // palette use only RGB channels, so remove alpha
                Uint32 oldrgb = ( *beg) & 0x00FFFFFF;
                std::map<Uint32, Uint32>::const_iterator i = map_rgb.find(oldrgb);
                if (i != map_rgb.end()) {
                    *beg = (alpha << 24) + i->second;
                }
            }
            ++beg;
        }

        return optimize ? create_optimized_surface(nsurf) : nsurf;
    }
    return surf;
}

surface brighten_image(const surface &surf, fixed_t amount, bool optimize) {
    if (surf == NULL) {
        return NULL;
    }

    surface nsurf(make_neutral_surface(surf));

    if (nsurf == NULL) {
        std::cerr << "could not make neutral surface...\n";
        return NULL;
    }

    {
        surface_lock lock(nsurf);
        Uint32* beg = lock.pixels();
        Uint32* end = beg + nsurf->w * surf->h;

        if (amount < 0) amount = 0;
        while (beg != end) {
            Uint8 alpha = ( *beg) >> 24;

            if (alpha) {
                Uint8 r, g, b;
                r = ( *beg) >> 16;
                g = ( *beg) >> 8;
                b = ( *beg);

                r = std::min<unsigned>(unsigned(fxpmult(r, amount)), 255);
                g = std::min<unsigned>(unsigned(fxpmult(g, amount)), 255);
                b = std::min<unsigned>(unsigned(fxpmult(b, amount)), 255);

                *beg = (alpha << 24) + (r << 16) + (g << 8) + b;
            }

            ++beg;
        }
    }

    return optimize ? create_optimized_surface(nsurf) : nsurf;
}

surface adjust_surface_alpha(const surface &surf, fixed_t amount, bool optimize) {
    if (surf == NULL) {
        return NULL;
    }

    surface nsurf(make_neutral_surface(surf));

    if (nsurf == NULL) {
        std::cerr << "could not make neutral surface...\n";
        return NULL;
    }

    {
        surface_lock lock(nsurf);
        Uint32* beg = lock.pixels();
        Uint32* end = beg + nsurf->w * surf->h;

        if (amount < 0) amount = 0;
        while (beg != end) {
            Uint8 alpha = ( *beg) >> 24;

            if (alpha) {
                Uint8 r, g, b;
                r = ( *beg) >> 16;
                g = ( *beg) >> 8;
                b = ( *beg);

                alpha = std::min<unsigned>(unsigned(fxpmult(alpha, amount)), 255);
                *beg = (alpha << 24) + (r << 16) + (g << 8) + b;
            }

            ++beg;
        }
    }

    return optimize ? create_optimized_surface(nsurf) : nsurf;
}

surface adjust_surface_alpha_add(const surface &surf, int amount, bool optimize) {
    if (surf == NULL) {
        return NULL;
    }

    surface nsurf(make_neutral_surface(surf));

    if (nsurf == NULL) {
        std::cerr << "could not make neutral surface...\n";
        return NULL;
    }

    {
        surface_lock lock(nsurf);
        Uint32* beg = lock.pixels();
        Uint32* end = beg + nsurf->w * surf->h;

        while (beg != end) {
            Uint8 alpha = ( *beg) >> 24;

            if (alpha) {
                Uint8 r, g, b;
                r = ( *beg) >> 16;
                g = ( *beg) >> 8;
                b = ( *beg);

                alpha = Uint8(std::max<int>(0, std::min<int>(255, int(alpha) + amount)));
                *beg = (alpha << 24) + (r << 16) + (g << 8) + b;
            }

            ++beg;
        }
    }

    return optimize ? create_optimized_surface(nsurf) : nsurf;
}

surface mask_surface(const surface &surf,
                     const surface &mask,
                     bool* empty_result,
                     const std::string& filename) {
    if (surf == NULL) {
        return NULL;
    }
    if (mask == NULL) {
        return surf;
    }

    surface nsurf = make_neutral_surface(surf);
    surface nmask(make_neutral_surface(mask));

    if (nsurf == NULL || nmask == NULL) {
        std::cerr << "could not make neutral surface...\n";
        return NULL;
    }
    if (nsurf->w != nmask->w) {
        // we don't support efficiently different width.
        // (different height is not a real problem)
        // This function is used on all hexes and usually only for that
        // so better keep it simple and efficient for the normal case
        std::stringstream ss;
        ss << "Detected an image with bad dimensions: ";
        if ( !filename.empty()) ss << filename << ": ";
        ss << nsurf->w << "x" << nsurf->h << "\n";
        std::cerr << ss.str();
        std::cerr << "It will not be masked, please use: " << nmask->w << "x" << nmask->h
                  << "\n";
        return nsurf;
    }

    bool empty = true;
    {
        surface_lock lock(nsurf);
        const_surface_lock mlock(nmask);

        Uint32* beg = lock.pixels();
        Uint32* end = beg + nsurf->w * surf->h;
        const Uint32* mbeg = mlock.pixels();
        const Uint32* mend = mbeg + nmask->w * nmask->h;

        while (beg != end && mbeg != mend) {
            Uint8 alpha = ( *beg) >> 24;

            if (alpha) {
                Uint8 r, g, b;
                r = ( *beg) >> 16;
                g = ( *beg) >> 8;
                b = ( *beg);

                Uint8 malpha = ( *mbeg) >> 24;
                if (alpha > malpha) {
                    alpha = malpha;
                }
                if (alpha) empty = false;

                *beg = (alpha << 24) + (r << 16) + (g << 8) + b;
            }

            ++beg;
            ++mbeg;
        }
    }
    if (empty_result) *empty_result = empty;

    return nsurf;
    //return create_optimized_surface(nsurf);
}

bool in_mask_surface(const surface &surf, const surface &mask) {
    if (surf == NULL) {
        return false;
    }
    if (mask == NULL) {
        return true;
    }

    if (surf->w != mask->w || surf->h != mask->h) {
        // not same size, consider it doesn't fit
        return false;
    }

    surface nsurf = make_neutral_surface(surf);
    surface nmask(make_neutral_surface(mask));

    if (nsurf == NULL || nmask == NULL) {
        std::cerr << "could not make neutral surface...\n";
        return false;
    }

    {
        surface_lock lock(nsurf);
        const_surface_lock mlock(nmask);

        const Uint32* mbeg = mlock.pixels();
        const Uint32* mend = mbeg + nmask->w * nmask->h;
        Uint32* beg = lock.pixels();
        // no need for 'end', because both surfaces have same size

        while (mbeg != mend) {
            Uint8 malpha = ( *mbeg) >> 24;
            if (malpha == 0) {
                Uint8 alpha = ( *beg) >> 24;
                if (alpha) return false;
            }
            ++mbeg;
            ++beg;
        }
    }

    return true;
}

surface submerge_alpha(const surface &surf,
                       int depth,
                       float alpha_base,
                       float alpha_delta,
                       bool optimize) {
    if (surf == NULL) {
        return NULL;
    }

    surface nsurf(make_neutral_surface(surf));

    {
        surface_lock lock(nsurf);

        Uint32* beg = lock.pixels();
        Uint32* limit = beg + (nsurf->h - depth) * nsurf->w;
        Uint32* end = beg + nsurf->w * nsurf->h;
        beg = limit;  // directlt jump to the bottom part

        while (beg != end) {
            Uint8 alpha = ( *beg) >> 24;

            if (alpha) {
                Uint8 r, g, b;
                r = ( *beg) >> 16;
                g = ( *beg) >> 8;
                b = ( *beg);
                int d = (beg - limit) / nsurf->w;  // current depth in pixels
                float a = alpha_base - d * alpha_delta;
                fixed_t amount = ftofxp(a < 0 ? 0 : a);
                alpha = std::min<unsigned>(unsigned(fxpmult(alpha, amount)), 255);
                *beg = (alpha << 24) + (r << 16) + (g << 8) + b;
            }

            ++beg;
        }

        /*
         for(int y = submerge_height; y < nsurf->h; ++y) {
         Uint32* cur = beg + y * nsurf->w;
         Uint32* row_end = beg + (y+1) * nsurf->w;
         float d = y * 1.0 / depth;
         double a = 0.2;//std::max<double>(0, (1-d)*0.3);
         fixed_t amount = ftofxp(a);
         while(cur != row_end) {
         Uint8 alpha = (*cur) >> 24;

         if(alpha) {
         Uint8 r, g, b;
         r = (*cur) >> 16;
         g = (*cur) >> 8;
         b = (*cur);
         alpha = std::min<unsigned>(unsigned(fxpmult(alpha,amount)),255);
         *cur = (alpha << 24) + (r << 16) + (g << 8) + b;
         }

         ++cur;
         }
         }*/

    }

    return optimize ? create_optimized_surface(nsurf) : nsurf;

}

surface light_surface(const surface &surf, const surface &lightmap, bool optimize) {
    if (surf == NULL) {
        return NULL;
    }
    if (lightmap == NULL) {
        return surf;
    }

    surface nsurf = make_neutral_surface(surf);

    if (nsurf == NULL) {
        std::cerr << "could not make neutral surface...\n";
        return NULL;
    }
    if (nsurf->w != lightmap->w) {
        // we don't support efficiently different width.
        // (different height is not a real problem)
        // This function is used on all hexes and usually only for that
        // so better keep it simple and efficient for the normal case
        std::cerr << "Detected an image with bad dimensions: " << nsurf->w << "x" << nsurf->h
                  << "\n";
        std::cerr << "It will not be lighted, please use: " << lightmap->w << "x" << lightmap->h
                  << "\n";
        return nsurf;
    }
    {
        surface_lock lock(nsurf);
        const_surface_lock llock(lightmap);

        Uint32* beg = lock.pixels();
        Uint32* end = beg + nsurf->w * nsurf->h;
        const Uint32* lbeg = llock.pixels();
        const Uint32* lend = lbeg + lightmap->w * lightmap->h;

        while (beg != end && lbeg != lend) {
            Uint8 alpha = ( *beg) >> 24;
            if (alpha) {
                Uint8 lr, lg, lb;

                lr = ( *lbeg) >> 16;
                lg = ( *lbeg) >> 8;
                lb = ( *lbeg);

                Uint8 r, g, b;
                r = ( *beg) >> 16;
                g = ( *beg) >> 8;
                b = ( *beg);

                int dr = (static_cast<int>(lr) - 128) * 2;
                int dg = (static_cast<int>(lg) - 128) * 2;
                int db = (static_cast<int>(lb) - 128) * 2;
                //note that r + dr will promote r to int (needed to avoid Uint8 math)
                r = std::max<int>(0, std::min<int>(255, r + dr));
                g = std::max<int>(0, std::min<int>(255, g + dg));
                b = std::max<int>(0, std::min<int>(255, b + db));

                *beg = (alpha << 24) + (r << 16) + (g << 8) + b;
            }
            ++beg;
            ++lbeg;
        }
    }

    return optimize ? create_optimized_surface(nsurf) : nsurf;
}

//GaussianBlur(InputArray src, OutputArray dst, Size ksize, double sigmaX, double sigmaY=0,
//             int borderType=BORDER_DEFAULT )

surface cut_surface(const surface &surf, SDL_Rect const &r) {
    if (surf == NULL) return NULL;

    surface res = create_compatible_surface(surf, r.w, r.h);

    if (res == NULL) {
        std::cerr << "Could not create a new surface in cut_surface()\n";
        return NULL;
    }

    size_t sbpp = surf->format->BytesPerPixel;
    size_t spitch = surf->pitch;
    size_t rbpp = res->format->BytesPerPixel;
    size_t rpitch = res->pitch;

    // compute the areas to copy
    SDL_Rect src_rect = r;
    SDL_Rect dst_rect = { 0, 0, r.w, r.h };

    if (src_rect.x < 0) {
        if (src_rect.x + src_rect.w <= 0) return res;
        dst_rect.x -= src_rect.x;
        dst_rect.w += src_rect.x;
        src_rect.w += src_rect.x;
        src_rect.x = 0;
    }
    if (src_rect.y < 0) {
        if (src_rect.y + src_rect.h <= 0) return res;
        dst_rect.y -= src_rect.y;
        dst_rect.h += src_rect.y;
        src_rect.h += src_rect.y;
        src_rect.y = 0;
    }

    if (src_rect.x >= surf->w || src_rect.y >= surf->h) return res;

    const_surface_lock slock(surf);
    surface_lock rlock(res);

    const Uint8* src = reinterpret_cast<const Uint8 *>(slock.pixels());
    Uint8* dest = reinterpret_cast<Uint8 *>(rlock.pixels());

    for (int y = 0; y < src_rect.h && (src_rect.y + y) < surf->h; ++y) {
        const Uint8* line_src = src + (src_rect.y + y) * spitch + src_rect.x * sbpp;
        Uint8* line_dest = dest + (dst_rect.y + y) * rpitch + dst_rect.x * rbpp;
        size_t size = src_rect.w + src_rect.x <= surf->w ? src_rect.w : surf->w - src_rect.x;

        assert(rpitch >= src_rect.w * rbpp);
        memcpy(line_dest, line_src, size * rbpp);
    }

    return res;
}
surface blend_surface(const surface &surf,
                      const double amount,
                      const Uint32 color,
                      const bool optimize) {
    if (surf == NULL) {
        return NULL;
    }

    surface nsurf(make_neutral_surface(surf));

    if (nsurf == NULL) {
        std::cerr << "could not make neutral surface...\n";
        return NULL;
    }

    {
        surface_lock lock(nsurf);
        Uint32* beg = lock.pixels();
        Uint32* end = beg + nsurf->w * surf->h;

        Uint16 ratio = amount * 256;
        const Uint16 red = ratio * static_cast<Uint8>(color >> 16);
        const Uint16 green = ratio * static_cast<Uint8>(color >> 8);
        const Uint16 blue = ratio * static_cast<Uint8>(color);
        ratio = 256 - ratio;

#ifdef PANDORA
        /*
         * Use an optimised version of the generic algorithm. The optimised
         * version processes 8 pixels a time. If the number of pixels is not an
         * exact multiple of 8 it falls back to the generic algorithm to handle
         * the last pixels.
         */
        uint16x8_t vred = vdupq_n_u16(red);
        uint16x8_t vgreen = vdupq_n_u16(green);
        uint16x8_t vblue = vdupq_n_u16(blue);

        uint8x8_t vratio = vdup_n_u8(ratio);

        const int div = (nsurf->w * surf->h) / 8;
        for(int i = 0; i < div; ++i, beg += 8) {
            uint8x8x4_t rgba = vld4_u8(reinterpret_cast<Uint8*>(beg));

            uint16x8_t b = vmull_u8(rgba.val[0], vratio);
            uint16x8_t g = vmull_u8(rgba.val[1], vratio);
            uint16x8_t r = vmull_u8(rgba.val[2], vratio);

            b = vaddq_u16(b, vblue);
            g = vaddq_u16(g, vgreen);
            r = vaddq_u16(r, vred);

            rgba.val[0] = vshrn_n_u16(b, 8);
            rgba.val[1] = vshrn_n_u16(g, 8);
            rgba.val[2] = vshrn_n_u16(r, 8);

            vst4_u8(reinterpret_cast<Uint8*>(beg), rgba);
        }
#endif
        while (beg != end) {
            Uint8 a = static_cast<Uint8>( *beg >> 24);
            Uint8 r = (ratio * static_cast<Uint8>( *beg >> 16) + red) >> 8;
            Uint8 g = (ratio * static_cast<Uint8>( *beg >> 8) + green) >> 8;
            Uint8 b = (ratio * static_cast<Uint8>( *beg) + blue) >> 8;

            *beg = (a << 24) | (r << 16) | (g << 8) | b;

            ++beg;
        }
    }

    return optimize ? create_optimized_surface(nsurf) : nsurf;
}

surface rotate_surface(const surface& surf,
                       float deg,
                       float scale,
                       int center_x,
                       int center_y,
                       int croporgrow,
                       bool optimize) {
    return rotate_surface(surf, (double)deg, (double)scale, center_x, center_y, croporgrow, optimize);
}

surface rotate_surface(const surface& surf,
                       double deg,
                       double scale,
                       int center_x,
                       int center_y,
                       int croporgrow,
                       bool optimize) {
    if (surf == NULL) { throw std::invalid_argument("Cannot rotate null surface."); }
    if (deg == 0.0) {
        std::cerr << "Warning, rotating surface by 0.0 deg.\n";
        return optimize ? create_optimized_surface(surf) : make_neutral_surface(surf);
    }
    if (croporgrow != SDL_UTILS_ROTATE_CROP && croporgrow != SDL_UTILS_ROTATE_GROW) {
        throw std::invalid_argument(
            "croporgrow must be either SDL_UTILS_ROTATE_CROP or SDL_UTILS_ROTATE_GROW.\n");
    }

    // Make sure both surfaces are always in the "neutral" pixel format
    surface dst(create_neutral_surface(surf->w, surf->h));
    surface src(make_neutral_surface(surf));
    if (src == NULL || dst == NULL) {
        throw std::logic_error("Could not create neutral surfaces. This is a BUG.");
    }

    { // Extra scoping used for the surface_lock.
        using namespace cv;
        Point ctr_rot = Point(center_x,center_y);  /// The center of rotation
        Mat rot_mat = getRotationMatrix2D(ctr_rot, deg, scale);

        const_surface_lock src_lock(src);
        surface_lock dst_lock(dst);

        //int rows, int cols, int type, void* data, size_t step=AUTO_STEP
        Mat src_mat = Mat(src->h, src->w, CV_8UC4, src->pixels, Mat::AUTO_STEP);
        Mat dst_mat = Mat(dst->h, dst->w, CV_8UC4, dst->pixels, Mat::AUTO_STEP);

        warpAffine(src_mat, dst_mat, rot_mat, dst_mat.size(), INTER_CUBIC,
                   BORDER_CONSTANT, 0x00000000);
    }

    return optimize ? create_optimized_surface(dst) : dst;
}

surface rotate_any_surface(const surface& surf, float angle, int zoom, int offset,
                           bool optimize) {
    return rotate_surface(surf, (double)angle, double(zoom)/(double)offset,
                          surf->w/2, surf->h/2, SDL_UTILS_ROTATE_CROP, optimize);
}

surface rotate_180_surface(const surface &surf, bool optimize) {
    return rotate_surface(surf, 180.0, 1.0, surf->w/2, surf->h/2,
                          SDL_UTILS_ROTATE_CROP, optimize);
}

surface rotate_90_surface(const surface &surf, bool clockwise, bool optimize) {
    return rotate_surface(surf, (clockwise ? -90.0 : 90.0), 1.0, surf->w/2, surf->h/2,
                          SDL_UTILS_ROTATE_CROP, optimize);
}


void put_pixel(const surface& surf, surface_lock& surf_lock, int x, int y, Uint32 pixel) {
    const int bpp = surf->format->BytesPerPixel;
    /* dst is the address to the pixel we want to set */
    Uint8* const dst = reinterpret_cast<Uint8*>(surf_lock.pixels()) + y * surf->pitch + x * bpp;
    switch (bpp) {
        case 1:
            *dst = pixel;
            break;
        case 2:
            *reinterpret_cast<Uint16*>(dst) = pixel;
            break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                dst[0] = (pixel >> 16) & 0xff;
                dst[1] = (pixel >> 8) & 0xff;
                dst[2] = pixel & 0xff;
            } else {
                dst[0] = pixel & 0xff;
                dst[1] = (pixel >> 8) & 0xff;
                dst[2] = (pixel >> 16) & 0xff;
            }
            break;
        case 4:
            *reinterpret_cast<Uint32*>(dst) = pixel;
            break;
        default:
            break;
    }
}

Uint32 get_pixel(const surface& surf, const const_surface_lock& surf_lock, int x, int y) {
    const int bpp = surf->format->BytesPerPixel;
    /* p is the address to the pixel we want to retrieve */
    const Uint8* const src = reinterpret_cast<const Uint8*>(surf_lock.pixels())
        + y * surf->pitch + x * bpp;
    switch (bpp) {
        case 1:
            return *src;
        case 2:
            return *reinterpret_cast<const Uint16*>(src);
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return src[0] << 16 | src[1] << 8 | src[2];
            else
                return src[0] | src[1] << 8 | src[2] << 16;
            break;
        case 4:
            return *reinterpret_cast<const Uint32*>(src);
    }
    return 0;
}


surface flip_surface(const surface &surf, bool optimize) {
    if (surf == NULL) { throw std::invalid_argument("Cannot flip null surface."); }

    surface nsurf(make_neutral_surface(surf));
    if (nsurf == NULL) { throw std::logic_error("Could not make neutral surface..."); }

    {
        surface_lock lock(nsurf);
        Uint32* const pixels = lock.pixels();

        for (int y = 0; y != nsurf->h; ++y) {
            for (int x = 0; x != nsurf->w / 2; ++x) {
                const int index1 = y * nsurf->w + x;
                const int index2 = (y + 1) * nsurf->w - x - 1;
                std::swap(pixels[index1], pixels[index2]);
            }
        }
    }

    return optimize ? create_optimized_surface(nsurf) : nsurf;
}

surface flop_surface(const surface &surf, bool optimize) {
    if (surf == NULL) {
        return NULL;
    }

    surface nsurf(make_neutral_surface(surf));

    if (nsurf == NULL) {
        std::cerr << "could not make neutral surface...\n";
        return NULL;
    }

    {
        surface_lock lock(nsurf);
        Uint32* const pixels = lock.pixels();

        for (int x = 0; x != nsurf->w; ++x) {
            for (int y = 0; y != nsurf->h / 2; ++y) {
                const int index1 = y * nsurf->w + x;
                const int index2 = (nsurf->h - y - 1) * surf->w + x;
                std::swap(pixels[index1], pixels[index2]);
            }
        }
    }

    return optimize ? create_optimized_surface(nsurf) : nsurf;
}

surface create_compatible_surface(const surface &surf, int width, int height) {
    if (surf == NULL) return NULL;

    if (width == -1) width = surf->w;

    if (height == -1) height = surf->h;

    surface s = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, surf->format->BitsPerPixel,
                                     surf->format->Rmask, surf->format->Gmask,
                                     surf->format->Bmask, surf->format->Amask);
    if (surf->format->palette) {
        SDL_SetPalette(s, SDL_LOGPAL, surf->format->palette->colors, 0,
                       surf->format->palette->ncolors);
    }
    return s;
}

void blit_surface(const surface& src,
                  const SDL_Rect* srcrect,
                  surface& dst,
                  const SDL_Rect* dstrect) {
    assert(src);
    assert(dst);

    // Get the areas to blit
    SDL_Rect dst_rect = create_rect(0, 0, dst->w, dst->h);
    if (dstrect) {
        dst_rect.x = dstrect->x;
        dst_rect.w -= dstrect->x;

        dst_rect.y = dstrect->y;
        dst_rect.h -= dstrect->y;

    }

    SDL_Rect src_rect = create_rect(0, 0, src->w, src->h);
    if (srcrect && srcrect->w && srcrect->h) {
        src_rect.x = srcrect->x;
        src_rect.y = srcrect->y;

        src_rect.w = srcrect->w;
        src_rect.h = srcrect->h;

        if (src_rect.x < 0) {
            if (src_rect.x + src_rect.w <= 0 || src_rect.x + dst_rect.w <= 0) return;
            dst_rect.x -= src_rect.x;
            dst_rect.w += src_rect.x;
            src_rect.w += src_rect.x;
            src_rect.x = 0;
        }
        if (src_rect.y < 0) {
            if (src_rect.y + src_rect.h <= 0 || src_rect.y + dst_rect.h <= 0) return;
            dst_rect.y -= src_rect.y;
            dst_rect.h += src_rect.y;
            src_rect.h += src_rect.y;
            src_rect.y = 0;
        }
        if (src_rect.x + src_rect.w > src->w) {
            if (src_rect.x >= src->w) return;
            src_rect.w = src->w - src_rect.x;
        }
        if (src_rect.y + src_rect.h > src->h) {
            if (src_rect.y >= src->h) return;
            src_rect.h = src->h - src_rect.y;
        }
    }

    assert(dst_rect.x >= 0);
    assert(dst_rect.y >= 0);

    // Get the blit size limits.
    const unsigned width = std::min(src_rect.w, dst_rect.w);
    const unsigned height = std::min(src_rect.h, dst_rect.h);

    {
        // Extra scoping used for the surface_lock.
        const_surface_lock src_lock(src);
        surface_lock dst_lock(dst);

        const Uint32* const src_pixels = src_lock.pixels();
        Uint32* dst_pixels = dst_lock.pixels();

        for (unsigned y = 0; y < height; ++y) {
            for (unsigned x = 0; x < width; ++x) {

                // We need to do the blitting using some optimizations
                // if the src is fully transparent we can ignore this pixel
                // if the src is fully opaque we can overwrite the destination with this pixel
                // if the destination is fully transparent we replace us with the source
                //
                // We do these optimizations between the extraction of the variables
                // to avoid creating variables not used (it might save us some cycles).

                const int src_offset = (y + src_rect.y) * src->w + (x + src_rect.x);
                assert(src_offset < src->w * src->h);
                const Uint32 src_pixel = src_pixels[src_offset];
                const Uint8 src_a = (src_pixel & 0xFF000000) >> 24;

                if ( !src_a) {
                    // Fully transparent source, ignore
                    continue;
                }

                const ptrdiff_t dst_offset = (y + dst_rect.y) * dst->w + (x + dst_rect.x);
                assert(dst_offset < dst->w * dst->h);
                if (src_a == 255) {
                    // Fully opaque source, copy
                    dst_pixels[dst_offset] = src_pixel;
                    continue;
                }

                const Uint32 dst_pixel = dst_pixels[dst_offset];
                Uint8 dst_a = (dst_pixel & 0xFF000000) >> 24;

                if ( !dst_a) {
                    // Fully transparent destination, copy
                    dst_pixels[dst_offset] = src_pixel;
                    continue;
                }

                const Uint8 src_r = (src_pixel & 0x00FF0000) >> 16;
                const Uint8 src_g = (src_pixel & 0x0000FF00) >> 8;
                const Uint8 src_b = src_pixel & 0x000000FF;

                Uint8 dst_r = (dst_pixel & 0x00FF0000) >> 16;
                Uint8 dst_g = (dst_pixel & 0x0000FF00) >> 8;
                Uint8 dst_b = dst_pixel & 0x000000FF;

                if (dst_a == 255) {

                    // Destination fully opaque blend the source.
                    dst_r = ( ( (src_r - dst_r) * src_a) >> 8) + dst_r;
                    dst_g = ( ( (src_g - dst_g) * src_a) >> 8) + dst_g;
                    dst_b = ( ( (src_b - dst_b) * src_a) >> 8) + dst_b;

                } else {

                    // Destination and source party transparent.

                    // acquired the data now do the blitting
                    const unsigned tmp_a = 255 - src_a;

                    const unsigned tmp_r = 1 + (src_r * src_a) + (dst_r * tmp_a);
                    dst_r = (tmp_r + (tmp_r >> 8)) >> 8;

                    const unsigned tmp_g = 1 + (src_g * src_a) + (dst_g * tmp_a);
                    dst_g = (tmp_g + (tmp_g >> 8)) >> 8;

                    const unsigned tmp_b = 1 + (src_b * src_a) + (dst_b * tmp_a);
                    dst_b = (tmp_b + (tmp_b >> 8)) >> 8;

                    dst_a += ( ( (255 - dst_a) * src_a) >> 8);
                }

                dst_pixels[dst_offset] = (dst_a << 24) | (dst_r << 16) | (dst_g << 8) | (dst_b);

            }
        }
    }
}

void fill_rect_alpha(SDL_Rect &rect, Uint32 color, Uint8 alpha, surface &target) {
    if (alpha == SDL_ALPHA_OPAQUE) {
        sdl_fill_rect(target, &rect, color);
        return;
    } else if (alpha == SDL_ALPHA_TRANSPARENT) {
        return;
    }

    surface tmp(create_compatible_surface(target, rect.w, rect.h));
    if (tmp == NULL) {
        return;
    }

    SDL_Rect r = { 0, 0, rect.w, rect.h };
    sdl_fill_rect(tmp, &r, color);
    SDL_SetAlpha(tmp, SDL_SRCALPHA, alpha);
    sdl_blit(tmp, NULL, target, &rect);
}

surface get_surface_portion(const surface &src, SDL_Rect &area, bool optimize_format) {
    if (src == NULL) {
        return NULL;
    }

    // Check if there is something in the portion
    if (area.x >= src->w || area.y >= src->h || area.x + area.w < 0 || area.y + area.h < 0) {
        return NULL;
    }

    if (area.x + area.w > src->w) {
        area.w = src->w - area.x;
    }
    if (area.y + area.h > src->h) {
        area.h = src->h - area.y;
    }

    // use same format as the source (almost always the screen)
    surface dst = create_compatible_surface(src, area.w, area.h);

    if (dst == NULL) {
        std::cerr << "Could not create a new surface in get_surface_portion()\n";
        return NULL;
    }

    sdl_blit(src, &area, dst, NULL);

    return optimize_format ? display_format_alpha(dst) : dst;
}

namespace {

struct not_alpha {
        not_alpha() {
        }

        // we assume neutral format
        bool operator()(Uint32 pixel) const {
            Uint8 alpha = pixel >> 24;
            return alpha != 0x00;
        }
};

}

SDL_Rect get_non_transparent_portion(const surface &surf) {
    SDL_Rect res = { 0, 0, 0, 0 };
    surface nsurf(make_neutral_surface(surf));
    if (nsurf == NULL) {
        std::cerr << "failed to make neutral surface\n";
        return res;
    }

    const not_alpha calc;

    surface_lock lock(nsurf);
    const Uint32* const pixels = lock.pixels();

    int n;
    for (n = 0; n != nsurf->h; ++n) {
        const Uint32* const start_row = pixels + n * nsurf->w;
        const Uint32* const end_row = start_row + nsurf->w;

        if (std::find_if(start_row, end_row, calc) != end_row) break;
    }

    res.y = n;

    for (n = 0; n != nsurf->h - res.y; ++n) {
        const Uint32* const start_row = pixels + (nsurf->h - n - 1) * surf->w;
        const Uint32* const end_row = start_row + nsurf->w;

        if (std::find_if(start_row, end_row, calc) != end_row) break;
    }

    // The height is the height of the surface,
    // minus the distance from the top and
    // the distance from the bottom.
    res.h = nsurf->h - res.y - n;

    for (n = 0; n != nsurf->w; ++n) {
        int y;
        for (y = 0; y != nsurf->h; ++y) {
            const Uint32 pixel = pixels[y * nsurf->w + n];
            if (calc(pixel)) break;
        }

        if (y != nsurf->h) break;
    }

    res.x = n;

    for (n = 0; n != nsurf->w - res.x; ++n) {
        int y;
        for (y = 0; y != nsurf->h; ++y) {
            const Uint32 pixel = pixels[y * nsurf->w + surf->w - n - 1];
            if (calc(pixel)) break;
        }

        if (y != nsurf->h) break;
    }

    res.w = nsurf->w - res.x - n;

    return res;
}

bool operator==(const SDL_Rect& a, const SDL_Rect& b) {
    return a.x == b.x && a.y == b.y && a.w == b.w && a.h == b.h;
}

bool operator!=(const SDL_Rect& a, const SDL_Rect& b) {
    return !operator==(a, b);
}

bool operator==(const SDL_Color& a, const SDL_Color& b) {
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

bool operator!=(const SDL_Color& a, const SDL_Color& b) {
    return !operator==(a, b);
}

SDL_Color inverse(const SDL_Color& color) {
    SDL_Color inverse;
    inverse.r = 255 - color.r;
    inverse.g = 255 - color.g;
    inverse.b = 255 - color.b;
    inverse.unused = 0;
    return inverse;
}

surface_restorer::surface_restorer() :
    target_(NULL), rect_(empty_rect), surface_(NULL) {
}

surface_restorer::surface_restorer(CVideo* target, const SDL_Rect& rect) :
    target_(target), rect_(rect), surface_(NULL) {
    update();
}

surface_restorer::~surface_restorer() {
    restore();
}

void surface_restorer::restore(SDL_Rect const &dst) const {
    if (surface_.null()) return;
    SDL_Rect dst2 = intersect_rects(dst, rect_);
    if (dst2.w == 0 || dst2.h == 0) return;
    SDL_Rect src = dst2;
    src.x -= rect_.x;
    src.y -= rect_.y;
    sdl_blit(surface_, &src, target_->getSurface(), &dst2);
    update_rect(dst2);
}

void surface_restorer::restore() const {
    if (surface_.null()) return;
    SDL_Rect dst = rect_;
    sdl_blit(surface_, NULL, target_->getSurface(), &dst);
    update_rect(rect_);
}

void surface_restorer::update() {
    if (rect_.w == 0 || rect_.h == 0)
        surface_.assign(NULL);
    else
        surface_.assign(::get_surface_portion(target_->getSurface(), rect_));
}

void surface_restorer::cancel() {
    surface_.assign(NULL);
}

void draw_rectangle(int x, int y, int w, int h, Uint32 color, surface target) {

    SDL_Rect top = create_rect(x, y, w, 1);
    SDL_Rect bot = create_rect(x, y + h - 1, w, 1);
    SDL_Rect left = create_rect(x, y, 1, h);
    SDL_Rect right = create_rect(x + w - 1, y, 1, h);

    sdl_fill_rect(target, &top, color);
    sdl_fill_rect(target, &bot, color);
    sdl_fill_rect(target, &left, color);
    sdl_fill_rect(target, &right, color);
}

void draw_solid_tinted_rectangle(int x,
                                 int y,
                                 int w,
                                 int h,
                                 int r,
                                 int g,
                                 int b,
                                 double alpha,
                                 surface target) {

    SDL_Rect rect = create_rect(x, y, w, h);
    fill_rect_alpha(rect, SDL_MapRGB(target->format, r, g, b), Uint8(alpha * 255), target);
}

void draw_centered_on_background(surface surf,
                                 const SDL_Rect& rect,
                                 const SDL_Color& color,
                                 surface target) {
    clip_rect_setter clip_setter(target, &rect);

    Uint32 col = SDL_MapRGBA(target->format, color.r, color.g, color.b, color.unused);
    //TODO: only draw background outside the image
    SDL_Rect r = rect;
    sdl_fill_rect(target, &r, col);

    if (surf != NULL) {
        r.x = rect.x + (rect.w - surf->w) / 2;
        r.y = rect.y + (rect.h - surf->h) / 2;
        sdl_blit(surf, NULL, target, &r);
    }
    update_rect(rect);
}

std::ostream& operator<<(std::ostream& s, const SDL_Rect& rect) {
    s << rect.x << ',' << rect.y << " x " << rect.w << ',' << rect.h;
    return s;
}

