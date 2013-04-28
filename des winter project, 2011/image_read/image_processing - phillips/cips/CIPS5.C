
   /*************************** 
   * 
   *   cips5.c 
   *   COMPOSITE FILE COMPRISING: 
   *   boole.c 
   *   overlay.c 
   *   txtrsubs.c 
   * 
   ***************************\ 




    /***********************************************
    *
    *       file d:\cips\boole.c
    *
    *       Functions: This file contains
    *          and_image
    *          or_image
    *          xor_image
    *          nand_image
    *          nor_image
    *          not_image
    *
    *       Purpose:
    *          These functions implement the basic
    *          Boolean algebra functions AND, OR,
    *          XOR, NAND, NOR, and NOT.
    *
    *       External Calls:
    *          wtiff.c - create_file_if_needed
    *                    write_array_into_tiff_image
    *          tiff.c - read_tiff_header
    *          rtiff.c - read_tiff_image
    *          numcvrt.c - get_integer
    *
    *       Modifications:
    *          3 March 1993 - created
    *
    ***********************************************/


#include "cips.h"



   /**************************************************
   *
   *   and_image(...
   *
   *   This function performs the Boolean AND 
   *   operation.  The output image = in1 AND in2.
   *   This works for 0 non-zero images.  If both
   *   in1 and in2 are non-zero, the output = in1.
   *
   ***************************************************/

and_image(in1_name, in2_name, out_name,
          the_image, out_image,
          il1, ie1, ll1, le1,
          il2, ie2, ll2, le2,
          il3, ie3, ll3, le3)
   char   in1_name[], in2_name[], out_name[];
   int    il1, ie1, ll1, le1,
          il2, ie2, ll2, le2,
          il3, ie3, ll3, le3;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];
{
   int    i, j, length, width;
   struct tiff_header_struct image_header;

   create_file_if_needed(in1_name, out_name, out_image);

   read_tiff_image(in1_name, the_image,
                   il1, ie1, ll1, le1);
   read_tiff_image(in2_name, out_image,
                   il2, ie2, ll2, le2);

   for(i=0; i<ROWS; i++){
      if ( (i%10) == 0) printf(" %d", i);
      for(j=0; j<COLS; j++){
         if( the_image[i][j] != 0   &&
             out_image[i][j] != 0)
             out_image[i][j] = the_image[i][j];
         else
             out_image[i][j] = 0;
      }  /* ends loop over j */
   }  /* ends loop over i */

   write_array_into_tiff_image(out_name, out_image,
                               il3, ie3, ll3, le3);

} /* ends and_image */





   /**************************************************
   *
   *   or_image(...
   *
   *   This function performs the Boolean OR 
   *   operation.  The output image = in1 OR in2.
   *   This works for 0 non-zero images.  If both
   *   in1 and in2 are non-zero, the output = in1.
   *   If in1 is non-zero, the output = in1.
   *   If in1 is zero and in2 is non-zero, the
   *   output = in2.
   *
   ***************************************************/

or_image(in1_name, in2_name, out_name,
         the_image, out_image,
         il1, ie1, ll1, le1,
         il2, ie2, ll2, le2,
         il3, ie3, ll3, le3)
   char  in1_name[], in2_name[], out_name[];
   int   il1, ie1, ll1, le1,
         il2, ie2, ll2, le2,
         il3, ie3, ll3, le3;
   short the_image[ROWS][COLS],
         out_image[ROWS][COLS];
{
   int    i, j, length, width;
   struct tiff_header_struct image_header;

   create_file_if_needed(in1_name, out_name, out_image);

   read_tiff_image(in1_name, the_image,
                   il1, ie1, ll1, le1);
   read_tiff_image(in2_name, out_image,
                   il2, ie2, ll2, le2);

   for(i=0; i<ROWS; i++){
      if ( (i%10) == 0) printf(" %d", i);
      for(j=0; j<COLS; j++){
         if( the_image[i][j] != 0   ||
             out_image[i][j] != 0){
             if(the_image[i][j] != 0)
                out_image[i][j] = the_image[i][j];
             else
                out_image[i][j] = out_image[i][j];
         }
         else
             out_image[i][j] = 0;
      }  /* ends loop over j */
   }  /* ends loop over i */

   write_array_into_tiff_image(out_name, out_image,
                               il3, ie3, ll3, le3);

} /* ends or_image */





   /**************************************************
   *
   *   xor_image(...
   *
   *   This function performs the Boolean XOR 
   *   operation.  The output image = in1 XOR in2.
   *   This works for 0 non-zero images.  If 
   *   in1 is non-zero and in2 is 0, output = in1. If
   *   in2 is non-zero and in1 is 0, output = in2.
   *   If both in1 and in2 are non-zero, output = 0.
   *   If both in1 and in2 are zero, output = 0.
   *
   ***************************************************/

