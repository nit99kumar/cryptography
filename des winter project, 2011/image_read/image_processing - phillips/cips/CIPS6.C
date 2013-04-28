
   /*************************** 
   * 
   *   cips6.c 
   *   COMPOSITE FILE COMPRISING: 
   *   filter.c 
   *   display.c 
   *   djet.c 
   *   scale.c 
   * 
   ***************************\ 



    /***********************************************
    *
    *    file d:\cips\filter.c
    *
    *    Functions: This file contains
    *       filter_image
    *       median_filter
    *       high_pixel
    *       low_pixel
    *       setup_filters
    *       get_filter_options
    *       median_of
    *       sort_elements
    *       swap
    *
    *    Purpose:
    *       These functions implement several
    *       types of basic spatial frequency
    *       filters.
    *
    *    External Calls:
    *       wtiff.c - round_off_image_size
    *                 create_file_if_needed
    *                 write_array_into_tiff_image
    *       tiff.c - read_tiff_header
    *       rtiff.c - read_tiff_image
    *       numcvrt.c - get_integer
    *
    *
    *    Modifications:
    *       15 February 1992 - created
    *
    *************************************************/

#include "cips.h"


     /*******************************************
     *
     *   Define the filter masks.
     *
     *******************************************/

short lpf_filter_6[3][3] =
   { {0, 1, 0},
     {1, 2, 1},
     {0, 1, 0}};

short lpf_filter_9[3][3] =
   { {1, 1, 1},
     {1, 1, 1},
     {1, 1, 1}};

short lpf_filter_10[3][3] =
   { {1, 1, 1},
     {1, 2, 1},
     {1, 1, 1}};

short lpf_filter_16[3][3] =
   { {1, 2, 1},
     {2, 4, 2},
     {1, 2, 1}};

short lpf_filter_32[3][3] =
   { {1,  4, 1},
     {4, 12, 4},
     {1,  4, 1}};

short hpf_filter_1[3][3] =
   { { 0, -1,  0},
     {-1,  5, -1},
     { 0, -1,  0}};

short hpf_filter_2[3][3] =
   { {-1, -1, -1},
     {-1,  9, -1},
     {-1, -1, -1}};

short hpf_filter_3[3][3] =
   { { 1, -2,  1},
     {-2,  5, -2},
     { 1, -2,  1}};





     /*******************************************
     *
     *   filter_image(...
     *
     *   This function filters an image by using
     *   a single 3x3 mask.
     *
     *******************************************/


filter_image(in_name, out_name, the_image, out_image,
             il, ie, ll, le, filter, type)
   char   in_name[], out_name[];
   int    il, ie, ll, le, type;
   short  filter[3][3],
          the_image[ROWS][COLS],
          out_image[ROWS][COLS];

{
   int    a, b, d, i, j, k,
          length, max, sum, width;
   struct tiff_header_struct image_header;


   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_header(in_name, &image_header);

   d = type;
   if(type == 2 || type == 3) d = 1;

   max = 255;
   if(image_header.bits_per_pixel == 4)
      max = 16;

   read_tiff_image(in_name, the_image, il, ie, ll, le);

         /* Do convolution over image array */
   printf("\n");
   for(i=1; i<ROWS-1; i++){
      if( (i%10) == 0) printf("%d ", i);
      for(j=1; j<COLS-1; j++){
         sum = 0;
         for(a=-1; a<2; a++){
            for(b=-1; b<2; b++){
               sum = sum +
                     the_image[i+a][j+b] *
                     filter[a+1][b+1];
            }
         }
         sum               = sum/d;
         if(sum < 0)   sum = 0;
         if(sum > max) sum = max;
         out_image[i][j]   = sum;

      }  /* ends loop over j */
   }  /* ends loop over i */

   fix_edges(out_image, 1);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends filter_image */




     /*******************************************
     *
     *   high_pixel(..
     *
     *   This function replaces the pixel at
     *   the center of a 3x3, 5x5, etc. area
     *   with the max for that area.
     *
     *******************************************/

high_pixel(in_name, out_name, the_image, out_image,
           il, ie, ll, le, size)
   char   in_name[], out_name[];
   int    il, ie, ll, le, size;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];
{
   int    a, b, count, i, j, k,
          length, sd2, sd2p1, ss, width;
   short  *elements;
   struct tiff_header_struct image_header;

   sd2   = size/2;
   sd2p1 = sd2 + 1;

      /**********************************************
      *
      *   Allocate the elements array large enough
      *   to hold size*size shorts.
      *
      **********************************************/

   ss       = size*size;
   elements = (short *) malloc(ss * sizeof(short));

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

      /***************************
      *
      *   Loop over image array
      *
      ****************************/

   printf("\n");
   for(i=sd2; i<ROWS-sd2; i++){
      if( (i%10) == 0) printf("%d ", i);
      for(j=sd2; j<COLS-sd2; j++){
         count = 0;
         for(a=-sd2; a<sd2p1; a++){
            for(b=-sd2; b<sd2p1; b++){
               elements[count] = the_image[i+a][j+b];
               count++;
            }
         }
         sort_elements(elements, &ss);
         out_image[i][j] = elements[ss-1];
      }  /* ends loop over j */
   }  /* ends loop over i */

   fix_edges(out_image, sd2);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

   free(elements);

}  /* ends high_pixel */




     /*******************************************
     *
     *   low_pixel(..
     *
     *   This function replaces the pixel at
     *   the center of a 3x3, 5x5, etc. area
     *   with the min for that area.
     *
     *******************************************/


low_pixel(in_name, out_name, the_image, out_image,
          il, ie, ll, le, size)
   char   in_name[], out_name[];
   int    il, ie, ll, le, size;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];

{
   int    a, b, count, i, j, k,
          length, sd2, sd2p1, ss, width;
   short  *elements;
   struct tiff_header_struct image_header;

   sd2   = size/2;
   sd2p1 = sd2 + 1;

      /**********************************************
      *
      *   Allocate the elements array large enough
      *   to hold size*size shorts.
      *
      **********************************************/

   ss       = size*size;
   elements = (short *) malloc(ss * sizeof(short));

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

      /***************************
      *
      *   Loop over image array
      *
      ****************************/

   printf("\n");
   for(i=sd2; i<ROWS-sd2; i++){
      if( (i%10) == 0) printf("%d ", i);
      for(j=sd2; j<COLS-sd2; j++){
         count = 0;
         for(a=-sd2; a<sd2p1; a++){
            for(b=-sd2; b<sd2p1; b++){
               elements[count] = the_image[i+a][j+b];
               count++;
            }
         }
         sort_elements(elements, &ss);
         out_image[i][j] = elements[0];
      }  /* ends loop over j */
   }  /* ends loop over i */

   fix_edges(out_image, sd2);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

   free(elements);

}  /* ends low_pixel */




     /*******************************************
     *
     *   median_filter(..
     *
     *   This function performs a median filter
     *   on an image using a size (3x3, 5x5, etc.)
     *   specified in the call.
     *
     *******************************************/

median_filter(in_name, out_name, the_image, out_image,
              il, ie, ll, le, size)
   char   in_name[], out_name[];
   int    il, ie, ll, le, size;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];

{
   int    a, b, count, i, j, k,
          length, sd2, sd2p1, ss, width;
   short  *elements;
   struct tiff_header_struct image_header;

   sd2   = size/2;
   sd2p1 = sd2 + 1;

      /**********************************************
      *
      *   Allocate the elements array large enough
      *   to hold size*size shorts.
      *
      **********************************************/

   ss       = size*size;
   elements = (short *) malloc(ss * sizeof(short));

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

      /***************************
      *
      *   Loop over image array
      *
      ****************************/

   printf("\n");
   for(i=sd2; i<ROWS-sd2; i++){
      if( (i%10) == 0) printf("%d ", i);
      for(j=sd2; j<COLS-sd2; j++){
         count = 0;
         for(a=-sd2; a<sd2p1; a++){
            for(b=-sd2; b<sd2p1; b++){
               elements[count] = the_image[i+a][j+b];
               count++;
            }
         }
         out_image[i][j] = median_of(elements, &ss);
      }  /* ends loop over j */
   }  /* ends loop over i */

   fix_edges(out_image, sd2);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

   free(elements);

}  /* ends median_filter */



    /***********************************************
    *
    *    median_of(...
    *
    *    This function finds and returns the
    *    median value of the elements array.
    *
    *    As a side result, it also sorts the
    *    elements array.
    *
    ***********************************************/

