/**
 * @file 		img.hpp is part of wesnoth-ng
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
#ifndef IMG_HPP_
#define IMG_HPP_

#include <SDL2/SDL.h>
#include <vector>
#include <stdlib.h>
#include <exception>

/** @defgroup CnE Constants and Enums ********************************************************/

/**@ingroup CnE
 * @brief Supported Color-Spaces. */
typedef enum class color_space_t {
        RGBA,   /**< Required for IO with SDL2 */
//        HSVA,
//        HSLA,
//        YUVA,
        XYZA,   /**< Intermediate for conversion CIE L*a*b*A to/from RGBA */
        LABA,   /**< CIE L*a*b* space */
        BOGUS   /**< Booooogus! */
} color_space;

/**@ingroup CnE
 * @brief Supported Channel Orders within 32 Bit Unsigned Int Pixels. */
typedef enum class pixel_order_t {
        ABGR,//!< ABGR  0xAABBGGRR
        ARGB,//!< ARGB  0xAARRGGBB
        BGRA,//!< BGRA  0xBBGGRRAA  (Default for SDL2 on little-endian machines.)
        RGBA,//!< RGBA  0xRRGGBBAA  (Most common order in end-user apps and references.)
        BOGUS//!< BOGUS Booooogus!
} pixel_order;

/**@ingroup CnE
 * @brief   Struct of constants for SDL2 Surfaces with ABGR order.*/
struct ABGR_consts {
    public:
        const pixel_order_t order {pixel_order_t::ABGR};
        const Uint32 amask {0xff000000};
        const Uint32 bmask {0x00ff0000};
        const Uint32 gmask {0x0000ff00};
        const Uint32 rmask {0x000000ff};
        const Uint32 ashift {24};
        const Uint32 bshift {16};
        const Uint32 gshift {8};
        const Uint32 rshift {0};
        const Uint8 bpp {32};
};

/**@ingroup CnE
 * @brief   Struct of constants for SDL2 Surfaces with ABGR order.*/
struct ARGB_consts {
    public:
        const pixel_order_t order {pixel_order_t::ABGR};
        const Uint32 amask {0xff000000};
        const Uint32 rmask {0x00ff0000};
        const Uint32 gmask {0x0000ff00};
        const Uint32 bmask {0x000000ff};
        const Uint32 ashift {24};
        const Uint32 rshift {16};
        const Uint32 gshift {8};
        const Uint32 bshift {0};
        const Uint8 bpp {32};
};

/**@ingroup CnE
 * @brief   Struct of constants for SDL2 Surfaces with ABGR order.*/
struct BGRA_consts {
    public:
        const pixel_order_t order {pixel_order_t::ABGR};
        const Uint32 bmask {0xff000000};
        const Uint32 gmask {0x00ff0000};
        const Uint32 rmask {0x0000ff00};
        const Uint32 amask {0x000000ff};
        const Uint32 bshift {24};
        const Uint32 gshift {16};
        const Uint32 rshift {8};
        const Uint32 ashift {0};
        const Uint8 bpp {32};
};

/**@ingroup CnE
 * @brief   Struct of constants for SDL2 Surfaces with ABGR order.*/
struct RGBA_consts {
    public:
        static const pixel_order_t order {pixel_order_t::ABGR};
        static const Uint32 rmask {0xff000000};
        static const Uint32 gmask {0x00ff0000};
        static const Uint32 bmask {0x0000ff00};
        static const Uint32 amask {0x000000ff};
        static const Uint32 rshift {24};
        static const Uint32 gshift {16};
        static const Uint32 bshift {8};
        static const Uint32 ashift {0};
        static const Uint8 bpp {32};
};

/** @class img @brief Simple container for image data. ****************************************/
class img {
    public:
        /**
         * @brief constructor
         * @param data      Pixel data
         * @param h         Image height
         * @param w         Image width
         * @param space     Image color space
         * @param order     Image channel order
         */
        img(std::vector<Uint8> &data,
            const Uint32 h,
            const Uint32 w,
            color_space_t space,
            pixel_order_t order
        );

