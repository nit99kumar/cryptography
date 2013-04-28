
    /***********************************************
    *
    *    file d:\cips\hour.c
    *
    *    Functions: This file contains
    *       main
    *       draw_hour_glass
    *       is_inside_the_image
    *
    *    Purpose:
    *       I use this program to create simple
    *       patterns in images for illustrations.
    *
    *    External Calls:
    *       wtiff.c - create_allocate_tiff_file
    *                 write_array_into_tiff_image
    *
    *    Modifications:
    *       14 February 1996 - created
    *
    ***********************************************/

#include "cips.h"




short the_image[ROWS][COLS];

main(argc, argv)
   int argc;
   char *argv[];
{
   int      i, ie, ii, il, 
            j, jj, le, l, ll, size;
   struct   tiff_header_struct image_header;



   if(argc != 3 ){
      printf("\n usage: hour file-name size");
      exit(-1);
   }

   size = atoi(argv[2]);

   image_header.lsb            = 1;
   image_header.bits_per_pixel = 8;
   image_header.image_length   = ROWS;
   image_header.image_width    = COLS;;
   image_header.strip_offset   = 1000;

   create_allocate_tiff_file(argv[1], &image_header, 
                             the_image);

   il     = 1;
   ie     = 1;
   ll     = ROWS+1;
   le     = COLS+1;

   /********************************************/

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
        the_image[i][j] = 0;

   for(i=0; i<(ROWS/size)+1; i++)
      for(j=0; j<(COLS/size)+1; j++)
         draw_hour_glass(the_image, 
                         i*(size-1),
                         j*(size-1),
                         40, 80, size);

   for(i=-1; i<(ROWS/size)+1; i++)
      for(j=-1; j<(COLS/size)+1; j++)
         draw_hour_glass(the_image, 
                         ((size-1)/2)+i*(size-1),
                         ((size-1)/2)+j*(size-1),
                         140, 180, size);


   /********************************************/

   write_array_into_tiff_image(argv[1], the_image,
                               il, ie, ll, le);

}  /* ends main */




draw_hour_glass(the_image, start_row, start_col,
                value1, value2, size)
   int   start_row, start_col, size;
   short the_image[ROWS][COLS], value1, value2;
{
   int col_number,
       gray_shade,
       i, 
       j, 
       j_end,
       j_start,
       row_number;

   for(i=0; i<=(size-1); i++){

      if(i <= (size-1)-i){
         j_start = i;
         j_end   = (size-1) - i;
      }
      else{
         j_end   = i;
         j_start = (size-1) - i;
      }

      for(j=j_start; j <= j_end; j++){

         if(i == 0         ||
            i == (size-1)  ||
            j == j_start   ||
            j == j_end)
            gray_shade = 0;
         else{
            if((j%2) == 0) gray_shade = value1;
            else           gray_shade = value2;
         }  /* ends else gray shade */
         row_number = i + start_row;
         col_number = j + start_col;
         if(is_inside_the_image(row_number, col_number))
            the_image[row_number][col_number] = gray_shade;
      }  /* ends loop over j */
   }  /* ends loop over i */
}  /* ends draw_hour_glass */





int is_inside_the_image(y, x)
    int x, y;
{
   int result = 1;
   if(x < 0     ||
      x >= COLS ||
      y < 0     ||
      y >= ROWS)
         result = 0;

   return(result);

}  /* ends is_inside_the_image */


