/**
 * @file 		img.cpp is part of wesnoth-ng
 * @author		justin
 * @date		Dec 5, 2013
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

#include <img.hpp>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <stdexcept>

img::img(std::vector<Uint8> &data, const Uint32 h, const Uint32 w,
         color_space_t space, pixel_order_t order) {
    h_ = h; w_ = w;
    space_ = space;
    order_ = order;
    for (auto p: data) {
        fpix_.push_back((float)p / 255.0f);
    }
}

img::img(Uint8* data, const Uint32 h, const Uint32 w,
         color_space_t space, pixel_order_t order){
    h_ = h; w_ = w;
    space_ = space;
    order_ = order;
    for (Uint32 i = 0; i < h_ * w_ * 4; i++) {
        fpix_.push_back((float)data[i] / 255.0f);
    }
}

img::img(std::vector<float> &data, const Uint32 h, const Uint32 w,
         color_space_t space, pixel_order_t order){
    h_ = h; w_ = w;
    space_ = space;
    order_ = order;
    for (auto p: data) {
        fpix_.push_back(p);
    }
}

img::img(float* data, const Uint32 h, const Uint32 w,
         color_space_t space, pixel_order_t order){
    h_ = h; w_ = w;
    space_ = space;
    order_ = order;
    for (Uint32 i = 0; i < h_ * w_ * 4; i++) {
        fpix_.push_back(data[i]);
    }
}

img::~img() {
}

void img::to_surf(SDL_Surface* surf) {
    if (surf->h != h_ ||
        surf->w != w_ ||
        surf->format->BitsPerPixel != 32) {
        throw std::invalid_argument("Inappropriate surface provided.");
    }
    SDL_LockSurface(surf);
    float val;
    for (Uint32 i = 0; i < h_ * w_ * 4; i++) {
        val = fpix_[i] * 255.0f;
        if (val < 0.0f) { val = 0.0f; } else if (val > 255.0f) { val = 255.0f; }
        ((Uint8*)surf->pixels)[i] = (Uint8)val;
    }
}
