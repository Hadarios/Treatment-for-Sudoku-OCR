#include <stdio.h>
#include <err.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "pixel_operations.h"
#include "sdl_func.h"
#include <math.h>

void hough(SDL_Surface *screen_surface, int width, int height, float* ave){
	int rhomax = sqrt(width*width+height*height);
	//printf("rhomax = %i, width = %i, height = %i\n", rhomax, width, height);
	double torad = 3.14159265/360.;
	double thetamax = 360.;
	//int acc[(int)thetamax][rhomax*2];
	int *acc = calloc((int)thetamax*rhomax*2, sizeof(int));
	//int skip = rhomax*0.02;
	for(int i = 0; i<width; i++){
		for(int j = 0; j<height; j++){
			Uint8 r, g, b;
                        Uint32 pixel = get_pixel(screen_surface, i, j);
                        SDL_GetRGB(pixel, screen_surface->format, &r, &g, &b);
			if(r != 0){
				for(int theta = 0; theta<thetamax; theta++){
					float thetar = theta*torad;
					int rho = i*cos(thetar) + j*sin(thetar);
					//acc[theta][rho+rhomax]+=1;
					*(acc+theta*rhomax*2+rho+rhomax)+=1;
				}
				
			}
		}
	}
	int max = 0;
	//int mid = 0;
	//int *to_sort = calloc(thetamax*rhomax*2, sizeof(int));
	for(int theta = 0; theta<thetamax; theta++){
		for(int rho = 0; rho<rhomax*2;rho++){
			//if(acc[theta][rho]>max) max = acc[theta][rho];
			if(*(acc+theta*rhomax*2+rho)>max) max = *(acc+theta*rhomax*2+rho);
			//mid+=acc[theta][rho];
			//to_sort[theta*rhomax*2+rho] = acc[theta][rho];
		}
	}
	//printf("----\n");
	/*mid = mid/(thetamax*rhomax*2);
	mergeSort(to_sort, 0, thetamax*rhomax*2-1);
	printArray(to_sort, thetamax*rhomax*2);
	int threshold = (to_sort[(int)thetamax*rhomax] + mid + max)/6;
	printf("max = %i ; mid = %i ; (max+mid+med)/6 = %i\n", max, mid, threshold);
	free(to_sort);*/
	//threshold = (max+rhomax)/5;
	//printf("t = %i\n", threshold);
	float factor = 0.5;
	float inf = 0.1;
	float sup = 0.9;
	int found = 0;
	int threshold;
	float count;
	int euh = 0;
	while(!found && euh<100){
		euh++;
		count = 0;
		threshold = factor*rhomax;
		for(int theta = 0; theta<thetamax; theta++){
			for(int rho = 0; rho<rhomax*2; rho++){
				//if(acc[theta][rho] >= threshold){	
				if(*(acc+theta*rhomax*2+rho) >= threshold){	
					count++;
					/*float thetar = (float)theta*torad;
					float c = cos(thetar);
					float s = sin(thetar);
					float x0 = c*(float)(rho-rhomax);
					float y0 = s*(float)(rho-rhomax);	
					if(y0 == 0) draw_line(save_surface, x0, 0, x0, height-1);
					else {
						float a = -x0/y0;
						float b = y0 - a*x0;
						float x1 = x0 + rhomax;
						float y1 = a*x1+b;
						draw_max_line(save_surface, width, height, x0, y0, x1, y1);
					//	printf("rho = %i, theta = %i, thetar = %f\n", rho-rhomax, theta, thetar);
					}*/	
	
					//rho+=skip;
				}
			}
		}
		//printf("count = %f ; count/rhomax = %f\n", count, count/rhomax);
		if(count/rhomax < 0.117f){
			sup = factor;
			factor = (inf+factor)/2;
		}	
		else if(count/rhomax > 0.123f){
			inf = factor;
			factor = (sup+factor)/2;
		}
		else found = 1;
	}
	if(!found) errx(1, "zbeub");
	//printf("----\n");
	float avep = 0;
	float aven = 0;
	int cp = 0;
	int cn = 0;
	for(int theta = 0; theta<thetamax; theta++){
		for(int rho = 0; rho<rhomax*2; rho++){
			//if(acc[theta][rho] >= threshold){	
			if(*(acc+theta*rhomax*2+rho) >= threshold){	
				float thetar = (float)theta*torad;
				float c = cos(thetar);
				float s = sin(thetar);
				float x0 = c*(float)(rho-rhomax);
				float y0 = s*(float)(rho-rhomax);	
				if(y0 != 0){
					float a = atan(-x0/y0)/torad/2.;
					a = (a>0)?(a):(-a);
					if(a<45){avep+=a;cp++;}
					else{aven+=a;cn++;}
					
				}
				else cp++;
				/*float thetar = (float)theta*torad;
				float c = cos(thetar);
				float s = sin(thetar);
				float x0 = c*(float)(rho-rhomax);
				float y0 = s*(float)(rho-rhomax);	
				if(y0 == 0) draw_line(screen_surface, x0, 0, x0, height-1);
				else {
					float a = -x0/y0;
					float b = y0 - a*x0;
					float x1 = x0 + rhomax;
					float y1 = a*x1+b;
					draw_max_line(screen_surface, width, height, x0, y0, x1, y1);
					update_surface(screen_surface, screen_surface);
				//	printf("rho = %i, theta = %i, thetar = %f\n", rho-rhomax, theta, thetar);
				}	
*/
				//rho+=rhomax*0.005;
				//rho+=6;
			}
		}
	}
	free(acc);
	avep/=cp;
	aven/=cn;
	//printf("average ap = %f\naverage an = %f\n", avep, aven);
	/*float mid = (avep+aven)/2;
	float diffp = (avep-mid)/avep;
	float diffn = (aven-mid)/aven;
	if(diffp < 0) diffp*=-1;
	else diffn*=-1;
	float diff = (diffp+diffn)/2;
	printf("mid = %F ; diffp = %f ; diffn = %f ; diff = %f\n", mid, diffp, diffn, diff);
	if(diff <= 0.09) return (avep-aven>0)?(2*avep-aven):(2*aven-avep);
	return 0;*/
	ave[0] = avep;
	ave[1] = aven;

}










