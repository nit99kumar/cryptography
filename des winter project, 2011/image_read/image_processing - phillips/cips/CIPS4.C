
   /*************************** 
   * 
   *   cips4.c 
   *   COMPOSITE FILE COMPRISING: 
   *   gin.c 
   *   hist.c 
   *   pi.c 
   *   ht.c 
   * 
   ***************************\ 



        /*******************************************************
        *
        *       file d:\cips\gin.c
        *
        *       Functions: This file contains
        *           get_image_name
        *           get_directory_name
        *           extract_base_image_name
        *
        *       Purpose - This function prompts the user to
          *            enter the name of an image.
        *
        *       External Calls:
        *                       clear_buffer
        *
        *       Modifications:
        *           26 September 86 - now uses vision3.h
        *               instead of vision2.h and the read_string
        *               and get_integer instead of  scanf.
        *           11 March 1987 - this function was
        *               removed from the file ip.c and put
        *               in file gin.c.
        *
        ******************************************************/


#include "cips.h"




   /*********************************************
    *
    * get_image_name(...
    *
    * This function reads in the desired image
    * file name.
    *
    *********************************************/

get_image_name(name)
   char name[];
{
   char base_name[80],
        dir_name[80],
        new_name[80],
        response[80];
   int  l;

   printf("\n\nImage name is--%s\n", name);
   printf("\nDo you want to change:");
   printf("\n (f) file name");
   printf("\n (d) directory name");
   printf("\n (n) no change");
   printf("\n     _\b");
   gets(response);

   if((response[0] == 'F') ||
      (response[0] == 'f')){
      printf("\n\nEnter file name (name only no extension)");
      printf("\n--");
      gets(new_name);
      extract_directory_name(name, dir_name);
      sprintf(name, "%s%s.tif", dir_name, new_name);
   }

   if((response[0] == 'D') ||
      (response[0] == 'd')){
      printf("\n\nEnter directory name\n--");
      gets(dir_name);
      l = strlen(dir_name);
      if(dir_name[l-1] != 47){
         dir_name[l]   = '/';
         dir_name[l+1] = '\0';
      }
      printf("\n\nEnter file name (name only no extension)");
      printf("\n--");
      gets(new_name);
      sprintf(name, "%s%s.tif", dir_name, new_name);
   }

}       /* ends get_image_name  */




   /*********************************************
    *
    * extract_directory_name(...
    *
    * This function extracts the sub-directory
    * name out of a file name.
    *
    *********************************************/

extract_directory_name(file_name, dir_name)
   char file_name[], dir_name[];
{
   int i, j, k;

   i = 1;
   j = 0;
   k = 0;
   while(i){
      if(file_name[k] == 47  ||
         file_name[k] == 92)     j = k;
      if(file_name[k] == '\0')   i = 0;
      k++;
   }
   j++;
   strncpy(dir_name, file_name, j);
   dir_name[j] = '\0';

}  /* ends extract_directory_name */





   /*********************************************
    *
    *   extract_base_image_name(...
    *
    *   This function looks at a full file name
    *   and pulls off the sub-directory name and
    *   the file extension and returns the base
    *   file name.
    *
    *********************************************/

extract_base_file_name(file_name, base_name)
   char base_name[], file_name[];
{
   int i, j, k;
   i = 1;
   j = 0;
   k = 0;
   while(i){
      if(file_name[k] == 47  ||
         file_name[k] == 92)     j = k;
      if(file_name[k] == '\0')   i = 0;
      k++;
   }

   i = 1;
   k = 0;
   j++;
   while(i){
      if(file_name[j] == '.')
         i = 0;
      else
         base_name[k] = file_name[j];
      j++;
      k++;
   }
   k--;
    base_name[k] = '\0';
printf("\nEBN> base is %s", base_name);
}  /* ends extract_base_file_name */

   /**************************************************
   *
   *   file d:\cips\hist.c
   *
   *   Functions: This file contains
   *       calculate_histogram
   *       calculate_histogram
   *       zero_histogram
   *       perform_histogram_equalization
   *       show_histogram
   *       print_histogram
   *       smooth_histogram
   *       display_histogram
   *
   *   Purpose: These functions calculate and display 
   *      the histogram of an input image array.
   *
   *   Modifications:
   *       July 86 - ported to IBM-PC
   *       August 1990 - modified for use in the
   *           C Image Processing System
   *       March 1992 - removed the hardwired values
   *           of 100 and replaced them with ROWS
   *           and COLS.  There are still some
   *           hardwired numbers in this file, but
   *           they deal with displaying a histogram.
   *       October 4, 1992 - added the smooth histogram
   *           function.
   *
   **************************************************/



