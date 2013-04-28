

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


#include "cips.h"

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
