
       /***********************************************
       *
       *       file d:\cips\roundoff.c
       *
       *       Functions: This file contains
       *          main
       *
       *       Purpose:
       *          This program takes an image file and
       *          rounds if off by writing it to an new
       *          file that has an even multiple of
       *          rows and cols.
       *
       *       External Calls:
       *          tiff.c - read_tiff_header
       *          rtiff.c - read_tiff_image
       *          gin.c - get_image_name
       *          gpcips - get_parameters
       *          numcvrt.c - get_integer
       *          wtiff.c - create_allocate_tiff_file
       *                    write_array_into_tiff_image
       *
       *       Modifications:
       *          31 March 1991 - created
       *          8  May 1993 - Made this program
       *                        command line driven.
       *
       ***********************************************/

#include "cips.h"



short the_image[ROWS][COLS];
short out_image[ROWS][COLS];

main(argc, argv)
   int argc;
   char *argv[];
{
   char     caption[80], name[80], name2[80], rep[80];
   int      count, i, ie, il, j, le, length, ll,
            new_ie, new_il, vertical, width;
   struct   tiff_header_struct image_header;



   my_clear_text_screen();

   if(argc < 5   ||
     (argc > 5  &&  argc < 7)){
      printf("\nusage: roundoff in-image out-image"
          " length width [il ie]"
          "\n"
          "\n       length and width to be multiplied"
          " by ROWS and COLS"
          "\n"
          "\n       If you do not specify il ie"
          " they will be set to 1 1."
          "\n       ll le will always be"
          " il+ROWS and ie+COLS"
          "\n");
      exit(0);
   }

   new_ie = 1;
   new_il = 1;

   strcpy(name,  argv[1]);
   strcpy(name2, argv[2]);
   length = atoi(argv[3]);
   width  = atoi(argv[4]);

   if(argc > 5){
      il = atoi(argv[5]);
      ie = atoi(argv[6]);
      ll = il + ROWS;
      le = ie + COLS;
   }
   else{
      il     = 1;
      ie     = 1;
      ll     = ROWS;
      le     = COLS;
   }


   printf("\nCreating the output image file %s", 
          name2);
   read_tiff_header(name, &image_header);
   image_header.image_length = length*ROWS;
   image_header.image_width  = width*COLS;
   if(image_header.bits_per_pixel == 4)
      image_header.bits_per_pixel = 8;
   create_allocate_tiff_file(name2, &image_header,
                             out_image);

   count = 1;

   for(i=0; i<length; i++){
      for(j=0; j<width; j++){
         printf("\nRunning %d of %d", 
                count, length*width);
         count++;
         read_tiff_image(name, the_image,
                         il + i*ROWS,
                         ie + j*COLS,
                         il + i*ROWS + ROWS,
                         ie + j*COLS + COLS);
         write_array_into_tiff_image(name2, the_image,
                         new_il + i*ROWS,
                         new_ie + j*COLS,
                         new_il + i*ROWS + ROWS,
                         new_ie + j*COLS + COLS);
      }
   }

}  /* ends main */
