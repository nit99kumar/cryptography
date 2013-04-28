

       /*********************************************
       *
       *       file c:\cips\emboss.c
       *
       *       Functions: This file contains
       *          main
       *          emboss_convolution
       *
       *       Purpose:
       *          This file contains the main calling
       *          routine in an edge detection program.
       *
       *       External Calls:
       *          imageio.c - create_image_file
       *                  read_image_array
       *                  write_image_array
       *                  get_image_size
       *                  allocate_image_array
       *                  free_image_array
       *
       *       Modifications:
       *          2 February 1991 - created
       *          1 January 1993 - added calls to
       *             range and variance functions.
       *          3 October 1998 - modified to work with 
       *             all I O routines in imageio.c.
       *
       ***********************************************/

#include "cips.h"



short emboss_0_mask[3][3] =  {
       {-1,  0,  1},
       {-1,  1,  1},
       {-1,  0,  1} };

short emboss_1_mask[3][3] =  {
       { 0,  1,  1},
       {-1,  1,  1},
       {-1, -1,  0} };

short emboss_2_mask[3][3] =  {
       { 1,  1,  1},
       { 0,  1,  0},
       { 1,  1, -1} };

short emboss_3_mask[3][3] =  {
       { 1,  1,  0},
       { 1,  1, -1},
       { 0, -1, -1} };

short emboss_4_mask[3][3] =  {
       { 1,  0, -1},
       { 1,  1, -1},
       { 1,  0, -1} };

short emboss_5_mask[3][3] =  {
       { 0, -1, -1},
       { 1,  1, -1},
       { 1,  1,  0} };

short emboss_6_mask[3][3] =  {
       {-1, -1, -1},
       { 0,  1,  0},
       { 1,  1,  1} };

short emboss_7_mask[3][3] =  {
       {-1, -1,  0},
       {-1,  1,  1},
       { 0,  1,  1} };



   /***************************
   *
   *   Directions for the masks
   *  3 2 1
   *  4 x 0
   *  5 6 7
   *
   ****************************/





short **the_image;
short **out_image;

main(argc, argv)
   int argc;
   char *argv[];
{

   char in_name[80], out_name[80];
   int  high, i, ie, il, j, le, ll, size,
        threshold, type;
   int  zero, one, two, three, four, five,
        six, seven;
   long bitsperpixel, length, width;



   if(argc < 13  ||  argc > 13){
    printf("\n"
    "\n   usage: emboss in-file out-file"
    " threshold? threshold-value"
    " zero one two three four five"
    " six seven"
    "\n   threshold?   1-threshold on   2-threshold off\n"
    "\n   zero through seven are 1 or 0");
    exit(0);
   }

   strcpy(in_name, argv[1]);
   strcpy(out_name, argv[2]);
   threshold = atoi(argv[3]);
   high      = atoi(argv[4]);
   zero      = atoi(argv[5]);
   one       = atoi(argv[6]);
   two       = atoi(argv[7]);
   three     = atoi(argv[8]);
   four      = atoi(argv[9]);
   five      = atoi(argv[10]);
   six       = atoi(argv[11]);
   seven     = atoi(argv[12]);


   if(does_not_exist(in_name)){
      printf("\nERROR input file %s does not exist",
             in_name);
      exit(0);
   }  /* ends if does_not_exist */

   create_image_file(in_name, out_name);
   get_image_size(in_name, &length, &width);
   get_bitsperpixel(in_name, &bitsperpixel);
   the_image = allocate_image_array(length, width);
   out_image = allocate_image_array(length, width);
   read_image_array(in_name, the_image);


   emboss_convolution(the_image, out_image,
               threshold, high, 
               zero, one, 
               two, three, four,
               five, six, seven,
               length, width,
               bitsperpixel);

   write_image_array(out_name, out_image);
   free_image_array(the_image, length);
   free_image_array(out_image, length);

}  /* ends main  */




  /*******************************************************
  *
  *   emoboss_convolution(...
  *
  *   This function performs convolution between the input
  *   image and 8 3x3 masks.  The result is placed in
  *   the out_image.
  *
  ********************************************************/