#define PRINT_WIDTH  80
#define FORMFEED     '\014'




          /*****************************************
          *
          *    perform_histogram_equalization(...
          *
          *    This function performs histogram
          *    equalization on the input image array.
          *
          ******************************************/

perform_histogram_equalization(image, histogram,
                               new_grays, area)
   float new_grays, area;
   short image[ROWS][COLS];
   unsigned long histogram[];
{
   int i,
       j,
       k;
   unsigned long sum,
            sum_of_h[256];

   double constant;

   sum = 0;
   for(i=0; i<256; i++){
      sum         = sum + histogram[i];
      sum_of_h[i] = sum;
   }

      /* constant = new # of gray levels div by area */
   constant = new_grays/area;
   for(i=0; i<ROWS; i++){
      for(j=0; j<COLS; j++){
         k           = image[i][j];
         image[i][j] = sum_of_h[k] * constant;
      }
   }
}  /* ends perform_histogram_equalization */




        /*****************************************
        *
        *   zero_histogram(...
        *
        *   This function clears or zeros a
        *   histogram array.
        *
        ******************************************/

zero_histogram(histogram)
   unsigned long histogram[];
{
   int i;
   for(i=0; i<=GRAY_LEVELS; i++)
      histogram[i] = 0;
}  /* ends zero_histogram */




        /*****************************************
        *
        *   calculate_histogram(...
        *
        *   This function calculates the histogram
        *   for an input image arry.
        *
        ******************************************/

calculate_histogram(image, histogram)
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
}  /* ends calculate_histogram */




        /******************************************
        *
        *   show_histogram(histogram)
        *
        *   This function shows the histogram
        *   on the screen as numbers and stars.
        *
        *******************************************/

show_histogram(histogram)
        unsigned long histogram[];
{
        int     count,
                i,
                j;
        unsigned long max, scale;


        max   = 0;
        count = 0;

        for(i=0; i<GRAY_LEVELS; i++)
           if(histogram[i] > max)
              max = histogram[i];

        if(max > (70 - 12))
           scale = max/(70 - 12);
        else
           scale = 1;

        printf("\n max=%ld scale=%ld",max, scale);

        printf("\n\ngray    count");
        printf("\nlevel");

        for(i=0; i<256; i++){
           if(histogram[i] == 0)
              ++count;
           else
              count = 0;

           if(count < 2){
              printf("\n %4d: %7ld",i,histogram[i]);
              for(j=0; j<((int)(histogram[i]/scale));
                  j++){
                 printf("*");
              }   /* ends loop over j             */
           }      /* ends if count < 5            */
        }         /* ends loop over i GRAY_LEVELS */
}       /* ends show_histogram */



        /*********************************************
        *
        *   print_histogram(histogram)
        *
        *   This function prints the histogram
        *   input to the function.
        *
        **********************************************/

print_histogram(histogram, name)
        char name[];
        unsigned long histogram[];
{
        char    string[300],
                output[300];

        int     count,
                i,
                j,
                line_counter,
                print_counter;
        unsigned long scale, max;

        FILE *printer;

        if( (printer = fopen("prn", "w")) == NULL)
           printf("\nPH> Could not open printer");
        else
           printf("\nPH> The print file is opened");

        max           = 0;
        count         = 0;
        print_counter = 0;

        for(i=0; i<256; i++)
           if(histogram[i] > max)
              max = histogram[i];

        if(max > (PRINT_WIDTH - 12))
           scale = max/(PRINT_WIDTH - 12);
        else
           scale = 1;

        printf("\n max=%ld scale=%ld",max, scale);

        printf("\nPI> Print header");
        line_counter = 0;
        hist_long_clear_buffer(string);
        sprintf(string, 
           "          This image is -- %s --\n", 
           name);
        fputs(string, printer);
        ++line_counter;

        hist_long_clear_buffer(string);
        sprintf(string, " \n");
        fputs(string, printer);
        ++line_counter;

        hist_long_clear_buffer(string);
        sprintf(string, "          gray    count\n");
        fputs(string, printer);
        ++line_counter;
        hist_long_clear_buffer(string);
        sprintf(string, "          level\n");
        fputs(string, printer);
        ++line_counter;

        for(i=0; i<256; i++){
           if(histogram[i] == 0)
              ++count;
           else
              count = 0;

           if(count < 2){
              printf(" %4d: %7ld",i,histogram[i]);
              print_counter++;
              if(print_counter >= 6){
                 printf("\n");
                 print_counter = 0;
              }  /* ends if print_counter >= 6 */

              hist_long_clear_buffer(string);
              sprintf(string,
                "           %3d: %7ld ->",
                i,histogram[i]);
              fputs(string, printer);
              hist_long_clear_buffer(string);
              sprintf(output, " ");
              for(j=0; j<((int)(histogram[i]/scale)); 
                  j++){
                 sprintf(string, "*");
                 strcat(output, string);
              }         /* ends loop over j */
              fputs(string, printer); 
              fputc('\n', printer);
              ++line_counter;
              if(line_counter >= 55){
                 line_counter = 0;
                 putc(FORMFEED, printer);
              }  /* ends if line_counter >=55  */
           }  /* ends if count < 2 */
        }  /* ends loop over i */
        putc(FORMFEED, printer);
        fclose(printer);

}        /* ends print_histogram */




        /*******************************************
        *
        *   display_histogram(histogram)
        *
        *   This function shows the histogram
        *   input to the function.
        *
        ********************************************/

