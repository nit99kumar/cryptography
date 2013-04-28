
    /***********************************************
    *
    *    file warpsubs.c
    *
    *    Functions: This file contains
    *       warp
    *       warp_loop
    *       bi_warp_loop
    *       object_warp
    *       full_warp_loop
    *       bi_full_warp_loop
    *       get_warp_options
    *
    *    Purpose:
    *       These functions performs different
    *       geometric operations.
    *
    *    External Calls:
    *       geosubs.c - bilinear_interpolate
    *
    *    Modifications:
    *       20 October 1993- created
    *       27 August 1998 - modified to work on 
    *            entire images at once.
    *
    *************************************************/

#include "cips.h"

#define FILL 150


     /*******************************************
     *
     *   warp(..
     *
     *   This routine warps a rowsxcols section
     *   of an image.  The il, ie parameters
     *   specify which rowsxcols section of
     *   the image to warp.  The x_control and
     *   y_control parameters are the control
     *   points inside that section.  Therefore,
     *   x_control and y_control will always be
     *   less the cols and rows.
     *
     *   The point coordinates are for the four
     *   corners of a four side figure.
     *      x1,y1     x2,y2
     *
     *      x4,y4     x3,y3
     *
     *******************************************/

warp(the_image, out_image,
     x_control, y_control,
     bilinear,
     rows, cols)
   int    bilinear, x_control, y_control;
   long   cols, rows;
   short  **the_image,
          **out_image;
{
   int    cols_div_2, extra_x, extra_y, i, j,
          rows_div_2, x1, x2, x3, x4, y1, y2, y3, y4;

   cols_div_2 = cols/2;
   rows_div_2 = rows/2;

      /***********************************
      *
      *   1 - upper left quarter
      *
      ***********************************/

   x1 = 0;
   x2 = cols_div_2;
   x3 = x_control;
   x4 = 0;

   y1 = 0;
   y2 = 0;
   y3 = y_control;
   y4 = rows_div_2;

   extra_x = 0;
   extra_y = 0;

   if(bilinear)
      bi_warp_loop(the_image, out_image,
                   x1, x2, x3, x4,
                   y1, y2, y3, y4,
                   extra_x, extra_y,
                   rows, cols);
   else
      warp_loop(the_image, out_image,
                x1, x2, x3, x4,
                y1, y2, y3, y4,
                extra_x, extra_y,
                rows, cols);

      /***********************************
      *
      *   2 - upper right quarter
      *
      ***********************************/

   x1 = cols_div_2;
   x2 = cols-1;
   x3 = cols-1;
   x4 = x_control;

   y1 = 0;
   y2 = 0;
   y3 = rows_div_2;
   y4 = y_control;

   extra_x = cols_div_2;
   extra_y = 0;


   if(bilinear)
      bi_warp_loop(the_image, out_image,
                   x1, x2, x3, x4,
                   y1, y2, y3, y4,
                   extra_x, extra_y,
                   rows, cols);
   else
      warp_loop(the_image, out_image,
                x1, x2, x3, x4,
                y1, y2, y3, y4,
                extra_x, extra_y,
                rows, cols);

      /***********************************
      *
      *   3 - lower right quarter
      *
      ***********************************/

   x1 = x_control;
   x2 = cols-1;
   x3 = cols-1;
   x4 = cols_div_2;

   y1 = y_control;
   y2 = rows_div_2;
   y3 = rows-1;
   y4 = rows-1;

   extra_x = cols_div_2;
   extra_y = rows_div_2;

   if(bilinear)
      bi_warp_loop(the_image, out_image,
                   x1, x2, x3, x4,
                   y1, y2, y3, y4,
                   extra_x, extra_y,
                   rows, cols);
   else
      warp_loop(the_image, out_image,
                x1, x2, x3, x4,
                y1, y2, y3, y4,
                extra_x, extra_y,
                rows, cols);

      /***********************************
      *
      *   4 - lower left quarter
      *
      ***********************************/

   x1 = 0;
   x2 = x_control;
   x3 = cols_div_2;
   x4 = 0;

   y1 = rows_div_2;
   y2 = y_control;
   y3 = rows-1;
   y4 = rows-1;

   extra_x = 0;
   extra_y = rows_div_2;

   if(bilinear)
      bi_warp_loop(the_image, out_image,
                   x1, x2, x3, x4,
                   y1, y2, y3, y4,
                   extra_x, extra_y,
                   rows, cols);
   else
      warp_loop(the_image, out_image,
                x1, x2, x3, x4,
                y1, y2, y3, y4,
                extra_x, extra_y,
                rows, cols);

}  /* ends warp */





     /*******************************************
     *
     *   warp_loop(..
     *
     *   This routine sets up the coefficients
     *   and loops through a quarter of the
     *   rowsxcols section of the image that
     *   is being warped.
     *
     *******************************************/