xor_image(in1_name, in2_name, out_name,
          the_image, out_image,
          il1, ie1, ll1, le1,
          il2, ie2, ll2, le2,
          il3, ie3, ll3, le3)
   char   in1_name[], in2_name[], out_name[];
   int    il1, ie1, ll1, le1,
          il2, ie2, ll2, le2,
          il3, ie3, ll3, le3;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];
{
   int    i, j, length, width;
   short  answer;
   struct tiff_header_struct image_header;

   create_file_if_needed(in1_name, out_name, out_image);

   read_tiff_image(in1_name, the_image,
                   il1, ie1, ll1, le1);
   read_tiff_image(in2_name, out_image,
                   il2, ie2, ll2, le2);

   for(i=0; i<ROWS; i++){
      if ( (i%10) == 0) printf(" %d", i);
      for(j=0; j<COLS; j++){
         if( (the_image[i][j] != 0 &&
              out_image[i][j] == 0))
             answer = the_image[i][j];
         if( (the_image[i][j] == 0 &&
              out_image[i][j] != 0))
             answer = out_image[i][j];
         if( (the_image[i][j] == 0 &&
              out_image[i][j] == 0))
             answer = 0;
         if( (the_image[i][j] != 0 &&
              out_image[i][j] != 0))
             answer = 0;
         out_image[i][j] = answer;
      }  /* ends loop over j */
   }  /* ends loop over i */

   write_array_into_tiff_image(out_name, out_image,
                               il3, ie3, ll3, le3);

} /* ends xor_image */





   /**************************************************
   *
   *   nand_image(...
   *
   *   This function performs the Boolean NAND 
   *   operation.  The output image = in1 NAND in2.
   *   This works for 0 non-zero images.  If both
   *   in1 and in2 are non-zero, the output = 0.
   *   Otherwise, the output = value.
   *
   ***************************************************/

nand_image(in1_name, in2_name, out_name,
           the_image, out_image,
           il1, ie1, ll1, le1,
           il2, ie2, ll2, le2,
           il3, ie3, ll3, le3, value)
   char    in1_name[], in2_name[], out_name[];
   int     il1, ie1, ll1, le1,
           il2, ie2, ll2, le2,
           il3, ie3, ll3, le3;
   short   the_image[ROWS][COLS],
           out_image[ROWS][COLS], value;
{
   int    i, j, length, width;
   struct tiff_header_struct image_header;

   create_file_if_needed(in1_name, out_name, out_image);

   read_tiff_image(in1_name, the_image,
                   il1, ie1, ll1, le1);
   read_tiff_image(in2_name, out_image,
                   il2, ie2, ll2, le2);

   for(i=0; i<ROWS; i++){
      if ( (i%10) == 0) printf(" %d", i);
      for(j=0; j<COLS; j++){
         if( the_image[i][j] != 0   &&
             out_image[i][j] != 0)
             out_image[i][j] = 0;
         else
             out_image[i][j] = value;
      }  /* ends loop over j */
   }  /* ends loop over i */

   write_array_into_tiff_image(out_name, out_image,
                               il3, ie3, ll3, le3);

} /* ends nand_image */






   /**************************************************
   *
   *   nor_image(...
   *
   *   This function performs the Boolean NOR 
   *   operation.  The output image = in1 NOR in2.
   *   This works for 0 non-zero images.  If niether
   *   in1 nor in2 are non-zero, the output = value.
   *   That is, if both in1 and in2 are zero, the
   *   output = value.
   *
   ***************************************************/

nor_image(in1_name, in2_name, out_name,
          the_image, out_image,
          il1, ie1, ll1, le1,
          il2, ie2, ll2, le2,
          il3, ie3, ll3, le3, value)
   char   in1_name[], in2_name[], out_name[];
   int    il1, ie1, ll1, le1,
          il2, ie2, ll2, le2,
          il3, ie3, ll3, le3;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS], value;
{
   int    i, j, length, width;
   struct tiff_header_struct image_header;

   create_file_if_needed(in1_name, out_name, out_image);

   read_tiff_image(in1_name, the_image,
                   il1, ie1, ll1, le1);
   read_tiff_image(in2_name, out_image,
                   il2, ie2, ll2, le2);

   for(i=0; i<ROWS; i++){
      if ( (i%10) == 0) printf(" %d", i);
      for(j=0; j<COLS; j++){
         if( the_image[i][j] == 0   &&
             out_image[i][j] == 0)
             out_image[i][j] = value;
         else
             out_image[i][j] = 0;
      }  /* ends loop over j */
   }  /* ends loop over i */

   write_array_into_tiff_image(out_name, out_image,
                               il3, ie3, ll3, le3);

} /* ends nor_image */





   /**************************************************
   *
   *   not_image(...
   *
   *   This function will complement the values of the
   *   input image and put them into the output image.
   *   It will complement using a 0-value scheme where
   *   value is one of the input parameters.
   *
   ***************************************************/

