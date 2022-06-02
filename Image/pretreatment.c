#include <stdio.h>
#include <err.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "pixel_operations.h"
#include "sdl_func.h"
#include <math.h>

void grayscale(SDL_Surface *save_surface, SDL_Surface *screen_surface, int width, int height){
	for(int i = 0; i<width; i++){
		for(int j = 0; j<height; j++){
			Uint32 pixel = get_pixel(screen_surface, i, j);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, screen_surface->format, &r, &g, &b);
			Uint8 a = 0.3*r + 0.59*g + 0.11*b;
			pixel = SDL_MapRGB(screen_surface->format, a, a, a);
			put_pixel(save_surface, i, j, pixel);
		}
	}
}

void contrast(SDL_Surface *save_surface, SDL_Surface *screen_surface, int width, int height){

	for(int i = 0; i<width; i++){
                for(int j = 0; j<height; j++){
                	Uint8 inf = 0;
			int k = 1;
			Uint32 pixel = get_pixel(screen_surface, i, j);
                        Uint8 r, g, b;
                        SDL_GetRGB(pixel, screen_surface->format, &r, &g, &b);
			while(k<=10){
				Uint8 sup = k*255/10;
				if(r>=inf && r<=sup){
					sup = 255-sup;
					pixel = SDL_MapRGB(screen_surface->format, sup, sup, sup);
                        		put_pixel(save_surface, i, j, pixel);
					k = 11;
				}
				else{inf = sup;k++;}
			}
		}
        }


}

void normalize(SDL_Surface *save_surface, SDL_Surface *screen_surface, int width, int height){
	
	Uint8 m = 0;
	for(int i = 0; i<width; i++){
                for(int j = 0; j<height; j++){
			Uint32 pixel = get_pixel(screen_surface, i, j);
                        Uint8 r, g, b;
                        SDL_GetRGB(pixel, screen_surface->format, &r, &g, &b);
			if(m<r) m = r;
		}
	}
	for(int i = 0; i<width; i++){
                for(int j = 0; j<height; j++){
			Uint32 pixel = get_pixel(screen_surface, i, j);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, screen_surface->format, &r, &g, &b);
			r = 255-r*255/m;
			pixel = SDL_MapRGB(screen_surface->format, r, r, r);
			put_pixel(save_surface, i, j, pixel);
		}
	}

}

void median(SDL_Surface *save_surface, SDL_Surface *screen_surface, int width, int height){
	
	for(int i = 0; i<width; i++){
                for(int j = 0; j<height; j++){
			Uint8 list[9] = {0,0,0,0,0,0,0,0,0};
			list[0] = get_r(screen_surface, i, j);
			if(i>0){
				list[1] = get_r(screen_surface, i-1, j);
				if(j>0) list[2] = get_r(screen_surface, i-1, j-1);
				if(j<height-1) list[3] = get_r(screen_surface, i-1, j+1);
			}
			if(i<width-1){
				list[4] = get_r(screen_surface, i+1, j);
				if(j>0) list[5] = get_r(screen_surface, i+1, j-1);
				if(j<height-1) list[6] = get_r(screen_surface, i+1, j+1);	
			}
			if(j>0) list[7] = get_r(screen_surface, i, j-1);
			if(j<height-1) list[8] = get_r(screen_surface, i, j+1);
			sort(list, 9);
			Uint8 m = list[4];	
			Uint32 pixel = SDL_MapRGB(screen_surface->format, m, m, m);
                        put_pixel(save_surface, i, j, pixel);
		}
	}

}

void blur(SDL_Surface *save_surface, SDL_Surface *screen_surface, int width, int height){
	float gaussian[3][3] =
	{
		{1./16., 2./16., 1./16.},
		{2./16., 4./16., 1./16.},
		{1./16., 2./16., 1./16.},
	};
	Uint32 pixel;
	float m = 0;
	float s = 0;
	for(int i = 0; i<width; i++){
		for(int j = 0; j<height; j++){
			s = 0;
			for(int a = -1; a<2; a++){
				for(int b = -1; b<2; b++){
					if(i+a < width && i+a >= 0 && j+b <height && j+b >= 0)
					{
						pixel = get_pixel(screen_surface, i+a, j+b);
						Uint8 r, g, b1;
						SDL_GetRGB(pixel, screen_surface->format, &r, &g, &b1);
						m = gaussian[a+1][b+1];
						s += m*(float)r;
					}
				}
			}
			pixel = SDL_MapRGB(save_surface->format, (Uint8)s, (Uint8)s, (Uint8)s);
			put_pixel(save_surface, i, j, pixel);
		}
	}
	

}

