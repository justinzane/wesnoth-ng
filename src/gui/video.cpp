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
 *  Video-testprogram, standalone
 */

#include "video.hpp"

#include "font.hpp"
#include "sdl_utils.hpp"

#include "../image.hpp"
#include "../log.hpp"
#include "../preferences.hpp"
#include "../preferences_display.hpp"

#include <boost/foreach.hpp>
#include <boost/type_traits/is_const.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>

#include <stddef.h>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

static lg::log_domain log_display("display");
#define LOG_DP LOG_STREAM(info, log_display)
#define ERR_DP LOG_STREAM(err, log_display)

namespace {
bool fullScreen = false;
int disallow_resize = 0;
}

namespace {
struct event {
        int x, y, w, h;
        bool in;
        event(const SDL_Rect& rect, bool i) :
            x(i ? rect.x : rect.x + rect.w), y(rect.y), w(rect.w), h(rect.h), in(i) {
        }
};

bool operator<(const event& a, const event& b) {
    if (a.x != b.x) return a.x < b.x;
    if (a.in != b.in) return a.in;
    if (a.y != b.y) return a.y < b.y;
    if (a.h != b.h) return a.h < b.h;
    if (a.w != b.w) return a.w < b.w;
    return false;
}

bool operator==(const event& a, const event& b) {
    return a.x == b.x && a.y == b.y && a.w == b.w && a.h == b.h && a.in == b.in;
}

struct segment {
        int x, count;
        segment() :
            x(0), count(0) {
        }
        segment(int x, int count) :
            x(x), count(count) {
        }
};

std::vector<SDL_Rect> update_rects;
std::vector<event> events;
std::map<int, segment> segments;

static void calc_rects() {
    events.clear();

    BOOST_FOREACH(SDL_Rect const &rect, update_rects) {
        events.push_back(event(rect, true));
        events.push_back(event(rect, false));
    }

    std::sort(events.begin(), events.end());
    std::vector<event>::iterator events_end = std::unique(events.begin(), events.end());

    segments.clear();
    update_rects.clear();

    for (std::vector<event>::iterator iter = events.begin(); iter != events_end; ++iter) {
        std::map<int, segment>::iterator lower = segments.find(iter->y);
        if (lower == segments.end()) {
            lower = segments.insert(std::make_pair(iter->y, segment())).first;
            if (lower != segments.begin()) {
                std::map<int, segment>::iterator prev = lower;
                --prev;
                lower->second = prev->second;
            }
        }

        if (lower->second.count == 0) {
            lower->second.x = iter->x;
        }

        std::map<int, segment>::iterator upper = segments.find(iter->y + iter->h);
        if (upper == segments.end()) {
            upper = segments.insert(std::make_pair(iter->y + iter->h, segment())).first;
            std::map<int, segment>::iterator prev = upper;
            --prev;
            upper->second = prev->second;
        }

        if (iter->in) {
            while (lower != upper) {
                ++lower->second.count;
                ++lower;
            }
        } else {
            while (lower != upper) {
                lower->second.count--;
                if (lower->second.count == 0) {
                    std::map<int, segment>::iterator next = lower;
                    ++next;

                    int x = lower->second.x, y = lower->first;
                    unsigned w = iter->x - x;
                    unsigned h = next->first - y;
                    SDL_Rect a = create_rect(x, y, w, h);

                    if (update_rects.empty()) {
                        update_rects.push_back(a);
                    } else {
                        SDL_Rect& p = update_rects.back(), n;
                        int pa = p.w * p.h, aa = w * h, s = pa + aa;
                        int thresh = 51;

                        n.w = std::max<int>(x + w, p.x + p.w);
                        n.x = std::min<int>(p.x, x);
                        n.w -= n.x;
                        n.h = std::max<int>(y + h, p.y + p.h);
                        n.y = std::min<int>(p.y, y);
                        n.h -= n.y;

                        if (s * 100 < thresh * n.w * n.h) {
                            update_rects.push_back(a);
                        } else {
                            p = n;
                        }
                    }

                    if (lower == segments.begin()) {
                        segments.erase(lower);
                    } else {
                        std::map<int, segment>::iterator prev = lower;
                        --prev;
                        if (prev->second.count == 0) segments.erase(lower);
                    }

                    lower = next;
                } else {
                    ++lower;
                }
            }
        }
    }
}

bool update_all = false;
}

static void clear_updates() {
    update_all = false;
    update_rects.clear();
}

//namespace {
//surface ui_surface_ = NULL;
//bool fake_interactive = false;
//}

bool non_interactive() {
    return false;
    /// @todo FIXME
//    if (fake_interactive) return false;
//    return SDL_GetVideoSurface() == NULL;
}

surface display_format_alpha(surface surf) {
    return surf;
    /// @todo FIXME
//    if (SDL_GetVideoSurface() != NULL)
//        return SDL_DisplayFormatAlpha(surf);
//    else if (ui_surface_ != NULL)
//        return SDL_ConvertSurface(surf, ui_surface_->format, 0);
//    else
//        return NULL;
}