void hough_cut(SDL_Surface* dilated_surface, SDL_Surface* save_surface, SDL_Surface** d_r, SDL_Surface** s_r, int width, int height){
	
	int rhomax = sqrt(width*width+height*height);
	double torad = 3.14159265/360.;
	double thetamax = 360.;
//	int acc[(int)thetamax][rhomax*2];
	int* acc = calloc(thetamax*rhomax*2, sizeof(int));
	
	for(int i = 0; i<width; i++){
		for(int j = 0; j<height; j++){
			Uint8 r, g, b;
                        Uint32 pixel = get_pixel(dilated_surface, i, j);
                        SDL_GetRGB(pixel, dilated_surface->format, &r, &g, &b);
			if(r != 0){
				for(int theta = 0; theta<thetamax; theta++){
					float thetar = theta*torad;
					int rho = i*cos(thetar) + j*sin(thetar);
					//acc[theta][rho+rhomax]+=1;
					*(acc+theta*rhomax*2+rho+rhomax)+=1;
				}
				
			}
		}
	}

	int max = 0;
	for(int theta = 0; theta<thetamax; theta++){
		for(int rho = 0; rho<rhomax*2;rho++){
			//if(acc[theta][rho]>max) max = acc[theta][rho];
			if(*(acc+theta*rhomax*2+rho)>max) max = *(acc+theta*rhomax*2+rho);
		}
	}

	float factor = 0.5;
	float inf = 0.1;
	float sup = 0.9;
	int found = 0;
	int threshold;
	float count;

	while(!found){
		count = 0;
		threshold = factor*rhomax;
		for(int theta = 0; theta<thetamax; theta++){
			for(int rho = 0; rho<rhomax*2; rho++){
				//if(acc[theta][rho] >= threshold){	
				if(*(acc+theta*rhomax*2+rho) >= threshold){	
					count++;
				}
			}
		}
		if(count/rhomax < 0.117f){
			sup = factor;
			factor = (inf+factor)/2;
		}	
		else if(count/rhomax > 0.123f){
			inf = factor;
			factor = (sup+factor)/2;
		}
		else found = 1;
	}

	found = 0;
	float x0 = width-1, y0 = height-1;
	float x1 = 0, y1 = 0;

	for(int rho = 0; rho<rhomax*2; rho++){
		for(int theta = 0; theta<20; theta++){
			//if(acc[theta][rho] >= threshold){
			if(*(acc+theta*rhomax*2+rho) >= threshold){
				float thetar = (float)theta*torad;
				float c = cos(thetar);
				float x = c*(float)(rho-rhomax);
				if(x<x0 && x>width*0.01) x0 = x;
				if(x>x1 && x<width*0.99) x1 = x;
			}
		}
	}
	for(int rho = 0; rho<rhomax*2; rho++){
		for(int theta = 340; theta<thetamax; theta++){
			//if(acc[theta][rho] >= threshold){
			if(*(acc+theta*rhomax*2+rho) >= threshold){
				float thetar = (float)theta*torad;
				float c = cos(thetar);
				float x = c*(float)(rho-rhomax);
				if(x<x0 && x>width*0.01) x0 = x;
				if(x>x1 && x<width*0.99) x1 = x;
			}
		}
	}
	for(int rho = 0; rho<rhomax*2; rho++){
		for(int theta = 160; theta<200; theta++){
			//if(acc[theta][rho] >= threshold){
			if(*(acc+theta*rhomax*2+rho) >= threshold){
				float thetar = (float)theta*torad;
				float s = sin(thetar);
				float y = s*(float)(rho-rhomax);
				if(y<y0 && y>height*0.01) y0 = y;
				if(y>y1 && y<height*0.99) y1 = y;
			}
		}
	}
	
	//printf("x0 = %f, y0 = %f ; x1 = %f, y1 = %f\n", x0,y0,x1,y1);
	free(acc);

	/*coord[0] = (x0-0.01*width>=0)?(x0-0.01*width):(0);
	coord[1] = (y0-0.01*height>=0)?(y0-0.01*height):(0);
	coord[2] = (x1+0.01*width<width)?(x1+0.01*width):(width-1);
	coord[3] = (y1+0.01*height<height)?(y1+0.01*height):(height-1);
*/


	x0 = (x0-0.01*width>=0)?(x0-0.01*width):(0);
	y0 = (y0-0.01*height>=0)?(y0-0.01*height):(0);
	x1 = (x1+0.01*width<width)?(x1+0.01*width):(width-1);
	y1 = (y1+0.01*height<height)?(y1+0.01*height):(height-1);
	


	SDL_Rect rect;
	rect.x = (int)x0;
	rect.y = (int)y0;
	rect.w = (int)(x1-x0);
	rect.h = (int)(y1-y0);
	
	//if(rect.w>rect.h && (float)rect.h/(float)rect.w < 0.9) rect.h = rect.w;
	//else if(rect.h>rect.w && (float)rect.w/(float)rect.h<0.9) rect.w = rect.h;
	
	//printf("rect.x = %i, rect.y = %i, rect.w = %i, rect.h = %i\n", rect.x, rect.y, rect.w, rect.h);

	SDL_Surface* dilated_r = SDL_CreateRGBSurface(SDL_HWSURFACE, rect.w, rect.h, dilated_surface->format->BitsPerPixel,
			dilated_surface->format->Rmask, dilated_surface->format->Gmask, dilated_surface->format->Bmask, dilated_surface->format->Amask);
	SDL_Surface* save_r = SDL_CreateRGBSurface(SDL_HWSURFACE, rect.w, rect.h, save_surface->format->BitsPerPixel,
			save_surface->format->Rmask, save_surface->format->Gmask, save_surface->format->Bmask, save_surface->format->Amask);
	SDL_BlitSurface(dilated_surface, &rect, dilated_r, NULL);
	SDL_BlitSurface(save_surface, &rect, save_r, NULL);

	SDL_FreeSurface(dilated_surface);
	SDL_FreeSurface(save_surface);
	
	*d_r = dilated_r;
	*s_r = save_r;

}









