/*
 * crap.c
 *
 *  Created on: Sep 20, 2013
 *      Author: justin
 */
#include "crap.h"
#include <time.h>
#include </usr/include/time.h>

Uint32 get_pixel32(SDL_Surface *surface, int x, int y) {
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *) surface->pixels;
    //Get the requested pixel
    return pixels[(y * surface->w) + x];
}

void put_pixel32(SDL_Surface *surface, int x, int y, Uint32 pixel) {
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *) surface->pixels;
    //Set the pixel
    pixels[(y * surface->w) + x] = pixel;
}

void rgba2surf_f(SDL_Surface* surf, float* red, float* green, float* blue, float* alpha) {
    Uint32 p = 0, x = 0, y = 0;
    Uint32* sdl_pix = (Uint32*) surf->pixels;

    for (y = 0; y < surf->h; y++) {
        for (x = 0; x < surf->w; x++) {
            p = y * surf->w + x;
            sdl_pix[p] = (((Uint32) red[p]) << 24) + (((Uint32) green[p]) << 16)
                         + (((Uint32) blue[p]) << 8) + (((Uint32) alpha[p]));
        }
    }
}

void rgba2surf_i(SDL_Surface* surf, Uint32* red, Uint32* green, Uint32* blue, Uint32* alpha) {
    Uint32 p = 0, x = 0, y = 0;
    Uint32* sdl_pix = (Uint32*) surf->pixels;

    for (y = 0; y < surf->h; y++) {
        for (x = 0; x < surf->w; x++) {
            p = y * surf->w + x;
            sdl_pix[p] = ((red[p]) << 24) + ((green[p]) << 16)
                         + ((blue[p]) << 8) + ((alpha[p]));
        }
    }
}

void surf2rgba_f(SDL_Surface* surf, float* red, float* green, float* blue, float* alpha) {
    if (surf == NULL) { return; }
    if (red == NULL) { return; }
    if (green == NULL) { return; }
    if (blue == NULL) { return; }
    if (alpha == NULL) { return; }

    Uint32 x = 0, y = 0, p = 0;
    Uint32 rmask = 0xff << 24;
    Uint32 gmask = 0xff << 16;
    Uint32 bmask = 0xff << 8;
    Uint32 amask = 0xff << 0;
    Uint32* sdl_pix = (Uint32*) surf->pixels;

    for (y = 0; y < surf->h; y++) {
        for (x = 0; x < surf->w; x++) {
            p = surf->w * y + x;
            red[p] =   (float) ((sdl_pix[p] & rmask) >> 24);
            green[p] = (float) ((sdl_pix[p] & gmask) >> 16);
            blue[p] =  (float) ((sdl_pix[p] & bmask) >> 8);
            alpha[p] = (float) ((sdl_pix[p] & amask) >> 0);
        }
    }
}

void surf2rgba_i(SDL_Surface* surf, Uint32* red, Uint32* green, Uint32* blue, Uint32* alpha) {
    if (surf == NULL) { return; }
    if (red == NULL) { return; }
    if (green == NULL) { return; }
    if (blue == NULL) { return; }
    if (alpha == NULL) { return; }

    Uint32 x = 0, y = 0, p = 0;
    Uint32 rmask = 0xff << 24;
    Uint32 gmask = 0xff << 16;
    Uint32 bmask = 0xff << 8;
    Uint32 amask = 0xff << 0;
    Uint32* sdl_pix = (Uint32*) surf->pixels;

    for (y = 0; y < surf->h; y++) {
        for (x = 0; x < surf->w; x++) {
            p = surf->w * y + x;
            red[p] =   ((sdl_pix[p] & rmask) >> 24);
            green[p] = ((sdl_pix[p] & gmask) >> 16);
            blue[p] =  ((sdl_pix[p] & bmask) >> 8);
            alpha[p] = ((sdl_pix[p] & amask) >> 0);
        }
    }
}

void blur_channel_f(Uint32 height, Uint32 width, Uint32 amount, float* pixels) {
    /** Input validation */
    if (pixels == NULL) { return; }
    if (height < 1 || height >= 2160) { return; }
    if (width < 1 || width >= 4320) { return; }
    if (amount > 4) { amount = 4; }
    if (amount == 0) { return; }

    Uint8 i = 0;
    Uint16 x = 0, sx = 0, y = 0, sy = 0;
    float output[width * height];

    for (i = 0; i < amount; i++) { /** Blur iteration loop */
        for (y = 0; y < height; y++) { /** row loop */
            if (y == 0) {
                sy = 1;
            } else if (y == (height - 1)) {
                sy = height - 2;
            } else {
                sy = y;
            }
            for (x = 1; x < (width - 1); x++) { /** column loop */
                if (x == 0) {
                    sx = 1;
                } else if (x == (width - 1)) {
                    sx = width - 2;
                } else {
                    sx = x;
                }
                output[y * width + x] = ((pixels[(sy - 1) * width + (sx - 1)] * DIAG_F  // top left
                                          + pixels[(sy - 1) * width + (sx + 1)] * DIAG_F  // top right
                                          + pixels[(sy + 1) * width + (sx - 1)] * DIAG_F  // bottom left
                                          + pixels[(sy + 1) * width + (sx - 1)] * DIAG_F  // bottom right
                                          + pixels[(sy - 1) * width + (sx + 0)] * NEXT_F  // top
                                          + pixels[(sy + 0) * width + (sx - 1)] * NEXT_F  // left
                                          + pixels[(sy + 0) * width + (sx - 1)] * NEXT_F  // right
                                          + pixels[(sy + 1) * width + (sx + 0)] * NEXT_F  // bottom
                                          + pixels[(sy + 0) * width + (sx + 0)] * SELF_F  // center
                                          ) / DIV_F);
            } /** end col loop */
        } /** end row loop */
        memcpy(pixels, output, sizeof(output));
    } /** end blur loop */
}