not_image(in_name, out_name, the_image, out_image,
          il, ie, ll, le, value)
   char   in_name[], out_name[];
   int    il, ie, ll, le;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          value;
{
   int    i, j, length, width;
   struct tiff_header_struct image_header;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         out_image[i][j] = value;

   for(i=0; i<ROWS; i++){
      if ( (i%10) == 0) printf(" %d", i);
      for(j=0; j<COLS; j++){
         if(the_image[i][j] == value)
             out_image[i][j] = 0;
      }  /* ends loop over j */
   }  /* ends loop over i */

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

} /* ends not_image */



    /***********************************************
    *
    *       file d:\cips\overlay.c
    *
    *       Functions: This file contains
    *          non_zero_overlay
    *          zero_overlay
    *          greater_overlay
    *          less_overlay
    *          average_overlay
    *
    *       Purpose:
    *          These functions implement the functions
    *          that overlay one image on top of another
    *          image.
    *
    *       External Calls:
    *          wtiff.c - create_file_if_needed
    *                    write_array_into_tiff_image
    *          tiff.c - read_tiff_header
    *          rtiff.c - read_tiff_image
    *          numcvrt.c - get_integer
    *
    *       Modifications:
    *          6 March 1993 - created
    *
    ***********************************************/





   /**************************************************
   *
   *   non_zero_overlay(...
   *
   *   This function overlays in1 on top of in2
   *   and writes the result to the output image.
   *   It writes any non-zero pixel from in1 on top
   *   of in2.
   *
   ***************************************************/

non_zero_overlay(in1_name, in2_name, out_name,
          the_image, out_image,
          il1, ie1, ll1, le1,
          il2, ie2, ll2, le2,
          il3, ie3, ll3, le3)
   char   in1_name[], in2_name[], out_name[];
   int    il1, ie1, ll1, le1,
          il2, ie2, ll2, le2,
          il3, ie3, ll3, le3;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];
{
   int    i, j, length, width;
   struct tiff_header_struct image_header;

   create_file_if_needed(in1_name, out_name, out_image);

   read_tiff_image(in1_name, the_image,
                   il1, ie1, ll1, le1);
   read_tiff_image(in2_name, out_image,
                   il2, ie2, ll2, le2);

   for(i=0; i<ROWS; i++){
      if ( (i%10) == 0) printf(" %d", i);
      for(j=0; j<COLS; j++){
         if(the_image[i][j] != 0)
            out_image[i][j] = the_image[i][j];
      }  /* ends loop over j */
   }  /* ends loop over i */

   write_array_into_tiff_image(out_name, out_image,
                               il3, ie3, ll3, le3);

} /* ends non_zero_overlay */





   /**************************************************
   *
   *   zero_overlay(...
   *
   *   This function overlays in1 on top of in2
   *   and writes the result to the output image.
   *   It writes any zero pixel from in1 on top
   *   of in2.
   *
   ***************************************************/

zero_overlay(in1_name, in2_name, out_name,
         the_image, out_image,
         il1, ie1, ll1, le1,
         il2, ie2, ll2, le2,
         il3, ie3, ll3, le3)
   char  in1_name[], in2_name[], out_name[];
   int   il1, ie1, ll1, le1,
         il2, ie2, ll2, le2,
         il3, ie3, ll3, le3;
   short the_image[ROWS][COLS],
         out_image[ROWS][COLS];
{
   int    i, j, length, width;
   struct tiff_header_struct image_header;

   create_file_if_needed(in1_name, out_name, out_image);

   read_tiff_image(in1_name, the_image,
                   il1, ie1, ll1, le1);
   read_tiff_image(in2_name, out_image,
                   il2, ie2, ll2, le2);

   for(i=0; i<ROWS; i++){
      if ( (i%10) == 0) printf(" %d", i);
      for(j=0; j<COLS; j++){
         if(the_image[i][j] == 0)
            out_image[i][j] = the_image[i][j];
      }  /* ends loop over j */
   }  /* ends loop over i */

   write_array_into_tiff_image(out_name, out_image,
                               il3, ie3, ll3, le3);

} /* ends zero_overlay */





   /**************************************************
   *
   *   greater_overlay(...
   *
   *   This function overlays in1 on top of in2
   *   and writes the result to the output image.
   *   It writes in1 on top of in2 if the value of
   *   in1 is greater than in2.
   *
   ***************************************************/

greater_overlay(in1_name, in2_name, out_name,
         the_image, out_image,
         il1, ie1, ll1, le1,
         il2, ie2, ll2, le2,
         il3, ie3, ll3, le3)
   char  in1_name[], in2_name[], out_name[];
   int   il1, ie1, ll1, le1,
         il2, ie2, ll2, le2,
         il3, ie3, ll3, le3;
   short the_image[ROWS][COLS],
         out_image[ROWS][COLS];
{
   int    i, j, length, width;
   struct tiff_header_struct image_header;

   create_file_if_needed(in1_name, out_name, out_image);

   read_tiff_image(in1_name, the_image,
                   il1, ie1, ll1, le1);
   read_tiff_image(in2_name, out_image,
                   il2, ie2, ll2, le2);

   for(i=0; i<ROWS; i++){
      if ( (i%10) == 0) printf(" %d", i);
      for(j=0; j<COLS; j++){
         if(the_image[i][j] > out_image[i][j])
            out_image[i][j] = the_image[i][j];
      }  /* ends loop over j */
   }  /* ends loop over i */

   write_array_into_tiff_image(out_name, out_image,
                               il3, ie3, ll3, le3);

} /* ends greater_overlay */





   /**************************************************
   *
   *   less_overlay(...
   *
   *   This function overlays in1 on top of in2
   *   and writes the result to the output image.
   *   It writes in1 on top of in2 if the value of
   *   in1 is less than in2.
   *
   ***************************************************/