float histo_mean(int histo[], int length){
	float mean = 0.0;
	for(int i = 0; i<length;i++){
		mean+=(float)histo[i];
	}
	return mean/(float)length;
}

float histo_sigma(int histo[], int length, float mean){
	float sigma = 0.0;
	for(int i = 0; i<length;i++){
		sigma+=(float)(histo[i]*histo[i]);
	}
	return sqrt((sigma/(float)length) - mean*mean);

}

int histo_peaks(int histo[], int length, float t, int best[], int k_max){
	int i = 0, k = 0;
	while(i<length && k<k_max){
		if(histo[i]>t){
			int local = histo[i];
			int j = i+1;
			while(j<length-1 && histo[j]>=local){
				local = histo[j];
				j++;
			}
			best[k] = j;
			i = j+j-i+length*0.05;
			k++;
		}
		i++;
	}
	return k;
}

int mean_dist(int histo[], int length){
	int mean = 0;
	for(int i = 1; i<length; i++){
		mean+=histo[i]-histo[i-1];
	}
	return mean/(length-1);
}

float sigma_dist(int histo[], int length, int mean){
	float sigma = 0.0;
	float d = 0.0;
	for(int i = 0; i<length;i++){
		d = histo[i+1]-histo[i];
		sigma+=(float)(d*d);
	}
	return sqrt((sigma/(float)length) - mean*mean);
}