median_of(elements, count)
   int   *count;
   short elements[];
{
   short median;

   sort_elements(elements, count);
   median = elements[*count/2];
   return(median);
}  /* ends median_of */



    /***********************************************
    *
    *    sort_elements(...
    *
    *    This function performs a simple bubble
    *    sort on the elements from the median
    *    filter.
    *
    ***********************************************/

sort_elements(elements, count)
   int   *count;
   short elements[];
{
   int i, j;
   j = *count;
   while(j-- > 1){
      for(i=0; i<j; i++){
         if(elements[i] > elements[i+1])
            swap(&elements[i], &elements[i+1]);
      }
   }
}  /* ends sort_elements */



    /***********************************************
    *
    *    swap(...
    *
    *    This function swaps two shorts.
    *
    ***********************************************/

swap(a, b)
   short *a, *b;
{
   short temp;
   temp  = *a;
   *a    = *b;
   *b    = temp;
}  /* ends swap */



    /************************************************
    *
    *    setup_filters(...
    *
    *    This function copies the filter mask
    *    values defined at the top of this file
    *    into the filter array.
    *
    ************************************************/


setup_filters(filter_type, low_high, filter)
   char   low_high[];
   int    filter_type;
   short  filter[3][3];
{
   int i, j;

   if(low_high[0] == 'l'  || low_high[0] =='L'){
      if(filter_type == 6){
         for(i=0; i<3; i++){
           for(j=0; j<3; j++){
             filter[i][j] = lpf_filter_6[i][j];
           }
         }
      }  /* ends if filter_type == 6 */

      if(filter_type == 9){
         for(i=0; i<3; i++){
           for(j=0; j<3; j++){
             filter[i][j] = lpf_filter_9[i][j];
           }
         }
      }  /* ends if filter_type == 9 */

      if(filter_type == 10){
         for(i=0; i<3; i++){
           for(j=0; j<3; j++){
             filter[i][j] = lpf_filter_10[i][j];
           }
         }
      }  /* ends if filter_type == 10 */


      if(filter_type == 16){
         for(i=0; i<3; i++){
           for(j=0; j<3; j++){
             filter[i][j] = lpf_filter_16[i][j];
           }
         }
      }  /* ends if filter_type == 16 */


      if(filter_type == 32){
         for(i=0; i<3; i++){
           for(j=0; j<3; j++){
             filter[i][j] = lpf_filter_32[i][j];
           }
         }
      }  /* ends if filter_type == 32 */
   }  /* ends low pass filter */




   if(low_high[0] == 'h'  || low_high[0] =='H'){
      if(filter_type == 1){
         for(i=0; i<3; i++){
           for(j=0; j<3; j++){
             filter[i][j] = hpf_filter_1[i][j];
           }
         }
      }  /* ends if filter_type == 1 */

      if(filter_type == 2){
         for(i=0; i<3; i++){
           for(j=0; j<3; j++){
             filter[i][j] = hpf_filter_2[i][j];
           }
         }
      }  /* ends if filter_type == 2 */

      if(filter_type == 3){
         for(i=0; i<3; i++){
           for(j=0; j<3; j++){
             filter[i][j] = hpf_filter_3[i][j];
           }
         }
      }  /* ends if filter_type == 3 */
   }  /* ends high pass filter */

}  /* ends setup_filters */



    /***********************************************
    *
    *    get_filter_options(...
    *
    *    This function queries the user for the
    *    parameters needed to perform filtering.
    *
    ***********************************************/


get_filter_options(filter_type, low_high)
    char low_high[];
    int  *filter_type;
{
    int not_finished, response;
    not_finished = 1;
    while(not_finished){

      printf("\nThe filter options are:\n");
      printf("\n\t1. Type of filter is %d",
              *filter_type);
      printf(
         "\n\t      (6, 9, 10, 16, 32 for low pass)");
      printf("\n\t      (1, 2, 3 for high pass)");
      printf("\n\t2. Low or High Pass filter is %s",
             low_high);
      printf("\n\t      l=low pass h=high pass "
             "m=median filter "
             "\n\t      i=high pixel o=low pixel");
      printf("\n\t      Type is the nxn size for"
            " median, high pixel, and low pixel");
      printf("\n\nEnter choice (0 = no change) _\b");


      get_integer(&response);

      if(response == 0){
        not_finished = 0;
      }

      if(response == 1){
        printf("\n\nEnter type of filter");
        printf("\n  _\b");
        get_integer(filter_type);
      }

      if(response == 2){
        printf("\n\nEnter l=low pass h=high pass "
              "m=median filter i=high pixel "
              "o=low pixel");
        printf("\n  _\b");
        gets(low_high);
      }
    }  /* ends while not_finished */

}  /* ends get_filter_options */


   /**************************************************
   *
   *   file d:\cips\display.c
   *
   *   Functions: This file contains
   *      display_image
   *      display_image_portion
   *      display_menu_for_display_image
   *      map_16_shades_of_gray
   *      my_map_64_shades_of_gray
   *      transform_the_colors
   *
   *   Purpose:
   *      These functions display images on a the
   *      monitor.
   *
   *      NOTE: This file is full of Microsoft
   *            specific code.  The PC C compiler
   *            makers all have their own routines
   *            for making dots appear on the
   *            screen.  I put the statement
   *            MSC 6.0 next to these calls.
   *
   *
   *   External Calls:
   *      rtiff.c - read_tiff_image
   *      cips.c - my_clear_text_screen
   *      hist.c - zero_histogram
   *               calculate_histogram
   *               perform_histogram_equalization
   *               display_histogram
   *
   *   Modifications:
   *      17 June 1987 - created
   *      August 1990 - extension modifications for use
   *          in the C Image Processing System.
   *
   ***************************************************/







   /***************************
   *
   *   display_image(...
   *
   ****************************/


display_image(file_name, image, il, ie, ll, le,
              image_header, monitor_type, 
              color_transform, invert, image_colors, 
              display_colors, show_hist, title)
   char    color_transform[],
           file_name[],
           monitor_type[],
           title[];
   int     display_colors,
           image_colors,
           invert,
           il,
           ie,
           ll,
           le,
           show_hist;
   short   image[ROWS][COLS];
   struct  tiff_header_struct *image_header;
{
   char  channels[80],
         response[80];

   int   a,
         b,
         c,
         channel,
         count,
         display_mode,
         dx_offset,
         dy_offset,
         key,
         horizontal,
         len,
         max_horizontal,
         max_vertical,
         not_finished,
         q,
         r,
         vertical,
         x_offset,
         y_offset;

   unsigned int block,
                color,
                i,
                j,
                x,
                y;
   unsigned long histogram[256], new_hist[256];


     /**********************************************
     *
     *   If you want to display the histogram and 
     *   do not want to perform hist equalization, 
     *   then zero the histogram for calculations.
     *
     **********************************************/

   if(  (show_hist == 1)   &&
        (color_transform[0] != 'H'))
      zero_histogram(histogram);