display_histogram(histogram, x, y,
                  line_color, data_color)
        int data_color, line_color, x, y;
        unsigned long histogram[];
{
        int     count,
                i,
                j,
                length;
        unsigned long scale, max;

        max   = 0;
        count = 0;

        for(i=0; i<256; i++)
           if(histogram[i] > max)
              max = histogram[i];

        if(max > (200 - 12))
           scale = max/(100 - 25);
        else
           scale = 1;

   /***************************
   *
   *   clear out an area for
   *   this histogram display
   *
   ****************************/

        my_setcolor(line_color);
        for(i=0; i<258; i++){
           for(j=0; j<100; j++){
              my_setpixel(x-1+i, y-j);
           }
        }


         /***************************
         *
         *  draw the histogram axes
         *
         ****************************/

        my_setcolor(0);

        my_moveto(x, y);
        my_lineto(x+255, y);
        my_moveto(x, y);
        my_lineto(x, y-95);
        my_moveto(x+50, y);
        my_lineto(x+50, y-95);
        my_moveto(x+100, y);
        my_lineto(x+100, y-95);
        my_moveto(x+150, y);
        my_lineto(x+150, y-95);
        my_moveto(x+200, y);
        my_lineto(x+200, y-95);
        my_moveto(x+255, y);
        my_lineto(x+255, y-95);

         /***************************
         *
         *  loop thru the histogram
         *  and plot the scaled data
         *
         ****************************/

        my_setlinestyle(0xFFFF);
        my_setcolor(data_color);

        for(i=0; i<256; i++){
           if(histogram[i] != 0){
              length = histogram[i]/scale;
              my_moveto(x+i, y);
              my_lineto(x+i, y-length);
           }
        }  /* ends loop over i GRAY_LEVELS */
}  /* ends display_histogram */




       /*****************************************
       *
       *   display_menu_for_histogram(...
       *
       *   This function shows the options
       *   available for histogram function.
       *
       ****************************************/

display_menu_for_histogram(print, vertical, 
                           horizontal)
   int *print, *vertical, *horizontal;
{
   char response[80];
   int  int_response, not_finished, r;

   not_finished = 1;
   while(not_finished){
      printf(
      "\n\nHISTOGRAM> Enter choice (0 for no change)");
      printf(
      "\nHISTOGRAM> 1. print is %d (1=print 0=display)",
         *print);
      printf(
      "\nHISTOGRAM> 2. # of vertical %dx%d areas %d",
         ROWS, COLS, *vertical);
      printf(
      "\nHISTOGRAM> 3. # of horizontal %dx%d areas %d",
         ROWS,COLS,*horizontal);
      printf("\nHISTOGRAM> _\b");
      get_integer(&r);

      if(r == 0)
         not_finished = 0;


      if(r == 1){
         printf(
            "\nHISTOGRAM> Enter 1 for print or 0 for display");
         printf("\nHISTOGRAM> ___");
         get_integer(&int_response);
         *print = int_response;
      }  /* ends if r == 1 */

      if(r == 2){
      printf(
     "\nHISTOGRAM> Enter # of vertical %dx%d areas ",
            ROWS,COLS);
         printf("\nHISTOGRAM> _\b");
         get_integer(&int_response);
         *vertical = int_response;
      }  /* ends if r == 2  */

      if(r == 3){
         printf(
     "\nHISTOGRAM> Enter # of horizontal %dx%d areas ",
          ROWS,COLS);
         printf("\nHISTOGRAM> ___");
         get_integer(&int_response);
         *horizontal = int_response;
      }  /* ends if r == 3 */

   }  /* ends while not_finished  */
}  /* ends display_menu  */




      /********************************************
      *
      *   calculate_area_histogram(...
      *
      *   This function calculates the histogram
      *   for several ROWSxCOLS arrays of an
      *   image.  The user inputs the number
      *   of vertical and horizontal ROWSxCOLS
      *   arrays to be histogramed.  This function
      *   uses the image file name to read the image
      *   arrays and calculate the overall
      *   histogram.
      *
      *********************************************/

