



#include "d:\cips\cips.h"

#define EIGHT  '\010'
#define ESCAPE 27
#define SIXTYFOUR 64
#define ONEHUNDRED 100
#define FORMFEED  '\014'

short image1[ROWS][COLS];
short image2[ROWS][COLS];
short r[200];

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





main()
{
   char caption[80],
        c[80],
        d[80],
        image_name[80],
        page[80],
        type[80];

   FILE *printer;

   int	i,
	invert,
        j,
        channel,
        il,
        ie,
        ll,
        le;

   unsigned long histogram[256], final_hist[256];

   printer = fopen("prn", "w");

      /************************************************
      *
      *   Print the page number and a few blank lines.
      *
      *************************************************/

   select_proportional_font(printer);

   printf("\n\nEnter page number ---\b\b\b");
   gets(page);

   strcpy(image_name, "                                           ");
   my_fwriteln(printer, image_name);
   my_fwriteln(printer, image_name);
   my_fwriteln(printer, image_name);
   my_fwriteln(printer, image_name);
   my_fwriteln(printer, image_name);

   my_fwrite(printer, image_name);
   my_fwrite(printer, image_name);

   sprintf(image_name,
     "                                                            %s", page);
   my_fwriteln(printer, image_name);

   strcpy(image_name, "      ");
   my_fwriteln(printer, image_name);
   my_fwriteln(printer, image_name);
   my_fwriteln(printer, image_name);
   my_fwriteln(printer, image_name);
   my_fwriteln(printer, image_name);

      /***************************************************
      *
      *   Get the following information:
      *      . image name
      *      . image parameters
      *      . size of image
      *      . type of image
      *
      ***************************************************/

   strcpy(image_name, "d:/pix/philips.tif");
   get_image_name(image_name);
   channel = 1;
   il =   1;
   ie =   1;
   ll = 100;
   le = 100;
   invert = 1;
   get_parameters(&il, &ie, &ll, &le);

   printf("\nEnter invert (1=on 0=off) -\b");
   get_integer(&invert);

   printf("\n\nEnter the caption for the figure\n->");
   gets(caption);

   printf("\n\nPrint    a) 100x200     b) 200x200\n->");
   gets(d);

   printf("\n\nPrint    a) original image    b) result image\n->");
   gets(type);


      /*****************************************************
      *
      *   Read in two image arrays and calculate histogram.
      *
      ******************************************************/

   printf("\nReading image");
   read_tiff_image(image_name, image1, il, ie, ll, le);
   ie = ie + 100;
   le = le + 100;

   printf("\nReading image");
   read_tiff_image(image_name, image2, il, ie, ll, le);

   if(invert == 1){
      for(i=0; i<ROWS; i++){
	 for(j=0; j<COLS; j++){
	    image1[i][j] = 255 - image1[i][j];
	    image2[i][j] = 255 - image2[i][j];
	 }
      }
   }

   zero_long_histogram(histogram);
   zero_long_histogram(final_hist);

   calculate_long_histogram(image1, histogram);
   printf("\nCalculated histogram");
   calculate_long_histogram(image2, histogram);
   printf("\nCalculated histogram");

        /**************************************************
        *
        *    If you are printing a result image then do not
        *    do histogram equalization.
        *
        *    If this is an orginal image then you have a
        *    choice of using histogram equalization before
        *    printing or using a straight transform.
        *
        ***************************************************/

   if( (type[0] == 'a') || (type[0] == 'A')){
      printf("\n\nDo you want to perform histogram equalization?(y/n) ");
      gets(c);
      if( (c[0] == 'y')         ||
          (c[0] == 'Y')){
         printf("\nPerforming histogram equalization");
         perform_histogram_equalization(image1, histogram);
         printf("\nPerforming histogram equalization");
         perform_histogram_equalization(image2, histogram);
      }  /* ends yes do histogram equalization */

      else{  /* no equalization so reduce
                gray levels from 256 to 64 */
         for(i=0; i<ROWS; i++){
            for(j=0; j<COLS; j++){
               image1[i][j] = image1[i][j]/4;
               image2[i][j] = image2[i][j]/4;
            }
         }
      }   /* ends else */

   }  /* ends if original image type */

   calculate_long_histogram(image1, final_hist);
   calculate_long_histogram(image2, final_hist);



        /**************************************************
        *
        *   Now set the graphics mode on the printer
        *   and print the image rows.
        *
        ***************************************************/

   printf("\nBegin");
   end_graphics_mode(printer);
   select_300_dpi_resolution(printer);
   set_raster_width(printer);
   start_raster_graphics(printer);
   select_full_graphics_mode(printer);



   for(i=0; i<100; i++){
      for(j=0; j<100; j++){
         r[j]          = image1[i][j];
         r[j+100] = image2[i][j];
      }  /* ends loop over j */

      end_graphics_mode(printer);
      select_300_dpi_resolution(printer);
      set_raster_width(printer);
      start_raster_graphics(printer);
      select_full_graphics_mode(printer);

      if( (type[0] == 'a') || (type[0] == 'A'))
         print_original_100_row(printer, r);
      else
         print_result_100_row(printer, r);

      printf("\n\tPrinting row %d", i);
   }  /* ends loop over i */

           /* ends first half */



      /*****************************************************
      *
      *   If you selected a 200x200 printout then repeat
      *   the above steps for 2 more 100x100 arrays
      *
      ******************************************************/

   if( (d[0] == 'b')   ||
       (d[0] == 'B')){

      ie = ie - 100;
      le = le - 100;
      il = il + 100;
      ll = ll + 100;


      printf("\nReading image");
      read_tiff_image(image_name, image1, il, ie, ll, le);
      ie = ie + 100;
      le = le + 100;

      printf("\nReading image");
      read_tiff_image(image_name, image2, il, ie, ll, le);

         /***********************************************
         *
         *   If printing a result image do not perform
         *   histogram equalization.
         *
         ***********************************************/

      if( (type[0] == 'a') || (type[0] == 'A')){
         if( (c[0] == 'y')   ||
             (c[0] == 'Y')){
            printf("\nPerforming histogram equalization");
            perform_histogram_equalization(image1, histogram);
            printf("\nPerforming histogram equalization");
            perform_histogram_equalization(image2, histogram);
         }  /* ends yes do histogram equalization */

         else{        /* no equalization so reduce
                gray levels from 256 to 64 */
            for(i=0; i<ROWS; i++){
               for(j=0; j<COLS; j++){
                  image1[i][j] = image1[i][j]/4;
                  image2[i][j] = image2[i][j]/4;
               }
            }
         }   /* ends else */
      }  /* ends if original image type */

      calculate_long_histogram(image1, final_hist);
      calculate_long_histogram(image2, final_hist);

      printf("\nBegin");
      end_graphics_mode(printer);
      select_300_dpi_resolution(printer);
      set_raster_width(printer);
      start_raster_graphics(printer);
      select_full_graphics_mode(printer);



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

         if( (type[0] == 'a') || (type[0] == 'A'))
            print_original_100_row(printer, r);
         else
            print_result_100_row(printer, r);

         printf("\n\tPrinting row %d", i);
      }  /* ends loop over i */


   }  /* ends if d == b */


      /* a couple of blank lines before the histogram */
   end_graphics_mode(printer);
   select_proportional_font(printer);
   strcpy(image_name, "      ");
   my_fwriteln(printer, image_name);
   my_fwriteln(printer, image_name);

   select_300_dpi_resolution(printer);
   set_raster_width(printer);
   start_raster_graphics(printer);
   select_full_graphics_mode(printer);
   print_hist_image(printer, final_hist);

   end_graphics_mode(printer);
   select_proportional_font(printer);
   strcpy(image_name, "      ");
   my_fwriteln(printer, image_name);
   my_fwriteln(printer, image_name);
   my_fwriteln(printer, image_name);
   my_fwriteln(printer, image_name);
   my_fwriteln(printer, image_name);
   my_fwriteln(printer, image_name);
   my_fwriteln(printer, image_name);
   my_fwriteln(printer, image_name);

   sprintf(image_name, "                      %s", caption);
   my_fwriteln(printer, image_name);

   putc(FORMFEED, printer);

   printf("\nEnd");
}