   not_finished = 1;
   while(not_finished){


      if(display_colors == 16){
         if(monitor_type[0] == 'V'){
            horizontal   = 4;
        vertical = 6;
        display_mode = VRES16COLOR; /* MSC 6.0 */
         }  /* ends if V */
         if(monitor_type[0] == 'E'){
            horizontal   = 3;
            vertical     = 6;
            display_mode = ERESCOLOR; /* MSC 6.0 */
         }  /* ends if E */

      }  /* ends if colors == 16 */

      else{
         horizontal   = 2;
         vertical     = 2;
         display_mode = MAXCOLORMODE; /* MSC 6.0 */
      }

        /********************************************
        *
        *   Somehow, my dx and dy offsets are 
        *   backwards from my horizontal and vertical 
        *   variables. Trying to center the images 
        *   on the screen. March 21 1992
        *
        ********************************************/

      max_horizontal = (image_header->image_length+50)
                       /COLS;
      max_vertical   = (image_header->image_width+50)
                       /ROWS;

      dy_offset = ((horizontal-max_horizontal)/2)
                  *COLS + 50;
      dx_offset = ((vertical-max_vertical)/2)
                  *ROWS + 20;

      if(max_horizontal > horizontal) dy_offset = 0;
      if(max_vertical > vertical)     dx_offset = 0;

      if(horizontal > max_horizontal) 
         horizontal = max_horizontal;
      if(vertical > max_vertical)     
         vertical   = max_vertical;


        /****************************************
        *
        *   If color transform wants histogram
        *   equalization, then read in the
        *   image arrays and calculate the
        *   histogram.   Zero both the histogram
        *   and the new_hist.  You will need the
        *   new_hist if you want to display the
        *   equalized hist.
        *
        *****************************************/

      if(color_transform[0] == 'H'){
         count = 1;
         zero_histogram(histogram);
         zero_histogram(new_hist);
         for(a=0; a<vertical; a++){
            for(b=0; b<horizontal; b++){

               x = a*COLS;
               y = b*ROWS;

               printf(
                  "\nDISPLAY> Calculating histogram");
               printf(" %d of %d",
                  count,vertical*horizontal);
               count++;
               read_tiff_image(file_name, image, il+y,
                            ie+x, ll+y, le+x);
               calculate_histogram(image, histogram);

            }  /* ends loop over b */
         }  /* ends loop over a */
      }  /* ends if display_mode == H */


        /* set graphics mode */

   my_setvideomode(display_mode); /* MSC 6.0 */
   if(display_colors == 16) 
      map_16_shades_of_gray(display_mode);
   if(display_colors == 256) 
      my_map_64_shades_of_gray();


        /****************************************
        *
        *   Loop over this size and
        *   read and display ROWSxCOLS arrays.
        *
        *   If you want to show the histogram AND
        *   do not want to do hist equalization
        *   then calculate the hist from the
        *   original image array.
        *
        *   If you want to do hist equalization
        *   then calculate the new_hist AFTER
        *   the image has been equalized by the
        *   the transform_the_colors function.
        *
        *   NOTE: Remember that the function
        *   transform_the_colors changes the
        *   gray shade values in image array.
        *
        *****************************************/

        /*****************************************
        *
        *   These statements place a gray 
        *   background across the display area of 
        *   a VGA screen.  This reduces the 
        *   contrast between the screen background 
        *   and the images you display. This makes 
        *   it easier to take photos.
        *
        *******************************************/

      /* MSC 6.0 */
      my_setlinestyle(0xFFFF);
      my_setcolor(10);
      for(i=0; i<640;i++){
         my_moveto(i, 0);
         my_lineto(i, 480);
      }

      for(a=0; a<vertical; a++){
         for(b=0; b<horizontal; b++){

            x = a*COLS;
            y = b*ROWS;
            read_tiff_image(file_name, image, il+y,
                            ie+x, ll+y, le+x);
            if(  (show_hist == 1)  &&
                 (color_transform[0] != 'H'))
               calculate_histogram(image, histogram);

            transform_the_colors(image, 
                                 color_transform,
                                 display_colors,
                                 image_colors, 
                                 histogram,
                                 horizontal, 
                                 vertical);

            if(color_transform[0] == 'H')
               calculate_histogram(image, new_hist);
          display_image_portion(image, x+dx_offset,
                                y+dy_offset, 
                                display_colors,
                                image_colors, invert);
         }        /* ends loop over b */
      }        /* ends loop over a */

          /*******************************************
          *
          *   Put in these statements to print a title 
          *   at the bottom of the display.  This is 
          *   nice for taking photos or articles 
          *   because it tells the reader what you 
          *   are trying to do.
          *
          ********************************************/

      /* MSC 6.0 */
      my_settextcolor(10);
      my_setbkcolor(1L);
      len = strlen(title);
      len = 40 - (len/2);
      my_settextposition(28, len);
      my_outtext(title);

         /***************************
         *
         *   if show_hist == 1 then
         *   display the histogram
         *   in the lower right hand
         *   corner of screen
         *
         *   If hist equalization was
         *   performed then show the
         *   new_hist.  If it wasn't
         *   done then show the original
         *   histogram.
         *
         ****************************/

      if(show_hist == 1){
         if(monitor_type[0] == 'V')
            y_offset = 470;
         if(monitor_type[0] == 'E')
            y_offset = 310;
         x_offset = 380;
         if(color_transform[0] == 'H')
            display_histogram(new_hist, x_offset,
                   y_offset, 5, 9);
         else{
            display_histogram(histogram, x_offset,
                   y_offset, 5, 9);
               /* xtra stuff for cips9
            display_histogram(histogram, 45,
                   370, 5, 9); */

               /*********************************
               *
               *   Change to show smoothed
               *   histogram next to the regular
               *   histogram.
               *
               **********************************/

               /* xtra stuff for cips9
            smooth_histogram(histogram);
            display_histogram(histogram, 345,
                   370, 5, 9); */
         }
      }

      gets(response);
      printf("\nEnter 0 to quit 1 to do again");
      get_integer(&not_finished);

          /* set display back to text mode */
      my_clear_text_screen();


   }  /* ends while not_finished  */

}  /* ends main  */




   /***********************************************
   *
   *   display_menu_for_display_image(
   *
   ************************************************/

display_menu_for_display_image(image_colors, 
                              display_colors,
                              invert, color_transform,
                              monitor_type,
                              show_hist)
   char color_transform[], monitor_type[];
   int  *invert, *image_colors, 
        *display_colors, *show_hist;
{
   char response[80];
   int  int_response, not_finished, r;

   not_finished = 1;
   while(not_finished){
      printf("\n\nDISPLAY> Enter choice "
             "(0 for no change) ");
      printf("\nDISPLAY> 1. Invert is %d (1=on 0=off)",
             *invert);
      printf("\nDISPLAY> 2. Color Transform-- %s",
             color_transform);
      printf("\nDISPLAY> 3. Input image has %d colors",
             *image_colors);
      printf("\nDISPLAY> 4. Display will show %d colors",
             *display_colors);
      printf("\nDISPLAY> 5. Monitor type is %s",
             monitor_type);
      printf("\nDISPLAY> 6. Histogram is %d", 
             *show_hist);
      printf("  (1=show 0=don't show)");
      printf("\nDISPLAY> _\b");
      get_integer(&r);

      if(r == 0){
         not_finished = 0;
      }

      if(r == 1){
         printf("\nDISPLAY> Enter 1 for invert on");
         printf(" 0 for invert off");
         printf("\nDISPLAY> ___");
         get_integer(&int_response);
         *invert = int_response;
      }  /* ends if r == 1 */

      if(r == 2){
         printf("\nDISPLAY> Enter the new color "
                "transform mode ");
         printf("\nDISPLAY> (S) Straight mode");
         printf("   (H) Histogram Equalization");
         printf("\nDISPLAY> _\b");
         gets(response);
         if((response[0] == 'S') ||
            (response[0] == 's'))
               strcpy(color_transform, 
                      "Straight mode");
         else
               strcpy(color_transform,
                "Histogram Equalization mode");
      }  /* ends if r == 2  */

      if(r == 3){
         printf("\nDISPLAY> Enter the number "
                "of colors");
         printf(" in the input image");
         printf("\nDISPLAY> ___");
         get_integer(&int_response);
         *image_colors = int_response;
      }  /* ends if r == 3 */

      if(r == 4){
         printf(
          "\nDISPLAY> Enter the number of colors "
          "for the display");
         printf("\nDISPLAY> ___");
         get_integer(&int_response);
         *display_colors = int_response;
      }  /* ends if r == 4 */

      if(r == 5){
         printf("\nDISPLAY> Enter the new monitor type");
         printf("\nDISPLAY> (E) EGA   (V) VGA");
         printf("   (C) CGA   (M) Monochrome");
         printf("\nDISPLAY> _\b");
         gets(response);
         if((response[0] == 'E') ||
            (response[0] == 'e'))
            strcpy(monitor_type, "EGA");
      if((response[0] == 'V') ||
         (response[0] == 'v'))
            strcpy(monitor_type, "VGA");
      if((response[0] == 'C') ||
         (response[0] == 'c'))
            strcpy(monitor_type, "CGA");
      if((response[0] == 'M') ||
         (response[0] == 'm'))
            strcpy(monitor_type, "Monochrome");
      }  /* ends if r == 5  */

      if(r == 6){
         printf(
            "\nDISPLAY> Enter 1 for show histogram "
            "0 for don't");
         printf("\nDISPLAY> ___");
         get_integer(&int_response);
         *show_hist = int_response;
      }  /* ends if r == 6 */

   }  /* ends while not_finished  */
}  /* ends display_menu  */




   /********************************
   *
   *   display_image_portion(...
   *
   *********************************/

