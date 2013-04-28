

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


#include "cips.h"





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
            /**horizontal = 4;
               vertical   = 6;**/
            horizontal = 8;
            vertical   = 6;
/* HERE IT IS! */
        display_mode = _XRES16COLOR; /* MSC 7.0 */
         }  /* ends if V */
         if(monitor_type[0] == 'E'){
            horizontal   = 3;
            vertical     = 6;
            display_mode = ERESCOLOR; /* MSC 6.0 */
         }  /* ends if E */

      }  /* ends if colors == 16 */

      else{
	     /********
            horizontal   = 2;
            vertical     = 2;
            display_mode = MAXCOLORMODE; ****//* MSC 6.0 */
            horizontal = 8;
            vertical   = 6;
/* HERE IT IS! */
        display_mode = _XRES256COLOR; /* MSC 7.0 */
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
/*********
   if(display_colors == 256) 
      my_map_64_shades_of_gray();
*********/


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


