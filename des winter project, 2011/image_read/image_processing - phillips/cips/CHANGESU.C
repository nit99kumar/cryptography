
    /***********************************************
    *
    *    file d:\cips\changesub.c
    *
    *    Functions: This file contains
    *       change_pixels
    *
    *    Purpose:
    *       This function changes the values of
    *       pixels in an image based on upper and
    *       and lower thresholds.
    *
    *    External Calls:
    *       wtiff.c - create_file_if_needed
    *       tiff.c - read_tiff_header
    *       rtiff.c - read_tiff_image
    *
    *    Modifications:
    *       20 May 1995 - created
    *
    *************************************************/

#include "cips.h"



   /**************************************************
   *
   *   change_pixels(...
   *
   *   This function changes the pixels in an image
   *   based on upper and lower thresholds.
   *
   ***************************************************/


change_pixels(in_name, out_name, the_image, out_image,
              upper_threshold, lower_threshold, value,
             il, ie, ll, le)
   char   in_name[], out_name[];
   short  the_image[ROWS][COLS], out_image[ROWS][COLS];
   short  lower_threshold, upper_threshold, value;
{
   int i, j;

   struct tiff_header_struct image_header;

   create_file_if_needed(in_name, out_name, out_image);
   read_tiff_header(in_name, &image_header);
   read_tiff_image(in_name, the_image, il, ie, ll, le);

   for(i=0; i<ROWS; i++){
      for(j=0; j<COLS; j++){
         if(the_image[i][j] >= lower_threshold &&
            the_image[i][j] <= upper_threshold)
            out_image[i][j] = value;
         else
            out_image[i][j] = the_image[i][j];
      }  /* ends loop over j */
   }  /* ends loop over i */

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

} /* ends change_pixels */