display_image_portion(image, x, y, display_colors, 
                      image_colors, invert)
   int      invert, display_colors, image_colors;
   short    image[ROWS][COLS];
   unsigned int x, y;
{
   unsigned int color, i, j;

      if(invert == 1){
        for(i=0; i<ROWS; i++)
           for(j=0; j<COLS; j++)
              image[i][j] = (display_colors-1)
                             - image[i][j];
      }  /* ends if invert == 1 */

      for(i=0; i<ROWS; i++){
         for(j=0; j<COLS; j++){

        /* MSC 6.0 */
        my_setcolor(image[i][j]);
        my_setpixel(j+x, i+y);
/*****
my_set_pixel(j+x, i+y, image[i][j]);
******/

         }  /* ends loop over j  */
      }     /* ends loop over i  */

}  /* ends display_image_portion  */





   /**********************************************
   *
   *   map_16_shades_of_gray(...
   *
   *   This function maps 16 shades of gray into
   *   the first 16 color indices.  This allows
   *   you to display a true "black and white"
   *   image on a color monitor.
   *
   *********************************************/

map_16_shades_of_gray(display_mode)
   int display_mode;
{
   /* all MSC 6.0 statements */
my_setvideomode(display_mode);
my_remappalette(0,  0x000000L);
my_remappalette(1,  0x040404L);
my_remappalette(2,  0x080808L);
my_remappalette(3,  0x0c0c0cL);
my_remappalette(4,  0x101010L);
my_remappalette(5,  0x141414L);
my_remappalette(6,  0x181818L);
my_remappalette(7,  0x1c1c1cL);
my_remappalette(8,  0x202020L);
my_remappalette(9,  0x242424L);
my_remappalette(10, 0x282828L);
my_remappalette(11, 0x2c2c2cL);
my_remappalette(12, 0x303030L);
my_remappalette(13, 0x343434L);
my_remappalette(14, 0x383838L);
my_remappalette(15, 0x3f3f3fL);
}




   /*********************************************
   *
   *   transform_the_colors(...
   *
   *   This function transforms the gray shades
   *   in the image array for display.  It can either
   *   do it in straight mode by multiplying or
   *   dividing or it can do it with hist
   *   equalization by calling the function
   *   perform_histogram_equalization.
   *
   *************************************************/

transform_the_colors(image, color_transform,
                     display_colors, image_colors,
                     histogram, horizontal,
                     vertical)
   char   color_transform[];
   int    display_colors, horizontal,
          image_colors, vertical;
   short  image[ROWS][COLS];
   unsigned long histogram[];
{
   int         color, i, j;
   float new_grays, area;
   unsigned long x;

   if(color_transform[0] == 'S'){
      for(i=0; i<ROWS; i++){
         for(j=0; j<COLS; j++){

            if( (display_colors == 16) &&
                (image_colors  == 256))
               color = image[i][j]/16;
            if( (display_colors == 16) &&
                (image_colors  == 16))
               color = image[i][j];
            if( (display_colors == 256) &&
                (image_colors  == 256))
               color = image[i][j];
            if( (display_colors == 256) &&
                (image_colors  == 16))
               color = image[i][j]*16;

            image[i][j] = color;

         }  /* ends loop over j        */
      }        /* ends loop over i        */
   }  /* ends if transform is straight */

   if(color_transform[0] == 'H'){

      area      = ((long)(vertical)) *
                  ((long)(horizontal));
      area      = area*10000.0;
      new_grays = display_colors;

      perform_histogram_equalization(image, histogram,
                                     new_grays, area);

   }  /* ends if transform is hist equalization */

}  /* ends transform_the_colors */




   /*************************************************
   *
   *   file d:\cips\djet.c
   *
   *   Functions: This file contains
   *      end_graphics_mode
   *      get_graphics_caption
   *      print_bytes
   *      print_graphics_image
   *      print_original_200_row
   *      select_300_dpi_resolution
   *      select_full_graphics_mode
   *      set_horizontal_offset
   *      set_raster_width
   *      start_raster_graphics
   *
   *   Purpose:
   *      These functions print a 200x200 image using
   *      dithering to an HP DeskJet or compatable 
   *      (Laserjet). This uses an 8x8 matrix which 
   *      gives 64 shades of gray.
   *
   *   External Calls:
   *          rtiff.c - read_tiff_image
   *           hist.c - zero_histogram
   *                    calculate_histogram
   *                    perform_histogram_equalization
   *
   *
   *   Modifications:
   *      January 1991 - created
   *      25 August 1991 - modified for use in the
   *         C Image Processing System.
   *
    ZDDDDD?   ZDDDDD?
    3     3   3     3   The function print_graphics_image
    3     3   3     3   begins with 2 100x100 image arrays
    3     3   3     3
    3     3   3     3
    @DDDDDY   @DDDDDY

    ZDDDDDDDDDDDDDDD?
    3               3   It joins them into
    3               3   1 100x200 image array
    3               3
    3               3
    @DDDDDDDDDDDDDDDY

    ZDDDDDDDDDDDDDDD?
    @DDDDDDDDDDDDDDDY
    ZDDDDDDDDDDDDDDD?
    @DDDDDDDDDDDDDDDY
           .            It loops and creates
           .            100 200 element image arrays
           .
    ZDDDDDDDDDDDDDDD?
    @DDDDDDDDDDDDDDDY


          The function print_original_200_row 
          receives a 200 element array
    ZBDDDDDDDDDDDDDDDDDDDDDDDDDDB?
    @ADDDDDDDDDDDDDDDDDDDDDDDDDDAY

          This array is transformed into a 8x200 
          array of characters called 'row'
    ZDDDDDDDDD ... ~DDDDDDD?
    CDDDDDDDDD ... ~DDDDDDD4
    CDDDDDDDDD ... ~DDDDDDD4
    CDDDDDDDDD ... ~DDDDDDD4
    CDDDDDDDDD ... ~DDDDDDD4
    CDDDDDDDDD ... ~DDDDDDD4
    CDDDDDDDDD ... ~DDDDDDD4
    CDDDDDDDDD ... ~DDDDDDD4
    @DDDDDDDDD ... ~DDDDDDDY

          Each column of this array is a 1x8 character
          array which is an 8-bit x 8-bit array
    IMM;
    :  :
    HMM<
          Each row of 'row' is passed to the funnction 
          print_bytes for graphics printing






   ***************************************************/



#define ESCAPE 27
#define FORMFEED  '\014'

short r[200];



      /*******************************************
      *
      *   The patterns array holds the rows to the
      *   8x8 matrices used for printing
      *   shades of gray.
      *
      ********************************************/

