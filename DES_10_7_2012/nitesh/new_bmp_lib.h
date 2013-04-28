#pragma pack(1)

#define TRUE 1
#define FALSE 0

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

typedef struct file_header
{
    unsigned char m_number[2];
    unsigned int size;
    short reserved1;
    short reserved2;
    unsigned int offset;
}
file_header;



typedef struct image_header
{
    unsigned int h_size;
    unsigned int width;
    unsigned int height;
    short color_planes;
    short b_pixel;
    unsigned int compression;
    unsigned int size_bitmap;
    unsigned int h_resolution;
    unsigned int v_resolution;
    unsigned int colors;
    unsigned int important_colors;
}
 image_header;



typedef struct color_palette 
{
    unsigned char b, g, r, junk;
}
 color_palette;



typedef struct bmp_color_pixel
{
    unsigned char b, g, r;
}
bmp_color_pixel;


typedef struct bmp_image
{ 
    file_header      f_header;
    image_header     i_header;
    color_palette    *c_table;
    bmp_color_pixel  **t_pixel;
    unsigned char    **i_pixel;
}
bmp_image;



void err ( char *string )
{
    printf("\n%s\n\nNow program will now exit.\n", string);
    exit(-1);
}

int four_multiple ( int num )
{
    if((num%4)==0)
       return (num);
    return ((num/4)+1)*4 ;   
}

/**************************************************************************************

	reads a bmp image ( indexed or true ) and returns a pointer of the type bmp_image
	
**************************************************************************************/

bmp_image* read_bmp ( char *input_filename)
{
	int i, index_size = 0, new_width;
	bmp_image *image;
	FILE *fp;
	
	//opening the file
	fp = fopen ( input_filename, "rb" );
	if ( fp == NULL )
		err ( " the input file failed to open while reading. ");

	//reading the file header and checking if it is a valid bitmap image file
	image = (bmp_image *) malloc ( sizeof( bmp_image ) );
	fread ( &image->f_header, sizeof( file_header ), 1, fp );
	if ( image->f_header.m_number[0] != 'B' && image->f_header.m_number[1] != 'M' )
		err ( " this is not a valid image file " );
	
	//reading the image header
	fread ( &image->i_header, sizeof( image_header ), 1, fp );
	
	//checking the image offset
	if ( image->f_header.offset == 1078 )
		index_size=256;
	
	if ( image->f_header.offset == 108 )
		index_size=16;

	new_width = four_multiple ( image->i_header.width );

	//allocating required space and reading the color table

	if ( image->f_header.offset != 54 ) {
	  image->c_table = ( color_palette * ) malloc ( index_size * sizeof(color_palette) );
	  fread ( image->c_table, index_size * 4, 1, fp );
	
	  image->i_pixel = ( unsigned char ** ) malloc ( image->i_header.height * sizeof(unsigned char *) );
	  for ( i = image->i_header.height-1; i>=0; i-- ) {
	    image->i_pixel[ i ] = ( unsigned char * ) malloc ( new_width * sizeof(unsigned char) );
	    fread ( image->i_pixel[ i ], new_width, 1 , fp );
	  }
	}
	
	else {
	  printf("check \n");
	  image->t_pixel = (bmp_color_pixel **) malloc ( image->i_header.height * sizeof(bmp_color_pixel *) );
	  for ( i= image->i_header.height-1; i>=0; i-- ) {
	    image->t_pixel[ i ] = ( bmp_color_pixel * ) malloc ( new_width * sizeof(bmp_color_pixel) );
	    fread ( image->t_pixel[ i ], 3 * new_width, 1, fp );
	  }
	}
	fclose ( fp );
	return image;
}

/****************************************************************************

	returns a true type image from an indexed image

****************************************************************************/

bmp_image* indexed_to_true ( bmp_image  *indexed_image )
{
	//checking if the image is indexed or not
	if ( indexed_image->f_header.offset == 54 )
		return indexed_image;
	
	int i, j, new_width;
	//allocating space and copying values
	bmp_image *true_image;
	true_image = (bmp_image * ) malloc ( sizeof(bmp_image) );
	true_image->f_header = indexed_image->f_header;
	
	new_width = four_multiple ( indexed_image->i_header.width );
	true_image->f_header.size = 54 + indexed_image->i_header.height * 3 * new_width;
	true_image->f_header.offset = 54;
	
	true_image->i_header = indexed_image->i_header;
	true_image->i_header.b_pixel = 24;
	
	true_image->t_pixel = ( bmp_color_pixel ** ) malloc ( true_image->i_header.height * sizeof(bmp_color_pixel *) );
	
	for ( i=0; i<true_image->i_header.height; i++ ) {
		true_image->t_pixel[ i ] = ( bmp_color_pixel * ) malloc ( new_width * sizeof(bmp_color_pixel) );
		for ( j=0; j < true_image->i_header.width; j++ ) {
			true_image->t_pixel[ i ][ j ].b = indexed_image->c_table[ indexed_image->i_pixel[ i ][ j ] ].b;
			true_image->t_pixel[ i ][ j ].g = indexed_image->c_table[ indexed_image->i_pixel[ i ][ j ] ].g;
			true_image->t_pixel[ i ][ j ].r = indexed_image->c_table[ indexed_image->i_pixel[ i ][ j ] ].r;
		}
		for ( ; j < new_width; j++ ) {
			true_image->t_pixel[ i ][ j ].b = true_image->t_pixel[ i ][ j ].g = true_image->t_pixel[ i ][ j ].r = 0;
		}
	}
	return true_image;
}

/****************************************************************************

	writes a bitmap file to another file
	
****************************************************************************/

void write_bmp ( char *output_filename, bmp_image* image )
{
	int i, new_width;
	FILE *fp;
	fp = fopen ( output_filename, "wb" );
	if ( fp == NULL )
		err ( " the file could not be opened for writing in write_bmp." );
	
	//writing file header
	fwrite ( &image->f_header, sizeof(file_header), 1, fp  );
	
	//writing image header
	fwrite ( &image->i_header, sizeof(image_header), 1, fp );
	
	new_width = four_multiple ( image->i_header.width );
	
	if ( image->f_header.offset != 54 ) {
		fwrite ( image->c_table, sizeof(image->c_table), 1, fp );
		for ( i = image->i_header.height-1; i >= 0; i-- ) {
			fwrite ( image->i_pixel[ i ], new_width, 1, fp );
		}
	}
	
	for ( i = image->i_header.height; i >= 0; i-- )
		fwrite ( image->t_pixel[ i ], 3 * new_width, 1, fp );
	
	fclose ( fp );
}
