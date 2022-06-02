#include <stdio.h>
#include <err.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <SDL/SDL_rotozoom.h>
#include "pixel_operations.h"
#include <math.h>

void init_sdl()
{
    // Init only the video part.
    // If it fails, die with an error message.
    if(SDL_Init(SDL_INIT_VIDEO) == -1)
        errx(1,"Could not initialize SDL: %s.\n", SDL_GetError());
}

SDL_Surface* load_image(char *path)
{
    SDL_Surface *img;

    // Load an image using SDL_image with format detection.
    // If it fails, die with an error message.
    img = IMG_Load(path);
    if (!img)
        errx(3, "can't load %s: %s", path, IMG_GetError());

    return img;
}

void save_image(SDL_Surface* to_save, char *path)
{
    if (SDL_SaveBMP(to_save, path) != 0)
    {
        errx(EXIT_FAILURE, "Error while saving file");
    }
}

SDL_Surface* display_image(SDL_Surface *img)
{
    SDL_Surface *screen;

    // Set the window to the same size as the image
    screen = SDL_SetVideoMode(img->w, img->h, 0, SDL_SWSURFACE|SDL_ANYFORMAT);
    if (screen == NULL)
    {
        // error management
        errx(1, "Couldn't set %dx%d video mode: %s\n",
                img->w, img->h, SDL_GetError());
    }

    // Blit onto the screen surface
    if(SDL_BlitSurface(img, NULL, screen, NULL) < 0)
        warnx("BlitSurface error: %s\n", SDL_GetError());

    // Update the screen
    SDL_UpdateRect(screen, 0, 0, img->w, img->h);

    // return the screen for further uses
    return screen;
}

void wait_for_keypressed()
{
    SDL_Event event;

    // Wait for a key to be down.
    do
    {
        SDL_PollEvent(&event);
    } while(event.type != SDL_KEYDOWN);

    // Wait for a key to be up.
    do
    {
        SDL_PollEvent(&event);
    } while(event.type != SDL_KEYUP);
}

void draw_line(SDL_Surface *surface, int x1, int y1, int x2, int y2){
	// bresenham line
	//printf("x1 = %i, y1 = %i\nx2 = %i, y2 = %i\n\n", x1, y1, x2, y2);
        Uint32 pixel = SDL_MapRGB(surface->format, 255, 0, 0);
	int steep = abs(y2 - y1) > abs(x2 - x1),
            inc = -1,
	    t;

        if (steep) {
		t = x1;
		x1 = y1;
		y1 = t;
		t = x2;
		x2 = y2;
		y2 = t;
        }
	
        if (x1 > x2) {
		t = x1;
		x1 = x2;
		x2 = t;
		t = y1;
		y1 = y2;
		y2 = t;
        }

        if (y1 < y2) {
                inc = 1;
        }

        int dx = abs(x2 - x1),
            dy = abs(y2 - y1),
            y = y1,
            e = 0;
        for(int x = x1; x <= x2; x++) {
                if (steep) {
                        put_pixel(surface, y, x, pixel);
                } else {
                        put_pixel(surface, x, y, pixel);
                }

                if ((e + dy) << 1 < dx) {
                        e = e + dy;
                } else {
                        y += inc;
                        e = e + dy - dx;
                }
        }
}

void draw_max_line(SDL_Surface *surface, int width, int height, float x1, float y1, float x2, float y2){
	//printf("x1 = %f, y1 = %f\nx2 = %f, y2 = %f\n\n", x1, y1, x2, y2);
	if(x1==x2){
		if(x1>=0 && x1<width) draw_line(surface, (int)x1, 0, (int)x2, height-1);
	}
	else if(y1 == y2){
		if(y1>=0 && y1<height) draw_line(surface, 0, y1, width-1, y1);
	}
	else{
		float a = (y1-y2)/(x1-x2);
		//printf("a = %f\n",a);
		float b = y1 - a*x1;
		//printf("b = %f\n", b);
		//printf("x1 = %f, y1 = %f, a = %f\nb = y1 - a*x1 = %f - %f*%f = %f-%f = %f = %f",
		//		x1,y1,a,y1,a,x1,y1,a*x1,y1-a*x1,b);
		int x0,y0,xmax,ymax;
		if(b>height-1) {y0 = height-1;x0 = (y0-b)/a;}
		else if (b<0) {y0 = 0; x0 = -b/a;}
		else {x0 = 0; y0 = b;}
		ymax = (width-1)*a + b;
		if(ymax>height-1) {ymax = height-1; xmax = (ymax-b)/a;}
		else if (ymax<0) {ymax = 0; xmax = -b/a;}
		else xmax = width-1;
		/*if(x0>=0 && x0<width && xmax>=0 && xmax<width)*/draw_line(surface,x0,y0,xmax,ymax);
	}
}

