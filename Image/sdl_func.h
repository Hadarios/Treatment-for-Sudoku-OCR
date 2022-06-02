#ifndef SDL_FUNC_H
#define SDL_FUNC_H

#include <stdio.h>
#include <err.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "pixel_operations.h"

void init_sdl();
SDL_Surface* load_image(char *path);
void save_image(SDL_Surface* to_save, char* path);
SDL_Surface* display_image(SDL_Surface *img);
void wait_for_keypressed();
void draw_line(SDL_Surface *surface, int x1, int y1, int x2, int y2);
void draw_max_line(SDL_Surface *surface, int width, int height, float x1, float y1, float x2, float y2);
SDL_Surface* rotation(SDL_Surface *save_surface, int width, int height, float angle);
void sort(Uint8* to_sort, int size);
Uint8 get_r(SDL_Surface* surface, int i, int j);
Uint8 get_g(SDL_Surface* surface, int i, int j);
Uint8 get_b(SDL_Surface* surface, int i, int j);
Uint32 interpolate(SDL_Surface* surface, int top, int bottom, int left, int right, double hor, double ver);
void resize(SDL_Surface *surface, SDL_Surface** s, int width, int height, int w, int h);
Uint32 interpolate2(SDL_Surface* surface, int top, int bottom, int left, int right, double hor, double ver);
void resize2(SDL_Surface *surface, SDL_Surface** s, int width, int height, int w, int h);
SDL_Surface* copy_surface(SDL_Surface* to_copy);
void img_to_matrix(char* path, int side, double* matrix);
//void draw_low(SDL_Surface *save_surface, int width, int height, int x0, int y0, int x1, int y1, Uint32 pixel);
//void draw_high(SDL_Surface *save_surface, int width, int height, int x0, int y0, int x1, int y1, Uint32 pixel);
//void draw_line(SDL_Surface *save_surface, int width, int height, int x1, int y1, int x2, int y2);
//void draw_max_line(SDL_Surface *save_surface, int width, int height, int x1, int y1, int x2, int y2);
#endif
