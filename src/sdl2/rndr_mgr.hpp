/**
 * @file 		rndr_mgr.hpp is part of wesnoth-ng
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

#ifndef rndr_mgr_HPP_
#define rndr_mgr_HPP_

#include <SDL2/SDL.h>
#include <atomic>
#include <exception>
#include <mutex>
#include <memory>
#include <string>

/** @class rndr_mgr TODO: WRITEME */
class rndr_mgr {
    public:
        static rndr_mgr& get_rndr_mgr() {
            static rndr_mgr instance_;
            return instance_;
        }

        Uint32 get_window_flags() { return SDL_GetWindowFlags(wind_); }

        SDL_Surface* get_window_surface() { return SDL_GetWindowSurface(wind_); }

        std::pair<int, int>& get_window_size();

        bool is_fullscreen() const {
            return ((SDL_GetWindowFlags(wind_) & SDL_WINDOW_FULLSCREEN) ||
                    (SDL_GetWindowFlags(wind_) & SDL_WINDOW_FULLSCREEN_DESKTOP));
        }

        /**@brief   function to stop the screen being redrawn.
         * @details Anything that happens while the update is locked will be hidden
         * from the user's view.
         * @note that this function is re-entrant, meaning that if lock_updates()
         * is called twice, unlock_updates() must be called twice to unlock updates. */
        void lock_updates() { screen_locked_ += 1; }

        /** @brief function to start the screen being redrawn. Anything that happens
         * while the update is locked will be hidden from the user's view.
         * @note that this function is re-entrant, meaning that if lock_updates(true)
         * is called twice, lock_updates(false) must be called twice to unlock updates. */
        void unlock_updates() {
            (screen_locked_ >= 1) ? screen_locked_ -= 1 : screen_locked_ = 0;
        }

        /** @brief function to stop the screen being redrawn. Anything that happens
         * while the update is locked will be hidden from the user's view.
         * @note that this function is re-entrant, meaning that if lock_updates(true)
         * is called twice, lock_updates(false) must be called twice to unlock updates. */
        bool update_locked() const { return (screen_locked_ >= 1); }

        void lock_resize() { resize_locked_ += 1; }
        void unlock_resize() {
            (resize_locked_ >= 1) ? resize_locked_ -= 1 : resize_locked_ = 0;
        }
        bool resize_locked() const { return (screen_locked_ >=1 ); }

        void blit_surf(SDL_Surface* surf, SDL_Rect* src_rect, SDL_Rect* dst_rect);

        void render();

    private:
        std::pair<int, int> win_size_ {0,0};
        SDL_DisplayMode disp_mode_;
        Uint32 pixfmt_ {0};
        SDL_Window* wind_ {nullptr};
        SDL_Renderer* rend_ {nullptr};
        SDL_Surface* surf_ {nullptr};
        SDL_Texture* txtr_ {nullptr};

        std::atomic<int> screen_locked_ {0};
        std::atomic<int> resize_locked_ {0};

        rndr_mgr();
        rndr_mgr(const rndr_mgr& rs) = delete;
        rndr_mgr operator=(const rndr_mgr& rs) = delete;
        ~rndr_mgr();
};

#endif /* rndr_mgr_HPP_ */