char patterns[64][8] =
   { {255, 255, 255, 255, 255, 255, 255, 255},
     {255, 255, 255, 255, 255, 255, 255, 127},
     {255, 255, 255, 255, 255, 255, 255,  63},
     {255, 255, 255, 255, 255, 255, 255,  31},
     {255, 255, 255, 255, 255, 255, 255,  15},
     {255, 255, 255, 255, 255, 255, 255,   7},
     {255, 255, 255, 255, 255, 255, 255,   3},
     {255, 255, 255, 255, 255, 255, 255,   1},
     {255, 255, 255, 255, 255, 255, 255,   0},
     {255, 255, 255, 255, 255, 255, 127,   0},
     {255, 255, 255, 255, 255, 255,  63,   0},
     {255, 255, 255, 255, 255, 255,  31,   0},
     {255, 255, 255, 255, 255, 255,  15,   0},
     {255, 255, 255, 255, 255, 255,   7,   0},
     {255, 255, 255, 255, 255, 255,   3,   0},
     {255, 255, 255, 255, 255, 255,   1,   0},
     {255, 255, 255, 255, 255, 255,   0,   0},
     {255, 255, 255, 255, 255, 127,   0,   0},
     {255, 255, 255, 255, 255,  63,   0,   0},
     {255, 255, 255, 255, 255,  31,   0,   0},
     {255, 255, 255, 255, 255,  15,   0,   0},
     {255, 255, 255, 255, 255,   7,   0,   0},
     {255, 255, 255, 255, 255,   3,   0,   0},
     {255, 255, 255, 255, 255,   1,   0,   0},
     {255, 255, 255, 255, 255,   0,   0,   0},
     {255, 255, 255, 255, 127,   0,   0,   0},
     {255, 255, 255, 255,  63,   0,   0,   0},
     {255, 255, 255, 255,  31,   0,   0,   0},
     {255, 255, 255, 255,  15,   0,   0,   0},
     {255, 255, 255, 255,   7,   0,   0,   0},
     {255, 255, 255, 255,   3,   0,   0,   0},
     {255, 255, 255, 255,   1,   0,   0,   0},
     {255, 255, 255, 255,   0,   0,   0,   0},
     {255, 255, 255, 127,   0,   0,   0,   0},
     {255, 255, 255,  63,   0,   0,   0,   0},
     {255, 255, 255,  31,   0,   0,   0,   0},
     {255, 255, 255,  15,   0,   0,   0,   0},
     {255, 255, 255,   7,   0,   0,   0,   0},
     {255, 255, 255,   3,   0,   0,   0,   0},
     {255, 255, 255,   1,   0,   0,   0,   0},
     {255, 255, 255,   0,   0,   0,   0,   0},
     {255, 255, 127,   0,   0,   0,   0,   0},
     {255, 255,  63,   0,   0,   0,   0,   0},
     {255, 255,  31,   0,   0,   0,   0,   0},
     {255, 255,  15,   0,   0,   0,   0,   0},
     {255, 255,   7,   0,   0,   0,   0,   0},
     {255, 255,   3,   0,   0,   0,   0,   0},
     {255, 255,   1,   0,   0,   0,   0,   0},
     {255, 255,   0,   0,   0,   0,   0,   0},
     {255, 127,   0,   0,   0,   0,   0,   0},
     {255,  63,   0,   0,   0,   0,   0,   0},
     {255,  31,   0,   0,   0,   0,   0,   0},
     {255,  15,   0,   0,   0,   0,   0,   0},
     {255,   7,   0,   0,   0,   0,   0,   0},
     {255,   3,   0,   0,   0,   0,   0,   0},
     {255,   1,   0,   0,   0,   0,   0,   0},
     {255,   0,   0,   0,   0,   0,   0,   0},
     {127,   0,   0,   0,   0,   0,   0,   0},
     { 63,   0,   0,   0,   0,   0,   0,   0},
     { 31,   0,   0,   0,   0,   0,   0,   0},
     { 15,   0,   0,   0,   0,   0,   0,   0},
     {  7,   0,   0,   0,   0,   0,   0,   0},
     {  3,   0,   0,   0,   0,   0,   0,   0},
     {  1,   0,   0,   0,   0,   0,   0,   0}};



   /************************************************
   *
   *   print_graphics_image(...
   *
   ************************************************/

print_graphics_image(image1, image2, image_name,
                     il, ie, ll, le, image_colors,
                     invert, caption, show_hist,
                     color_transform)
   char  caption[], image_name[], color_transform[];
   int   image_colors, invert,
         il, ie, ll, le, show_hist;
   short image1[ROWS][COLS], image2[ROWS][COLS];
{
   char c[80],
        page[80];

   FILE *printer;

   int  i,
        j;

   unsigned long histogram[256], final_hist[256];
   printer = fopen("prn", "w");


      /**********************************************
      *
      *   Print a few blank lines on the page.
      *
      ***********************************************/

   strcpy(page, "                             \n");
   fputs(page, printer);
   fputs(page, printer);


      /***********************************************
      *
      *   Read in two image arrays.
      *
      ************************************************/

   printf("\nReading image");
   read_tiff_image(image_name, image1, il, ie, ll, le);


   printf("\nReading image");
   read_tiff_image(image_name, image2, 
                   il, ie+100, ll, le+100);


      /**********************************************
      *
      *   If show_hist is 1 OR do hist equalization
      *   then zero the histogram and
      *   calculate it for the two image arrays.
      *
      ***********************************************/

   if( (show_hist == 1)  ||
       (color_transform[0] == 'H')){
      zero_histogram(histogram);
      zero_histogram(final_hist);
      printf("\nDJET> Calculating histograms");
      calculate_histogram(image1, histogram);
      calculate_histogram(image2, histogram);
   }

        /*********************************************
        *
        *   Alter the images to 64 gray shades.
        *   Either do it with straight multiply
        *   and divide or use hist equalization.
        *
        *   If using hist equalization then you must
        *   also read and calculate the hist for
        *   the other two image arrays that will be
        *   printed.
        *
        **********************************************/

   if(color_transform[0] == 'S'){
   if(image_colors == 256){
      for(i=0; i<ROWS; i++){
         for(j=0; j<COLS; j++){
            image1[i][j] = image1[i][j]/4;
            image2[i][j] = image2[i][j]/4;
        }
      }
   }  /* ends if image_colors == 256 */


   if(image_colors == 16){
      for(i=0; i<ROWS; i++){
         for(j=0; j<COLS; j++){
            image1[i][j] = image1[i][j]*4;
            image2[i][j] = image2[i][j]*4;
        }
      }
   }  /* ends if image_colors == 16 */
   }  /* ends if color_transform == S */

   if(color_transform[0] == 'H'){

      printf("\nReading image");
      read_tiff_image(image_name, image1, 
                   il+100, ie, ll+100, le);
      printf("\nReading image");
      read_tiff_image(image_name, image2,
                   il+100, ie+100, ll+100, le+100);
      printf("\nDJET> Calculating histograms");
      calculate_histogram(image1, histogram);
      calculate_histogram(image2, histogram);

      printf("\nReading image");
      read_tiff_image(image_name, image1, 
                   il, ie, ll, le);

      printf("\nReading image");
      read_tiff_image(image_name, image2, 
                   il, ie+100, ll, le+100);

      printf("\nDJET> Equalizing histogram");
      perform_histogram_equalization(image1, histogram,
                                     64.0, 40000.0);
      printf("\nDJET> Equalizing histogram");
      perform_histogram_equalization(image2, histogram,
                                     64.0, 40000.0);

      printf("\nDJET> Calculating histograms");
      calculate_histogram(image1, final_hist);
      calculate_histogram(image2, final_hist);


   }  /* ends if color_transform == H */



      /*********************************************
      *
      *   If invert is set them invert the 
      *   transformed image arrays (they now 
      *   only have 64 shades of gray).
      *
      **********************************************/

   if(invert == 1){
      for(i=0; i<ROWS; i++){
         for(j=0; j<COLS; j++){
               image1[i][j] = 63 - image1[i][j];
               image2[i][j] = 63 - image2[i][j];
         }
      }
   }



        /********************************************
        *
        *   Now set the graphics mode on the printer
        *
        *********************************************/

   printf("\nBegin");
   end_graphics_mode(printer);
   select_300_dpi_resolution(printer);
   set_raster_width(printer);
   start_raster_graphics(printer);
   select_full_graphics_mode(printer);

        /*********************************************
        *
        *   Print the two arrays to make a 100x200 
        *   output. To do this you loop over 100 rows, 
        *   set the r buffer to the image values, set 
        *   the graphics, and print the row via 
        *   function print_original_200_row.
        *
        **********************************************/

   for(i=0; i<100; i++){
      for(j=0; j<100; j++){
         r[j]     = image1[i][j];
         r[j+100] = image2[i][j];
      }  /* ends loop over j */

      end_graphics_mode(printer);
      select_300_dpi_resolution(printer);
      set_raster_width(printer);
      start_raster_graphics(printer);
      select_full_graphics_mode(printer);

      print_original_200_row(printer, r);

      printf("\n\tPrinting row %d", i);
   }  /* ends loop over i */

           /* ends first half */



      /**********************************************
      *
      *   In order to print 200x200 repeat
      *   the above steps for 2 more 100x100 arrays
      *
      ***********************************************/


   printf("\nReading image");
   read_tiff_image(image_name, image1, 
                il+100, ie, ll+100, le);
   printf("\nReading image");
   read_tiff_image(image_name, image2,
                il+100, ie+100, ll+100, le+100);


        /*********************************************
        *
        *   Alter the images to 64 shades of gray.
        *
        *   Either do it with straight multiply
        *   and divide or use hist equalization.
        *
        **********************************************/


   if(color_transform[0] == 'S'){
   if(image_colors == 256){
      for(i=0; i<ROWS; i++){
         for(j=0; j<COLS; j++){
            image1[i][j] = image1[i][j]/4;
            image2[i][j] = image2[i][j]/4;
        }
      }
   }  /* ends if image_colors == 256 */


   if(image_colors == 16){
      for(i=0; i<ROWS; i++){
         for(j=0; j<COLS; j++){
            image1[i][j] = image1[i][j]*4;
            image2[i][j] = image2[i][j]*4;
        }
      }
   }  /* ends if image_colors == 16 */
   }  /* ends if color_transform == S */



   if(color_transform[0] == 'H'){

      printf("\nDJET> Equalizing histogram");
      perform_histogram_equalization(image1, histogram,
                                     64.0, 40000.0);
      printf("\nDJET> Equalizing histogram");
      perform_histogram_equalization(image2, histogram,
                                     64.0, 40000.0);

      printf("\nDJET> Calculating histograms");
      calculate_histogram(image1, final_hist);
      calculate_histogram(image2, final_hist);

   }  /* ends if color_transform == S */




      /************************************************
      *
      *   If invert is set them invert the transformed
      *   image arrays (they now only have 64 shades
      *   of gray).
      *
      *************************************************/

   if(invert == 1){
      for(i=0; i<ROWS; i++){
         for(j=0; j<COLS; j++){
               image1[i][j] = 63 - image1[i][j];
               image2[i][j] = 63 - image2[i][j];
         }
      }
   }



   printf("\nBegin");
   end_graphics_mode(printer);
   select_300_dpi_resolution(printer);
   set_raster_width(printer);
   start_raster_graphics(printer);
   select_full_graphics_mode(printer);


    /***********************************************
    *
    *   Print the two arrays to make a 100x200 output.
    *   To do this you loop over 100 rows, set the
    *   r buffer to the image values, set the
    *   graphics, and print the row via function
    *   print_original_200_row.
    *
    *************************************************/

   for(i=0; i<100; i++){
      for(j=0; j<100; j++){
         r[j]     = image1[i][j];
         r[j+100] = image2[i][j];
      }  /* ends loop over j */

      end_graphics_mode(printer);
      select_300_dpi_resolution(printer);
      set_raster_width(printer);
      start_raster_graphics(printer);
      select_full_graphics_mode(printer);

      print_original_200_row(printer, r);

      printf("\n\tPrinting row %d", i);
   }  /* ends loop over i */


      /**********************************************
      *
      *   If show_hist is 1 then calculate the 
      *   histogram for the two image arrays and 
      *   print the histogram.
      *
      ***********************************************/

   if(show_hist == 1){
      if(color_transform[0] == 'S'){
         calculate_histogram(image1, histogram);
         calculate_histogram(image2, histogram);
         print_hist_image(printer, histogram);
      }
      if(color_transform[0] == 'H'){
         print_hist_image(printer, final_hist);
      }
   }

        /*********************************************
        *
        *   Print a couple of blank lines then print
        *   the caption.
        *
        **********************************************/

   end_graphics_mode(printer);
   strcpy(page, "      \n");
   fputs(page, printer);
   fputs(page, printer);

   sprintf(page, "                      %s\n", caption);
   fputs(page, printer);

   fputc(FORMFEED, printer);

   fclose(printer);

   printf("\nEnd");

}  /* ends print_graphics_image */



   /******************************************
   *
   *   get_graphics_caption(...
   *
   ******************************************/