int histo(SDL_Surface *save_surface, int width, int height, int lines[]){
	int histo_h[height];
	int histo_v[width];
	memset(histo_h, 0, sizeof(histo_h));
	memset(histo_v, 0, sizeof(histo_v));
	
	for(int i = 0; i<width; i++){
		for(int j = 0; j<height; j++){		
			Uint8 r, g, b;
                        Uint32 pixel = get_pixel(save_surface, i, j);
                        SDL_GetRGB(pixel, save_surface->format, &r, &g, &b);
			if(r==255){
				histo_h[j]+=1;
				histo_v[i]+=1;
			}
		}
	}

	/*for(int i = 0; i<width; i++){	
		if(histo_v[i]>0) draw_line(save_surface, i, 0, i, histo_v[i]-1);
	}
	for(int i = 0; i<height; i++){	
		if(histo_h[i]>0) draw_line(save_surface, 0, i, histo_h[i]-1, i);
	}
	update_surface(screen_surface, save_surface);
	wait_for_keypressed();
	*/

	float mean_v = histo_mean(histo_v,width);
	float mean_h = histo_mean(histo_h,height);	

	float sigma_v = histo_sigma(histo_v, width, mean_v);
	float sigma_h = histo_sigma(histo_h, height, mean_h);
	//printf("meanv = %f, sigmav = %f\n", mean_v, sigma_v);
	//printf("meanh = %f, sigmah = %f\n", mean_h, sigma_h);
	/*-------------------------------------------------------*/

	int v = sigma_v>sigma_h;
	//printf("v = %i\n", v);
	int best[16], kb, mdb, sdb, infb, supb, lines0, lines1;
	if(v) {lines0 = 0; lines1 = 1; kb = histo_peaks(histo_v, width, mean_v+sigma_v, best, 16);}
	else {lines0 = 2; lines1 = 3; kb = histo_peaks(histo_h, height, mean_h+sigma_h, best, 16);}
	mdb = mean_dist(best, kb);
	sdb = sigma_dist(best, kb-1, mdb);
	infb = mdb-sdb;
	supb = mdb+sdb;

	int found = 0;
	int i = 0;
	while(!found && i<kb-1){
		int dist = best[i+1]-best[i];
		if(dist>infb && dist<supb){
			lines[lines0] = best[i];
			int j = i+1;
			while(!found){
				if(j == kb-1 || j == i+9) found = 1;
				else{
					dist = best[j+1]-best[j];
					if(dist>infb && dist<supb) j++;
					else{
						if(j==kb-2) found = 1;
						else{
							dist = best[j+2]-best[j+1];
							if(dist>infb && dist<supb){
								j+=2;
							}
							else found = 1;
						}
					}
				}
			}
			lines[lines1] = best[j];
		}
		i++;
	}
	
	int dist = sqrt((lines[lines0]-lines[lines1])*(lines[lines0]-lines[lines1]));
	int bestw[16], kw;
	if(v){
		kw = histo_peaks(histo_h, height, mean_h+sigma_h, bestw, 16);
	}
	else{
		kw = histo_peaks(histo_v, width, mean_v+sigma_v, bestw, 16);
	}
	for(int i = 0; i<kw; i++){
	}
	found = 0;
	i = 0;
	int j = 0;
	while(i<kw-1 && !found){
		j = i+1;
		while(j<kw && !found){
			int distw = bestw[j] - bestw[i];
			if(dist*0.95<distw && distw<dist*1.05) found = 1;
			else j++;
		}
		i++;
	}
	i--;
	if(v){lines[2] = bestw[i]; lines[3] = bestw[j];}
	else{lines[0] = bestw[i]; lines[1] = bestw[j];}
	return dist;

	/*-------------------------------------------------------*/

	/*int best_h[16], best_v[16];
	int kv = histo_peaks(histo_v, width, mean_v+sigma_v, best_v, 16);
	int kh = histo_peaks(histo_h, height, mean_h+sigma_h, best_h, 16);
	for(int i = 0; i<16; i++){	
		printf("best_v[%i] = %i, best_h[%i] = %i\n", i, best_v[i], i, best_h[i]);
	}

	int md_v = mean_dist(best_v, kv);
	int md_h = mean_dist(best_h, kh);
	
	float sd_v = sigma_dist(best_v, kv-1, md_v);
	float sd_h = sigma_dist(best_h, kh-1, md_h);
	
	int inf_v = md_v - sd_v;
	int sup_v = md_v + sd_v;
	int inf_h = md_h - sd_h;
	int sup_h = md_h + sd_h;
	printf("mdh = %i, sd_h = %f, infh = %i, suph = %i\n", md_h, sd_h, inf_h, sup_h);
	printf("mdv = %i, sd_v = %f, infv = %i, supv = %i\n", md_v, sd_v, inf_v, sup_v);

	int found = 0;
	int i = 0;
	while(!found && i<kv-1){
		int dist = best_v[i+1]-best_v[i];
		if(dist>inf_v && dist<sup_v){
			lines[0] = best_v[i];
			int j = i+1;
			while(!found){
				if(j == kv-1 || j == i+9) found = 1;
				else{
					dist = best_v[j+1]-best_v[j];
					if(dist>inf_v && dist<sup_v) j++;
					else{
						if(j==kv-2) found = 1;
						else{
							dist = best_v[j+2]-best_v[j+1];
							if(dist>inf_v && dist<sup_v){
								j+=2;
							}
							else found = 1;
						}
					}
				}
			}
			lines[1] = best_v[j];
		}
		i++;
	}
	i = 0;
	found = 0;
	while(!found && i<kh-1){
		int dist = best_h[i+1]-best_h[i];
		printf("dist %i = %i\n", i, dist);
		if(dist>inf_h && dist<sup_h){
			printf("i = %i\n", i);
			lines[2] = best_h[i];
			int j = i+1;
			while(!found){
				printf("j = %i\n", j);
				if(j == kh-1 || j == i+9) found = 1;
				else{
					dist = best_h[j+1]-best_h[j];
					printf("dist %i = %i\n", j, dist);
					if(dist>inf_h && dist<sup_h) j++;
					else{
						if(j==kh-2) found = 1;
						else{
							dist = best_h[j+2]-best_h[j+1];
							printf("dist %i = %i\n", j+1, dist);
							if(dist>inf_h && dist<sup_h){
								j+=2;
							}
							else found = 1;
						}
					}
				}
			}
			lines[3] = best_h[j];
		}
		i++;
	}*/
	/*while(!found && i<kh-9){
		int dist = best_h[i+1]-best_h[i];
		if(dist>md_h*0.8 && dist<md_h*1.2){
			//printf("i = %i\n", i);
			lines[2] = best_h[i];
			lines[3] = best_h[i+9];
			found = 1;
		}
		i++;
	}*/


}

