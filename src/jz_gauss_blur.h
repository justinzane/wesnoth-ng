/*
 * crap.h
 *
 *  Created on: Sep 20, 2013
 *      Author: justin
 */
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <omp.h>

const float DIAG_F = 0.707106781187f;
const float NEXT_F = 1.0f;
const float SELF_F = 4.0f;
const float DIV_F  = 10.8284271247f;

const Uint32 DIAG_I = 5;
const Uint32 NEXT_I = 7;
const Uint32 SELF_I = 20;
const Uint32 DIV_I  = 68;

/**
 * Converts red, green, blue and alpha **float** valued pixmaps to an SDL surface.
 * @note No validation is done to make sure that the pixmaps are the same size and
 * depth as the surface.
 */
void rgba2surf_f(SDL_Surface* surf,   /**< [out]   The surface to be written to. */
                 float* red,          /**< [in]    Red pixels. */
                 float* green,        /**< [in]    Green pixels. */
                 float* blue,         /**< [in]    Blue pixels. */
                 float* alpha)        /**< [in]    Alpha pixels. */
    __attribute__((hot, nonnull));

/**
 * Converts red, green, blue and alpha **UJint32** valued pixmaps to an SDL surface.
 * @note No validation is done to make sure that the pixmaps are the same size and
 * depth as the surface.
 */
void rgba2surf_i(SDL_Surface* surf,    /**< [out]   The surface to be written to. */
                 Uint32* red,          /**< [in]    Red pixels. */
                 Uint32* green,        /**< [in]    Green pixels. */
                 Uint32* blue,         /**< [in]    Blue pixels. */
                 Uint32* alpha)        /**< [in]    Alpha pixels. */
    __attribute__((hot, nonnull));

/**
 * Converts an SDL Surface into separate **float** valued pixmaps for
 * each channel. Expects an 8 bpp RGBA surface.
 */
void surf2rgba_f(SDL_Surface* surf,   /**< [out]   The surface to be written to. */
                 float* red,          /**< [in]    Red pixels. */
                 float* green,        /**< [in]    Red pixels. */
                 float* blue,         /**< [in]    Red pixels. */
                 float* alpha)        /**< [in]    Red pixels. */
    __attribute__((hot, nonnull));

/**
 * Converts an SDL Surface into separate **Uint32** valued pixmaps for
 * each channel. Expects an 8 bpp RGBA surface.
 */
void surf2rgba_i(SDL_Surface* surf,   /**< [out]   The surface to be written to. */
                 Uint32* red,          /**< [in]    Red pixels. */
                 Uint32* green,        /**< [in]    Red pixels. */
                 Uint32* blue,         /**< [in]    Red pixels. */
                 Uint32* alpha)        /**< [in]    Red pixels. */
    __attribute__((hot, nonnull));

/**
 * Performs an iterative box blur approximation to a gaussian blur. This is
 * the **float** version.
 * @note    The height, width and amount are intentionally limited to small
 * values, based on the UHD 2160p standard, since this is intended only for
 * use with SDL surfaces.
 */
void blur_channel_f(Uint32 height,    /**< [in]        pixmap height, 0 < height <= 2160 */
                    Uint32 width,     /**< [in]        pixmap width, 0 < width <= 3840 */
                    Uint32 amount,    /**< [in]        1 <= blur iterations <= 4 */
                    float* pixels)    /**< [in]        pixmap data pointer */
    __attribute__((hot, nonnull));

/**
 * Performs an iterative box blur approximation to a gaussian blur. This is
 * the **Uint32** version.
 * @note    The height, width and amount are intentionally limited to small
 * values, based on the UHD 2160p standard, since this is intended only for
 * use with SDL surfaces.
 */
void blur_channel_i(Uint32 height,    /**< [in]        pixmap height, 0 < height <= 2160 */
                    Uint32 width,     /**< [in]        pixmap width, 0 < width <= 3840 */
                    Uint32 amount,    /**< [in]        1 <= blur iterations <= 4 */
                    Uint32* pixels)   /**< [in]        pixmap data pointer */
    __attribute__((hot, nonnull));

int main(int argc, char* args[]);
