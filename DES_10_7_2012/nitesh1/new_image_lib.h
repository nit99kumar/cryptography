#pragma pack(1)

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "new_bmp_lib.h"

#define TRUE 1
#define False 0

typedef struct color_pixel
{
	unsigned char b, g, r;
}
color_pixel;

typedef struct alpha_channel
{
	unsigned char alpha;
}
alpha_channel;

typedef struct image
{
	unsigned int height;
	unsigned int width;
	char is_grayscale;
	color_pixel **c_matrix;
	alpha_channel ** alpha;
}
image;

/********************************************************************

	converts  a coloured image to a grayscale image
	
********************************************************************/

void color_to_grayscale ( image *c_image )
{
  if ( c_image->is_grayscale )
    return;
  
  int i, j;
  float grayscale;
  c_image->is_grayscale = TRUE;
   
 
  for ( i = 0; i < c_image->height; i++ ) {
    for ( j = 0; j < c_image->width; j++ ) {
      grayscale = (c_image->c_matrix[ i ][ j ].r * 0.2989 + c_image->c_matrix[ i ][ j ].g * 0.587 + c_image->c_matrix[ i ][ j ].b * 0.1140) ;
      c_image->c_matrix[ i ][ j ].r = c_image->c_matrix[ i ][ j ].g = c_image->c_matrix[ i ][ j ].b = (unsigned char) grayscale;
    }
  }
}
/*******************************************************************

	makes a new instance of the type image from bmp_image 
	and returns a pointer to the new image
	
*******************************************************************/

image* image_from_bmp ( bmp_image *bmp )
{
	int i, j, new_width;
	image *img;
	bmp_image *new_bmp;
	img = ( image * ) malloc ( sizeof(image) );
	
	new_bmp = bmp;
	if ( bmp->f_header.offset != 54 )
	  new_bmp = indexed_to_true ( bmp );
	
	img->width = new_bmp->i_header.width;
	img->height = new_bmp->i_header.height;

	img->is_grayscale = FALSE;
	
	img->c_matrix = ( color_pixel ** ) malloc ( new_bmp->i_header.height * sizeof(color_pixel *) );
	for ( i= 0; i < new_bmp->i_header.height; i++ ) {
		img->c_matrix[ i ] = ( color_pixel * ) malloc ( new_bmp->i_header.width * sizeof(color_pixel) );
		for ( j = 0; j < new_bmp->i_header.width; j++ ) {
		  img->c_matrix[ i ][j].r = new_bmp->t_pixel[ i ][j].r  ;
		  img->c_matrix[ i ][j].g = new_bmp->t_pixel[ i ][j].g  ;
		  img->c_matrix[ i ][j].b = new_bmp->t_pixel[ i ][j].b  ;
		}
	}
	return img;
}

/*******************************************************************

	returns a true type bmp_image type file from a image 
	type pointer variable.
	
*******************************************************************/

bmp_image* bmp_from_image ( image *img )
{

	if(img->c_matrix == NULL)
        err("In bmp_from_img in \"my_Image_lib.h\"\nThe input Image is not valid");
	int i, j; 
  bmp_image *new_img;
  
  new_img = ( bmp_image * ) malloc ( sizeof(bmp_image) );
  //copying the file header
  new_img->f_header.m_number[ 0 ] = 'B';
  new_img->f_header.m_number[ 1 ] = 'M';
  new_img->f_header.size = img->height * four_multiple ( img->width ) + 54;
  new_img->f_header.reserved1 = new_img->f_header.reserved2 = 0;
  new_img->f_header.offset = 54;
  
  //copying the image header
  new_img->i_header.h_size = 40;
  new_img->i_header.width = img->width;
  new_img->i_header.height = img->height;
  new_img->i_header.color_planes = 0;
  new_img->i_header.b_pixel = 24;
  new_img->i_header.compression = 0;
  new_img->i_header.size_bitmap = img->height * img->width;
  new_img->i_header.h_resolution = 0;
  new_img->i_header.v_resolution = 0;
  new_img->i_header.colors = 0;
  new_img->i_header.important_colors = 0;
  
  printf("bmp\n %d %d\n", img->height, img->width );
  new_img->t_pixel = ( bmp_color_pixel ** ) malloc ( img->height * sizeof(bmp_color_pixel *) );
  
  for ( i = 0; i < img->height; i++ ) {
   
    new_img->t_pixel[ i ] = ( bmp_color_pixel * ) malloc ( four_multiple( img->width ) * sizeof(bmp_color_pixel) );
    printf("%d\n", i);
    for ( j = 0; j < img->width; j++ ) {
      new_img->t_pixel[ i ][ j ].r = img->c_matrix[ i ][ j ].r;
      new_img->t_pixel[ i ][ j ].g = img->c_matrix[ i ][ j ].g;
      new_img->t_pixel[ i ][ j ].b = img->c_matrix[ i ][ j ].b;
    }
    for ( ; j < four_multiple ( img->width ); j++ ) {
      new_img->t_pixel[ i ][ j ].r = new_img->t_pixel[ i ][ j ].g = new_img->t_pixel[ i ][ j ].b = 0;
    }
  }
  
  return new_img;
}