print_hist_image(printer, hist)
   FILE *printer;
   long hist[];
{
   char   c, d;
   int          i, j, k, limit;
   long   max;

   d = 0;
   c = 170;

      /********************************
      *
      *   First scale the histogram
      *
      *********************************/

   max = 0;
   for(i=0; i<64; i++)
      if(hist[i] > max) max = hist[i];

printf("\n\n\nmax is %d", max);
   if(max > 200){
      for(i=0; i<64; i++){
        hist[i] = hist[i]*200/max;
printf("\nhist[i] = %d", hist[i]);
      }
   }

      /********************************
      *
      *   Second print it
      *
      *********************************/

   for(i=0; i<64; i++){
      printf("\n\t\thist row %d hist[i]=%d", i, hist[i]);

            /* print the line 8 times */
      for(k=0; k<8; k++){

         end_graphics_mode(printer);
         select_300_dpi_resolution(printer);
         set_raster_width(printer);
         start_raster_graphics(printer);
         select_full_graphics_mode(printer);
         set_horizontal_offset(printer);

            /* this prints 200 bytes so print 200 */
         putc(ESCAPE, printer);
         putc('*', printer);
         putc('b', printer);
         putc('2', printer);
         putc('0', printer);
         putc('0', printer);
         putc('W', printer);

         if(hist[i] >= 200) hist[i] = 200;
         limit = 200 - hist[i];

         if(hist[i] == 0)        putc(c, printer);

         for(j=0; j<hist[i]; j++)
           putc(c, printer);
         for(j=0; j<limit; j++)
           putc(d, printer);

      }  /* ends loop over k */

   }  /* ends loop over i */

}  /* print_hist_image */