emboss_convolution(image, out_image,
                   threshold, high,
                   zero, one, two, three, four,
                   five, six, seven,
                   rows, cols, bitsperpixel)
   short **image,
         **out_image;
   int   high, threshold;
   int    zero, one, two, three, four, five,
          six, seven;
   long   rows, cols, bitsperpixel;
{

   int a, b, i, is_present, j, sum;

   short  max,
          min,
          new_hi,
          new_low;

   new_hi  = 250;
   new_low = 16;
   if(bitsperpixel == 4){
       new_hi  = 10;
       new_low = 3;
   }

   min = 0;
   max = 255;
   if(bitsperpixel == 4)
      max = 16;

     /* clear output image array */
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
         out_image[i][j] = 0;

   printf("\n ");

   for(i=1; i<rows-1; i++){
      for(j=1; j<cols-1; j++){


         /******************************
         *
         *  Look at all 8 directions 
         *  convolve only if the 
         *  direction is set
         *
         ******************************/

         /* 0 direction */
      if(zero){
         sum = 0;
         for(a=-1; a<2; a++){
            for(b=-1; b<2; b++){
               sum = sum + image[i+a][j+b] *
                     emboss_0_mask[a+1][b+1];
            }
         }
            if(sum > max) sum = max;
            if(sum < 0)   sum = 0;
         if(sum > out_image[i][j])
            out_image[i][j]   = sum;
      }  /* ends if zero */


         /* 1 direction */
      if(one){
         sum = 0;
         for(a=-1; a<2; a++){
            for(b=-1; b<2; b++){
               sum = sum + image[i+a][j+b] * 
                     emboss_1_mask[a+1][b+1];
            }
         }
            if(sum > max) sum = max;
            if(sum < 0)   sum = 0;
         if(sum > out_image[i][j])
            out_image[i][j]   = sum;
      }  /* ends if one */


         /* 2 direction */
      if(two){
         sum = 0;
         for(a=-1; a<2; a++){
            for(b=-1; b<2; b++){
               sum = sum + image[i+a][j+b] * 
                     emboss_2_mask[a+1][b+1];
            }
         }
            if(sum > max) sum = max;
            if(sum < 0)   sum = 0;
         if(sum > out_image[i][j])
            out_image[i][j]   = sum;
      }  /* ends if two */


         /* 3 direction */
      if(three){
         sum = 0;
         for(a=-1; a<2; a++){
            for(b=-1; b<2; b++){
               sum = sum + image[i+a][j+b] * 
                     emboss_3_mask[a+1][b+1];
            }
         }
            if(sum > max) sum = max;
            if(sum < 0)   sum = 0;
         if(sum > out_image[i][j])
            out_image[i][j]   = sum;
      }  /* ends if three */


         /* 4 direction */
      if(four){
         sum = 0;
         for(a=-1; a<2; a++){
            for(b=-1; b<2; b++){
               sum = sum + image[i+a][j+b] * 
                     emboss_4_mask[a+1][b+1];
            }
         }
            if(sum > max) sum = max;
            if(sum < 0)   sum = 0;
         if(sum > out_image[i][j])
            out_image[i][j]   = sum;
      }  /* ends if four */


         /* 5 direction */
      if(five){
         sum = 0;
         for(a=-1; a<2; a++){
            for(b=-1; b<2; b++){
               sum = sum + image[i+a][j+b] * 
                     emboss_5_mask[a+1][b+1];
            }
         }
            if(sum > max) sum = max;
            if(sum < 0)   sum = 0;
            /* Correction 12-27-92
               see file header for
               details. */
         if(sum > out_image[i][j])
            out_image[i][j]   = sum;
      }  /* ends if five */


         /* 6 direction */
      if(six){
         sum = 0;
         for(a=-1; a<2; a++){
            for(b=-1; b<2; b++){
               sum = sum + image[i+a][j+b] * 
                     emboss_6_mask[a+1][b+1];
            }
         }
            if(sum > max) sum = max;
            if(sum < 0)   sum = 0;
         if(sum > out_image[i][j])
            out_image[i][j]   = sum;
      }  /* ends if six */


         /* 7 direction */
      if(seven){
         sum = 0;
         for(a=-1; a<2; a++){
            for(b=-1; b<2; b++){
               sum = sum + image[i+a][j+b] * 
                     emboss_7_mask[a+1][b+1];
            }
         }
            if(sum > max) sum = max;
            if(sum < 0)   sum = 0;
         if(sum > out_image[i][j])
            out_image[i][j]   = sum;
      }  /* ends if seven */


      }  /* ends loop over j */
   }  /* ends loop over i */


     /* if desired, threshold the output image */
   if(threshold == 1){
       for(i=0; i<rows; i++){
          for(j=0; j<cols; j++){
             if(out_image[i][j] > high){
                  out_image[i][j] = new_hi;
             }
             else{
                  out_image[i][j] = new_low;
             }
          }
       }
   }  /* ends if threshold == 1 */

}  /* ends emboss_convolution */