warp_loop(the_image, out_image,
          x1, x2, x3, x4,
          y1, y2, y3, y4,
          extra_x, extra_y,
          rows, cols)
   int    extra_x, extra_y,
          x1, x2, x3, x4,
          y1, y2, y3, y4;
   long   cols, rows;
   short  **the_image,
          **out_image;
{
   int    cols_div_2, denom, i, j, rows_div_2,
          xa, xb, xab, x_out, ya, yb, yab, y_out;

   cols_div_2 = cols/2;
   rows_div_2 = rows/2;
   denom      = cols_div_2 * rows_div_2;

      /***********************************
     *
     *   Set up the terms for the
     *   spatial transformation.
     *
     ***********************************/

   xa  = x2 - x1;
   xb  = x4 - x1;
   xab = x1 - x2 + x3 - x4;

   ya  = y2 - y1;
   yb  = y4 - y1;
   yab = y1 - y2 + y3 - y4;

      /***********************************
     *
     *   Loop through a quadrant and
     *   perform the spatial
     *   transformation.
     *
     ***********************************/

      /* NOTE a=j b=i */

   printf("\n");
   for(i=0; i<rows_div_2; i++){
      if( (i%10) == 0) printf("%d ", i);
      for(j=0; j<cols_div_2; j++){

         x_out = x1 + (xa*j)/cols_div_2 +
               (xb*i)/rows_div_2 + (xab*i*j)/(denom);
         y_out = y1 + (ya*j)/cols_div_2 +
               (yb*i)/rows_div_2 + (yab*i*j)/(denom);

         if(x_out < 0       ||
            x_out >= cols   ||
            y_out < 0       ||
            y_out >= rows)
            out_image[i+extra_y][j+extra_x] = FILL;
         else
            out_image[i+extra_y][j+extra_x] =
             the_image[y_out][x_out];

      }  /* ends loop over j */
   }  /* ends loop over i */

}   /* ends warp_loop */





     /*******************************************
     *
     *   bi_warp_loop(..
     *
     *   This routine sets up the coefficients
     *   and loops through a quarter of the
     *   rowsxcols section of the image that
     *   is being warped.
     *
     *   This version of the routine uses bilinear
     *   interpolation to find the gray shades.
     *   It is more accurate than warp_loop,
     *   but takes longer because of the floating
     *   point calculations.
     *
     *******************************************/

bi_warp_loop(the_image, out_image,
             x1, x2, x3, x4,
             y1, y2, y3, y4,
             extra_x, extra_y,
             rows, cols)
   int    extra_x, extra_y,
          x1, x2, x3, x4,
          y1, y2, y3, y4;
   long   cols, rows;
   short  **the_image,
          **out_image;
{
   double cols_div_2, denom, di, dj, rows_div_2,
          xa, xb, xab, x_out, ya, yb, yab, y_out;
   int    i, j;

   cols_div_2 = (double)(cols)/2.0;
   rows_div_2 = (double)(rows)/2.0;
   denom      = cols_div_2 * rows_div_2;

     /***********************************
     *
     *   Set up the terms for the
     *   spatial transformation.
     *
     ***********************************/

   xa  = x2 - x1;
   xb  = x4 - x1;
   xab = x1 - x2 + x3 - x4;

   ya  = y2 - y1;
   yb  = y4 - y1;
   yab = y1 - y2 + y3 - y4;

     /***********************************
     *
     *   Loop through a quadrant and
     *   perform the spatial
     *   transformation.
     *
     ***********************************/

      /* NOTE a=j b=i */

   printf("\n");
   for(i=0; i<rows_div_2; i++){
      if( (i%10) == 0) printf("%d ", i);
      for(j=0; j<cols_div_2; j++){

       di = (double)(i);
       dj = (double)(j);

         x_out = x1 +
                 (xa*dj)/cols_div_2 +
                 (xb*di)/rows_div_2 +
                 (xab*di*dj)/(denom);
         y_out = y1 +
                 (ya*dj)/cols_div_2 +
                 (yb*di)/rows_div_2 +
                 (yab*di*dj)/(denom);

         out_image[i+extra_y][j+extra_x] =
          bilinear_interpolate(the_image, 
                               x_out, y_out, 
                               rows, cols);

      }  /* ends loop over j */
   }  /* ends loop over i */

}   /* ends bi_warp_loop */





     /*******************************************
     *
     *   object_warp(..
     *
     *   This routine warps a rowsxcols section
     *   of an image.  The il, ie parameters
     *   specify which rowsxcols section of
     *   the image to warp.  The x_control and
     *   y_control parameters are the control
     *   points inside that section.  Therefore,
     *   x_control and y_control will always be
     *   less the cols and rows.
     *
     *   The point coordinates are for the four
     *   corners of a four side figure.
     *      x1,y1     x2,y2
     *
     *      x4,y4     x3,y3
     *
     *******************************************/

