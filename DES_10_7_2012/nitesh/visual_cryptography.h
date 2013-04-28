#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>

#include "new_image_lib.h"

#define TRUE 1
#define False 0

void encrypt ( image *img )
{
	int i, j, k = 0, l = 0, prob;
	image *img1, *img2;
	bmp_image *img1_bmp, *img2_bmp;
	
	img1 = ( image * ) malloc ( sizeof(image) );
	img2 = ( image * ) malloc ( sizeof(image) );
	
	img1->height = 2 * img->height;
	img1->width = 2 * img->width;
	img1->is_grayscale = img_is_grayscale;
	
	img2->height = 2 * img->height;
	img2->width = 2 * img->width;
	img2->is_grayscale = img->is_grayscale;
	
	img1->c_matrix = ( color_pixel ** ) malloc ( img1->height * sizeof(color_pixel *) );
	img2->c_matrix = ( color_pixel ** ) malloc ( img2->height * sizeof(color_pixel *) );
	
	for ( i = 0; i < img->height; i++ ) {
		img1->c_matrix[ k ] = ( color_pixel * ) malloc ( img1->width * sizeof(color_pixel) );
		img2->c_matrix[ k ] = ( color_pixel * ) malloc ( img2->width * sizeof(color_pixel) );
		img1->c_matrix[ k + 1 ] = ( color_pixel * ) malloc ( img1->width * sizeof(color_pixel) );
		img2->c_matrix[ k + 1 ] = ( color_pixel * ) malloc ( img2->width * sizeof(color_pixel) );
		for ( j = 0; j < img->width; i++ ) {
			//calculating the probabilty
			srand ( time (NULL) );
			prob = rand() % 2;
			
			//if white pixel is encountered
			if ( img->c_matrix[ i ][ j ].r == 0 ) {
				if ( prob == 1 ) {
					img1->c_matrix[ k ][ l ].r = img1->c_matrix[ k ][ l ].g = img1->c_matrix[ k ][ l ].b = 0;
					img1->c_matrix[ k ][ l + 1].r = img1->c_matrix[ k ][ l + 1 ].g = img1->c_matrix[ k ][ l + 1 ].b = 255;
					img1->c_matrix[ k + 1 ][ l ].r = img1->c_matrix[ k + 1 ][ l ].g = img1->c_matrix[ k + 1 ][ l ].b = 255;
					img1->c_matrix[ k + 1 ][ l + 1 ].r = img1->c_matrix[ k + 1 ][ l + 1 ].g = img1->c_matrix[ k + 1 ][ l + 1 ].b = 0;
					
					img2->c_matrix[ k ][ l ].r = img2->c_matrix[ k ][ l ].g = img2->c_matrix[ k ][ l ].b = 0;
					img2->c_matrix[ k][ l + 1 ].r = img2->c_matrix[ k ][ l + 1 ].g = img2->c_matrix[ k ][ l + 1 ].b = 255;
					img2->c_matrix[ k + 1 ][ l ].r = img2->c_matrix[ k + 1 ][ l ].g = img2->c_matrix[ k + 1 ][ l ].b = 255;
					img2->c_matrix[ k + 1 ][ l + 1 ].r = img2->c_matrix[ k + 1 ][ l + 1 ].g = img2->c_matrix[ k + 1 ][ l + 1 ].b = 0;
				}
				else {
					img1->c_matrix[ k ][ l ].r = img1->c_matrix[ k ][ l ].g = img1->c_matrix[ k ][ l ].b = 255;
					img1->c_matrix[ k ][ l + 1 ].r = img1->c_matrix[ k ][ l + 1 ].g = img1->c_matrix[ k ][ l + 1 ].b = 0;
					img1->c_matrix[ k + 1 ][ l ].r = img1->c_matrix[ k + 1 ][ l ].g = img1->c_matrix[ k + 1 ][ l ].b = 0;
					img1->c_matrix[ k + 1 ][ l + 1 ].r = img1->c_matrix[ k + 1 ][ l + 1 ].g = img1->c_matrix[ k + 1 ][ l + 1 ].b = 255;
					
					img2->c_matrix[ k ][ l ].r = img2->c_matrix[ k ][ l ].g = img2->c_matrix[ k ][ l ].b = 255;
					img2->c_matrix[ k ][ l + 1 ].r = img2->c_matrix[ k ][ l + 1 ].g = img2->c_matrix[ k ][ l + 1 ].b = 0;
					img2->c_matrix[ k + 1 ][ l ].r = img2->c_matrix[ k + 1 ][ l ].g = img2->c_matrix[ k +1 ][ l ].b = 0;
					img2->c_matrix[ k + 1 ][ l + 1 ].r = img2->c_matrix[ k + 1 ][ l + 1 ].g = img2->c_matrix[ k + 1 ][ l + 1 ].b = 255;
				}
			}
			
			//if pixel encounterd is black
			if ( img->c_matrix.r == 255 ) {
				if ( prob == 1 ) {
					img1->c_matrix[ k ][ l ].r = img1->c_matrix[ k ][ l ].g = img1->c_matrix[ k ][ l ].b = 0;
					img1->c_matrix[ k ][ l + 1 ].r = img1->c_matrix[ k ][ l + 1 ].g = img1->c_matrix[ k ][ l + 1 ].b = 255;
					img1->c_matrix[ k + 1 ][ l ].r = img1->c_matrix[ k + 1 ][ l ].g = img1->c_matrix[ k + 1 ][ l ].b = 255;
					img1->c_matrix[ k + 1 ][ l + 1 ].r = img1->c_matrix[ k + 1 ][ l + 1 ].g = img1->c_matrix[ k + 1 ][ l + 1 ].b = 0;
					
					img2->c_matrix[ k ][ l ].r = img2->c_matrix[ k ][ l ].g = img2->c_matrix[ k ][ l ].b = 255;
					img2->c_matrix[ k ][ l + 1].r = img2->c_matrix[ k ][ l + 1].g = img2->c_matrix[ k ][ l + 1 ].b = 0;
					img2->c_matrix[ k + 1 ][ l ].r = img2->c_matrix[ k + 1 ][ l ].g = img2->c_matrix[ k + 1 ][ l ].b = 0;
					img2->c_matrix[ k + 1 ][ l + 1 ].r = img2->c_matrix[ k + 1 ][ l + 1 ].g = img2->c_matrix[ k + 1 ][ l + 1 ].b = 255;
				}
				else {
					img1->c_matrix[ k ][ l ].r = img1->c_matrix[ k ][ l ].g = img1->c_matrix[ k ][ l ].b = 255;
					img1->c_matrix[ k ][ l + 1 ].r = img1->c_matrix[ k ][ l + 1 ].g = img1->c_matrix[ k ][ l + 1 ].b = 0;
					img1->c_matrix[ k + 1 ][ l ].r = img1->c_matrix[ k + 1 ][ l ].g = img1->c_matrix[ k + 1 ][ l ].b = 0;
					img1->c_matrix[ k + 1 ][ l + 1 ].r = img1->c_matrix[ k + 1 ][ l + 1 ].g = img1->c_matrix[ k + 1 ][ l + 1 ].b = 255;
					
					img2->c_matrix[ k ][ l ].r = img2->c_matrix[ k ][ l ].g = img2->c_matrix[ k ][ l ].b = 0;
					img2->c_matrix[ k ][ l + 1 ].r = img2->c_matrix[ k ][ l + 1 ].g = img2->c_matrix[ k ][ l + 1 ].b = 255;
					img2->c_matrix[ k + 1 ][ l ].r = img2->c_matrix[ k + 1 ][ l ].g = img2->c_matrix[ k + 1 ][ l ].b = 255;
					img2->c_matrix[ k + 1 ][ l + 1 ].r = img2->c_matrix[ k + 1 ][ l + 1 ].g = img2->c_matrix[ k + 1 ][ l + 1 ].b = 0;
				}
			}
			l += 2;
		}
		k += 2;
		l = 0;
	}
	img1_bmp = bmp_from_image ( img1 );
	img2_bmp = bmp_from_image ( img2 );
	write_bmp ( "img1_bmp.bmp", img1_bmp );
	write_bmp ( "img2_bmp.bmp", img2_bmp );
}

void decrypt ( image *img1, image *img2 )
{
	int i, j, k = 0, l = 0;
	image *img;
	bmp_image *img_bmp;
	
	img = ( image * ) malloc ( sizeof(image) );
	img->height = img1->height;
	img->width = img1->width;
	img->is_grayscale = TRUE;
	
	img->c_matrix = ( color_pixel ** ) malloc ( img->height * sizeof(color_pixel *) );
	for ( i = 0; i < img->height; i++ ) {
		img->c_matrix[ i ] = ( color_pixel * ) malloc ( img->width * sizeof(color_pixel) );
		for ( j =0; j < img->width; j++ ) {
			if ( img1->c_matrix[ i ][ j ].r == img2->c_matrix[ i ][ j ].r ) 
				img->c_matrix[ i ][ j ].r = img->c_matrix[ i ][ j ].g = img->c_matrix[ i ][ j ].b = img1->c_matrix[ i ][ j ].r;
			else
				img->c_matrix[ i ][ j ].r = img->c_matrix[ i ][ j ].g = img->c_matrix[ i ][ j ].b = 255;
		}
	}
	img_bmp = bmp_from_image ( img );
	write_bmp( "img_bmp.bmp", img_bmp );
}