void cut_squares(SDL_Surface *save_surface, int side, int x0, int y0){
	int c = side/9;
	SDL_Surface* cut_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, c, c, save_surface->format->BitsPerPixel,
                        save_surface->format->Rmask, save_surface->format->Gmask, save_surface->format->Bmask, save_surface->format->Amask);
	for(int i = 0; i<9; i++){
		for(int j = 0; j<9; j++){
			for(int x = 0; x<c; x++){
				for(int y = 0; y<c; y++){
					Uint32 pixel = get_pixel(save_surface, x+x0+c*i, y+y0+c*j);
					put_pixel(cut_surface, x, y, pixel);
				}
			}
			char path[10];
			sprintf(path, "%d.bmp", i+j*9+1);
			//SDL_Surface* resized = resize(cut_surface, c, c, 64, 64);
			save_image(cut_surface, path);
			//save_image(resized, path);
			//SDL_FreeSurface(resized);
		}
	}
	SDL_FreeSurface(cut_surface);

}

int treatment(SDL_Surface *save_surface, SDL_Surface *screen_surface, SDL_Surface* dilated_surface, int width, int height){

	float* ave = malloc(sizeof(float)*2);
	hough(dilated_surface, width, height, ave);
	float hough_angle = ave[0];
	//printf("%f\n", hough_angle);
	SDL_Surface* dilated_r;
	SDL_Surface* save_r;
	//SDL_Surface* dilated_r = dilated_surface;
	//SDL_Surface* save_r = save_surface;

	SDL_Surface* d_r;
	SDL_Surface* s_r;
	if(hough_angle > 5){
		d_r = rotation(dilated_surface, width, height, -hough_angle);
		s_r = rotation(save_surface, width, height, -hough_angle);
		//printf("ave0 = %f ; ave1 = %f\n", ave[0], ave[1]);

		float* ave2 = malloc(sizeof(float)*2);
		hough(d_r, d_r->w, d_r->h, ave2);
		//printf("ave20 = %f ; ave21 = %f\n", ave2[0], ave2[1]);
		if(ave2[0] < 5) {
			SDL_FreeSurface(dilated_surface);
			SDL_FreeSurface(save_surface);
		}
		else{
			SDL_FreeSurface(d_r);
			SDL_FreeSurface(s_r);
			d_r = rotation(dilated_surface, width, height, -ave[1]);
			s_r = rotation(save_surface, width, height, -ave[1]);
			SDL_FreeSurface(dilated_surface);
			SDL_FreeSurface(save_surface);
		}
		free(ave2);


	}else{d_r = dilated_surface; s_r = save_surface;}

	free(ave);

	/*float* coord = malloc(sizeof(float)*4);
	hough_cut(d_r, d_r->w, d_r->h, coord);
	
	SDL_Rect rect;
	rect.x = (int)coord[0];
	rect.y = (int)coord[1];
	rect.w = (int)(coord[2]-coord[0]);
	rect.h = (int)(coord[3]-coord[1]);
	free(coord);
	if(rect.w>rect.h && (float)rect.h/(float)rect.w < 0.9) rect.h = rect.w;
	else if(rect.h>rect.w && (float)rect.w/(float)rect.h<0.9) rect.w = rect.h;
	
	printf("rect.x = %i, rect.y = %i, rect.w = %i, rect.h = %i\n", rect.x, rect.y, rect.w, rect.h);

	dilated_r = SDL_CreateRGBSurface(SDL_HWSURFACE, rect.w, rect.h, d_r->format->BitsPerPixel,
			d_r->format->Rmask, d_r->format->Gmask, d_r->format->Bmask, d_r->format->Amask);
	save_r = SDL_CreateRGBSurface(SDL_HWSURFACE, rect.w, rect.h, s_r->format->BitsPerPixel,
			s_r->format->Rmask, s_r->format->Gmask, s_r->format->Bmask, s_r->format->Amask);
	SDL_BlitSurface(d_r, &rect, dilated_r, NULL);
	SDL_BlitSurface(s_r, &rect, save_r, NULL);
	*/
	SDL_FreeSurface(screen_surface);	
	screen_surface = display_image(d_r);
	wait_for_keypressed();

	hough_cut(d_r, s_r, &d_r, &s_r, d_r->w, d_r->h);
	//hough_cut(d_r, s_r, &d_r, &s_r, d_r->w, d_r->h);


	/*SDL_FreeSurface(d_r);
	SDL_FreeSurface(s_r);*/
	dilated_r = d_r;
	save_r = s_r;

	SDL_FreeSurface(screen_surface);	
	screen_surface = display_image(dilated_r);
	update_surface(screen_surface, dilated_r);
		
	width = dilated_r->w;
	height = dilated_r->h;

	/*} else {
		dilated_r = dilated_surface;
		save_r = save_surface;

	}*/
	wait_for_keypressed();
	int lines[4];
	int side = histo(dilated_r, width, height, lines);
	int x0 = lines[0]; //x1 = lines[1];
	//int x2 = x0, x3 = x1;
	int y0 = lines[2];// y2 = lines[3];
	//int y1 = y0, y3 = y2;
	//printf("x0 = %i ; y0 = %i\n", x0, y0);
	//printf("x1 = %i ; y1 = %i\n", x1, y1);
	//printf("x2 = %i ; y2 = %i\n", x2, y2);
	//printf("x3 = %i ; y3 = %i\n", x3, y3);
	//printf("side = %i\n", side);
	/*draw_line(dilated_r, x0, y0, x1, y1);
	draw_line(dilated_r, x1, y1, x3, y3);
	draw_line(dilated_r, x3, y3, x2, y2);
	draw_line(dilated_r, x0, y0, x2, y2);*/

	draw_line(dilated_r, x0, y0, x0+side, y0);
	draw_line(dilated_r, x0, y0, x0, y0+side);
	draw_line(dilated_r, x0+side, y0, x0+side, y0+side);
	draw_line(dilated_r, x0, y0+side, x0+side, y0+side);

	update_surface(screen_surface, dilated_r);
	wait_for_keypressed();
	cut_squares(save_r, side, x0, y0);

	SDL_FreeSurface(dilated_r);
	SDL_FreeSurface(save_r);
	SDL_FreeSurface(screen_surface);

	return 0;
}
