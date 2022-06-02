#ifndef PRETREATMENT_H
#define PRETREATMENT_H

#include <stdio.h>
#include <err.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "pixel_operations.h"
#include "sdl_func.h"

void grayscale(SDL_Surface *save_surface, SDL_Surface *screen_surface, int width, int height);
void contrast(SDL_Surface *save_surface, SDL_Surface *screen_surface, int width, int height);
void normalize(SDL_Surface *save_surface, SDL_Surface *screen_surface, int width, int height);
void median(SDL_Surface *save_surface, SDL_Surface *screen_surface, int width, int height);
void blur(SDL_Surface *save_surface, SDL_Surface *screen_surface, int width, int height);
void adaptative(SDL_Surface *save_surface, SDL_Surface *screen_surface, int width, int height);
void sobel(SDL_Surface *save_surface, SDL_Surface *screen_surface, int width, int height);
void binarize(SDL_Surface *save_surface, SDL_Surface *screen_surface, int width, int height);
void dilate(SDL_Surface *save_surface, SDL_Surface *screen_surface, int width, int height);

SDL_Surface* pretreatment();

#endif
