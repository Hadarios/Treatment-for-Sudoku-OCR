#ifndef TREATMENT_H
#define TREATMENT_H

#include <stdio.h>
#include <err.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "pixel_operations.h"
#include "sdl_func.h"

void hough(SDL_Surface *screen_surface, int width, int height, float* ave);
void hough_cut(SDL_Surface *dilated_surface, SDL_Surface *save_surface, SDL_Surface** d_r, SDL_Surface** s_r, int width, int height);
int treatment(SDL_Surface *save_surface, SDL_Surface *screen_surface, SDL_Surface* dilated_surface, int width, int height);
int histo(SDL_Surface *save_surface, int width, int height, int lines[]);
void mean_dist(int histo[], int length);
int histo_peaks(int histo[], int length, float t, int best[], int k_max);
float histo_sigma(int histo[], int length, float mean);
float histo_mean(int histo[], int length);
float sigma_dist(int histo[], int length, int mean);
void cut_squares(SDL_Surface *save_surface, int side, int x0, int y0);



#endif