less_overlay(in1_name, in2_name, out_name,
         the_image, out_image,
         il1, ie1, ll1, le1,
         il2, ie2, ll2, le2,
         il3, ie3, ll3, le3)
   char  in1_name[], in2_name[], out_name[];
   int   il1, ie1, ll1, le1,
         il2, ie2, ll2, le2,
         il3, ie3, ll3, le3;
   short the_image[ROWS][COLS],
         out_image[ROWS][COLS];
{
   int    i, j, length, width;
   struct tiff_header_struct image_header;

   create_file_if_needed(in1_name, out_name, out_image);

   read_tiff_image(in1_name, the_image,
                   il1, ie1, ll1, le1);
   read_tiff_image(in2_name, out_image,
                   il2, ie2, ll2, le2);

   for(i=0; i<ROWS; i++){
      if ( (i%10) == 0) printf(" %d", i);
      for(j=0; j<COLS; j++){
         if(the_image[i][j] < out_image[i][j])
            out_image[i][j] = the_image[i][j];
      }  /* ends loop over j */
   }  /* ends loop over i */

   write_array_into_tiff_image(out_name, out_image,
                               il3, ie3, ll3, le3);

} /* ends less_overlay */





   /**************************************************
   *
   *   average_overlay(...
   *
   *   This function mixes in1 and in2
   *   and writes the result to the output image.
   *   It writes the average of in1 and in2 to the
   *   output image.
   *
   ***************************************************/

average_overlay(in1_name, in2_name, out_name,
         the_image, out_image,
         il1, ie1, ll1, le1,
         il2, ie2, ll2, le2,
         il3, ie3, ll3, le3)
   char  in1_name[], in2_name[], out_name[];
   int   il1, ie1, ll1, le1,
         il2, ie2, ll2, le2,
         il3, ie3, ll3, le3;
   short the_image[ROWS][COLS],
         out_image[ROWS][COLS];
{
   int    i, j, length, width;
   struct tiff_header_struct image_header;

   create_file_if_needed(in1_name, out_name, out_image);

   read_tiff_image(in1_name, the_image,
                   il1, ie1, ll1, le1);
   read_tiff_image(in2_name, out_image,
                   il2, ie2, ll2, le2);

   for(i=0; i<ROWS; i++){
      if ( (i%10) == 0) printf(" %d", i);
      for(j=0; j<COLS; j++){
         out_image[i][j] =
            (the_image[i][j] + out_image[i][j])/2;
      }  /* ends loop over j */
   }  /* ends loop over i */

   write_array_into_tiff_image(out_name, out_image,
                               il3, ie3, ll3, le3);

} /* ends average_overlay */

    /***********************************************
    *
    *    file d:\cips\txtrsubs.c
    *
    *    Functions: This file contains
    *       sigma
    *       skewness
    *       amean
    *       adifference
    *       difference_array
    *       hurst
    *       compare
    *       get_texture_options
    *
    *    Purpose:
    *       These functions calculate measures
    *       that help distinguish textures.
    *
    *    External Calls:
    *       wtiff.c - round_off_image_size
    *                 create_file_if_needed
    *                 write_array_into_tiff_image
    *       tiff.c - read_tiff_header
    *       rtiff.c - read_tiff_image
    *       edge.c - fix_edges
    *       fitt.c - fit
    *       filter.c - sort_elements
    *
    *    Modifications:
    *       12 August 1993- created
    *
    *************************************************/




     /*******************************************
     *
     *   sigma(..
     *
     *   This calculates the variance and the 
     *   sigma for a sizeXsize area.
     *
     *   It sums the squares of the difference
     *   between each pixel and the mean of
     *   the area and divides that by the
     *   number of pixels in the area.
     *
     *   The output image is set to the square
     *   root of the variance since the variance
     *   will almost certainly be out of range
     *   for the image.  The square root of the
     *   variance will be sigma.
     *
     *******************************************/

