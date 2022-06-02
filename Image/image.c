#include "pretreatment.h"
#include "treatment.h"
#include <stdio.h>

int main(int argc, char** argv){

	SDL_Surface* screen_surface;
	SDL_Surface* save_surface;
	init_sdl();
	SDL_Surface* load_surface = load_image((argc<2)?("my_image.jpg"):(argv[1]));
	int width = load_surface->w;
	int height = load_surface->h;
	
	float angle = 0;
	int res;

	printf("Rotation angle : ");
	res = scanf("%f", &angle);
	if(res!=1) errx(res, "invalid angle");

	screen_surface = display_image(load_surface);

	wait_for_keypressed();

	save_surface = pretreatment(load_surface, screen_surface, load_surface->w, load_surface->h);
	//SDL_Surface* dilated_surface = save_surface;	

	SDL_Surface* dilated_surface;

	if((int)angle%360 != 0){
		dilated_surface = rotation(save_surface, save_surface->w, save_surface->h, angle);
		SDL_FreeSurface(save_surface);
		save_surface = rotation(load_surface, load_surface->w, load_surface->h, angle);
		SDL_FreeSurface(load_surface);
		width = dilated_surface->w;
		height = dilated_surface->h;	
	}
	else{dilated_surface = save_surface; save_surface = load_surface;}
	save_image(dilated_surface, "dilated.bmp");

	SDL_FreeSurface(screen_surface);
	screen_surface = display_image(dilated_surface);
	update_surface(screen_surface, dilated_surface);

	treatment(save_surface, screen_surface, dilated_surface, width, height);

	return 0;
}