/*******************************************************************

	retruns a new instance of type image .i.e creates a copy
	
*******************************************************************/

image* image_from_image ( image *img )
{
	int i, j;
	image *img_copy;
	img_copy = ( image * ) malloc ( sizeof(image) );
	
	img_copy->height = img->height;
	img_copy->width = img->width;
	img_copy->is_grayscale = img->is_grayscale;
	
	img_copy->c_matrix = ( color_pixel ** ) malloc ( img->height * sizeof(color_pixel *) );
	for ( i = 0; i < img->height; i++ ) {
		img_copy->c_matrix[ i ] = ( color_pixel *) malloc ( img->width * sizeof(color_pixel) );
		for ( j = 0; j < img->width; j++ ) {
			img_copy->c_matrix[ i ][ j ].r = img->c_matrix[ i ][ j ].r;
			img_copy->c_matrix[ i ][ j ].g = img->c_matrix[ i ][ j ].g;
			img_copy->c_matrix[ i ][ j ].b = img->c_matrix[ i ][ j ].b;
		}
	}
	return img_copy;
}

/*******************************************************************

	creates text file from image using values of red pixels
	
*******************************************************************/

void text_from_image ( image *img , char *output_filename, int th_value )
{
	int i, j;
	FILE *fp;
	char ch1 = '@';
	char ch2 = ' ';
	char ch3 = '\n';
	image *grayscale;
	
	grayscale = image_from_image ( img );
	color_to_grayscale (grayscale );
	
	fp = fopen ( output_filename, "w");
	if ( fp == NULL )
		err ( " error in opening file for writing in text_from_image." );
		
	for ( i = 0; i < grayscale->height; i++ ) {
		for ( j = 0; j < grayscale->width; j++ ) {
			if ( grayscale->c_matrix[ i ][ j ].r < th_value ) 
				fwrite ( &ch1, 1, 1, fp );
			fwrite ( &ch2, 1, 1, fp );
		}
		fwrite ( &ch3, 1, 1, fp );
	}
	fclose ( fp );
}

/********************************************************************

	creates a negative copy of the image

********************************************************************/

void negative_of_image ( image *img )
{
	int i, j;
	unsigned char copy;
	bmp_image *neg_image;
	image *grayscale = image_from_image ( img );
	color_to_grayscale ( grayscale );

	for ( i = 0; i < grayscale->height; i++ ) {
		for ( j = 0; j < grayscale->width; j++ ) {
			copy = grayscale->c_matrix[ i ][ j ].r;
			grayscale->c_matrix[ i ][ j ].r = grayscale->c_matrix[ i ][ j ].g= grayscale->c_matrix[ i ][ j ].r = 255 - copy;
		}
	}
      	printf("gray\n %d, %d\n", grayscale->height, grayscale->width );
	neg_image = bmp_from_image ( grayscale );
	write_bmp ( "neg_image.bmp", neg_image );
}