sigma(in_name, out_name, the_image, out_image,
      il, ie, ll, le, size, threshold, high)
   char   in_name[], out_name[];
   int    il, ie, ll, le,
          high, threshold, size;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];
{
   int      a, b, count, i, j, k,
            max, mean, new_hi, new_low,
            sd2, sd2p1;
   short    sigma;
   struct   tiff_header_struct image_header;
   unsigned long diff, variance;

   sd2   = size/2;
   sd2p1 = sd2 + 1;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

   max     = 255;
   new_hi  = 250;
   new_low = 16;
   if(image_header.bits_per_pixel == 4){
       new_hi  = 10;
       new_low = 3;
       max     = 16;
   }

      /***************************
      *
      *   Loop over image array
      *
      ****************************/

   printf("\n");
   for(i=sd2; i<ROWS-sd2; i++){
      if( (i%10) == 0) printf("%d ", i);
      for(j=sd2; j<COLS-sd2; j++){

            /*****************************
            *
            *   Run through the small area
            *   and calculate the mean.
            *
            ******************************/

         mean = 0;
         for(a=-sd2; a<sd2p1; a++){
            for(b=-sd2; b<sd2p1; b++){
               mean = mean + the_image[i+a][j+b];
            }
         }
         mean = mean/(size*size);

            /*****************************
            *
            *   Run through the small area
            *   again and the calculate the
            *   variance.
            *
            ******************************/

         variance = 0;
         diff     = 0;
         for(a=-sd2; a<sd2p1; a++){
            for(b=-sd2; b<sd2p1; b++){
               diff     = the_image[i+a][j+b] - mean;
               variance = variance + (diff*diff);
            }
         }

         variance = variance/(size*size);
         sigma    = sqrt(variance);
         if(sigma > max) sigma = max;
         out_image[i][j] = sigma;

      }  /* ends loop over j */
   }  /* ends loop over i */


     /* if desired, threshold the output image */
   if(threshold == 1){
       for(i=0; i<ROWS; i++){
          for(j=0; j<COLS; j++){
             if(out_image[i][j] > high){
                  out_image[i][j] = new_hi;
             }
             else{
                  out_image[i][j] = new_low;
             }
          }
       }
   }  /* ends if threshold == 1 */

   fix_edges(out_image, sd2);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);


}  /* ends sigma */





     /*******************************************
     *
     *   skewness(..
     *
     *   This calculates the skewness for a
     *   sizeXsize area.
     *
     *   Look at Levine's book page 449 for
     *   the formula.
     *   "Vision in Man and Machine" by
     *   Martin D. Levine, McGraw Hill, 1985.
     *
     *******************************************/

skewness(in_name, out_name, the_image, out_image,
         il, ie, ll, le, size, threshold, high)
   char   in_name[], out_name[];
   int    il, ie, ll, le,
          high, threshold, size;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];
{
   int      a, b, count, i, j, k,
            max, mean, new_hi, new_low,
            sd2, sd2p1;
   long     cube;
   short    sigma, skew;
   struct   tiff_header_struct image_header;
   unsigned long diff, sigma3, variance;

   sd2   = size/2;
   sd2p1 = sd2 + 1;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

   max     = 255;
   new_hi  = 250;
   new_low = 16;
   if(image_header.bits_per_pixel == 4){
       new_hi  = 10;
       new_low = 3;
       max     = 16;
   }

      /***************************
      *
      *   Loop over image array
      *
      ****************************/

   printf("\n");
   for(i=sd2; i<ROWS-sd2; i++){
      if( (i%10) == 0) printf("%d ", i);
      for(j=sd2; j<COLS-sd2; j++){

            /*****************************
            *
            *   Run through the small area
            *   and calculate the mean.
            *
            ******************************/

         mean = 0;
         for(a=-sd2; a<sd2p1; a++){
            for(b=-sd2; b<sd2p1; b++){
               mean = mean + the_image[i+a][j+b];
            }
         }
         mean = mean/(size*size);

            /*****************************
            *
            *   Run through the small area
            *   again and the calculate the
            *   variance and the cube.
            *
            ******************************/

         variance = 0;
         diff     = 0;
         cube     = 0;
         for(a=-sd2; a<sd2p1; a++){
            for(b=-sd2; b<sd2p1; b++){
               diff     = the_image[i+a][j+b] - mean;
               cube     = cube + (diff*diff*diff);
               variance = variance + (diff*diff);
            }
         }

         variance        = variance/(size*size);
         sigma           = sqrt(variance);
         sigma3          = sigma*sigma*sigma;
         if(sigma3 == 0)
            sigma3       = 1;
         skew            = cube/(sigma3*size*size);
         out_image[i][j] = skew;
         if(out_image[i][j] > max)
            out_image[i][j] = max;

      }  /* ends loop over j */
   }  /* ends loop over i */



     /* if desired, threshold the output image */
   if(threshold == 1){
       for(i=0; i<ROWS; i++){
          for(j=0; j<COLS; j++){
             if(out_image[i][j] > high){
                  out_image[i][j] = new_hi;
             }
             else{
                  out_image[i][j] = new_low;
             }
          }
       }
   }  /* ends if threshold == 1 */

   fix_edges(out_image, sd2);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);


}  /* ends skewness */





     /*******************************************
     *
     *   amean(..
     *
     *   This calculates the mean measure
     *   for a sizeXsize area.
     *
     *   Look at Levine's book page 451 for
     *   the formula.
     *   "Vision in Man and Machine" by
     *   Martin D. Levine, McGraw Hill, 1985.
     *
     *******************************************/