calculate_area_histogram(histogram, vertical, 
                   horizontal, the_image, name, il, 
                   ie, ll, le)
   char     name[];
   int      horizontal, il, ie, ll, le, vertical;
   short    the_image[ROWS][COLS];
   unsigned long histogram[];
{
   int count, i, j;

   printf("\nCalculating histograms");
   zero_histogram(histogram);
   count = 1;

   for(i=0; i<vertical; i++){
      for(j=0; j<horizontal; j++){
         printf("\n\tcalculating %d of %d",
               count, horizontal*vertical);
         read_tiff_image(name, the_image,
               il+i*ROWS, ie+j*COLS, 
               ll+i*ROWS, le+j*COLS);
         calculate_histogram(the_image, histogram);
         count++;
      }  /* ends loop over j */
   }  /* ends loop over i */

}  /* calcualte_area_histogram */



      /********************************************
      *
      *   smooth_histogram(...
      *
      *   This function smoothes the input histogram
      *   and returns it.  It uses a simple averaging
      *   scheme where each point in the histogram
      *   is replaced by the average of itself and
      *   the two points on either side of it.
      *
      *********************************************/

smooth_histogram(histogram)
   unsigned long histogram[];
{
   int i;
   unsigned long new_hist[GRAY_LEVELS+1];

   zero_histogram(new_hist);

   new_hist[0] = (histogram[0] + histogram[1])/2;
   new_hist[GRAY_LEVELS] =
      (histogram[GRAY_LEVELS] +
       histogram[GRAY_LEVELS-1])/2;

   for(i=1; i<GRAY_LEVELS; i++){
      new_hist[i] = (histogram[i-1] +
                     histogram[i]   +
                     histogram[i+1])/3;
   }

   for(i=0; i<=GRAY_LEVELS; i++)
      histogram[i] = new_hist[i];

}  /* ends smooth_histogram */



hist_long_clear_buffer(string)
   char string[];
{
   int i;
   for(i=0; i<300; i++)
      string[i] = ' ';
}


   /**************************************************
   *
   *   file d:\cips\pi.c
   *
   *   Functions: This file contains
   *       print_image
   *       print_image_array
   *       perform_printing
   *       print_column_header
   *
   *   Purpose - These functions print an image out 
   *       to the line printer.  The parameters in 
   *       this function are defined differently 
   *       than in most other CIPS functions.  
   *       The parameters il, ie, ll, le are 
   *       coordinates inside the 100x100 image array.
   *       The parameters first_line and first_element
   *       are coordinates for the entire image file.
   *       So, if you want to start printing at row 10
   *       and column 10 of the image file you would 
   *       call:
   *
   *       read_tiff_image(name, the_image, 10, 10,
   *                       110, 110);
   *       print_image(the_image, name, 1, 1, 1, 
   *                   100, 18, 10, 10);
   *
   *       In normal print mode you can only print 17
   *       columns.
   *       le - ie = 17.
   *
   *
   *   External Calls:
   *       none
   *
   *   Modifications:
   *       6 January 1987 - created
   *       14 August 1990 - modified to be part of the
   *           C Image Processing System
   *       14 June 1993 - removed calls to
   *           my_fwrite my_fwriteln
   *
   **************************************************/

#define  FORMFEED  '\014'

   /*********************************************
   *
   *   printf_image(...
   *
   *********************************************/

print_image(the_image, name, channel, il, ie, ll, le,
            first_line, first_element)
   char  name[];
   int   channel, il, ie, ll, le, 
         first_line, first_element;
   short the_image[ROWS][COLS];
{
   char printer_name[MAX_NAME_LENGTH];
   FILE *printer;

   strcpy(printer_name, "prn");
   if( (printer = fopen(printer_name, "w")) == NULL)
      printf("\nPI> Could not open printer");
   else{
      printf("\nPI> The print file is opened");

           /*****************************************
           *
           *   If your printer has some form of
           *   condensed printing, you can send those
           *   commands via software using the fputc
           *   function.  For example, if your printer
           *   needs to sequence X Y Z to start
           *   condensed printing, insert the following
           *   three calls right here:
           *   fputc('X', printer);
           *   fputc('Y', printer);
           *   fputc('Z', printer);
           *
           ******************************************/

      perform_printing(printer, the_image, name, 
                       channel, il, ll, ie, le, 
                       first_line, first_element);
      fclose(printer);
   }  /* ends else print  */
}     /* ends print_image  */




   /*********************************************
   *
   *   perform_printing(...
   *
   *********************************************/