void adaptative(SDL_Surface *save_surface, SDL_Surface *screen_surface, int width, int height){

	int sum;
	int* intImg = calloc(width*height, sizeof(int));
	for(int i = 0; i<width; i++){
        	sum = 0;
        	for(int j = 0; j<height; j++){
                	sum+=get_r(save_surface, i, j);
                	if(!i) intImg[i*height + j] = sum;
                	else intImg[i*height + j] = intImg[(i-1)*height + j]+sum;
        	}
	}
	int s = fmax(width,height)/16;
	for(int i = 0; i<width; i++){
        	for(int j = 0; j<height; j++){
                	int x1 = fmax(i - s, 1);
                	int x2 = fmin(i + s, width - 1);
                	int y1 = fmax(j - s, 1);
                	int y2 = fmin(j + s, height - 1);
                	int count = (x2 - x1) * (y2 - y1);
                	sum = intImg[x2*height + y2]
                	        - intImg[x2*height + y1-1]
                	        - intImg[(x1-1)*height + y2]
                	        + intImg[(x1-1)*height + y1 - 1];
                	Uint8 m = (get_r(screen_surface, i, j)*count<=sum/2)?(255):(0);
			Uint32 pixel = SDL_MapRGB(save_surface->format, m, m, m);
			put_pixel(save_surface, i, j, pixel);
        	}
	}
	free(intImg);
}




void sobel(SDL_Surface *save_surface, SDL_Surface *screen_surface, int width, int height){
	Uint32 pixel;
	float gradientx[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
	float gradienty[3][3] = {{-1,-2,-1},{0,0,0},{1,2,1}};
	for(int i = 1; i<width-1; i++){
		for(int j = 1; j<height-1; j++){
			float gx = 0;
			float gy = 0;
			for(int gi = -1; gi<2; gi++){
				for(int gj = -1; gj<2; gj++){
					//if(i+gi < width && i+gi >= 0 && j+gj <height && j+gj >= 0){
						pixel = get_pixel(screen_surface, i+gi, j+gj);
	                			Uint8 r, g, b;
						SDL_GetRGB(pixel, screen_surface->format, &r, &g, &b);
						gx += r*gradientx[gi+1][gj+1];	
						gy += r*gradienty[gi+1][gj+1];
					//}
				}
			}
			float g = ((gx<0)?(-gx):(gx)) + ((gy<0)?(-gy):(gy));
			g = (g>255)?(255):(g);
			pixel = SDL_MapRGB(save_surface->format, (Uint8)g, (Uint8)g, (Uint8)g);
			put_pixel(save_surface, i, j, pixel);
		}
	}
}

void binarize(SDL_Surface *save_surface, SDL_Surface *screen_surface, int width, int height){
	Uint32 pixel;
	float histo[256];
	memset(histo, 0.0, sizeof(histo));
	for(int i = 0; i<width; i++){
		for(int j = 0; j<height; j++){
			pixel = get_pixel(screen_surface, i, j);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, screen_surface->format, &r, &g, &b);
			histo[r]+=1.0;
		}
    	}
	float test = 0.0;
	for(int i = 0; i<256; i++){
		histo[i]/=(float)(width*height);
		test+=histo[i];
	}
	double probability[256], mean[256], between[256], max_between = 0.0;
	int threshold = 0;

	memset(probability, 0.0, sizeof(probability));
	memset(mean, 0.0, sizeof(mean));
	memset(between, 0.0, sizeof(between));

	probability[0] = histo[0];

	for(int i = 1; i<256; i++){
		probability[i] = probability[i-1] + histo[i];
		mean[i] = mean[i-1] + i*histo[i];
	}

	for(int i = 0; i<255; i++){
		if(probability[i]!=0.0 && probability[i]!=1.0)
			between[i] = pow(mean[255] * probability[i] - mean[i], 2) / (probability[i] * (1.0 - probability[i]));
		else
			between[i] = 0.0;
		if(between[i] > max_between){
			max_between = between[i];
			threshold = i;
		}
	}
	for(int i = 0; i<width;i++){
		for(int j = 0; j<height; j++){
			pixel = get_pixel(screen_surface, i, j);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, screen_surface->format, &r, &g, &b);
			r = (r>threshold)?(255):(0);
			pixel = SDL_MapRGB(save_surface->format, (Uint8)r, (Uint8)r, (Uint8)r);
			put_pixel(save_surface, i, j, pixel);
		}
	}

}