amean(in_name, out_name, the_image, out_image,
          il, ie, ll, le, size)
   char   in_name[], out_name[];
   int    il, ie, ll, le,
          size;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];
{
   int      a, b, count, i, j, k, max,
            sd2, sd2p1;
   short    pixel;
   struct   tiff_header_struct image_header;
   unsigned long big;

   sd2   = size/2;
   sd2p1 = sd2 + 1;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

   max = 255;
   if(image_header.bits_per_pixel == 4){
       max = 16;
   }

      /**************************************
      *
      *   Calculate the gray level difference
      *   array.
      *
      ***************************************/

   difference_array(the_image, out_image, size);
   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         the_image[i][j] = out_image[i][j];

      /**************************************
      *
      *   Loop over the image array and
      *   calculate the mean measure.
      *
      ***************************************/

   printf("\n");
   for(i=sd2; i<ROWS-sd2; i++){
      if( (i%10) == 0) printf("%d ", i);
      for(j=sd2; j<COLS-sd2; j++){

         pixel = 0;
         for(a=-sd2; a<sd2p1; a++){
            for(b=-sd2; b<sd2p1; b++){
               pixel = pixel + the_image[i+a][j+b];
            }
         }
      out_image[i][j] = pixel/(size*size);
      if(out_image[i][j] > max)
         out_image[i][j] = max;

      }  /* ends loop over j */
   }  /* ends loop over i */

   fix_edges(out_image, sd2);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends amean */





     /*******************************************
     *
     *   adifference(..
     *
     *   This function performs the difference
     *   operation for a specified array
     *   in an image file.
     *
     *******************************************/

adifference(in_name, out_name, the_image, out_image,
            il, ie, ll, le, size)
   char   in_name[], out_name[];
   int    il, ie, ll, le,
          size;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];
{
   int      sd2, sd2p1;
   struct   tiff_header_struct image_header;

   sd2   = size/2;
   sd2p1 = sd2 + 1;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

   difference_array(the_image, out_image, size);

   fix_edges(out_image, sd2);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends adifference */





     /*******************************************
     *
     *   difference_array(..
     *
     *   This function takes the input image
     *   array the_image and places in out_image
     *   the gray level differences of the pixels
     *   in the_image.  It uses the size
     *   parameter for the distance between pixels
     *   used to get the difference.
     *
     *******************************************/

difference_array(the_image, out_image, size)
   int    size;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];
{
   int i, j, sd2;
   sd2   = size/2;

   for(i=sd2; i<ROWS-sd2; i++){
      for(j=sd2; j<COLS-sd2; j++){
         out_image[i][j] =
            abs(the_image[i][j] - 
                the_image[i+sd2][j+sd2]);
      }  /* ends loop over j */
   }  /* ends loop over i */

   fix_edges(out_image, sd2);

}  /* ends difference_array */



     /*******************************************
     *
     *   hurst(..
     *
     *   This routine performs the Hurst 
     *   operation as described in "The Image 
     *   Processing Handbook" by John C. Russ
     *   CRC Press 1992.
     *
     *   The following show the definitions of
     *   the pixel classes used in this routine.
     *
     *   3x3 case
     *       c b c
     *     d b a b d
     *       c b c
     *
     *   5x5 case
     *     f e d e f
     *     e c b c e
     *     d b a b d
     *     e c b c e
     *     f e d e f
     *
     *   7x7 case
     *       h g h
     *     f e d e f
     *   h e c b c e h
     *   g d b a b d g
     *   h e c b c e h
     *     f e d e f
     *       h g h
     *
     *******************************************/

hurst(in_name, out_name, the_image, out_image,
      il, ie, ll, le, size)
   char   in_name[], out_name[];
   int    il, ie, ll, le, size;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];