perform_printing(printer, the_image, name, channel,
                 il, ll, ie, le, first_line, 
                 first_element)
   char  name[];
   FILE  *printer;
   int   channel, il, ie, ll, le, 
         first_line, first_element;
   short the_image[ROWS][COLS];
{
   char output[3*COLS],
        response[80],
        string[3*COLS];
   int  i,
        j,
        k,
        line_counter;


   printf("\nPI> Print header");
   line_counter = 0;
   pi_long_clear_buffer(string);
   sprintf(string, "     This image is -- %s --\n", 
           name);
   fputs(string, printer);
   ++line_counter;

   pi_long_clear_buffer(string);
   sprintf(string, "     The parameters are:\n");
   fputs(string, printer);
   ++line_counter;

   pi_long_clear_buffer(string);
   sprintf(string, 
           "     channel=%d il=%d ll=%d ie=%d le=%d\n",
           channel, first_line, first_line+ll-2,
           first_element, first_element+le-2);
   fputs(string, printer);
   ++line_counter;

   pi_long_clear_buffer(string);
   sprintf(string, " \n");
   fputs(string, printer);
   ++line_counter;

   print_column_header(&line_counter, first_element, 
                       ie, le, output, string, 
                       printer);

   for(i=il; i<ll; i++){

      pi_long_clear_buffer(string);

        /* now print the image  */
      sprintf(string, "      ");
      pi_long_clear_buffer(output);
      sprintf(output, "%3d-", i+first_line-1);
      strcat(string, output);
      for(j=ie; j<le; j++){
         pi_long_clear_buffer(output);
         sprintf(output,"%4d",the_image[i][j]);
         strcat(string, output);
      }  /* ends loop over j columns */

      fputs(string, printer); fputc('\n', printer);
      line_counter = line_counter + 1;
      if(line_counter >= 53){
         line_counter = 0;
         putc(FORMFEED, printer);
         print_column_header(&line_counter, 
                             first_element, ie,
                             le, output, string, 
                             printer);
      }  /* ends if line_counter >=65  */
   }  /* ends loop over i rows */

   for(i=line_counter; i<66; i++){
      pi_long_clear_buffer(string);
      sprintf(string, " \n");
      fputs(string, printer);
   }

}     /* ends perform_printing  */




   /*********************************************
   *
   *   print_column_header(...
   *
   *********************************************/

print_column_header(line_counter, first_element, 
                    ie, le, output,
                    string, printer)
   char string[], output[];
   FILE *printer;
   int  first_element, ie, le, *line_counter;
{
   int k;

   pi_long_clear_buffer(string);
   sprintf(string, "          ");


   for(k=first_element;k<(first_element+(le-ie));k++){
      pi_long_clear_buffer(output);
      sprintf(output, "-%3d", k);
      strcat(string, output);
   }  /* ends loop over k  */
   fputs(string, printer); fputc('\n', printer);
   *line_counter = *line_counter + 1;
}  /* ends print_column_header  */




   /*************************************************
   *
   *   print_image_array(...
   *
   *   This function prints a 100x100 image array.
   *
   **************************************************/

print_image_array(the_image)
   short the_image[ROWS][COLS];
{
   char response[80];
   printf("\nPIA> Enter a comment line\n--");
   gets(response);
                                /*     il  ie  ll    le    */
   print_image(the_image, response, 0,  1,  1, 100,  17, 0, 0);
   print_image(the_image, response, 0,  1, 18, 100,  35, 0, 0);
   print_image(the_image, response, 0,  1, 36, 100,  53, 0, 0);
   print_image(the_image, response, 0,  1, 54, 100,  71, 0, 0);
   print_image(the_image, response, 0,  1, 72, 100,  89, 0, 0);
   print_image(the_image, response, 0,  1, 90, 100, 100, 0, 0);

}  /* ends print_image_array  */




   /*********************************************
   *
   *   pi_long_clear_string(...
   *
   *********************************************/

