/**
 * @file 		sdl2_rndr_mgr.cpp is part of wesnoth-ng
 * @author		Justin Zane Chudgar, <justin@justinzane.com>
 * @date		Dec 6, 2013
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

#include <sdl2/sdl2_rndr_mgr.hpp>

sdl2_rndr_mgr::sdl2_rndr_mgr() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0 ) { throw; }
    if (SDL_GetNumVideoDisplays() < 1) {
        fprintf(stderr, "Num displays:  %d\n", SDL_GetNumVideoDisplays()); throw;
    }
    if (SDL_GetDesktopDisplayMode(0, &disp_mode_) != 0) {
        fprintf(stderr, "\nUnable to get display into:  %s\n", SDL_GetError()); throw;
    }
    pixfmt_ = disp_mode_.format;
    win_size_.first = disp_mode_.w;
    win_size_.second = disp_mode_.h;

    wind_ = SDL_CreateWindow("WesnothNG", 0, 0, 0, 0,
                             SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_OPENGL);
    rend_ = SDL_CreateRenderer(wind_, 0, 0);
    surf_ = SDL_GetWindowSurface(wind_);
    get_window_size();
    txtr_ = SDL_CreateTexture(rend_, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                              win_size_.first, win_size_.second);
}

std::pair<int, int>& sdl2_rndr_mgr::get_window_size() {
    SDL_GetWindowSize(wind_, &(win_size_.first), &(win_size_.second));
    return win_size_;
}

void sdl2_rndr_mgr::blit_surf(SDL_Surface* surf, SDL_Rect* src_rect, SDL_Rect* dst_rect) {
    SDL_BlitSurface(surf, src_rect, surf_, dst_rect);
    render();
}

void sdl2_rndr_mgr::render() {
    SDL_UpdateTexture(txtr_, nullptr, surf_->pixels, surf_->pitch);
    SDL_RenderClear(rend_);
    SDL_RenderCopy(rend_, txtr_, nullptr, nullptr);
    SDL_RenderPresent(rend_);
}
sdl2_rndr_mgr::~sdl2_rndr_mgr() {
    SDL_Quit();
}