{
   float  x[8], y[8], sig[8];
   float  aa, bb, siga, sigb, chi2, q;
   int    ndata, mwt;

   int    a, b, count, i, j, k,
          new_hi, new_low, length,
          number, sd2, sd2p1, ss, width;
   short  *elements, max, prange;
   struct tiff_header_struct image_header;

      /**********************************************
      *
      *   Initialize the ln's of the distances.
      *   Do this one time to save computations.
      *
      **********************************************/

   x[1] = 0.0;        /* ln(1)        */
   x[2] = 0.34657359; /* ln(sqrt(2))  */
   x[3] = 0.69314718; /* ln(2)        */
   x[4] = 0.80471896; /* ln(sqrt(5))  */
   x[5] = 1.03972077; /* ln(sqrt(8))  */
   x[6] = 1.09861229; /* ln(3)        */
   x[7] = 1.15129255; /* ln(sqrt(10)) */

   sig[1] = 1.0;
   sig[2] = 1.0;
   sig[3] = 1.0;
   sig[4] = 1.0;
   sig[5] = 1.0;
   sig[6] = 1.0;
   sig[7] = 1.0;

   sd2 = size/2;

      /**********************************
      *
      *   Create out file and read
      *   input file.
      *
      ***********************************/

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

   max = 255;
   if(image_header.bits_per_pixel == 4){
      max = 16;
   }

      /***************************
      *
      *   Loop over image array
      *
      ****************************/

   printf("\n");
   for(i=sd2; i<ROWS-sd2; i++){
      if( (i%2) == 0) printf("%d ", i);
      for(j=sd2; j<COLS-sd2; j++){

         for(k=1; k<=7; k++) y[k] = 0.0;

            /*************************************
            *
            *   Go through each pixel class, set
            *   the elements array, sort it, get
            *   the range, and take the ln of the
            *   range.
            *
            *************************************/

            /* b pixel class */
         number      = 4;
         elements    = (short *)
                       malloc(number * sizeof(short));
         elements[0] = the_image[i-1][j];
         elements[1] = the_image[i+1][j];
         elements[2] = the_image[i][j-1];
         elements[3] = the_image[i][j+1];
         sort_elements(elements, &number);
         prange = elements[number-1] - elements[0];
         if(prange < 0)  prange = prange*(-1);
         if(prange == 0) prange = 1;
         y[1] = log(prange);

            /* c pixel class */
         elements[0] = the_image[i-1][j-1];
         elements[1] = the_image[i+1][j+1];
         elements[2] = the_image[i+1][j-1];
         elements[3] = the_image[i-1][j+1];
         sort_elements(elements, &number);
         prange = elements[number-1] - elements[0];
         if(prange < 0)  prange = prange*(-1);
         if(prange == 0) prange = 1;
         y[2] = log(prange);

            /* d pixel class */
         elements[0] = the_image[i-2][j];
         elements[1] = the_image[i+2][j];
         elements[2] = the_image[i][j-2];
         elements[3] = the_image[i][j+2];
         sort_elements(elements, &number);
         prange = elements[number-1] - elements[0];
         if(prange < 0)  prange = prange*(-1);
         if(prange == 0) prange = 1;
         y[3] = log(prange);

            /* f pixel class */
         if(size == 5  ||  size == 7){
         elements[0] = the_image[i-2][j-2];
         elements[1] = the_image[i+2][j+2];
         elements[2] = the_image[i+2][j-2];
         elements[3] = the_image[i-2][j+2];
         sort_elements(elements, &number);
         prange = elements[number-1] - elements[0];
         if(prange < 0)  prange = prange*(-1);
         if(prange == 0) prange = 1;
         y[5] = log(prange);
         }  /* ends if size == 5 */

            /* g pixel class */
         if(size == 7){
         elements[0] = the_image[i-3][j];
         elements[1] = the_image[i+3][j];
         elements[2] = the_image[i][j-3];
         elements[3] = the_image[i][j+3];
         sort_elements(elements, &number);
         prange = elements[number-1] - elements[0];
         if(prange < 0)  prange = prange*(-1);
         if(prange == 0) prange = 1;
         y[6] = log(prange);
         }  /* ends if size == 7 */

         free(elements);

            /* e pixel class */
         if(size == 5  ||  size == 7){
         number      = 8;
         elements    = (short *)
                       malloc(number * sizeof(short));
         elements[0] = the_image[i-1][j-2];
         elements[1] = the_image[i-2][j-1];
         elements[2] = the_image[i-2][j+1];
         elements[3] = the_image[i-1][j+2];
         elements[4] = the_image[i+1][j+2];
         elements[5] = the_image[i+2][j+1];
         elements[6] = the_image[i+2][j-1];
         elements[7] = the_image[i+1][j-2];
         sort_elements(elements, &number);
         prange = elements[number-1] - elements[0];
         if(prange < 0)  prange = prange*(-1);
         if(prange == 0) prange = 1;
         y[4] = log(prange);
         }  /* ends if size == 5 */

            /* h pixel class */
         if(size == 7){
         elements[0] = the_image[i-1][j-3];
         elements[1] = the_image[i-3][j-1];
         elements[2] = the_image[i-3][j+1];
         elements[3] = the_image[i-1][j+3];
         elements[4] = the_image[i+1][j+3];
         elements[5] = the_image[i+3][j+1];
         elements[6] = the_image[i+3][j-1];
         elements[7] = the_image[i+1][j-3];
         sort_elements(elements, &number);
         prange = elements[number-1] - elements[0];
         if(prange < 0)  prange = prange*(-1);
         if(prange == 0) prange = 1;
         y[7] = log(prange);
         }  /* ends if size == 7 */

         free(elements);

            /*************************************
            *
            *   Call the fit routine to fit the
            *   data to a straight line. y=mx+b
            *   The answer you want is the slope
            *   of the line.  That is returned
            *   in the parameter bb.
            *
            *************************************/
         ndata = size;
         mwt   = 1;
         fit(x, y, ndata, sig, mwt, &aa, &bb,
             &siga, &sigb, &chi2, &q);

         out_image[i][j] = (short)(bb*64.0);
         if(out_image[i][j] > max)
            out_image[i][j] = max;
         if(out_image[i][j] < 0)
            out_image[i][j] = 0;

      }  /* ends loop over j */
   }  /* ends loop over i */


   fix_edges(out_image, sd2);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends hurst */





     /*******************************************
     *
     *   compare(..
     *
     *   This function compares a sizeXsize area
     *   starting at line,element in an image
     *   with all the sizeXsize areas in the
     *   image.
     *
     *******************************************/

compare(in_name, out_name, the_image, out_image,
          il, ie, ll, le, line, element, size)
   char   in_name[], out_name[];
   int    il, ie, ll, le,
          line, element, size;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];
{
   int      a, b, count, i, j, k, max,
            sd2, sd2p1;
   short    pixel;
   struct   tiff_header_struct image_header;
   int      big, diff;

      /**************************************
      *
      *   Declare and allocate memory for the
      *   two dimensional small array.
      *
      ***************************************/

   short **small;
   small = malloc(size * sizeof(short  *));
   for(i=0; i<size; i++){
      small[i] = malloc(size * sizeof(short ));
      if(small[i] == '\0'){
         printf("\n\tmalloc of small[%d] failed", i);
         exit(0);
      }
   }

      /**************************************
      *
      *   Read in the part of the input image
      *   that contains the line element
      *   portion.
      *
      ***************************************/

   read_tiff_image(in_name, the_image,
                   line, element,
                   line+size, element+size);

   for(i=0; i<size; i++)
      for(j=0; j<size; j++)
        small[i][j] = the_image[i][j];

      /**************************************
      *
      *   Create the output image and read
      *   in the input data.
      *
      ***************************************/

   sd2   = size/2;
   sd2p1 = sd2 + 1;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

   max = 255;
   if(image_header.bits_per_pixel == 4){
       max = 16;
   }

      /**************************************
      *
      *   Loop over the image array and
      *   calculate the contrast measure.
      *
      ***************************************/

   printf("\n");
   for(i=sd2; i<ROWS-sd2; i++){
      if( (i%10) == 0) printf("%d ", i);
      for(j=sd2; j<COLS-sd2; j++){

         big = 0;
         for(a=-sd2; a<sd2p1; a++){
            for(b=-sd2; b<sd2p1; b++){
            diff = small[a+sd2][b+sd2] -
                   the_image[i+a][j+b];
            big  = big + abs(diff);
            }
         }

      big = big/(size*size);
      out_image[i][j] = big;
      if(out_image[i][j] > max)
         out_image[i][j] = max;

      }  /* ends loop over j */
   }  /* ends loop over i */

   fix_edges(out_image, sd2);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

      /**************************************
      *
      *   Free the memory for the
      *   two dimensional small array.
      *
      ***************************************/

   for(i=0; i<size; i++)
      free(small[i]);

}  /* ends compare */





     /*******************************************
     *
     *   get_texture_options(..
     *
     *   This function queries the user for the
     *   parameters needed to use the texture
     *   operators.
     *
     *******************************************/