pi_long_clear_buffer(string)
   char string[];
{
   int i;
   for(i=0; i<300; i++)
      string[i] = ' ';
}


   /**************************************************
   *
   *   file d:\cips\ht.c
   *
   *   Functions: This file contains
   *       display_using_halftoning
   *       half_tone
   *       show_half_tone
   *       get_threshold_value
   *
   *   Purpose: This program displays an image using 
   *      a halftoning process.  The algorithm was 
   *      taken from "Personal computer based image 
   *      processing with halftoning," John A Saghri, 
   *      Hsieh S. Hou, Andrew Tescher, Optical 
   *      Engineering, March 1986, Vol.25, No. 3, 
   *      pp 499-503. The display_using_halftoning
   *      determines display size and reads the image.
   *      The half_tone function implements the 
   *      algorithm shown on page 502 of the article.
   *
   *      The function print_halftone_array prints 
   *      a half toned image array to a regular line 
   *      printer.
   *
   *
   *   External Calls:
   *      rtiff.c - read_tiff_image
   *      numcvrt.c - get_integer
   *
   *   Modifications:
   *      30 September 86 - created
   *      18 August 1990 - modified for use in the
   *          C Image Processing System.
   *
   *
   **************************************************/



#define  FORMFEED  '\014'

float eg[ROWS][COLS], ep[ROWS][COLS];

display_using_halftoning(in_image, file_name,
         il, ie, ll, le, threshold, invert,
         image_colors, image_header, monitor_type,
         print, show_hist, color_transform)

   char  color_transform[], file_name[], 
         monitor_type[];
   int   image_colors, invert,
         il, ie, ll, le, threshold,
         print, show_hist;
   short in_image[ROWS][COLS];
   struct tiff_header_struct *image_header;

{
   char response[80];

   int  a,
        b,
        channel,
        color,
        count,
        data_color,
        display_mode,
        horizontal,
        i,
        j,
        k,
        l,
        line_color,
        max_horizontal,
        max_vertical,
        not_finished,
        one,
        vertical,
        x,
        x_offset,
        y,
        y_offset,
        zero;

   float area, new_grays;

   unsigned long histogram[256], new_hist[256];


       if(  (show_hist == 1)  &&
            (color_transform[0] != 'H'))
          zero_histogram(histogram);

         /*******************************************
         *
         *   Use the monitor type to set the vertical
         *   horizontal and display_mode parameters.
         *   Also set the values for one and zero.
         *   one and zero will vary depending on the
         *   monitor type.
         *
         ********************************************/


      if(  (monitor_type[0] == 'M')  ||
           (monitor_type[0] == 'm')){
         vertical     = 3;
         horizontal   = 2;
         display_mode = HRESBW;
         one          = 1;
         zero         = 0;
      }

      if(  (monitor_type[0] == 'C')  ||
           (monitor_type[0] == 'c')){
         vertical     = 3;
         horizontal   = 2;
         display_mode = MRES4COLOR;
         one          = 3;
         zero         = 1;
      }

      if(  (monitor_type[0] == 'V')  ||
           (monitor_type[0] == 'v')){
         vertical     = 6;
         horizontal   = 4;
         display_mode = VRES16COLOR;
         one          = 5;
         zero         = 1;
      }

      if(  (monitor_type[0] == 'E')  ||
           (monitor_type[0] == 'e')){
         vertical     = 6;
         horizontal   = 3;
         display_mode = ERESCOLOR;
         one          = 5;
         zero         = 1;
      }

      max_horizontal = (image_header->image_length+50)
                         /COLS;
      max_vertical   = (image_header->image_width+50)
                         /ROWS;

      if(horizontal > max_horizontal) 
            horizontal = max_horizontal;
      if(vertical > max_vertical) 
            vertical = max_vertical;

      if(print == 1){
         vertical   = 1;
         horizontal = 1;
      }



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

               printf("\nHT> Calculating histogram");
               printf(" %d of %d",
                      count,vertical*horizontal);
               count++;

