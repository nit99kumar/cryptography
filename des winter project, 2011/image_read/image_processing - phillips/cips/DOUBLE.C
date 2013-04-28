

    /***********************************************
    *
    *   file d:\cips\double.c
    *
    *   Functions: This file contains
    *      main
    *
    *   Purpose:
    *      This file contains the main calling
    *      routine for a program which enlarges
    *      an image by a factor of two using the
    *      replication method.
    *
    *   External Calls:
    *      gin.c - get_image_name
    *      numcvrt.c - get_integer
    *                  int_convert
    *      tiff.c - read_tiff_header
    *
    *   Modifications:
    *      7 November 1992 - created
    *
    *************************************************/

#include "cips.h"



short the_image[ROWS][COLS];
short out_image[ROWS][COLS];

main(argc, argv)
   int argc;
   char *argv[];
{

   char     method[80], in_name[80], out_name[80];
   int      a, A, b, B, count, count1, factor,
            i, I, j, J, length, width,
            il, ie, ll, le;
   struct   tiff_header_struct image_header;

   my_clear_text_screen();

       /******************************************
       *
       *  Interpret the command line parameters.
       *
       *******************************************/

   if(argc < 3 || argc > 3){
    printf(
     "\n"
     "\n usage: double in-file out-file"
     "\n        This program doubles (enlarges)"
     "\n        the in-file using replication."
     "\n");
    exit(0);
   }

   strcpy(in_name,   argv[1]);
   strcpy(out_name,  argv[2]);

   il = 1;
   ie = 1;
   ll = ROWS+1;
   le = COLS+1;
   factor = 2;

       /********************************************
       *
       *   Read the input image header and setup
       *   the looping counters.
       *
       *   Create the output image.
       *
       ********************************************/

   read_tiff_header(in_name, &image_header);

   length = (ROWS-10 + image_header.image_length)/ROWS;
   width  = (COLS-10 + image_header.image_width)/COLS;
   count  = 1;
   count1 = 1;

   image_header.image_length = length*ROWS*2;
   image_header.image_width  = width*COLS*2;
   create_allocate_tiff_file(out_name, &image_header,
                             out_image);

       /********************************************
       *
       *   Read and double each 100x100 area of the
       *   input image and write them to the output
       *   image.
       *
       **********************************************/

   count = 1;
   for(I=0; I<length; I++){
      for(J=0; J<width; J++){
         printf("\nrunning %d of %d", 
                count1++, length*width);

         read_tiff_image(in_name, the_image, 
                         il+I*ROWS, ie+J*COLS, 
                         ll+I*ROWS, le+J*COLS);
         count = 1;
         for(A=0; A<factor; A++){
          for(B=0; B<factor; B++){
           for(i=0; i<ROWS/factor; i++){
            for(j=0; j<COLS/factor; j++){
             for(a=0; a<factor; a++){
              for(b=0; b<factor; b++){
               out_image[factor*i+a][factor*j+b] =
               the_image[i+A*ROWS/factor][j+B*COLS/factor];
              }  /* ends loop over b */
             }  /* ends loop over a */
            }  /* ends loop over j */
           }  /* ends loop over i */
           printf("\n\tzooming replication %3d of %3d",
                  count++, factor*factor);
           write_array_into_tiff_image(out_name, 
               out_image, 1+A*ROWS+I*ROWS*factor, 
               1+B*COLS+J*COLS*factor, 
               101+A*ROWS+I*ROWS*factor, 
               101+B*COLS+J*COLS*factor);
          }  /* ends loop over B */
         }  /* ends loop over A */

      }  /* ends loop over J */
   }  /* ends loop over I */
}  /* ends main  */
