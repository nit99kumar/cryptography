#include <stdio.h>
#include <malloc.h>
#include <time.h>

#include "my_image_lib.h"
                  
int main    (
                int argc,
                char **argv
            )
{
    bmp_image   *bitmap_image;
    
    Image       *plain_img;
   
     
    
    bitmap_image = read_bmp("me1.bmp");
    printf("r=%d\n", bitmap_image->t_pixel[0][0].r); 
    return 0;
    
}