               read_tiff_image(file_name, in_image, 
                            il+y, ie+x, ll+y, le+x);
               calculate_histogram(in_image, 
                                   histogram);

            }  /* ends loop over b */
         }  /* ends loop over a */
      }  /* ends if display_mode == H */




           /* set graphics mode */
      if(print == 0)
         my_setvideomode(display_mode); /* MSC 6.0 */
      else{
         printf("\n\nHT> Calculating for printing ");
         printf("\nHT> Counting from 0 to 99\n");
      }

        /********************************************
        *
        *   Loop over horizontal and vertical. Read
        *   the image array and display it after
        *   calculating the half tone values.
        *
        *
        *   If you want to show the histogram AND
        *   do not want to do hist equalization
        *   then calculate the hist from the
        *   original image array.
        *
        *   If you want to do hist equalization
        *   then calculate the new_hist AFTER
        *   the image has been equalized.
        *
        *   We will equalize the histogram down
        *   to half the original shades of gray
        *   and will cut the threshold in half.
        *
        *****************************************/



      for(i=0; i<horizontal; i++){
         for(j=0; j<vertical; j++){
            read_tiff_image(file_name, in_image, 
                           il+i*ROWS, ie+j*COLS, 
                           ll+i*ROWS, le+j*COLS);

            if(   (show_hist == 1)  &&
                  (color_transform[0] != 'H'))
               calculate_histogram(in_image, histogram);

            if(color_transform[0] == 'H'){

               area = ((long)(vertical))*
                      ((long)(horizontal));
               area = area*10000.0;
               new_grays = image_colors/2;

               perform_histogram_equalization(in_image,
                        histogram, new_grays, area);

               calculate_histogram(in_image, new_hist);

            }  /* ends if color_transform == S */

            if(color_transform[0] == 'H')
               half_tone(in_image, threshold/2, 
                      eg, ep, i, j,
                      one, zero, invert, print);

            else
               half_tone(in_image, threshold, 
                      eg, ep, i, j,
                      one, zero, invert, print);

         }  /* ends loop over j */
      }  /*  ends loop over i */



         /***************************
         *
         *   if show_hist == 1 then
         *   display the histogram
         *   in the lower right hand
         *   corner of screen
         *
         ****************************/

      if(  (show_hist == 1)   &&
           (print == 0)){

         if(monitor_type[0] == 'V'){
            y_offset   = 470;
            x_offset   = 380;
            line_color = 3;
            data_color = 8;
         }

         if(monitor_type[0] == 'E'){
            y_offset   = 310;
            x_offset   = 380;
            line_color = 3;
            data_color = 8;
         }

         if(monitor_type[0] == 'M'){
            y_offset   = 190;
            x_offset   = 60;
            line_color = 1;
            data_color = 1;
         }

         if(monitor_type[0] == 'C'){
            y_offset   = 190;
            x_offset   = 60;
            line_color = 1;
            data_color = 3;
         }

         if(color_transform[0] == 'S')
            display_histogram(histogram, x_offset,
                   y_offset, line_color, data_color);

         if(color_transform[0] == 'H')
            display_histogram(new_hist, x_offset,
                   y_offset, line_color, data_color);

      }  /* ends if show_hist == 1 and print == 0 */



      if(print == 1) 
         printf("\n\nHT> Hit ENTER to continue");
      gets(response);
      my_clear_text_screen();

}  /* ends main  */





       /*********************************************
       *
       *   half_tone(...
       *
       *   ep[m][n] = sum of erros propogated
       *               to position (m,n).
       *   eg[m][n] = total error generated at
       *               location (m,n).
       *
       **********************************************/

