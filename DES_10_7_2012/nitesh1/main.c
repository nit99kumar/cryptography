#include <stdio.h>
#include "new_image_lib.h"

int main()
{
	image *img;
	bmp_image *img_bmp;
	
	img_bmp = read_bmp("me1.bmp");
	printf("[0][0].r=%d\n",  img_bmp->t_pixel[0][0].r);
	return 0;
}