get_graphics_caption(caption)
   char caption[];
{
   printf("\nEnter the caption to be printed\n---");
   gets(caption);

}  /* ends get_graphics_caption */




   /************************************************
   *
   *   select_full_graphics_mode(...
   *
   ************************************************/

select_full_graphics_mode(printer)

   FILE *printer;
{
   fputc(ESCAPE, printer);
   fputc('*', printer);
   fputc('b', printer);
   fputc('0', printer);
   fputc('M', printer);

}



   /************************************************
   *
   *   set_horizontal_offset(...
   *
   ************************************************/

set_horizontal_offset(printer)

   FILE *printer;
{
   fputc(ESCAPE, printer);
   fputc('*', printer);
   fputc('b', printer);
   fputc('4', printer);
   fputc('9', printer);
   fputc('6', printer);
   fputc('X', printer);

}





   /************************************************
   *
   *   set_shorter_horizontal_offset(...
   *
   ************************************************/

set_shorter_horizontal_offset(printer)

   FILE *printer;
{
   fputc(ESCAPE, printer);
   fputc('*', printer);
   fputc('b', printer);
   fputc('4', printer);
   fputc('8', printer);
   fputc('0', printer);
   fputc('X', printer);

}





   /************************************************
   *
   *   end_graphics_mode(...
   *
   ************************************************/

end_graphics_mode(printer)
   FILE *printer;
{
   fputc(ESCAPE, printer);
   fputc('*', printer);
   fputc('r', printer);
   fputc('B', printer);
}



   /************************************************
   *
   *   set_raster_width(...
   *
   ************************************************/

set_raster_width(printer)

   FILE *printer;
{
   fputc(ESCAPE, printer);
   fputc('*', printer);
   fputc('r', printer);
   fputc('2', printer);
   fputc('2', printer);
   fputc('0', printer);
   fputc('0', printer);
   fputc('S', printer);

}




   /************************************************
   *
   *   start_raster_graphics(...
   *
   ************************************************/

start_raster_graphics(printer)

   FILE *printer;
{
   fputc(ESCAPE, printer);
   fputc('*', printer);
   fputc('r', printer);
   fputc('0', printer);
   fputc('A', printer);

}




   /************************************************
   *
   *   select_300_dpi_resolution(...
   *
   ************************************************/

select_300_dpi_resolution(printer)

   FILE *printer;
{
   fputc(ESCAPE, printer);
   fputc('*', printer);
   fputc('t', printer);
   fputc('3', printer);
   fputc('0', printer);
   fputc('0', printer);
   fputc('R', printer);

}




   /************************************************
   *
   *   print_bytes(...
   *
   ************************************************/

print_bytes(printer, buffer)
   FILE *printer;
   char buffer[];
{
   int        i;

   fputc(ESCAPE, printer);
   fputc('*', printer);
   fputc('b', printer);
   fputc('2', printer);
   fputc('0', printer);
   fputc('0', printer);
   fputc('W', printer);


   for(i=0; i<200; i++){
      fputc(buffer[i], printer);
   }
}  /* ends print_bytes */




   /**************************************************
   *
   *   print_original_200_row(...
   *
   ***************************************************/

print_original_200_row(printer, short_row)
   FILE  *printer;
   short short_row[200];
{
   char  row[8][200];
   char  c[200], response[80];
   int         i, j, k;
   short value;
   for(i=0; i<200; i++){
      value = short_row[i];
      if(value > 63) value = 63;
      if(value < 0)  value =  0;

      for(j=0; j<8; j++)
         row[j][i] = patterns[value][j];

   }  /* ends loop over i */

   for(i=0; i<8; i++){
      for(j=0; j<200; j++)
         c[j] = row[i][j];
      set_horizontal_offset(printer);
      print_bytes(printer, c);
   }  /* ends loop over i */

}  /* ends print_original_200_row */






   /***********************************
   *
   *   print_hist_image(...
   *
   ************************************/