void update_rect(const SDL_Rect& rect_value) {
    if (update_all) { return; }

    SDL_Rect rect = rect_value;

    if (rect.x < 0) {
        if (rect.x * -1 >= int(rect.w)) return;

        rect.w += rect.x;
        rect.x = 0;
    }

    if (rect.y < 0) {
        if (rect.y * -1 >= int(rect.h)) return;

        rect.h += rect.y;
        rect.y = 0;
    }

    if (rect.x + rect.w > ui_window::ui_surface_->w) {
        rect.w = ui_window::ui_surface_->w - rect.x;
    }

    if (rect.y + rect.h > ui_window::ui_surface_->h) {
        rect.h = ui_window::ui_surface_->h - rect.y;
    }

    if (rect.x >= ui_window::ui_surface_->w) {
        return;
    }

    if (rect.y >= ui_window::ui_surface_->h) {
        return;
    }

    update_rects.push_back(rect);
}

void update_whole_screen() {
    update_all = true;
}

ui_window::ui_window() :
    help_string_(0),
    updates_locked_(false),
    resizes_locked_(false) {
    if (SDL_CreateWindowAndRenderer(800,    /// @todo get me from preferences
                                    600,    /// @todo get me from preferences
                                    (SDL_WINDOW_MAXIMIZED &
                                     SDL_WINDOW_ALLOW_HIGHDPI &
                                     SDL_WINDOW_RESIZABLE), /// @todo get me from preferences
                                    &ui_window_,
                                    &ui_renderer_) == -1) {
        ERR_DP<< "Could not initialize SDL_video: " << SDL_GetError() << "\n";
        throw std::logic_error("Could not initialize SDL_Window.");
    }
    ui_surface_ = SDL_GetWindowSurface(ui_window_);
    ui_texture_ = SDL_CreateTexture(ui_renderer_,
                                    SDL_PIXELFORMAT_ARGB8888,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    ui_surface_->w,
                                    ui_surface_->h);
}

ui_window::~ui_window() {
    LOG_DP<< "calling SDL_Quit()\n";
    SDL_Quit();
    LOG_DP << "called SDL_Quit()\n";
}

void ui_window::blit_surface(int x, int y, surface surf, SDL_Rect* srcrect, SDL_Rect* clip_rect) {
    surface target(getSurface());
    SDL_Rect dst = create_rect(x, y, 0, 0);

    const clip_rect_setter clip_setter(target, clip_rect, clip_rect != NULL);
    sdl_blit(surf, srcrect, target, &dst);
}

bool ui_window::isFullScreen() const {
    return (SDL_GetWindowFlags(ui_window_) && SDL_WINDOW_FULLSCREEN);
}

void ui_window::flip() {
//    if (update_all) {
//        /// @todo need to change to textures
//        //::SDL_Flip(ui_surface_);
////	    SDL_UpdateTexture(sdlTexture, NULL, screen->pixels, screen->pitch);
////	    SDL_RenderClear(sdlRenderer);
////	    SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
////	    SDL_RenderPresent(sdlRenderer);
//    } else if (update_rects.empty() == false) {
//        calc_rects();
//        if ( !update_rects.empty()) {
//            SDL_UpdateRects(ui_surface_, update_rects.size(), &update_rects[0]);
//        }
//    }
//
//    clear_updates();
}

surface ui_window::getSurface() {
    return surface(ui_surface_);
}

surface ui_window::get_surface() {
    return surface(ui_surface_);
}

/// @todo reenable and fix these once SDL2 basically works.
/// @todo move these into a status_bar.hpp or similar. They DO NOT belon in ui_window.
//int ui_window::set_help_string(const std::string& str) {
//    font::remove_floating_label(help_string_);
//
//    const SDL_Color color = { 0, 0, 0, 0xbb };
//
//    int size = font::SIZE_LARGE;
//
//    while (size > 0) {
//        if (font::line_width(str, size) > getx()) {
//            size--;
//        } else {
//            break;
//        }
//    }
//
//    const int border = 5;
//
//    font::floating_label flabel(str);
//    flabel.set_font_size(size);
//    flabel.set_position(getx() / 2, gety());
//    flabel.set_bg_color(color);
//    flabel.set_border_size(border);
//
//    help_string_ = font::add_floating_label(flabel);
//
//    const SDL_Rect& rect = font::get_floating_label_rect(help_string_);
//    font::move_floating_label(help_string_, 0.0, -double(rect.h));
//    return help_string_;
//}
//
//void ui_window::clear_help_string(int handle) {
//    if (handle == help_string_) {
//        font::remove_floating_label(handle);
//        help_string_ = 0;
//    }
//}
//
//void ui_window::clear_all_help_strings() {
//    clear_help_string(help_string_);
//}

void resize_monitor::process(events::pump_info &info) {
    if (info.resize_dimensions.first  >= preferences::min_allowed_width() &&
        info.resize_dimensions.second >= preferences::min_allowed_height() &&
        disallow_resize == 0) {
        preferences::set_resolution(info.resize_dimensions);
    }
}
