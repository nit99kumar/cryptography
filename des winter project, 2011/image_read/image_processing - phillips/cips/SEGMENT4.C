
       /***********************************************
       *
       *       file d:\cips\segment4.c
       *
       *       Functions: This file contains
       *
       *
       *
       *
       *
       *
       *
       *       Purpose:
       *
       *       External Calls:
       *          wtiff.c - round_off_image_size
       *                    create_allocate_tiff_file
       *                    write_array_into_tiff_image
       *          tiff.c - read_tiff_header
       *          rtiff.c - read_tiff_image
       *          numcvrt.c - get_integer
       *
       *
       *       Modifications:
       *          5 December 1992 - created
       *
       *************************************************/

#include "cips.h"



     /*******************************************
     *
     *   edm8(..
     *
     *   This function calculates the Euclidean
     *   distance measure for objects in an image.
     *   It calculates the distance from any 
     *   pixel=value to the nearest zero pixel
     *
     *******************************************/


edm8(in_name, out_name, the_image, out_image,
          il, ie, ll, le, value)
   char   in_name[], out_name[];
   int    il, ie, ll, le;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          value;
{
   int    a, b, count, i, j, k;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         out_image[i][j] = 0;

      /***************************
      *
      *   Loop over image array
      *
      ****************************/

   printf("\n");

   for(i=0; i<ROWS; i++){
      if( (i%10) == 0) printf("%3d", i);
      for(j=0; j<COLS; j++){
         if(the_image[i][j] == value)
            out_image[i][j] = distance_8(the_image, i, j, value);
      }  /* ends loop over j */
   }  /* ends loop over i */

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends edm8 */



     /*******************************************
     *
     *   distance_8(..
     *
     *   This function finds the distance from
     *   a pixel to the nearest zero pixel.
     *   It search in all eight directions.
     *
     *******************************************/

distance_8(the_image, a, b, value)
   int   a, b;
   short the_image[ROWS][COLS], value;
{
   int i, j, measuring;
   short dist1  = 0,
         dist2  = 0,
         dist3  = 0,
         dist4  = 0,
         dist5  = 0,
         dist6  = 0,
         dist7  = 0,
         dist8  = 0,
         result = 0;

      /* straight up */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      i--;
      if(i >= 0){
         if(the_image[i][j] == value)
            dist1++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   result = dist1;

      /* straight down */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      i++;
      if(i <= ROWS-1){
         if(the_image[i][j] == value)
            dist2++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   if(dist2 <= result)
      result = dist2;

      /* straight left */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j--;
      if(j >= 0){
         if(the_image[i][j] == value)
            dist3++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   if(dist3 <= result)
      result = dist3;

      /* straight right */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j++;
      if(j <= COLS-1){
         if(the_image[i][j] == value)
            dist4++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   if(dist4 <= result)
      result = dist4;

      /* left and up */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j--;
      i--;
      if(j >= 0 && i>=0){
         if(the_image[i][j] == value)
            dist5++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   dist5 = (dist5*14)/10;
   if(dist5 <= result)
      result = dist5;

      /* right and up */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j++;
      i--;
      if(j <=COLS-1 && i>=0){
         if(the_image[i][j] == value)
            dist6++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   dist6 = (dist6*14)/10;
   if(dist6 <= result)
      result = dist6;

      /* right and down */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j++;
      i++;
      if(j <=COLS-1 && i<=ROWS-1){
         if(the_image[i][j] == value)
            dist7++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   dist7 = (dist7*14)/10;
   if(dist7 <= result)
      result = dist7;

      /* left and down */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j--;
      i++;
      if(j >=0 && i<=ROWS-1){
         if(the_image[i][j] == value)
            dist8++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   dist8 = (dist8*14)/10;
   if(dist8 <= result)
      result = dist8;

   return(result);
}  /* ends distance_8 */




     /*******************************************
     *
     *   alledm8(..
     *
     *
     *******************************************/


alledm8(in_name, out_name, the_image, out_image,
          il, ie, ll, le)
   char   in_name[], out_name[];
   int    il, ie, ll, le;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];
{
   int    a, b, count, i, j, k,
          length, width;
   short  v;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         out_image[i][j] = 0;

      /***************************
      *
      *   Loop over image array
      *
      * If you want to do an EDM for everything
      * object and non object and detected regions etc
      * you start at a point and measure untill you
      * hit anything that != that point
      *
      ****************************/

   printf("\n");

   for(i=0; i<ROWS; i++){
      if( (i%10) == 0) printf("%3d", i);
      for(j=0; j<COLS; j++){
         v               = the_image[i][j];
         out_image[i][j] = distance_8(the_image, i, j, v);
      }  /* ends loop over j */
   }  /* ends loop over i */

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends alledm8 */




     /*******************************************
     *
     *   mat8(..
     *
     *   This function finds the medial axis
     *   transform for objects in an image.
     *   The mat are those points that are
     *   minimally distant to more than one
     *   boundary point.
     *
     *******************************************/


mat8(in_name, out_name, the_image, out_image,
          il, ie, ll, le, value)
   char   in_name[], out_name[];
   int    il, ie, ll, le;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          value;
{
   int    a, b, count, i, j, k,
          length, width;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         out_image[i][j] = 0;

      /***************************
      *
      *   Loop over image array
      *
      ****************************/

   printf("\n");

   for(i=0; i<ROWS; i++){
      if( (i%10) == 0) printf("%3d", i);
      for(j=0; j<COLS; j++){
         if(the_image[i][j] == value)
            out_image[i][j] = mat_d_8(the_image, i, j, value);
      }  /* ends loop over j */
   }  /* ends loop over i */

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends mat8 */



     /*******************************************
     *
     *   mat_d_8(..
     *
     *   This function helps find the medial
     *   axis transform.
     *
     *   This function measures the distances
     *   from the point to a zero pixel in all
     *   eight directions.  Look for the two
     *   shortest distances in the eight distances.
     *   If the two shortest distances are
     *   equal, then the point in question
     *   is minimally distant to more than one
     *   boundary point.  Therefore, it is
     *   on the medial axis so return a value.
     *   Otherwise, return zero.
     *
     *******************************************/

mat_d_8(the_image, a, b, value)
   int   a, b;
   short the_image[ROWS][COLS], value;
{
   int i, j, measuring;
   short dist1  = 0,
         dist2  = 0,
         dist3  = 0,
         dist4  = 0,
         dist5  = 0,
         dist6  = 0,
         dist7  = 0,
         dist8  = 0,
         min1   = 255,
         min2   = 255,
         result = 0;

      /* straight up */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      i--;
      if(i >= 0){
         if(the_image[i][j] == value)
            dist1++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   result = dist1;
   min1   = dist1;

      /* straight down */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      i++;
      if(i <= ROWS-1){
         if(the_image[i][j] == value)
            dist2++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   if(dist2 <= result)
      result = dist2;
   if(dist2 < min1){
      min2 = min1;
      min1 = dist2;
   }
   else
      if(dist2 < min2)
         min2 = dist2;

      /* straight left */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j--;
      if(j >= 0){
         if(the_image[i][j] == value)
            dist3++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   if(dist3 <= result)
      result = dist3;
   if(dist3 < min1){
      min2 = min1;
      min1 = dist3;
   }
   else
      if(dist3 < min2)
         min2 = dist3;

      /* straight right */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j++;
      if(j <= COLS-1){
         if(the_image[i][j] == value)
            dist4++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   if(dist4 <= result)
      result = dist4;
   if(dist4 < min1){
      min2 = min1;
      min1 = dist4;
   }
   else
      if(dist4 < min2)
         min2 = dist4;

      /* left and up */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j--;
      i--;
      if(j >= 0 && i>=0){
         if(the_image[i][j] == value)
            dist5++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   dist5 = ((dist5*14)+7)/10;
   if(dist5 <= result)
      result = dist5;
   if(dist5 < min1){
      min2 = min1;
      min1 = dist5;
   }
   else
      if(dist5 < min2)
         min2 = dist5;

      /* right and up */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j++;
      i--;
      if(j <=COLS-1 && i>=0){
         if(the_image[i][j] == value)
            dist6++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   dist6 = ((dist6*14)+7)/10;
   if(dist6 <= result)
      result = dist6;
   if(dist6 < min1){
      min2 = min1;
      min1 = dist6;
   }
   else
      if(dist6 < min2)
         min2 = dist6;

      /* right and down */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j++;
      i++;
      if(j <=COLS-1 && i<=ROWS-1){
         if(the_image[i][j] == value)
            dist7++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   dist7 = ((dist7*14)+7)/10;
   if(dist7 <= result)
      result = dist7;
   if(dist7 < min1){
      min2 = min1;
      min1 = dist7;
   }
   else
      if(dist7 < min2)
         min2 = dist7;

      /* left and down */
   measuring = 1;
   i = a;
   j = b;
   while(measuring){
      j--;
      i++;
      if(j >=0 && i<=ROWS-1){
         if(the_image[i][j] == value)
            dist8++;
         else
            measuring = 0;
      }
      else
         measuring = 0;
   }  /* ends while measuring */
   dist8 = ((dist8*14)+7)/10;
   if(dist8 <= result)
      result = dist8;
   if(dist8 < min1){
      min2 = min1;
      min1 = dist8;
   }
   else
      if(dist8 < min2)
         min2 = dist8;

   if(min1 == min2)
      result = value;
   else 
      result = 0;

   if(min1 == 0)
      result = 0;

   return(result);
}  /* ends mat_d_8 */