half_tone(in_image, threshold, eg, ep, yoff,
           xoff, one, zero, invert, print)
   int   invert, threshold, xoff, yoff,
         one, print, zero;
   float eg[ROWS][COLS], ep[ROWS][COLS];
   short in_image[ROWS][COLS];
{
   float c[2][3],
         sum_p,
         t,
         tt;
   int   color, count, i, j, m, n;
   short srow, scol;

   c[0][0] = 0.0;
   c[0][1] = 0.2;
   c[0][2] = 0.0;
   c[1][0] = 0.6;
   c[1][1] = 0.1;
   c[1][2] = 0.1;
   count   =   0;

      /***********************************************
      *
      *   Calculate the total propogated error
      *   at location(m,n) due to prior
      *   assignment.
      *
      *   Go through the input image.  If the output
      *   should be one then display that pixel as such.
      *   If the output should be zero then display it
      *   that way.
      *
      *   Also set the pixels in the input image array
      *   to 1's and 0's in case the print option
      *   was chosen.
      *
      ************************************************/

   for(i=0; i<ROWS; i++){
      for(j=0; j<COLS; j++){
         eg[i][j] = 0.0;
         ep[i][j] = 0.0;
      }
   }

       /**********************************************
       *
       *   29 February 1988 - Fix to remove a solid 
       *   line at the bottom of each region. Loop 
       *   over ROWS-1 and then draw an extra line.
       *
       **********************************************/

   for(m=0; m<ROWS-1; m++){
      for(n=0; n<COLS; n++){

         sum_p = 0.0;
         for(i=0; i<2; i++){
            for(j=0; j<3; j++){
               sum_p = sum_p + 
                       c[i][j] * eg[m-i+1][n-j+1];
            }  /* ends loop over j */
         }     /* ends loop over i */

         ep[m][n] = sum_p;
         t = in_image[m][n] + ep[m][n];
         tt = t;

               /**********************************
               *
               *    Here set the point [m][n]=one
               *
               ***********************************/

         if(t > threshold){
            eg[m][n] = t - threshold*2;
            ++count;
            color = one;
            if(invert == 1) color = zero;

            scol = (short)(n + xoff*COLS);
            srow = (short)(m + yoff*ROWS);

            if(invert == 1)
               in_image[m][n] = 1;
            else
               in_image[m][n] = 0;

            if(print == 0){
               my_setcolor(color);      /* MSC 6.0 */
               my_setpixel(scol, srow); /* MSC 6.0 */
            }  /* ends if print == 0 */
         }  /* ends if t > threshold  */


               /**********************************
               *
               *    Here set the point [m][n]=zero
               *
               ***********************************/

         else{
            eg[m][n] = t;
            color = zero;
            if(invert == 1) color = one;

            scol = (short)(n + xoff*COLS);
            srow = (short)(m + yoff*ROWS);

            if(invert == 1)
               in_image[m][n] = 0;
            else
               in_image[m][n] = 1;

            if(print == 0){
               my_setcolor(color);      /* MSC 6.0 */
               my_setpixel(scol, srow); /* MSC 6.0 */
            } /* ends if print == 0 */

         }  /* ends else t <= threshold  */

      }  /* ends loop over n columns */

      if(print == 1) printf("%3d", m);

   }         /* ends loop over m rows */


      /* Now display an extra line if print is 0 */
   if(print == 0){
      for(j=0; j<COLS; j++){

        if(in_image[ROWS-2][j] == 1)
           color = zero;
        else
           color = one;

        if(invert == 1){
            if(in_image[ROWS-2][j] == 1)
               color = zero;
            else
               color = one;
        }  /* ends if invert == 1 */

        scol = (short)(j + xoff*COLS);
        srow = (short)(ROWS-1 + yoff*ROWS);
        my_setcolor(color);      /* MSC 6.0 */
        my_setpixel(scol, srow); /* MSC 6.0 */

      }        /* ends loop over j */
   }        /* ends if print == 0 */


   if(print == 1) print_halftone_array(in_image);

}  /*  ends half_tone  */





   /*******************************
   *
   *   get_threshold_value(...
   *
   ********************************/

get_threshold_value(threshold, print)
   int *print, *threshold;
{
   int i;
   printf("\nHT> The threshold = %d", *threshold);
   printf("\nHT> Enter new theshold value ");
   printf("(0 for no change) \n___\b\b\b");
   get_integer(&i);
   if((i != 0) && (i!= *threshold))
      *threshold = i;

   printf(
      "\nHT> print = %d (1 for print  0 for display)", 
      *print);
   printf("\nHT> Enter print value  \n_\b");
   get_integer(&i);
   *print = i;
}




   /********************************************
   *
   *   print_halftone_array(...
   *
   *   This function takes the halftoned images
   *   and prints it to a line printer.  If the
   *   image array has a 1 then print a ' '.
   *   If the image array has a 0 then print
   *   a '*'.
   *
   *********************************************/


print_halftone_array(image)
   short image[ROWS][COLS];
{
   char printer_name[80], response[80], string[101];
   FILE *printer;
   int        i, j, l, line_counter;

   line_counter = 0;

   strcpy(printer_name, "prn");

   if( (printer = fopen(printer_name, "w")) == NULL)
      printf("\nHT> Could not open printer");
   else{
      printf("\nOpened printer and now printing");

    /*************************************************
    *
    *   Loop over the rows in the image.  For each row
    *   first clear out the string print buffer.
    *   Then go through the columns and set the string
    *   to either '*' or ' '.  Finally, write the
    *   string out to the printer.
    *
    *************************************************/

   for(i=0; i<ROWS; i++){

      for(l=0; l<COLS+1; l++) string[l] = '\0';

      for(j=0; j<COLS; j++){
         if(image[i][j] == 1)
            string[j] = '*';
         else
            string[j] = ' ';
      }  /* ends loop over j columns */

      printf("%3d", i);
      fputs(string, printer);
      fputc('\n', printer);
      line_counter = line_counter + 1;
      if(line_counter >= 53){
         line_counter = 0;
         putc(FORMFEED, printer);
      }  /* ends if line_counter >=53  */

   }  /* ends loop over i rows */


   }  /* ends opened printer */

   putc(FORMFEED, printer);
   fclose(printer);

}  /* ends print_halftone_array */
