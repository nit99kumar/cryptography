

       /***********************************************
       *
       *       file d:\cips\main422.c
       *
       *       Functions: This file contains
       *          main
       *
       *       Purpose:
       *          This file contains the main calling
       *          routine for a program which shrinks
       *          a 400x400 image down to a 200x200
       *          image.
       *
       *       External Calls:
       *          gin.c - get_image_name
       *          numcvrt.c - get_integer
       *                      int_convert
       *          tiff.c - read_tiff_header
       *          scale.c - shrink_image_array
       *
       *       Modifications:
       *          18 April 1992 - created
       *
       *************************************************/

#include "cips.h"



short the_image[ROWS][COLS];
short out_image[ROWS][COLS];

main(argc, argv)
   int argc;
   char *argv[];
{

   char     method[80], name[80], name2[80];
   int      count, length, width;
   struct   tiff_header_struct image_header;

   my_clear_text_screen();

       /***********************************************
       *
       *       Interpret the command line parameters.
       *
       ************************************************/

   if(argc < 4 || argc > 4){
    printf(
     "\n"
     "\n usage: main422 in-file out-file method"
     "\n        where the in-file should be 400x400"
     "\n        and the out-file will be 200x200"
     "\n        method can be Average, Median, Corner"
     "\n");
    exit(0);
   }

   strcpy(name,   argv[1]);
   strcpy(name2,  argv[2]);
   strcpy(method, argv[3]);

   if(method[0] != 'A' &&
      method[0] != 'a' &&
      method[0] != 'M' &&
      method[0] != 'm' &&
      method[0] != 'C' &&
      method[0] != 'c'){
      printf("\nERROR: Did not enter a valid method"
             "\n       The valid methods are:"
             "\n       Average, Median, Corner");
      exit(-2);
   }

   create_file_if_needed(name, name2, out_image);

       /***********************************************
       *
       *   Read and shrink each 200x200 area of the  
       *   input image and write them to the output  
       *   image.                        
       *
       ************************************************/

   count = 1;

   printf(" %d", count++);
   shrink_image_array(name, name2, the_image,
                      out_image, 1, 1, 101, 101,
                      1, 1, 101, 101,
                      2, method);

   printf(" %d", count++);
   shrink_image_array(name, name2, the_image,
                      out_image, 1, 201, 101, 301,
                      1, 101, 101, 201,
                      2, method);

   printf(" %d", count++);
   shrink_image_array(name, name2, the_image,
                      out_image, 201, 1, 301, 101,
                      101, 1, 201, 101, 
                      2, method);

   printf(" %d", count++);
   shrink_image_array(name, name2, the_image,
                      out_image, 201, 201, 301, 301, 
                      101, 101, 201, 201, 
                      2, method);

}  /* ends main  */