        /**
         * @brief constructor
         * @param data      Pixel data
         * @param h         Image height
         * @param w         Image width
         * @param space     Image color space
         * @param order     Image channel order
         */
        img(Uint8* data,
            const Uint32 h,
            const Uint32 w,
            color_space_t space,
            pixel_order_t order
        );

        /**
         * @brief constructor
         * @param data      Pixel data
         * @param h         Image height
         * @param w         Image width
         * @param space     Image color space
         * @param order     Image channel order
         */
        img(std::vector<float> &data,
            const Uint32 h,
            const Uint32 w,
            color_space_t space,
            pixel_order_t order
        );

        /**
         * @brief constructor
         * @param data      Pixel data
         * @param h         Image height
         * @param w         Image width
         * @param space     Image color space
         * @param order     Image channel order
         */
        img(float* data,
            const Uint32 h,
            const Uint32 w,
            color_space_t space,
            pixel_order_t order
        );

        void to_surf(SDL_Surface* surf);

        virtual ~img();

    private:
        color_space_t space_ {color_space::BOGUS};
        pixel_order_t order_ {pixel_order::BOGUS};
        std::vector<float> fpix_ {};
        Uint32 h_ {0};
        Uint32 w_ {0};
};

/** @defgroup SRF SDL2 Surface Related Functions **********************************************/

/**@ingroup SRF
 * @brief Determines the channel order for subpixels; i.e. RGBA, ABGR, etc. */
pixel_order get_surf_order(SDL_Surface* surf);

/**@ingroup SRF
 * @brief Change the channel order of subpixel data within a surface. */
void surf_remap(SDL_Surface* src, SDL_Surface* dst);

/**@ingroup SRF
 * @brief  Fill the surf with the data from pixar.
 * @details Denormalizes the floating point data to the 0-255 range. */
void pixar2surf(float* pixar, SDL_Surface* surf);

/**@ingroup SRF
 * @brief   Fill pixar with the data from surf.
 * @details frees the contents of pixar, then normalizes the 0-255 data to the 0.0-1.0 range. */
void surf2pixar(SDL_Surface* surf, float* pixar);

void surf_change_nothing(SDL_Surface* surf, float bogus __attribute__((unused)));

/**@ingroup SRF
 * @brief   Change the hue of the surface by -180.0 to 180.0 degrees.
 * @details Red + 120deg = Green, Red - 120deg = Blue, etc.
 * @note    This conversion is done in CIEL*a*b* space. */
void surf_change_hue(SDL_Surface* surf, float hue_adj_deg);

/**@ingroup SRF
 * @brief   Change the saturation of the surface by -100.0% to 100% degrees.
 * @details No saturation is greyscale, full saturation is cartoon vivid.
 * @note    This conversion is done in CIEL*a*b* space. */
void surf_change_sat(SDL_Surface* surf, float sat_adj_pct);

/**@ingroup SRF
 * @brief   Change the brightness of the surface by -100.0% to 100% degrees.
 * @details No brightness is black, full brightness is white.
 * @note    This conversion is done in CIEL*a*b* space. */
void surf_change_lum(SDL_Surface* surf, float lum_adj_pct);

/**@ingroup SRF
 * @brief   The absolute difference, per pixel per channel between surfaces in 32 bpp format.
 * @details This also takes the difference between alphas, so this is likely **not** what
 * you want since two opaque pixels will become transparent. */
void surf_diff_rgba(SDL_Surface* a, SDL_Surface* b, SDL_Surface* diff);

/**@ingroup SRF
 * @brief   The absolute difference, per pixel per RGB channel between surfaces in 32 bpp format.
 * @details This function **averages** the two alpha values per pixel. */
void surf_diff_rgbx(SDL_Surface* a, SDL_Surface* b, SDL_Surface* diff);

/**@ingroup SRF
 * @brief   True if BGRA. */
bool is_surf_bgra(SDL_Surface* surf);

/**@ingroup SRF
 * @brief   True if RGBA. */
bool is_surf_rgba(SDL_Surface* surf);

/**@ingroup SRF
 * @brief   True if ARGB. */
bool is_surf_argb(SDL_Surface* surf);