void blur_channel_i(Uint32 height, Uint32 width, Uint32 amount, Uint32* pixels) {
    /** Input validation */
    if (pixels == NULL) { return; }
    if (height < 1 || height >= 2160) { return; }
    if (width < 1 || width >= 4320) { return; }
    if (amount > 4) { amount = 4; }
    if (amount == 0) { return; }

    Uint8 i = 0;
    Uint16 x = 0, sx = 0, y = 0, sy = 0;
    Uint32 output[width * height];

    for (i = 0; i < amount; i++) { /** Blur iteration loop */
        for (y = 0; y < height; y++) { /** row loop */
            if (y == 0) {
                sy = 1;
            } else if (y == (height - 1)) {
                sy = height - 2;
            } else {
                sy = y;
            }
            for (x = 1; x < (width - 1); x++) { /** column loop */
                if (x == 0) {
                    sx = 1;
                } else if (x == (width - 1)) {
                    sx = width - 2;
                } else {
                    sx = x;
                }
                output[y * width + x] = ((pixels[(sy - 1) * width + (sx - 1)] * DIAG_I  // top left
                                          + pixels[(sy - 1) * width + (sx + 1)] * DIAG_I  // top right
                                          + pixels[(sy + 1) * width + (sx - 1)] * DIAG_I  // bottom left
                                          + pixels[(sy + 1) * width + (sx - 1)] * DIAG_I  // bottom right
                                          + pixels[(sy - 1) * width + (sx + 0)] * NEXT_I  // top
                                          + pixels[(sy + 0) * width + (sx - 1)] * NEXT_I  // left
                                          + pixels[(sy + 0) * width + (sx - 1)] * NEXT_I  // right
                                          + pixels[(sy + 1) * width + (sx + 0)] * NEXT_I  // bottom
                                          + pixels[(sy + 0) * width + (sx + 0)] * SELF_I  // center
                                          ) / DIV_I);
            } /** end col loop */
        } /** end row loop */
        memcpy(pixels, output, sizeof(output));
    } /** end blur loop */
}

void bench(struct timespec* start, struct timespec* end, char* msg) {
    double startsecs = (double)start->tv_sec + (double)start->tv_nsec / 1000000000.0;
    double endsecs = (double)end->tv_sec + (double)end->tv_nsec / 1000000000.0;
    printf("->\tBench Time\t%s:\t%15.9f\n", (endsecs-startsecs), msg);
}

int main(int argc, char* args[]) {
    //The images
    SDL_Surface* src_img = NULL;
    SDL_Surface* dst_img = NULL;
    SDL_Surface* screen = NULL;

    float red_f[4096] = { 0.0f };
    float grn_f[4096] = { 0.0f };
    float blu_f[4096] = { 0.0f };
    float alp_f[4096] = { 0.0f };
    Uint32 red_i[4096] = { 0.0f };
    Uint32 grn_i[4096] = { 0.0f };
    Uint32 blu_i[4096] = { 0.0f };
    Uint32 alp_i[4096] = { 0.0f };

    int counter = 0;
    struct timespec start, end;

    SDL_Init(SDL_INIT_EVERYTHING);
    screen = SDL_SetVideoMode(480, 320, 32, SDL_SWSURFACE);
    src_img = SDL_LoadBMP("/home/justin/src/crap/test2.bmp");
    dst_img = SDL_CreateRGBSurface(SDL_SWSURFACE, src_img->w, src_img->h,
                                   src_img->format->BitsPerPixel, src_img->format->Rmask,
                                   src_img->format->Gmask, src_img->format->Bmask,
                                   src_img->format->Amask);

    SDL_BlitSurface(src_img, NULL, screen, NULL);
    SDL_Flip(screen);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    for (counter = 0; counter < 32768; counter++) {
        surf2rgba_f(src_img, red_f, grn_f, blu_f, alp_f);
        blur_channel_f(64, 64, counter % 4, red_f);
        blur_channel_f(64, 64, counter % 4, grn_f);
        blur_channel_f(64, 64, counter % 4, blu_f);
        rgba2surf_f(dst_img, red_f, grn_f, blu_f, alp_f);
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    bench(&start, &end, "float");

    SDL_BlitSurface(dst_img, NULL, screen, NULL);
    SDL_Flip(screen);

    SDL_BlitSurface(src_img, NULL, screen, NULL);
    SDL_Flip(screen);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    for (counter = 0; counter < 32768; counter++) {
        surf2rgba_i(src_img, red_i, grn_i, blu_i, alp_i);
        blur_channel_i(64, 64, counter % 4, red_i);
        blur_channel_i(64, 64, counter % 4, grn_i);
        blur_channel_i(64, 64, counter % 4, blu_i);
        rgba2surf_i(dst_img, red_i, grn_i, blu_i, alp_i);
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    bench(&start, &end, "Uint32");

    SDL_BlitSurface(dst_img, NULL, screen, NULL);
    SDL_Flip(screen);

    SDL_FreeSurface(src_img);
    SDL_FreeSurface(dst_img);
    SDL_Quit();
    return 0;
}