select_proportional_font(printer)

   FILE *printer;
{
   putc(ESCAPE, printer);
   putc('(', printer);
   putc('0', printer);
   putc('U', printer);
   putc(ESCAPE, printer);
   putc('(', printer);
   putc('s', printer);
   putc('0', printer);
   putc('u', printer);
   putc('1', printer);
   putc('p', printer);
   putc('1', printer);
   putc('2', printer);
   putc('v', printer);
   putc('0', printer);
   putc('s', printer);
   putc('0', printer);
   putc('b', printer);
   putc('5', printer);
   putc('t', printer);
   putc('2', printer);
   putc('Q', printer);

}



select_full_graphics_mode(printer)

   FILE *printer;
{
   putc(ESCAPE, printer);
   putc('*', printer);
   putc('b', printer);
   putc('0', printer);
   putc('M', printer);

}


set_horizontal_offset(printer)

   FILE *printer;
{
   putc(ESCAPE, printer);
   putc('*', printer);
   putc('b', printer);
   putc('4', printer);
   putc('9', printer);
   putc('6', printer);
   putc('X', printer);

}


end_graphics_mode(printer)

   FILE *printer;
{
   putc(ESCAPE, printer);
   putc('*', printer);
   putc('r', printer);
   putc('B', printer);
}


set_raster_width(printer)

   FILE *printer;
{
   putc(ESCAPE, printer);
   putc('*', printer);
   putc('r', printer);
   putc('2', printer);
   putc('2', printer);
   putc('0', printer);
   putc('0', printer);
   putc('S', printer);

}



start_raster_graphics(printer)

   FILE *printer;
{
   putc(ESCAPE, printer);
   putc('*', printer);
   putc('r', printer);
   putc('0', printer);
   putc('A', printer);

}