/**@ingroup SRF
 * @brief   True if ABGR. */
bool is_surf_abgr(SDL_Surface* surf);

/** @defgroup CLR_def Color Definitions *******************************************************/

/**@ingroup CLR_def
 * @brief Matrix for conversion of CIEXYZ to RGB */
const float XYZ2RGB_MAT[3][3] =
    { {  0.41847f  , -0.091169f ,  0.0009209f},
      { -0.15866f  ,  0.25243f  , -0.0025498f},
      { -0.082835f ,  0.015708f ,  0.1786f   }};

/**@ingroup CLR_def
 * @brief Matrix for conversion of RGB to CIEXYZ */
const float RGB2XYZ_MAT[3][3]
    { { 2.76883088f,  1.0f       ,  0.0f       },
      { 1.75170933f,  4.59060858f,  0.05650675f},
      { 1.13013505f,  0.06006668f,  5.5941685f } };

/**@ingroup CLR_def
 * @brief CIE X tristimulus value at D65. */
const float X_d65 { 95.047f};

/**@ingroup CLR_def
 * @brief CIE Y tristimulus value at D65. */
const float Y_d65 {100.000f};

/**@ingroup CLR_def
 * @brief CIE Z tristimulus value at D65. */
const float Z_d65 {108.883f};

/** @defgroup CLR_func Color Functions ********************************************************/

/**@ingroup CLR_func
 * @brief Change the hue of a surface by hue_adj_rad radians, considering hue to
 *  be circular from 0.0 rad = red to 2pi/3 rad = green to 4pi/3 rad = blue. */
void hsla_change_hue(float* pixar, float hue_adj_rad);

/**@ingroup CLR_func
 * @brief Change the saturation of a surface by sat_adj, considering sat to
 *  be from 0.0 = greyscale to 1.0 = vivid. */
void hsla_change_sat(float* pixar, float sat_adj);

/**@ingroup CLR_func
 * @brief Change the brightness of a surface by lum_adj degrees, considering lum to
 *  be from 0.0 = black to 1.0 = white. */
void hsla_change_lum(float* pixar, float lum_adj);

/**@ingroup CLR_func
 * @brief Change the hue of a surface by hue_adj_rad radians, considering hue to
 *  be circular from 0.0 rad = red to 2pi/3 rad = green to 4pi/3 rad = blue. */
void laba_change_hue(float* pixar, float hue_adj_rad);

/**@ingroup CLR_func
 * @brief Change the saturation of a surface by sat_adj, considering sat to
 *  be from 0.0 = greyscale to 1.0 = vivid. */
void laba_change_sat(float* pixar, float sat_adj);

/**@ingroup CLR_func
 * @brief Change the brightness of a surface by lum_adj degrees, considering lum to
 *  be from 0.0 = black to 1.0 = white. */
void laba_change_lum(float* pixar, float lum_adj);

/**@ingroup CLR_func
 * @brief converts an RGBA pixel into an HSLA pixel. */
void hsla2rgba(float* pixar);

/**@ingroup CLR_func
 * @brief converts an CIEL*a*b*A pixel into a RGBA pixel. */
void laba2rgba(float* pixar);

/**@ingroup CLR_func
 * @brief converts an CIEL*a*b*A pixel into a CIEXYZA pixel. */
void laba2xyza(float* pixar);

/**@ingroup CLR_func
 * @brief converts an RGBA pixel into a CIEL*a*b*A pixel. */
void rgba2laba(float* pixar);

/**@ingroup CLR_func
 * @brief converts an RGBA pixel into an HSLA pixel. */
void rgba2hsla(float* pixar);

/**@ingroup CLR_func
 * @brief converts an RGBA pixel into a CIEXYZA pixel. */
void rgba2xyza(float* pixar);

/**@ingroup CLR_func
 * @brief converts a CIEXYZA pixel into a CIEL*a*b*A pixel. */
void xyza2laba(float* pixar);

/**@ingroup CLR_func
 * @brief converts a CIEXYZA pixel into an ABGR pixel. */
void xyza2rgba(float* pixar);

#endif /* IMG_HPP_ */