print_hist_image(printer, hist)
   FILE *printer;
   unsigned long hist[];
{
   char   c, d;
   int          i, j, k;
   unsigned long limit, max;

   d = 0;
   c = 255;

      /********************************
      *
      *   First scale the histogram
      *
      *********************************/

   max = 0;
   for(i=0; i<256; i++)
      if(hist[i] > max) max = hist[i];

   if(max > 200){
      for(i=0; i<256; i++){
        hist[i] = (hist[i]*200)/max;
      }
   }


      /********************************
      *
      *   Second print it
      *
      *   Print a space between the image
      *   and the histogram.
      *
      *********************************/


   for(i=0; i<20; i++){
         end_graphics_mode(printer);
         select_300_dpi_resolution(printer);
         set_raster_width(printer);
         start_raster_graphics(printer);
         select_full_graphics_mode(printer);
         set_horizontal_offset(printer);
         fputc(ESCAPE, printer);
         fputc('*', printer);
         fputc('b', printer);
         fputc('2', printer);
         fputc('0', printer);
         fputc('0', printer);
         fputc('W', printer);

         for(j=0; j<200; j++)
            fputc(d, printer);
   }


   printf("\n\nHIST> Now printing the histogram");
   for(i=0; i<256; i++){
      printf("\n\tHIST> Histogram[%d]=%ld", 
            i, hist[i]);

            /* print the line 2 times */
      for(k=0; k<2; k++){

         end_graphics_mode(printer);
         select_300_dpi_resolution(printer);
         set_raster_width(printer);
         start_raster_graphics(printer);
         select_full_graphics_mode(printer);


            /***************************
            *
            *  Print grid marks every
            *  50 pixels.  Do this by
            *  setting a shorter margin
            *  then printing 2 marks then
            *  the data.
            *
            ****************************/

         if( (i ==   0) ||
             (i ==  50) ||
             (i == 100) ||
             (i == 150) ||
             (i == 200) ||
             (i == 255)){

            set_shorter_horizontal_offset(printer);
            fputc(ESCAPE, printer);
            fputc('*', printer);
            fputc('b', printer);
            fputc('2', printer);
            fputc('0', printer);
            fputc('2', printer);
            fputc('W', printer);

            fputc(c, printer);
            fputc(c, printer);


            if(hist[i] >= 200)
               hist[i] = 200;

            limit = 200 - hist[i];

            if(hist[i] == 0)
               fputc(c, printer);

            for(j=0; j<hist[i]; j++)
               fputc(c, printer);

            for(j=0; j<limit; j++)
              fputc(d, printer);

         }  /* ends print grid marks */


            /***************************
            *
            *  If you do not print
            *  grid marks, set the normal
            *  margin and then print the
            *  data.
            *
            ****************************/

         else{
            set_horizontal_offset(printer);
            /* this prints 200 bytes so print 200 */
            fputc(ESCAPE, printer);
            fputc('*', printer);
            fputc('b', printer);
            fputc('2', printer);
            fputc('0', printer);
            fputc('0', printer);
            fputc('W', printer);

            if(hist[i] >= 200)
               hist[i] = 200;

            limit = 200 - hist[i];

            if(hist[i] == 0)
               fputc(c, printer);

            for(j=0; j<hist[i]; j++)
               fputc(c, printer);

            for(j=0; j<limit; j++)
              fputc(d, printer);

         }  /* ends else no grid marks */

      }  /* ends loop over k */

   }  /* ends loop over i */

}  /* ends print_hist_image */


    /***********************************************
    *
    *       file d:\cips\scale.c
    *
    *       Functions: This file contains
    *          zoom_image_array
    *          shrink_image_array
    *          interpolate_pixel
    *          average_pixel
    *          median_pixel
    *          get_scaling_options
    *          blank_image_array
    *
    *       Purpose:
    *          These functions implement image array
    *          zooming (enlarging) and shrinking.
    *
    *       External Calls:
    *          wtiff.c - round_off_image_size
    *                    create_file_if_needed
    *                    write_array_into_tiff_image
    *          tiff.c - read_tiff_header
    *          rtiff.c - read_tiff_image
    *          numcvrt.c - get_integer
    *          filter.c - median_of
    *
    *       Modifications:
    *          8 April 1992 - created
    *
    *************************************************/


     /*******************************************
     *
     *   zoom_image_array(...
     *
     *   This function zooms in on an input
     *   image array.  It zooms by enlarging
     *   an input image array and writing the
     *   resulting image arrays to an output
     *   image file.  It can zoom or enlarge by
     *   a factor of 2 or 4.
     *
     *   You can zoom or enlarge an image array
     *   by using either the replication or
     *   interpolation methods.
     *
     *******************************************/


zoom_image_array(in_name, out_name, the_image, out_image,
          il, ie, ll, le, scale, method)
   char   in_name[], out_name[], method[];
   int    il, ie, ll, le, scale;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];
{
   int    A, B, a, b, count, factor, 
          i, j, length, width;
   struct tiff_header_struct image_header;

           /******************************************
           *
           *   Check the scale factor.  If it is not
           *   a valid factor (2 or 4), then set
           *   it to 2.
           *
           ******************************************/

   factor = scale;
   if(factor != 2 &&
      factor != 4) factor = 2;

   create_file_if_needed(in_name, out_name, out_image);

      /*******************************************
      *
      *   Let me try to explain the nested loops
      *   shown below.
      *
      *   We will enlarge the_image by the factor.
      *   Therefore, we want to divide the_image
      *   into parts of size ROWS/factor by
      *   COLS/factor.  We will loop through
      *   the_image parts ROWS/factor and COLS/factor
      *   using the loops over i and j.
      *
      *   We divided the_image into parts so we must
      *   include all of these parts.  That is why we
      *   do the loops over A and B.  There are
      *   factor*factor parts.
      *
      *   Finally, we do the loops over a and b.
      *   We must replicate every element in the_image
      *   factor*factor times and put these into
      *   out_image.  The a and b loops perform this
      *   task.
      *
      *   The equations inside the []'s for
      *   the_image and out_image are also confusing.
      *   If you work them out, you'll see that we
      *   are bouncing through the image arrays
      *   and fitting the pixels into the right
      *   places.
      *
      *   The final proof is that this works.
      *
      *   One final note:  the factor should divide
      *   evenly into ROWS.  For example, ROWS=100
      *   so using a factor of 8 is not good.
      *   100/8 = 12.5 and this leave you with
      *   an empty strip along the right and
      *   bottom edges of the out_image.
      *
      *******************************************/

           /*****************************************
           *
           *   Replication method
           *
           ******************************************/

   if(method[0] == 'r' || method[0] == 'R'){
      read_tiff_image(in_name, the_image, 
                      il, ie, ll, le);
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
        printf("\nzooming replication %3d of %3d",
               count++, factor*factor);
        write_array_into_tiff_image(out_name, out_image,
            1+A*ROWS, 1+B*COLS, 101+A*ROWS, 101+B*COLS);
       }  /* ends loop over B */
      }  /* ends loop over A */
   }  /* ends replication method */

           /***************************
           *
           *   Interpolation method
           *
           ****************************/

   if(method[0] == 'i' || method[0] == 'I'){
      read_tiff_image(in_name, the_image,
                      il, ie, ll, le);
      count = 1;
      for(A=0; A<factor; A++){
         for(B=0; B<factor; B++){
          for(i=0; i<ROWS/factor; i++){
           for(j=0; j<COLS/factor; j++){
            for(a=0; a<factor; a++){
             for(b=0; b<factor; b++){
                out_image[factor*i+a][factor*j+b] =
                  interpolate_pixel(the_image, A, B,
                              i, j, a, b, factor);
             }  /* ends loop over b */
            }  /* ends loop over a */
           }  /* ends loop over j */
          }  /* ends loop over i */
       printf("\nzooming interpolation %3d of %3d",
                     count++, factor*factor);
       write_array_into_tiff_image(out_name, out_image,
                 1+A*ROWS, 1+B*COLS,
                 101+A*ROWS, 101+B*COLS);
     }  /* ends loop over B */
    }  /* ends loop over A */
   }  /* ends interpolation method */


}  /* ends zoom_image_array */





    /***********************************************
    *
    *    interpolate_pixel(...
    *
    *    This function interpolates between pixel
    *    values and returns the interlopated value.
    *
    ***********************************************/

interpolate_pixel(the_image, A, B, i, j, a, b, factor)
   int   A, B, a, b, factor, i, j;
   short the_image[ROWS][COLS];
{
   int   num, x = 0, y = 0;
   short diff, result;

   if(a > 0) y = 1;
   if(b > 0) x = 1;
   diff = 
      the_image[y+i+A*ROWS/factor][x+j+B*COLS/factor] -
      the_image[i+A*ROWS/factor][j+B*COLS/factor];

          /******************************************
          *
          * If you are at the edge of the input image
          * array, then you cannot interpolate to the
          * next point because there is no next point.
          * Therefore, set the diff to 0.
          *
          *******************************************/

   if((y+i+A*ROWS/factor) >= ROWS) diff = 0;
   if((x+j+B*COLS/factor) >= COLS) diff = 0;

   num = a+b;
   if(num > factor) num = factor;
   result = the_image[i+A*ROWS/factor][j+B*COLS/factor] +
            num*diff/factor;
   return(result);
}  /* ends interpolate_pixel */





     /*******************************************
     *
     *   shrink_image_array(...
     *
     *   This function shrinks a part of an
     *   image.  It takes a part of an input
     *   image (described by il1, ie1, ll1, le1)
     *   shrinks a 200x200 or 400x400 area down
     *   to a 100x100 array, and writes this result
     *   to an output file.  The location in the
     *   output file is described by il2, ie2,
     *   ll2, le2.
     *
     *   You can shrink the input image area
     *   by using either the averaging, median,
     *   or corner method.
     *
     *******************************************/