object_warp(the_image, out_image,
            x1, y1, x2, y2, 
            x3, y3, x4, y4, 
            bilinear, rows, cols)
   int    bilinear, 
          x1, y1, x2, y2, 
          x3, y3, x4, y4;
   long   cols, rows;
   short  **the_image,
          **out_image;
{
   int    extra_x = 0, 
          extra_y = 0;

      /***********************************
      *
      *   Call the warp loop function you
      *   need (with or without bilinear
      *   interpolation).
      *
      ***********************************/

   if(bilinear)
      bi_full_warp_loop(the_image, out_image,
                        x1, x2, x3, x4,
                        y1, y2, y3, y4,
                        extra_x, extra_y,
                        rows, cols);
   else
      full_warp_loop(the_image, out_image,
                     x1, x2, x3, x4,
                     y1, y2, y3, y4,
                     extra_x, extra_y,
                     rows, cols);

}  /* ends object_warp */




     /*******************************************
     *
     *   full_warp_loop(..
     *
     *   This routine sets up the coefficients
     *   and loops through an entire
     *   rowsxcols image that is being warped.
     *
     *******************************************/

full_warp_loop(the_image, out_image,
               x1, x2, x3, x4,
               y1, y2, y3, y4,
               extra_x, extra_y,
               rows, cols)
   int    extra_x, extra_y,
          x1, x2, x3, x4,
          y1, y2, y3, y4;
   long   cols, rows;
   short  **the_image,
          **out_image;
{
   int    cols_div_2, denom, i, j, rows_div_2,
          xa, xb, xab, x_out, ya, yb, yab, y_out;

   denom      = cols * rows;

      /***********************************
     *
     *   Set up the terms for the
     *   spatial transformation.
     *
     ***********************************/

   xa  = x2 - x1;
   xb  = x4 - x1;
   xab = x1 - x2 + x3 - x4;

   ya  = y2 - y1;
   yb  = y4 - y1;
   yab = y1 - y2 + y3 - y4;

      /***********************************
     *
     *   Loop through the image and
     *   perform the spatial
     *   transformation.
     *
     ***********************************/

      /* NOTE a=j b=i */

   printf("\n");
   for(i=0; i<rows; i++){
      if( (i%10) == 0) printf("%d ", i);
      for(j=0; j<cols; j++){

         x_out = x1 + (xa*j)/cols +
               (xb*i)/rows + (xab*i*j)/(denom);
         y_out = y1 + (ya*j)/cols +
               (yb*i)/rows + (yab*i*j)/(denom);

         if(x_out < 0       ||
            x_out >= cols   ||
            y_out < 0       ||
            y_out >= rows)
            out_image[i+extra_y][j+extra_x] = FILL;
         else
            out_image[i+extra_y][j+extra_x] =
             the_image[y_out][x_out];

      }  /* ends loop over j */
   }  /* ends loop over i */

}   /* ends full_warp_loop */





     /*******************************************
     *
     *   bi_full_warp_loop(..
     *
     *   This routine sets up the coefficients
     *   and loops through an entire
     *   rowsxcols image that is being warped.
     *
     *   This version of the routine uses bilinear
     *   interpolation to find the gray shades.
     *   It is more accurate than warp_loop,
     *   but takes longer because of the floating
     *   point calculations.
     *
     *******************************************/

bi_full_warp_loop(the_image, out_image,
                  x1, x2, x3, x4,
                  y1, y2, y3, y4,
                  extra_x, extra_y,
                  rows, cols)
   int    extra_x, extra_y,
          x1, x2, x3, x4,
          y1, y2, y3, y4;
   long   cols, rows;
   short  **the_image,
          **out_image;
{
   double denom, di, dj,
          xa, xb, xab, x_out, ya, yb, yab, y_out;
   int    i, j;

   denom      = cols * rows;

     /***********************************
     *
     *   Set up the terms for the
     *   spatial transformation.
     *
     ***********************************/

   xa  = x2 - x1;
   xb  = x4 - x1;
   xab = x1 - x2 + x3 - x4;

   ya  = y2 - y1;
   yb  = y4 - y1;
   yab = y1 - y2 + y3 - y4;

     /***********************************
     *
     *   Loop through the image and
     *   perform the spatial
     *   transformation.
     *
     ***********************************/

      /* NOTE a=j b=i */

   printf("\n");
   for(i=0; i<rows; i++){
      if( (i%10) == 0) printf("%d ", i);
      for(j=0; j<cols; j++){

         di = (double)(i);
         dj = (double)(j);

         x_out = x1 +
                 (xa*dj)/cols +
                 (xb*di)/rows +
                 (xab*di*dj)/(denom);
         y_out = y1 +
                 (ya*dj)/cols +
                 (yb*di)/rows +
                 (yab*di*dj)/(denom);

         out_image[i+extra_y][j+extra_x] =
          bilinear_interpolate(the_image, 
                               x_out, y_out,
                               rows, cols);

      }  /* ends loop over j */
   }  /* ends loop over i */

}   /* ends bi_full_warp_loop */