SDL_Surface* rotation(SDL_Surface* save_surface, int width, int height, float angle){
	SDL_Surface* rotated;
	Uint32 pixel;
	int mx, my, mxr, myr, bx, by;
	float radian, tcos, tsin;
	double widthr, heightr;
	radian = angle*3.141592653589793/180.0;
	tcos = cos(radian);
	tsin = sin(radian);
	widthr = ceil(width*fabs(tcos) + height*fabs(tsin));
	heightr = ceil(height*fabs(tcos) + width*fabs(tsin));
	rotated = SDL_CreateRGBSurface(SDL_HWSURFACE, widthr, heightr, save_surface->format->BitsPerPixel,
			save_surface->format->Rmask, save_surface->format->Gmask, save_surface->format->Bmask, save_surface->format->Amask);
	if(!rotated) errx(1, "can't allocate memory");
	
	int w = rotated->w, h = rotated->h;
	mxr = w/2.;
	myr = h/2.;
	mx = width/2.;
	my = height/2.;
	
	for(int i = 0; i<w; i++){
		for(int j = 0; j<h; j++){
			bx = (ceil (tcos * (i-mxr) + tsin * (j-myr) + mx));
			by = (ceil (-tsin * (i-mxr) + tcos * (j-myr) + my));
			if (bx>=0 && bx<width && by>=0 && by<height){
				pixel = get_pixel(save_surface, bx, by);
				put_pixel(rotated, i, j, pixel);
			}
		}
	}
	
	return rotated;
}

void sort(Uint8* to_sort, int size){
	for(int i = size-1; i>0; i--){
		for(int j = 0; j<i; j++){
			if(to_sort[j] > to_sort[j+1]){
				Uint8 e = to_sort[j];
				to_sort[j] = to_sort[j+1];
				to_sort[j+1] = e;
			}
		}
	}
}

Uint8 get_r(SDL_Surface* surface, int i, int j){
	Uint32 pixel = get_pixel(surface, i, j);
	Uint8 r, g, b;
	SDL_GetRGB(pixel, surface->format, &r, &g, &b);
	return r;
}

Uint8 get_g(SDL_Surface* surface, int i, int j){
	Uint32 pixel = get_pixel(surface, i, j);
	Uint8 r, g, b;
	SDL_GetRGB(pixel, surface->format, &r, &g, &b);
	return g;
}

Uint8 get_b(SDL_Surface* surface, int i, int j){
	Uint32 pixel = get_pixel(surface, i, j);
	Uint8 r, g, b;
	SDL_GetRGB(pixel, surface->format, &r, &g, &b);
	return b;
}


Uint32 interpolate(SDL_Surface* surface, int top, int bottom, int left, int right, double hor, double ver)
{
    Uint8 top_left = get_r(surface, left, top);
    Uint8 top_right = get_r(surface, right, top);
    Uint8 bottom_left = get_r(surface, left, bottom);
    Uint8 bottom_right = get_r(surface, right, bottom);

    double horp = hor - (double)left;
    double verp = ver - (double)top;

    double top_block = (double)top_left + horp * (double)(top_right - top_left);
    double bottom_block = (double)bottom_left + horp * (double)(bottom_right - bottom_left);    

    Uint8 p =  top_block + verp * (bottom_block - top_block);
    return SDL_MapRGB(surface->format, p, p, p);
}

