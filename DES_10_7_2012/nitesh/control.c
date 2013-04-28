#include <stdio.h>
#include <stdlib.h>

#include "new_image_lib.h"
#include "visual_cryptography.h"

int main ( void )
{
    bmp_image *bmp1, *bmp2, *bmp3;
    image *img1, *img2, *img3;
    char *input_filename1 = "me.bmp";
	char *input_filename2 = "img1_bmp.bmp";
	char *input_filename3 = "img2_bmp.bmp";
  
    bmp1 = read_bmp ( input_filename1 );
	bmp2 = read_bmp ( input_filename2 );
	bmp3 = read_bmp ( input_filename3 );
	
    img1 = image_from_bmp ( bmp1 );
	img2 = image_from_bmp ( bmp2 );
	img3 = image_from_bmp ( bmp3 );

	encrypt ( img1 );
	decrypt ( img2, img3 );
 //   negative_of_image ( img );

    return 0;
}