void dilate(SDL_Surface *save_surface, SDL_Surface *screen_surface, int width, int height){
	
	Uint32 pixel;
	for(int i = 0; i<width; i++){
		for(int j = 0; j<height; j++){	
			Uint8 r, g, b;
			pixel = get_pixel(screen_surface, i, j);
			SDL_GetRGB(pixel, screen_surface->format, &r, &g, &b);
			if(r==0){	
				int i1 = -1;
				int j1 = -1;
				Uint8 white = 0;
				while(i1<2 && white == 0){
					while(j1<2 && white ==0){	
						if(i+i1 < width && i+i1 >= 0 && j+j1 <height && j+j1 >= 0){
							pixel = get_pixel(screen_surface, i+i1, j+j1);
							SDL_GetRGB(pixel, screen_surface->format, &white, &g, &b);
						}	
						j1++;
					}
					i1++;
				}
				if(white != 0){
					pixel = SDL_MapRGB(save_surface->format, 255, 255, 255); 
					put_pixel(save_surface, i, j, pixel);
				}
			}
		}
    }
}

SDL_Surface* pretreatment(SDL_Surface *save_surface, SDL_Surface *screen_surface, int width, int height)
{
    
/*------------------------------------------------*/

	grayscale(save_surface, screen_surface, width, height);
	update_surface(screen_surface, save_surface);
	wait_for_keypressed();

/*------------------------------------------------*/

	contrast(save_surface, screen_surface, width, height);
	update_surface(screen_surface, save_surface);
	wait_for_keypressed();

/*------------------------------------------------*/

	normalize(save_surface, screen_surface, width, height);
	update_surface(screen_surface, save_surface);
	wait_for_keypressed();

/*------------------------------------------------*/

	median(save_surface, screen_surface, width, height);
	update_surface(screen_surface, save_surface);
	wait_for_keypressed();

/*------------------------------------------------*/

	blur(save_surface, screen_surface, width, height);
	update_surface(screen_surface, save_surface);
	wait_for_keypressed();

/*------------------------------------------------*/

	adaptative(save_surface, screen_surface, width, height);
	update_surface(screen_surface, save_surface);
	wait_for_keypressed();

/*------------------------------------------------*/

	dilate(save_surface, screen_surface, width, height);
	update_surface(screen_surface, save_surface);

/*------------------------------------------------*/

	dilate(save_surface, screen_surface, width, height);
	update_surface(screen_surface, save_surface);

/*------------------------------------------------*/

	dilate(save_surface, screen_surface, width, height);
	update_surface(screen_surface, save_surface);
	wait_for_keypressed();

/*------------------------------------------------*/

	sobel(save_surface, screen_surface, width, height);
	update_surface(screen_surface, save_surface);
	wait_for_keypressed();
	SDL_Surface* dilated_surface = copy_surface(save_surface);

/*------------------------------------------------*/

	dilate(dilated_surface, screen_surface, width, height);
	update_surface(screen_surface, dilated_surface);

/*------------------------------------------------*/

	dilate(dilated_surface, screen_surface, width, height);
	update_surface(screen_surface, dilated_surface);

/*------------------------------------------------*/

	dilate(dilated_surface, screen_surface, width, height);
	update_surface(screen_surface, dilated_surface);
	wait_for_keypressed();

/*------------------------------------------------*/
	return dilated_surface;
}
