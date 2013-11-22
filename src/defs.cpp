/**
 * @file 		defs.cpp is part of Project sdl2_test
 * @author		justin
 * @date		Nov 15, 2013
 * @copyright   Copyright justin, Nov 15, 2013
 * @section		<license> License
 * > sdl2_test is free software: you can redistribute it and/or modify
 * > it under the terms of the GNU General Public License as published by
 * > the Free Software Foundation, either version 3 of the License, or
 * > (at your option) any later version.
 * >
 * > sdl2_test is distributed in the hope that it will be useful,
 * > but WITHOUT ANY WARRANTY; without even the implied warranty of
 * > MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * > GNU General Public License for more details.
 * >
 * > You should have received a copy of the GNU General Public License
 * > along with sdl2_test.  If not, see <a href="http://www.gnu.org/licenses/">
 * > http://www.gnu.org/licenses/</a>.
 * @brief		TODO WRITEME
 * @details		TODO WRITEME
 */

#include "defs.hpp"
#include <emmintrin.h>

long long get_vec_hash(std::vector<Uint32>& vec) {
    long long digest;
    gcry_md_hash_buffer(GCRY_MD_MD5, (void*)&digest, (void*)vec.data(), vec.size());
    return digest;
}

long long get_vec_hash(std::vector<Uint8>& vec) {
    long long digest;
    gcry_md_hash_buffer(GCRY_MD_MD5, (void*)&digest, (void*)vec.data(), vec.size());
    return digest;
}

/** @todo Modify this to use OpenMP/Cilk+/OpenACC/magic. */
std::vector<Uint8> event2vec(SDL_Event* evt) {
    std::vector<Uint8> vec;
    for (Uint32 i = 0; i < sizeof(SDL_Event); i++) {
        vec.push_back( ((Uint8*)evt)[i] );
    }
    vec.shrink_to_fit();
    return vec;
}

/** @todo Modify this to use OpenMP/Cilk+/OpenACC/magic. */
SDL_Event vec2event(std::vector<Uint8> vec) {
    SDL_Event evt;
    for (Uint32 i = 0; i < sizeof(SDL_Event); i++) {
        ((Uint8*)&evt)[i] = vec[i];
    }
    return evt;
}

/** @todo Modify this to use OpenMP/Cilk+/OpenACC/magic. */
std::vector<Uint32> blitparams2vec(SDL_Surface*    src_surf,
                                   const SDL_Rect* src_rect,
                                   SDL_Rect*       dst_rect) {
    std::vector<Uint32> bv;
    bv.push_back(src_surf->w);  // 0
    bv.push_back(src_surf->h);  // 1
    bv.push_back(src_rect->x);  // 2
    bv.push_back(src_rect->y);  // 3
    bv.push_back(src_rect->w);  // 4
    bv.push_back(src_rect->h);  // 5
    bv.push_back(dst_rect->x);  // 6
    bv.push_back(dst_rect->y);  // 7
    bv.push_back(dst_rect->w);  // 8
    bv.push_back(dst_rect->h);  // 9
    SDL_LockSurface(src_surf);
    for (Uint64 i = 0; i < (Uint64)(src_surf->h * src_surf->w); i++) {
        bv.push_back(((Uint32*)(src_surf->pixels))[i]);  //10 -> x*y+10
    };
    SDL_UnlockSurface(src_surf);
    bv.shrink_to_fit();
    return bv;
}

/** @todo Modify this to use OpenMP/Cilk+/OpenACC/magic. */
std::tuple<SDL_Surface, SDL_Rect, SDL_Rect> vec2blitparams(std::vector<Uint32> bv) {
    SDL_Rect sr, dr;
    Uint32 ssw = bv[0];
    Uint32 ssh = bv[1];
    sr.x = bv[2];
    sr.y = bv[3];
    sr.w = bv[4];
    sr.h = bv[5];
    dr.x = bv[6];
    dr.y = bv[7];
    dr.w = bv[8];
    dr.h = bv[9];
    Uint32 ssp[ssw*ssh];
    for (Uint64 i = 0; i < (ssw*ssh); i++){
        ssp[i] = bv[i+10];
    }
    SDL_Surface* ss = SDL_CreateRGBSurfaceFrom(ssp, ssw, ssh, 32, 32, RMASK, GMASK, BMASK, AMASK);
    return std::tuple<SDL_Surface, SDL_Rect, SDL_Rect>(*ss, sr, dr);
}

