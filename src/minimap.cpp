/**
 * @file src/minimap.cpp
 * @project The Battle for Wesnoth NG - https://github.com/justinzane/wesnoth-ng
 * @brief 
 * @authors 
 * @copyright Copyright (C) 2003 - 2013 by David White <dave@whitevine.net>
 Part of the Battle for Wesnoth Project http://www.wesnoth.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY.

 See the COPYING file for more details.
 */

#define GETTEXT_DOMAIN "wesnoth-lib"

#include "global.hpp"
#include "minimap.hpp"

#include "gettext.hpp"
#include "image.hpp"
#include "log/log.hpp"
#include "log/log.hpp"
#include "map.hpp"
#include "sdl_utils.hpp"

#include "team.hpp"
#include "serdes/wml_exception.hpp"
#include "formula/formula_string_utils.hpp"
#include "formula/formula_string_utils.hpp"

static lg::log_domain log_display("display");
#define DBG_DP LOG_STREAM(debug, log_display)
#define WRN_DP LOG_STREAM(warn, log_display)

namespace image {

SDL_Surface* getMinimap(int w, int h, const gamemap &map, const team *vw) {
    const int scale = 8;

    DBG_DP<< "creating minimap " << int(map.w()*scale*0.75) << "," << map.h()*scale << "\n";

    const size_t map_width = map.w() * scale * 3 / 4;
    const size_t map_height = map.h() * scale;
    if (map_width == 0 || map_height == 0) {
        throw std::invalid_argument("w * h must be greater than 0.");
    }

    SDL_Surface* minimap = SDL_CreateRGBSurface(0, w, h, RGBA_consts::bpp, RGBA_consts::rmask,
                                                RGBA_consts::gmask, RGBA_consts::bmask,
                                                RGBA_consts::amask);

    typedef mini_terrain_cache_map cache_map;
    cache_map *normal_cache = &mini_terrain_cache;
    cache_map *fog_cache = &mini_fogged_terrain_cache;

    for (int y = 0; y != map.total_height(); ++y) {
        for (int x = 0; x != map.total_width(); ++x) {
            SDL_Surface* surf = SDL_CreateRGBSurface(0, w, h, RGBA_consts::bpp,
                                                     RGBA_consts::rmask, RGBA_consts::gmask,
                                                     RGBA_consts::bmask, RGBA_consts::amask);

            const map_location loc(x, y);
            if (map.on_board(loc)) {

                const bool shrouded = (vw != nullptr && vw->shrouded(loc));
                // shrouded hex are not considered fogged (no need to fog a black image)
                const bool fogged = (vw != nullptr && !shrouded && vw->fogged(loc));
                const t_translation::t_terrain terrain =
                    shrouded ? t_translation::VOID_TERRAIN : map[loc];
                const terrain_type& terrain_info = map.get_terrain_info(terrain);

                bool need_fogging = false;

                cache_map* cache = fogged ? fog_cache : normal_cache;
                cache_map::iterator i = cache->find(terrain);

                if (fogged && i == cache->end()) {
                    // we don't have the fogged version in cache
                    // try the normal cache and ask fogging the image
                    cache = normal_cache;
                    i = cache->find(terrain);
                    need_fogging = true;
                }

                if (i == cache->end()) {
                    std::string base_file = "terrain/" + terrain_info.minimap_image() + ".png";
                    SDL_Surface* tile = get_image(base_file, image::HEXED);

                    //Compose images of base and overlay if necessary
                    // NOTE we also skip overlay when base is missing (to avoid hiding the error)
                    if (tile != nullptr && map.get_terrain_info(terrain).is_combined()) {
                        std::string overlay_file = "terrain/"
                            + terrain_info.minimap_image_overlay() + ".png";
                        SDL_Surface overlay = get_image(overlay_file, image::HEXED);

                        if (overlay != nullptr && overlay != tile) {
                            SDL_Surface combined = create_neutral_surface(tile->w, tile->h);
                            SDL_Rect r = create_rect(0, 0, 0, 0);
                            SDL_BlitSurface(tile, nullptr, combined, &r);
                            r.x = std::max(0, (tile->w - overlay->w) / 2);
                            r.y = std::max(0, (tile->h - overlay->h) / 2);
                            //blit_SDL_Surface needs neutral surface
                            SDL_Surface overlay_neutral = make_neutral_surface(overlay);
                            blit_surface(overlay_neutral, nullptr, combined, &r);
                            tile = combined;
                        }
                    }

                    surf = scale_surface_sharp(tile, scale, scale);

                    i = normal_cache->insert(cache_map::value_type(terrain, surf)).first;
                }

                surf = i->second;

                if (need_fogging) {
                    surf = adjust_surface_color(surf, -50, -50, -50);
                    fog_cache->insert(cache_map::value_type(terrain, surf));
                }

                // we need a balanced shift up and down of the hexes.
                // if not, only the bottom half-hexes are clipped
                // and it looks asymmetrical.

                // also do 1-pixel shift because the scaling
                // function seems to do it with its rounding
                SDL_Rect maprect = create_rect(x * scale * 3 / 4 - 1,
                                               y * scale + scale / 4 * (is_odd(x) ? 1 : -1) - 1,
                                               0, 0);

                if (surf != nullptr)
                SDL_BlitSurface(surf, nullptr, minimap, &maprect);
            }
        }
    }

    double wratio = w * 1.0 / minimap->w;
    double hratio = h * 1.0 / minimap->h;
    double ratio = std::min<double>(wratio, hratio);

    minimap = scale_surface_sharp(minimap, static_cast<int>(minimap->w * ratio),
                                  static_cast<int>(minimap->h * ratio));

    DBG_DP<< "done generating minimap\n";

    return minimap;
}
}