void resize(SDL_Surface *surface, SDL_Surface** s, int width, int height, int w, int h)
{
    SDL_Surface* resized = SDL_CreateRGBSurface(SDL_HWSURFACE, w, h, surface->format->BitsPerPixel,
		    surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

    double xs = w/(double)width;
    double ys = h/(double)height;

    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            double x = i/xs;
            double y = j/ys;

            int top = floor(y);
            int bottom = top + 1;
            int left = floor(x);
            int right = left + 1;

            if (top < height && left < width && bottom < height && right < width)
            {
                Uint32 pixel = interpolate(surface, top, bottom, left, right, x, y);
		put_pixel(resized, i, j, pixel);
            }
        }
    }
    SDL_FreeSurface(surface);
    *s = resized;

}

Uint32 interpolate2(SDL_Surface* surface, int top, int bottom, int left, int right, double hor, double ver)
{
    Uint8 top_leftr = get_r(surface, left, top);
    Uint8 top_rightr = get_r(surface, right, top);
    Uint8 bottom_leftr = get_r(surface, left, bottom);
    Uint8 bottom_rightr = get_r(surface, right, bottom);

    Uint8 top_leftg = get_g(surface, left, top);
    Uint8 top_rightg = get_g(surface, right, top);
    Uint8 bottom_leftg = get_g(surface, left, bottom);
    Uint8 bottom_rightg = get_g(surface, right, bottom);

    Uint8 top_leftb = get_b(surface, left, top);
    Uint8 top_rightb = get_b(surface, right, top);
    Uint8 bottom_leftb = get_b(surface, left, bottom);
    Uint8 bottom_rightb = get_b(surface, right, bottom);

    double horp = hor - (double)left;
    double verp = ver - (double)top;

    double top_blockr = (double)top_leftr + horp * (double)(top_rightr - top_leftr);
    double bottom_blockr = (double)bottom_leftr + horp * (double)(bottom_rightr - bottom_leftr);    

    double top_blockg = (double)top_leftg + horp * (double)(top_rightg - top_leftg);
    double bottom_blockg = (double)bottom_leftg + horp * (double)(bottom_rightg - bottom_leftg);    

    double top_blockb = (double)top_leftb + horp * (double)(top_rightb - top_leftb);
    double bottom_blockb = (double)bottom_leftb + horp * (double)(bottom_rightb - bottom_leftb);    

    Uint8 r =  top_blockr + verp * (bottom_blockr - top_blockr);
    Uint8 g =  top_blockg + verp * (bottom_blockg - top_blockg);
    Uint8 b =  top_blockb + verp * (bottom_blockb - top_blockb);

    return SDL_MapRGB(surface->format, r, g, b);
}

void resize2(SDL_Surface *surface, SDL_Surface** s, int width, int height, int w, int h)
{
    SDL_Surface* resized = SDL_CreateRGBSurface(SDL_HWSURFACE, w, h, surface->format->BitsPerPixel,
		    surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

    double xs = w/(double)width;
    double ys = h/(double)height;

    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            double x = i/xs;
            double y = j/ys;

            int top = floor(y);
            int bottom = top + 1;
            int left = floor(x);
            int right = left + 1;

            if (top < height && left < width && bottom < height && right < width)
            {
                Uint32 pixel = interpolate2(surface, top, bottom, left, right, x, y);
		put_pixel(resized, i, j, pixel);
            }
        }
    }
    SDL_FreeSurface(surface);
    *s = resized;

}


SDL_Surface* copy_surface(SDL_Surface* to_copy){
	int width = to_copy->w;
	int height = to_copy->h;
	SDL_Surface* copy = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, to_copy->format->BitsPerPixel,
			to_copy->format->Rmask, to_copy->format->Gmask, to_copy->format->Bmask, to_copy->format->Amask);
	for(int i = 0; i<width; i++){
		for(int j = 0; j<height; j++){
			put_pixel(copy, i, j, get_pixel(to_copy, i, j));
		}
	}
	return copy;
}

void img_to_matrix(char* path, int side, double* matrix){

	SDL_Surface* img = load_image(path);
	resize(img, &img, img->w, img->h, side, side);
	for(int i = 0; i<side; i++){
		for(int j = 0; j<side; j++){
			 *(matrix+i*side+j) = (get_r(img, i, j)>0)?(1.0):(0.0);
		}
	}
	SDL_FreeSurface(img);

}
