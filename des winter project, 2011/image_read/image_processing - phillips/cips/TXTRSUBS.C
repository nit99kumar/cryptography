
    /********************************************
    *
    *    file txtrsubs.c
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
    *       utility.c - fix_edges
    *                   sort_elements
    *       fitt.c - fit
    *
    *    Modifications:
    *       12 August 1993- created
    *       27 August 1998 - modified to work on 
    *            entire images at once.
    *
    ********************************************/

#include "cips.h"



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

sigma(the_image, out_image,
      size, threshold, high,
      rows, cols, bits_per_pixel)
   int    high, threshold, size;
   long   bits_per_pixel, cols, rows;
   short  **the_image,
          **out_image;
{
   int      a, b, count, i, j, k,
            max, mean, new_hi, new_low,
            sd2, sd2p1;
   short    sigma;
   unsigned long diff, variance;

   sd2   = size/2;
   sd2p1 = sd2 + 1;

   max     = 255;
   new_hi  = 250;
   new_low = 16;
   if(bits_per_pixel == 4){
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
   for(i=sd2; i<rows-sd2; i++){
      if( (i%10) == 0) printf("%d ", i);
      for(j=sd2; j<cols-sd2; j++){

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

   fix_edges(out_image, sd2, rows-1, cols-1);

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

skewness(the_image, out_image,
         size, threshold, high,
         rows, cols, bits_per_pixel)
   int    high, threshold, size;
   long   bits_per_pixel, cols, rows;
   short  **the_image,
          **out_image;
{
   int      a, b, count, i, j, k,
            max, mean, new_hi, new_low,
            sd2, sd2p1;
   long     cube;
   short    sigma, skew;
   unsigned long diff, sigma3, variance;

   sd2   = size/2;
   sd2p1 = sd2 + 1;

   max     = 255;
   new_hi  = 250;
   new_low = 16;
   if(bits_per_pixel == 4){
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
   for(i=sd2; i<rows-sd2; i++){
      if( (i%10) == 0) printf("%d ", i);
      for(j=sd2; j<cols-sd2; j++){

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

   fix_edges(out_image, sd2, rows-1, cols-1);

}  /* ends skewness */




     /*******************************************
     *
     *   adifference(..
     *
     *   This function performs the difference
     *   operation for a specified array
     *   in an image file.
     *
     *******************************************/

adifference(the_image, out_image,
            size,
            rows, cols)
   int    size;
   long   cols, rows;
   short  **the_image,
          **out_image;
{
   int      sd2, sd2p1;

   sd2   = size/2;
   sd2p1 = sd2 + 1;

   difference_array(the_image, out_image, 
                    size, rows, cols);

   fix_edges(out_image, sd2, rows-1, cols-1);

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

difference_array(the_image, out_image, 
                 size, rows, cols)
   int    size;
   long   cols, rows;
   short  **the_image,
          **out_image;
{
   int i, j, sd2;
   sd2   = size/2;

   for(i=sd2; i<rows-sd2; i++){
      for(j=sd2; j<cols-sd2; j++){
         out_image[i][j] =
            abs(the_image[i][j] - 
                the_image[i+sd2][j+sd2]);
      }  /* ends loop over j */
   }  /* ends loop over i */

   fix_edges(out_image, sd2, rows-1, cols-1);

}  /* ends difference_array */




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

amean(the_image, out_image,
      size,
      rows, cols, bits_per_pixel)
   int    size;
   long   bits_per_pixel, cols, rows;
   short  **the_image,
          **out_image;
{
   int      a, b, count, i, j, k, max,
            sd2, sd2p1;
   short    pixel;
   unsigned long big;

   sd2   = size/2;
   sd2p1 = sd2 + 1;

   max = 255;
   if(bits_per_pixel == 4){
       max = 16;
   }

      /**************************************
      *
      *   Calculate the gray level difference
      *   array.
      *
      ***************************************/

   difference_array(the_image, out_image, 
                    size, rows, cols);
   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
         the_image[i][j] = out_image[i][j];

      /**************************************
      *
      *   Loop over the image array and
      *   calculate the mean measure.
      *
      ***************************************/

   printf("\n");
   for(i=sd2; i<rows-sd2; i++){
      if( (i%10) == 0) printf("%d ", i);
      for(j=sd2; j<cols-sd2; j++){

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

   fix_edges(out_image, sd2, rows-1, cols-1);

}  /* ends amean */





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

hurst(the_image, out_image,
      size,
      rows, cols, bits_per_pixel)
   int    size;
   long   bits_per_pixel, cols, rows;
   short  **the_image,
          **out_image;

{
   float  x[8], y[8], sig[8];
   float  aa, bb, siga, sigb, chi2, q;
   int    ndata, mwt;

   int    a, b, count, i, j, k,
          new_hi, new_low, length,
          number, sd2, sd2p1, ss, width;
   short  *elements, max, prange;

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
   printf("\nHURST>sd2=%d",sd2);
   if(sd2 < 2) sd2 = 2;
   printf("\nHURST>sd2=%d",sd2);

      /**********************************
      *
      *
      ***********************************/

   max = 255;
   if(bits_per_pixel == 4){
      max = 16;
   }

      /***************************
      *
      *   Loop over image array
      *
      ****************************/

   printf("\n");
   for(i=sd2; i<rows-sd2; i++){
      if( (i%10) == 0) printf("%d ", i);
      for(j=sd2; j<cols-sd2; j++){

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

         if(size == 5  ||  size == 7)
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

   fix_edges(out_image, sd2, rows-1, cols-1);

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

compare(the_image, out_image,
        line, element, size,
        rows, cols, bits_per_pixel)
   int    line, element, size;
   long   bits_per_pixel, cols, rows;
   short  **the_image,
          **out_image;
{
   int      a, b, count, i, j, k, max,
            sd2, sd2p1;
   short    pixel;
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
      *   The calling program read in the 
      *   part of the input image
      *   that contains the line element
      *   portion into the out_image array.  
      *   Copy that into the small array.
      *
      ***************************************/

   for(i=0; i<size; i++)
      for(j=0; j<size; j++)
        small[i][j] = out_image[i][j];

   for(i=0; i<rows; i++)
      for(j=0; j<cols; j++)
        out_image[i][j] = 0;

      /**************************************
      *
      *   Create the output image and read
      *   in the input data.
      *
      ***************************************/

   sd2   = size/2;
   sd2p1 = sd2 + 1;

   max = 255;
   if(bits_per_pixel == 4){
       max = 16;
   }

      /**************************************
      *
      *   Loop over the image array and
      *   calculate the contrast measure.
      *
      ***************************************/

   printf("\n");
   for(i=sd2; i<rows-sd2; i++){
      if( (i%10) == 0) printf("%d ", i);
      for(j=sd2; j<cols-sd2; j++){

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

   fix_edges(out_image, sd2, rows-1, cols-1);

      /**************************************
      *
      *   Free the memory for the
      *   two dimensional small array.
      *
      ***************************************/

   for(i=0; i<size; i++)
      free(small[i]);

}  /* ends compare */