get_texture_options(type, threshold, t_value, size, 
                    line, element)
   char type[];
   int  *threshold, *t_value, *size, *line, *element;
{
   int not_finished = 1, response;

   while(not_finished){

      printf("\n");
      printf("\n1. Type of texture operator is %s",
             type);  
      printf("\n   recall type: sigma      skewness");
      printf("\n                amean      adifference");
      printf("\n                hurst      compare");
      printf("\n2. threshold is %d (1=on 2=off)",
             *threshold);
      printf("\n3. threshold value is %d", *t_value);
      printf("\n4. size is %d", *size);
      printf("\n5. line is %d", *line);
      printf("\n6. element is %d\n", *element);
      printf("\n   usage for compare:");
      printf("\n   texture in-file out-file compare ");
      printf("line element size");
      printf("\n\nEnter choice (0 = no change) _\b");

      get_integer(&response);

      if(response == 0){
        not_finished = 0;
      }

      if(response == 1){
         printf("\nEnter type of texture operator");
         printf("\nrecall type: sigma  skewness");
         printf("\n             amean  adifference");
         printf("\n             hurst  compare\n");
         gets(type);
      }

      if(response == 2){
        printf("\n\nEnter threshold output ");
        printf("(1=on 2=off)");
        printf("\n  _\b");
        get_integer(threshold);
      }

      if(response == 3){
        printf("\n\nEnter threshold value");
        printf("\n  _\b");
        get_integer(t_value);
      }

      if(response == 4){
        printf("\n\nEnter size");
        printf("\n  _\b");
        get_integer(size);
      }

      if(response == 5){
        printf("\n\nEnter line");
        printf("\n  _\b");
        get_integer(line);
      }

      if(response == 6){
        printf("\n\nEnter element");
        printf("\n  _\b");
        get_integer(element);
      }

   }  /* ends while not_finished */
}  /* ends get_texture_options */
