/**
 * @file    src/sdl_utils.cpp
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
 */

#include "sdl_utils.hpp"

// Event Handling Utilities -------------------------------------------------------------------

SDL_Keymod sdl_keysym_from_name(std::string const &keyname) {
    static bool initialized = false;
    typedef std::map<std::string const, SDL_Keymod> keysym_map_t;
    static keysym_map_t keysym_map;

    if ( !initialized) {
        for (SDL_Keymod i = SDLK_FIRST; i < SDLK_LAST; i = SDL_Keymod(int(i) + 1)) {
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

// end Event Handling Utilities ---------------------------------------------------------------

// Geometric Utilities ------------------------------------------------------------------------
// end Geometric Utilities --------------------------------------------------------------------

// Color Utilities ----------------------------------------------------------------------------

SDL_Color invert_value(const SDL_Color* color) {
    assert(sizeof (float) == 32);       /** @note this function depends on 32 bit floats. */
    Uint32 clr = ((color->r << 24) +
                  (color->g << 16) +
                  (color->b <<  8) +
                  (color->a));                      // convert SDL_color to uint32 RGBA
    cv::Mat src_mat = cv::Mat(1, 1, CV_8UC4, clr);
    cv::Mat hsv_mat = cv::Mat(1, 1, CV_32FC4);
    cv::Mat dst_mat = cv::Mat(1, 1, CV_8UC4);
    cv::cvtColor(src_mat, hsv_mat, CV_RGB2HSV, 4);  // convert to HSV
    float* hsv_raw = (float*)hsv_mat.data;
    if (hsv_raw[2] >= 0.5f) {
        hsv_raw[2] -= 0.5f;
    } else {
        hsv_raw[2] += 0.5f;
    }                                               // invert the value
    cv::cvtColor(hsv_mat, dst_mat, CV_HSV2RGB, 4);  // convert back to RGBA
    Uint8* dst_raw = dst_mat.data;
    SDL_Color dst_clr;
    dst_clr.r = dst_raw[0];
    dst_clr.g = dst_raw[1];
    dst_clr.b = dst_raw[2];
    dst_clr.a = dst_raw[3];
    return dst_clr;
}

SDL_Color invert_hue(const SDL_Color* color) {
    assert(sizeof (float) == 32);       /** @note this function depends on 32 bit floats. */
    Uint32 clr = ((color->r << 24) +
                  (color->g << 16) +
                  (color->b <<  8) +
                  (color->a));                      // convert SDL_color to uint32 RGBA
    cv::Mat src_mat = cv::Mat(1, 1, CV_8UC4, clr);
    cv::Mat hsv_mat = cv::Mat(1, 1, CV_32FC4);
    cv::Mat dst_mat = cv::Mat(1, 1, CV_8UC4);
    cv::cvtColor(src_mat, hsv_mat, CV_RGB2HSV, 4);  // convert to HSV
    float* hsv_raw = (float*)hsv_mat.data;
    if (hsv_raw[0] >= 180.0f) {
        hsv_raw[2] -= 180.0f;
    } else {
        hsv_raw[2] += 180.0f;
    }                                               // invert the hue
    cv::cvtColor(hsv_mat, dst_mat, CV_HSV2RGB, 4);  // convert back to RGBA
    Uint8* dst_raw = dst_mat.data;
    SDL_Color dst_clr;
    dst_clr.r = dst_raw[0];
    dst_clr.g = dst_raw[1];
    dst_clr.b = dst_raw[2];
    dst_clr.a = dst_raw[3];
    return dst_clr;
}

// end Color Utilities ------------------------------------------------------------------------

// Image Utilities ----------------------------------------------------------------------------

/** Replacement for multiple existing scaling functions. */
SDL_Surface resize_surface(SDL_Surface* surf,
                           Uint32 h,
                           Uint32 w) {
    if (surf == nullptr) {
        throw std::invalid_argument("Cannot resize an null surface.");
    }
    if ((h == 0) || (w == 0)) {
        throw std::invalid_argument("Cannot resize a SDL_Surface to 0 area.");
    }

    SDL_LockSurface(surf);
    SDL_Surface* src = SDL_CreateRGBSurfaceFrom(surf->pixels, surf->w, surf->h,
                                                surf->format->BitsPerPixel, surf->pitch,
                                                surf->format->Rmask, surf->format->Gmask,
                                                surf->format->Bmask, surf->format->Amask);
    SDL_UnlockSurface(surf);
    if ((static_cast<int>(h) == surf->h) && (static_cast<int>(w) == surf->w)) {
        std::cerr << "Warning: Resizing a SDL_Surface to same size. " <<
            surf->h << "x" << surf->w << " -> " << h << "x" << w << "\n";
        return *src;
    }

    SDL_Surface* dst = SDL_CreateRGBSurface(0, w, h, BPP, RMASK, GMASK, BMASK, AMASK);

    /** TODO: Conditionally use OpenCL or CUDA if system supports them. */
    using namespace cv;
    SDL_LockSurface(dst);
    SDL_LockSurface(src);

    //int rows, int cols, int type, void* data, size_t step=AUTO_STEP
    Mat src_mat = Mat(surf->h, surf->w, CV_8UC4, src->pixels, Mat::AUTO_STEP);
    Mat dst_mat = Mat(h, w, CV_8UC4, dst->pixels, Mat::AUTO_STEP);

    resize(src_mat, dst_mat, dst_mat.size(), 0.0, 0.0, INTER_CUBIC);
    SDL_UnlockSurface(src);
    SDL_UnlockSurface(dst);
    return *dst;
}

/** @brief wrapper for resize_SDL_Surface */
SDL_Surface stretch_surface_horizontal(SDL_Surface* surf, Uint32 w) {
    return resize_surface(surf, (Uint32)surf->h, w);
}

/** @brief wrapper for resize_SDL_Surface */
SDL_Surface stretch_surface_vertical(SDL_Surface* surf, const unsigned h) {
    return resize_surface(surf, h, surf->w);
}

/** @brief wrapper for resize_SDL_Surface */
SDL_Surface scale_surface(SDL_Surface* surf, int w, int h) {
    return resize_surface(surf, h, w);
}

/** @brief wrapper for resize_SDL_Surface */
SDL_Surface scale_surface_sharp(SDL_Surface* surf, int w, int h) {
    return resize_surface(surf, h, w);
}

void surface_blur(SDL_Surface* src, SDL_Surface* dst, Uint8 depth) {
    if (src == nullptr || dst == nullptr) {
        throw std::invalid_argument("Cannot blur a null surface.");
    }
    if (depth < 1) { throw std::invalid_argument("Cannot blur with depth < 1."); }
    if (depth >= std::min(src->w, src->h)) {
        throw std::invalid_argument("Cannot blur with depth >= min(surf->w, surf->y).");
    }

    /** TODO: Conditionally use OpenCL or CUDA if system supports them. */
    SDL_LockSurface(dst);
    SDL_LockSurface(src);
    cv::Mat src_mat = cv::Mat(src->h, src->w, CV_8UC4, src->pixels, cv::Mat::AUTO_STEP);
    cv::Mat dst_mat = cv::Mat(src->h, src->w, CV_8UC4, dst->pixels,  cv::Mat::AUTO_STEP);
    cv::blur(src_mat, dst_mat, cv::Size((depth + 1), (depth + 1)), cv::Point(-1,-1));
    SDL_UnlockSurface(src);
    SDL_UnlockSurface(dst);
}

SDL_Surface surface_blur(SDL_Surface* surf, Uint8 depth) {
    SDL_Surface* dst = SDL_CreateRGBSurface(0, surf->w, surf->h, BPP, RMASK, GMASK, BMASK, AMASK);
    surface_blur(surf, dst, depth);
    return *dst;
}

void tile_surface(SDL_Surface* src, SDL_Surface* dst) {
    if (src == nullptr || dst == nullptr) {
        throw std::invalid_argument("Cannot tile a null surface.");
    }
    if (src->w >= dst->w || src->h >= dst->h) {
        throw std::invalid_argument("Must tile to a larger dst surface.");
    }

    SDL_LockSurface(src);
    SDL_LockSurface(dst);
    Uint32* srcpixels = src->pixels;
    Uint32* dstpixels = dst->pixels;

    const int xoff = (dst->w - src->w) / 2;
    const int yoff = (dst->h - src->h) / 2;

    for (int i = 0; i < dst->w * dst->h; ++i) {
        int x = ( (i % dst->w) - xoff);
        int y = ( (i / dst->w) - yoff);

        while ( (x += src->w) < 0) { /* DO NOTHING */
        }
        while ( (y += src->h) < 0) { /* DO NOTHING */
        }
        dstpixels[i] = srcpixels[(y % src->h) * src->w + (x % src->w)];
    }

    SDL_UnlockSurface(src);
    SDL_UnlockSurface(dst);
}

SDL_Surface tile_surface(SDL_Surface* src, int w, int h) {
    SDL_Surface* dst = SDL_CreateRGBSurface(0, src->w, src->h, BPP, RMASK, GMASK, BMASK, AMASK);
    tile_surface(src, dst);
    return *dst;
}

void surface_change_hue(SDL_Surface* surf, float hue_diff) {
    error
}

void surface_change_val(SDL_Surface* surf, float val_diff) {
    assert(sizeof (float) == 32);       /** @note this function depends on 32 bit floats. */
    SDL_LockSurface(surf);
    cv::Mat src_mat = cv::Mat(surf->h, surf->w, CV_8UC4, surf->pixels);
    cv::Mat hsv_mat = cv::Mat(surf->h, surf->w, CV_32FC4);
    cv::cvtColor(src_mat, hsv_mat, CV_RGBA2, 4);  // convert to HSV
    float* hsv_raw = (float*)hsv_mat.data;
    if (hsv_raw[2] >= 0.5f) {
        hsv_raw[2] -= 0.5f;
    } else {
        hsv_raw[2] += 0.5f;
    }                                               // invert the value
    cv::cvtColor(hsv_mat, dst_mat, CV_HSV2RGB, 4);  // convert back to RGBA
    Uint8* dst_raw = dst_mat.data;
    SDL_Color dst_clr;
    dst_clr.r = dst_raw[0];
    dst_clr.g = dst_raw[1];
    dst_clr.b = dst_raw[2];
    dst_clr.a = dst_raw[3];
    SDL_UnlockSurface(surf);
}

void surface_change_sat(SDL_Surface* surf, float sat_diff) {
    error
}

void surface_compose_overlay(SDL_Surface* top, std::pair<Uint32, Uint32> top_offset,
                             SDL_Surface* bot, std::pair<Uint32, Uint32> bot_offset,
                             SDL_Surface* dst) {
    // TODO Rewriteme to properly handle transparency.
    if (top == nullptr || bot == nullptr || dst == nullptr) {
        throw std::invalid_argument("Cannot compose with null arguments.");
    }
    if ((top->w + std::get<0>(top_offset) > dst->w) ||
        (top->h + std::get<1>(top_offset) > dst->h) ||
        (bot->w + std::get<0>(bot_offset) > dst->w) ||
        (bot->h + std::get<1>(bot_offset) > dst->h)) {
        throw std::invalid_argument("Src surfs + offsets must lie within dst surf.");
    }

    SDL_LockSurface(top);         SDL_LockSurface(bot);         SDL_LockSurface(dst);
    Uint32* toppix = top->pixels; Uint32* botpix = bot->pixels; Uint32* dstpix = dst->pixels;
    for (Uint32 y = 0; y < dst->h; y++) {
        for (Uint32 x = 0; x < dst->w; x++) {
            // Outside top
            if ((y < std::get<0>(top_offset) || y > top->h + std::get<0>(top_offset)) &&
                (x < std::get<1>(top_offset) || x > top->w + std::get<1>(top_offset))) {
                // Inside bot
                if ((y >= std::get<0>(bot_offset) || y <= bot->h + std::get<0>(bot_offset)) &&
                    (x >= std::get<1>(bot_offset) || x <= bot->w + std::get<1>(bot_offset))) {
                    dstpix[y][x] = botpix[y][x];
                } else { // Outside both top and bottom
                    dstpix[y][x] = 0x00000000;
                }
            } else { //Inside top
                dstpix[y][x] = toppix[y][x];
            }
        }
    }
    SDL_UnlockSurface(top);       SDL_UnlockSurface(bot);       SDL_UnlockSurface(dst);
}

SDL_Surface shadow_image(SDL_Surface* src) {
    if (src == nullptr) return nullptr;

    // Expanded 2pix each side.
    SDL_Surface* shadow = SDL_CreateRGBSurface(0, src->w + 4, src->h + 4,
                                               BPP, RMASK, GMASK, BMASK, AMASK);
    SDL_Surface* dst = SDL_CreateRGBSurface(0, src->w + 4, src->h + 4,
                                            BPP, RMASK, GMASK, BMASK, AMASK);
    surface_blur(src, shadow, 2);
    surface_change_sat(shadow, -0.75f);
    surface_change_val(shadow, -0.50f);
    surface_compose_overlay(src, shadow, dst);

    return *dst;
}

SDL_Surface recolor_image(SDL_Surface surf, const std::map<Uint32, Uint32>& map_rgb, bool optimize) {
    if (surf == nullptr) return nullptr;

    if ( !map_rgb.empty()) {
        SDL_Surface nsurf(make_neutral_surface(surf));
        if (nsurf == nullptr) {
            std::cerr << "failed to make neutral surface\n";
            return nullptr;
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

SDL_Surface brighten_image(const SDL_Surface* surf, fixed_t amount, bool optimize) {
    if (surf == nullptr) {
        return nullptr;
    }

    SDL_Surface nsurf(make_neutral_surface(surf));

    if (nsurf == nullptr) {
        std::cerr << "could not make neutral surface...\n";
        return nullptr;
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

SDL_Surface adjust_surface_alpha(const SDL_Surface* surf, fixed_t amount, bool optimize) {
    if (surf == nullptr) {
        return nullptr;
    }

    SDL_Surface nsurf(make_neutral_surface(surf));

    if (nsurf == nullptr) {
        std::cerr << "could not make neutral surface...\n";
        return nullptr;
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

SDL_Surface adjust_surface_alpha_add(const SDL_Surface* surf, int amount, bool optimize) {
    if (surf == nullptr) {
        return nullptr;
    }

    SDL_Surface nsurf(make_neutral_surface(surf));

    if (nsurf == nullptr) {
        std::cerr << "could not make neutral surface...\n";
        return nullptr;
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

SDL_Surface mask_surface(const SDL_Surface* surf,
                     const SDL_Surface* mask,
                     bool* empty_result,
                     const std::string& filename) {
    if (surf == nullptr) {
        return nullptr;
    }
    if (mask == nullptr) {
        return surf;
    }

    SDL_Surface nsurf = make_neutral_surface(surf);
    SDL_Surface nmask(make_neutral_surface(mask));

    if (nsurf == nullptr || nmask == nullptr) {
        std::cerr << "could not make neutral surface...\n";
        return nullptr;
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

bool in_mask_surface(const SDL_Surface* surf, const SDL_Surface* mask) {
    if (surf == nullptr) {
        return false;
    }
    if (mask == nullptr) {
        return true;
    }

    if (surf->w != mask->w || surf->h != mask->h) {
        // not same size, consider it doesn't fit
        return false;
    }

    SDL_Surface nsurf = make_neutral_surface(surf);
    SDL_Surface nmask(make_neutral_surface(mask));

    if (nsurf == nullptr || nmask == nullptr) {
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

SDL_Surface submerge_alpha(const SDL_Surface* surf,
                       int depth,
                       float alpha_base,
                       float alpha_delta,
                       bool optimize) {
    if (surf == nullptr) {
        return nullptr;
    }

    SDL_Surface nsurf(make_neutral_surface(surf));

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

SDL_Surface light_surface(const SDL_Surface* surf, const SDL_Surface* lightmap, bool optimize) {
    if (surf == nullptr) {
        return nullptr;
    }
    if (lightmap == nullptr) {
        return surf;
    }

    SDL_Surface nsurf = make_neutral_surface(surf);

    if (nsurf == nullptr) {
        std::cerr << "could not make neutral surface...\n";
        return nullptr;
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

SDL_Surface cut_surface(const SDL_Surface* surf, SDL_Rect const &r) {
    if (surf == nullptr) return nullptr;

    SDL_Surface res = create_compatible_surface(surf, r.w, r.h);

    if (res == nullptr) {
        std::cerr << "Could not create a new SDL_Surface in cut_surface()\n";
        return nullptr;
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
SDL_Surface blend_surface(const SDL_Surface* surf,
                      const double amount,
                      const Uint32 color,
                      const bool optimize) {
    if (surf == nullptr) {
        return nullptr;
    }

    SDL_Surface nsurf(make_neutral_surface(surf));

    if (nsurf == nullptr) {
        std::cerr << "could not make neutral surface...\n";
        return nullptr;
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

SDL_Surface rotate_surface(const SDL_Surface* surf,
                       float deg,
                       float scale,
                       int center_x,
                       int center_y,
                       int croporgrow,
                       bool optimize) {
    return rotate_surface(surf, (double)deg, (double)scale, center_x, center_y, croporgrow, optimize);
}

SDL_Surface rotate_surface(const SDL_Surface* surf,
                       double deg,
                       double scale,
                       int center_x,
                       int center_y,
                       int croporgrow,
                       bool optimize) {
    if (surf == nullptr) { throw std::invalid_argument("Cannot rotate null surface."); }
    if (deg == 0.0) {
        std::cerr << "Warning, rotating SDL_Surface by 0.0 deg.\n";
        return optimize ? create_optimized_surface(surf) : make_neutral_surface(surf);
    }
    if (croporgrow != SDL_UTILS_ROTATE_CROP && croporgrow != SDL_UTILS_ROTATE_GROW) {
        throw std::invalid_argument(
            "croporgrow must be either SDL_UTILS_ROTATE_CROP or SDL_UTILS_ROTATE_GROW.\n");
    }

    // Make sure both surfaces are always in the "neutral" pixel format
    SDL_Surface dst(create_neutral_surface(surf->w, surf->h));
    SDL_Surface src(make_neutral_surface(surf));
    if (src == nullptr || dst == nullptr) {
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

SDL_Surface rotate_any_surface(const SDL_Surface* surf, float angle, int zoom, int offset,
                           bool optimize) {
    return rotate_surface(surf, (double)angle, double(zoom)/(double)offset,
                          surf->w/2, surf->h/2, SDL_UTILS_ROTATE_CROP, optimize);
}

SDL_Surface rotate_180_surface(const SDL_Surface* surf, bool optimize) {
    return rotate_surface(surf, 180.0, 1.0, surf->w/2, surf->h/2,
                          SDL_UTILS_ROTATE_CROP, optimize);
}

SDL_Surface rotate_90_surface(const SDL_Surface* surf, bool clockwise, bool optimize) {
    return rotate_surface(surf, (clockwise ? -90.0 : 90.0), 1.0, surf->w/2, surf->h/2,
                          SDL_UTILS_ROTATE_CROP, optimize);
}


/** TODO FINISHME */
SDL_Surface mirror_surface(const SDL_Surface* surf,
                       const mirror_axis_t axis,
                       const bool optimize,
                       const double axis_x1,
                       const double axis_x2,
                       const double axis_y1,
                       const double axis_y2) {
    if (surf == nullptr) { throw std::invalid_argument("Cannot mirror null surface."); }
    // Make sure both surfaces are always in the "neutral" pixel format

    SDL_Surface dst(create_neutral_surface(surf->w, surf->h));
    SDL_Surface src(make_neutral_surface(surf));
    if (src == nullptr || dst == nullptr) {
        throw std::logic_error("Could not create neutral surfaces. This is a BUG.");
    }
    { //Scoping for SDL_Surface lock.
        using namespace cv;
        Mat src_mat = Mat(src->h, src->w, CV_8UC4, src->pixels, Mat::AUTO_STEP);
        Mat dst_mat = Mat(dst->h, dst->w, CV_8UC4, dst->pixels, Mat::AUTO_STEP);
        Mat aff_mat;
        Point2f src_pts[3];
        Point2f dst_pts[3];
        switch(axis) {
            case VERT:
                flip(src_mat, dst_mat, 1);
                break;
            case HORIZ:
                flip(src_mat, dst_mat, 0);
                break;
            case NE_SW_DIAG:
                src_pts[0] = Point2f((float)src->w,       (float)src->h);
                dst_pts[0] = Point2f(0.0f,                0.0f);
                src_pts[0] = Point2f((float)(src->w-0.5), (float)src->h);
                dst_pts[0] = Point2f(0.0f,                0.5f);
                src_pts[0] = Point2f((float)src->w,       (float)(src->h-0.5f));
                dst_pts[0] = Point2f(0.5f,                0.0f);
                aff_mat = getAffineTransform(src_pts, dst_pts);
                warpAffine(src_mat, dst_mat, aff_mat, dst_mat.size());
                break;
            case SE_NW_DIAG:
                /// TODO: Implement me.
                break;
            case CUSTOM:
                /// TODO: Implement me.
                break;
            default:
                throw std::logic_error("Should never get here.");
        }
    }
    return (optimize ? create_optimized_surface(dst) : dst);
}

void put_pixel(const SDL_Surface* surf, surface_lock& surf_lock, int x, int y, Uint32 pixel) {
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

Uint32 get_pixel(const SDL_Surface* surf, const const_surface_lock& surf_lock, int x, int y) {
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


SDL_Surface flip_surface(const SDL_Surface* surf, bool optimize) {
    return mirror_surface(surf, VERT, optimize);
}


SDL_Surface flop_surface(const SDL_Surface* surf, bool optimize) {
    return mirror_surface(surf, HORIZ, optimize);
}

SDL_Surface create_compatible_surface(const SDL_Surface* surf, int width, int height) {
    if (surf == nullptr) {
        throw std::invalid_argument("Cannot create compatible SDL_Surface given null.");
    }
    if (width == -1) width = surf->w;
    if (height == -1) height = surf->h;

    SDL_Surface s = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, surf->format->BitsPerPixel,
                                     surf->format->Rmask, surf->format->Gmask,
                                     surf->format->Bmask, surf->format->Amask);
    if (surf->format->palette) {
        SDL_SetPalette(s, SDL_LOGPAL, surf->format->palette->colors, 0,
                       surf->format->palette->ncolors);
    }
    return s;
}


SDL_Surface get_surface_portion(const SDL_Surface* src, SDL_Rect* area, bool optimize_format) {
    if (src == nullptr) { return nullptr; }

    // Check if there is something in the portion
    if (area.x >= src->w || area.y >= src->h || area.x + area.w < 0 || area.y + area.h < 0) {
        return nullptr;
    }

    if (area.x + area.w > src->w) {
        area.w = src->w - area.x;
    }
    if (area.y + area.h > src->h) {
        area.h = src->h - area.y;
    }

    // use same format as the source (almost always the screen)
    SDL_Surface dst = create_compatible_surface(src, area.w, area.h);

    if (dst == nullptr) {
        std::cerr << "Could not create a new SDL_Surface in get_surface_portion()\n";
        return nullptr;
    }

    sdl_blit(src, &area, dst, nullptr);

    return optimize_format ? display_format_alpha(dst) : dst;
}

SDL_Rect get_non_transparent_portion(const SDL_Surface* surf) {
    SDL_Rect res = { 0, 0, 0, 0 };
    /** TODO WRITEME
     * Figure out what the purpose of this is, then rewrite it. */
    return res;
}

void draw_centered_on_background(SDL_Surface surf,
                                 const SDL_Rect* rect,
                                 const SDL_Color* color,
                                 SDL_Surface target) {
    SDL_

    clip_rect_setter clip_setter(target, &rect);

    Uint32 col = SDL_MapRGBA(target->format, color.r, color.g, color.b, color.unused);
    //TODO: only draw background outside the image
    SDL_Rect r = rect;
    sdl_fill_rect(target, &r, col);

    if (surf != nullptr) {
        r.x = rect.x + (rect.w - surf->w) / 2;
        r.y = rect.y + (rect.h - surf->h) / 2;
        sdl_blit(surf, nullptr, target, &r);
    }
    update_rect(rect);
}

std::ostream& operator<<(std::ostream& s, const SDL_Rect* rect) {
    s << rect.x << ',' << rect.y << " x " << rect.w << ',' << rect.h;
    return s;
}

// end Image Utilities ------------------------------------------------------------------------
