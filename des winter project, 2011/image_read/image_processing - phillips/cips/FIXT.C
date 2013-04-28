
       /***********************************************
       *
       *   file d:\cips\fixt.c
       *
       *   Functions: This file contains
       *      main
       *
       *   Purpose:
       *      This file contains the main calling
       *      routine to fix thresholded images.
       *
       *   External Calls:
       *      gin.c - get_image_name
       *      numcvrt.c - get_integer
       *                  int_convert
       *      tiff.c - read_tiff_header
       *
       *
       *   Modifications:
       *      24 October 1992 - created
       *
       *************************************************/

#include "cips.h"



short the_image[ROWS][COLS];
short out_image[ROWS][COLS];
unsigned long histogram[GRAY_LEVELS+1];
long slopes[GRAY_LEVELS+1];

main(argc, argv)
   int argc;
   char *argv[];
{

   char name[80], name2[80], response[80];

   int  a, b, count, i, ie, il, j, k, le, length, ll,
        peak1, peak2, size,
        t, type, v, width;
   short background, hi, low, object, value;


   struct   tiff_header_struct image_header;

   my_clear_text_screen();

   if(argc < 4){
    printf("\n\nfixt in-file out-file value ");
    printf("\n");
    exit(0);
   }

   strcpy(name, argv[1]);
   strcpy(name2, argv[2]);
   value = atoi(argv[3]);

   il = 1;
   ie = 1;
   ll = ROWS+1;
   le = COLS+1;

   read_tiff_header(name, &image_header);

   length = (ROWS-10 + image_header.image_length)/ROWS;
   width  = (COLS-10 +image_header.image_width)/COLS;
   count  = 1;
   printf("\nlength=%d  width=%d", length, width);

   create_file_if_needed(name, name2, out_image);

     for(i=0; i<length; i++){
        for(j=0; j<width; j++){
          printf("\nrunning %d of %d", count, length*width);
          count++;
             read_tiff_image(name, the_image,
                  il+i*ROWS, ie+j*COLS,
                  ll+i*ROWS, le+j*COLS);

				 for(a=0; a<ROWS; a++){
				    for(b=0; b<COLS; b++){
					    if(the_image[a][b] != value)
						    the_image[a][b] = 100;
					 }
				 }

             write_array_into_tiff_image(name2, the_image,
                              il+i*ROWS,
                              ie+j*COLS,
                              ll+i*ROWS,
                              le+j*COLS);
        }  /* ends loop over i */
     }  /* ends loop over j */

}  /* ends main */