shrink_image_array(in_name, out_name, 
          the_image, out_image,
          il1, ie1, ll1, le1, il2, ie2, ll2, le2,
          scale, method)
   char   in_name[], out_name[], method[];
   int    il1, ie1, ll1, le1,
          il2, ie2, ll2, le2, scale;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];
{
   int    A, B, a, b, count, factor, 
          i, j, length, width;
   struct tiff_header_struct image_header;

           /******************************************
           *
           *   Check the scale factor.  If it is not
           *   a valid factor (2 or 4), then set
           *   it to 2.
           *
           ******************************************/

   factor = scale;
   if(factor != 2 &&
      factor != 4) factor = 2;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_header(in_name, &image_header);

      /**********************************************
      *
      *   Let me try to explain the nested loops
      *   shown below.
      *
      *   We will shrink the_image by the factor.
      *   Therefore, we want to read in factor*factor
      *   image arrays and produce one ROWS by COLS
      *   array for writing to disk.
      *   That is why we loop over A and B.
      *
      *   We want to set every pixel in the out_image
      *   array so we loop over i and j.
      *
      *   The equations inside the out_image []'s
      *   look a little strange.  What we are doing is
      *   setting every element and moving over every
      *   time through the loops over A and B.  
      *   The first loop is for i=0,49 then i=50,99 
      *   for a factor=2 and ROWS=100.
      *
      *   The final proof is that this works.
      *
      *   One final note:  the factor should divide
      *   evenly into ROWS.  For example, ROWS=100
      *   so using a factor of 8 is not good.
      *   100/8 = 12.5 and this leave you with
      *   an empty strip along the right and
      *   bottom edges of the out_image.
      *
      ************************************************/

           /********************************
           *
           *   Corner method
           *
           *********************************/

   if(method[0] == 'c' || method[0] == 'C'){
      count = 1;
      for(A=0; A<factor; A++){
       for(B=0; B<factor; B++){
        printf("\n shrinking by corner %3d of %3d",
                     count++, factor*factor);
        if(image_header.image_length < ll1+A*ROWS   ||
           image_header.image_width  < le1+B*COLS)
           blank_image_array(the_image);
        else
           read_tiff_image(in_name, the_image,
                           il1+A*ROWS, ie1+B*COLS,
                           ll1+A*ROWS, le1+B*COLS);
        for(i=0; i<ROWS/factor; i++){
         for(j=0; j<COLS/factor; j++){
            out_image[i+A*ROWS/factor][j+B*COLS/factor] =
                     the_image[factor*i][factor*j];
         }  /* ends loop over j */
        }  /* ends loop over i */
       }  /* ends loop over B */
      }  /* ends loop over A */
      write_array_into_tiff_image(out_name, out_image,
                                  il2, ie2, ll2, le2);
   } /* ends corner method */

           /******************************
           *
           *   Average Method
           *
           ******************************/

   if(method[0] == 'a' || method[0] == 'A'){
      count = 1;
      for(A=0; A<factor; A++){
       for(B=0; B<factor; B++){
        printf("\n shrinking by average %3d of %3d",
                     count++, factor*factor);
        if(image_header.image_length < ll1+A*ROWS   ||
           image_header.image_width  < le1+B*COLS)
           blank_image_array(the_image);
        else
           read_tiff_image(in_name, the_image,
                          il1+A*ROWS, ie1+B*COLS,
                          ll1+A*ROWS, le1+B*COLS);
        for(i=0; i<ROWS/factor; i++){
         for(j=0; j<COLS/factor; j++){
          out_image[i+A*ROWS/factor][j+B*COLS/factor] =
                average_pixel(the_image, factor, i, j);
         }  /* ends loop over j */
        }  /* ends loop over i */
       }  /* ends loop over B */
      }  /* ends loop over A */
      write_array_into_tiff_image(out_name, out_image,
                                  il2, ie2, ll2, le2);
   } /* ends average method */

           /************************
           *
           *   Median Method
           *
           *************************/

   if(method[0] == 'm' || method[0] == 'M'){
      count = 1;
      for(A=0; A<factor; A++){
       for(B=0; B<factor; B++){
        printf("\n shrinking by median %3d of %3d",
                     count++, factor*factor);
        if(image_header.image_length < ll1+A*ROWS   ||
           image_header.image_width  < le1+B*COLS)
           blank_image_array(the_image);
        else
           read_tiff_image(in_name, the_image,
                           il1+A*ROWS, ie1+B*COLS,
                           ll1+A*ROWS, le1+B*COLS);
        for(i=0; i<ROWS/factor; i++){
         for(j=0; j<COLS/factor; j++){
          out_image[i+A*ROWS/factor][j+B*COLS/factor] =
                median_pixel(the_image, factor, i, j);
         }  /* ends loop over j */
        }  /* ends loop over i */
       }  /* ends loop over B */
      }  /* ends loop over A */
      write_array_into_tiff_image(out_name, out_image,
                                  il2, ie2, ll2, le2);
   } /* ends median method */

}  /* ends shrink_image_array */





    /***********************************************
    *
    *   average_pixel(...
    *
    *   This function calculates the average
    *   pixel value of a factor x factor array
    *   of pixels inside the the_image array.
    *   The coordinates i and j point to the upper
    *   left hand corner of the small array.
    *
    ***********************************************/

average_pixel(the_image, factor, i, j)
   int   factor, i, j;
   short the_image[ROWS][COLS];
{
   int a, b, result = 0;

   for(a=0; a<factor; a++)
      for(b=0; b<factor; b++)
         result = result + 
                  the_image[factor*i+a][factor*j+a];
   result = result/(factor*factor);

   return(result);
}  /* ends average_pixel */





    /***********************************************
    *
    *   median_pixel(...
    *
    *   This function calculates the median
    *   pixel value of a factor x factor array
    *   of pixels inside the the_image array.
    *   The coordinates i and j point to the upper
    *   left hand corner of the small array.
    *
    ***********************************************/

median_pixel(the_image, factor, i, j)
   int   factor, i, j;
   short the_image[ROWS][COLS];
{
   int   a, b, count, ff, result = 0;
   short *elements;

   ff       = factor*factor;
   elements = (short *) malloc(ff * sizeof(short));

     count = 0;
   for(a=0; a<factor; a++){
         for(b=0; b<factor; b++){
            elements[count] = 
               the_image[factor*i+a][factor*j+b];
              count++;
        }
     }

   result = median_of(elements, &ff);
   free(elements);
   return(result);

}  /* ends median_pixel */





    /***********************************************
    *
    *    get_scaling_options(...
    *
    *    This function queries the user for the
    *    parameters needed to perform scaling.
    *
    ***********************************************/

get_scaling_options(zoom_shrink, scale, method)
   int *scale;
   char method[], zoom_shrink[];
{
   int not_finished = 1, response;

   while(not_finished){
      printf("\n\t1. Zoom or Shrink is - %s", 
             zoom_shrink);
      printf("\n\t2. Scale factor is %d", *scale);
      printf("\n\t3. Scaling Method is - %s", method);
      printf(
      "\n\t     Replication or Interpolation for Zooming"
      "\n\t     Averaging Median or Corner for Shrinking");
      printf("\n\n\tEnter choice (0 = no change) _\b");
      get_integer(&response);

      if(response == 0){
        not_finished = 0;
      }

      if(response == 1){
         printf("\nEnter Zoom or Shrink (z or s) __\b");
         gets(zoom_shrink);
      }

      if(response == 2){
         printf("\nEnter Scale Factor (2 or 4) __\b");
         get_integer(scale);
      }

      if(response == 3){
         printf("\nEnter Scaling Method: "
            "Replication or Interpolation for Zooming"
            "\n                      "
            "Averaging Median or Corner for Shrinking"
            "\n\t__\b");
         gets(method);
      }

   }  /* ends while not_finished */
}  /* ends get_scaling_options */





    /***********************************************
    *
    *   blank_image_array(...
    *
    *   This function blanks out an image array
    *   by filling it with zeros.
    *
    ***********************************************/

blank_image_array(image)
   short image[ROWS][COLS];
{
   int i, j;
   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         image[i][j] = 0;
}  /* ends blank_image_array */