select_300_dpi_resolution(printer)

   FILE *printer;
{
   putc(ESCAPE, printer);
   putc('*', printer);
   putc('t', printer);
   putc('3', printer);
   putc('0', printer);
   putc('0', printer);
   putc('R', printer);

}




print_byte_pattern(printer, b)
   FILE *printer;
   char b;
{
   char x;
   x = 1;

   putc(ESCAPE, printer);
   putc('*', printer);
   putc('b', printer);
   putc('1', printer);
   putc('W', printer);
   putc(b, printer);

}





print_bytes(printer, buffer)
   FILE *printer;
   char buffer[];
{
   int        i;

   putc(ESCAPE, printer);
   putc('*', printer);
   putc('b', printer);
   putc('2', printer);
   putc('0', printer);
   putc('0', printer);
   putc('W', printer);


   for(i=0; i<200; i++){
      putc(buffer[i], printer);
   }


}  /* ends print_bytes */









print_100_bytes(printer, buffer)
   FILE *printer;
   char buffer[];
{
   int        i;
   putc(ESCAPE, printer);
   putc('*', printer);
   putc('b', printer);
   putc('1', printer);
   putc('0', printer);
   putc('0', printer);
   putc('W', printer);

   for(i=0; i<100; i++){
      putc(buffer[i], printer);
   }
}  /* ends print_100_bytes */





print_original_100_row(printer, short_row)
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
      if(value < 32)
         value = 32 + (32-value) - 1;
      else
         if(value > 32)
            value = 32 - (value-32) - 1;


      for(j=0; j<8; j++){
         row[j][i] = patterns[value][j];
      }
   }

   for(i=0; i<8; i++){
      for(j=0; j<200; j++){
         c[j] = row[i][j];
      }
      set_horizontal_offset(printer);
      print_bytes(printer, c);
   }

}  /* ends print_original_100_row */





print_result_100_row(printer, short_row)
   FILE  *printer;
   short short_row[200];
{
   char  row[8][200];
   char  c[200], response[80];
   int         i, j, k;
   short value;

   for(i=0; i<200; i++){

      value = short_row[i];
      if(short_row[i] == 41) value = 63;
      if(short_row[i] == 69) value = 20;
      if(short_row[i] == 97) value = 40;


/***printf(" v=%d ", value);**/
      for(j=0; j<8; j++){
         row[j][i] = patterns[value][j];
      }
   }

   for(i=0; i<8; i++){
      for(j=0; j<200; j++){
         c[j] = row[i][j];
/*printf(" c[%d]=%x ", j, c[j]);*/
      }
printf("\ncalling print bytes i=%d j=%d", i, j);
      set_horizontal_offset(printer);
      print_bytes(printer, c);
   }

}  /* ends print_result_100_row */






perform_histogram_equalization(image, histogram)
   short image[ROWS][COLS];
   unsigned long histogram[];
{
   int area,
       i,
       j,
       k;
   unsigned long sum,
                 sum_of_h[256];
   double constant;


   sum = 0;
   for(i=0; i<256; i++){
      sum          = sum + histogram[i];
      sum_of_h[i] = sum;
   }

      /* constant = new # of gray levels div by area */
   constant = 64.0/20000.0;
   for(i=0; i<ROWS; i++){
      for(j=0; j<COLS; j++){
         k                 = image[i][j];
         image[i][j] = sum_of_h[k] * constant;
      }
   }
}  /* ends perform_histogram_equalization */






zero_long_histogram(histogram)
   unsigned long histogram[];
{
   int i;
   for(i=0; i<256; i++)
      histogram[i] = 0;
}  /* ends zero_long_histogram */




calculate_long_histogram(image, histogram)
   short  image[ROWS][COLS];
   unsigned long histogram[];
{
   int i,j,k;
   for(i=0; i<ROWS; i++){
      for(j=0; j<COLS; j++){
         k = image[i][j];
         histogram[k] = histogram[k] + 1;
      }
   }
}  /* ends calculate_long_histogram */
