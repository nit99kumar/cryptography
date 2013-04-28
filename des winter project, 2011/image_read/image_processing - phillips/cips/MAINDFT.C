


       /**************************************************
       *
       *       file d:\cips\maindft.c
       *
       *       Functions: This file contains
	   *          main
       *
       *       Purpose:
	   *          Serve as a test bed calling routine
	   *          for Discrete Fourier Transforms.
       *
       *       External Calls:
       *
       *       Modifications:
	   *          08 February 1991 - created
       *
       **************************************************/

#include "d:\cips\cips.h"


short image1[ROWS][COLS];
short image2[ROWS][COLS];
short image3[ROWS][COLS];
short image4[ROWS][COLS];

main()
{



   char caption[80],
        color_transform[80],
        monitor_type[80],
        name[80],
        name2[80],
        rep[80];

   int  color,
        display_colors,
        file_d,
        first_element,
        first_line,
        horizontal,
        i,
        ie,
        il,
        image_colors,
        invert,
        j,
        le,
		length,
        ll,
        not_finished,
        print,
        response,
        show_hist,
        threshold,
        vertical,
		width;

   long     mean_of_pixels;
   unsigned long histogram[256];
   struct   tiff_header_struct image_header;


   clear_text_screen();


   not_finished   =  1;
   response       = 99;

   il             = 1;
   ie             = 1;
   ll             = ROWS+1;
   le             = COLS+1;

   print          =   0;
   threshold      = 128;
   display_colors = 16;
   image_colors   = 16;
   invert         =  0;
   vertical       =  3;
   horizontal     =  3;
   show_hist      =  0;
   strcpy(color_transform, "Straight mode");
   strcpy(monitor_type, "VGA");


   strcpy(name,  "d:/pix/adam256.tif");
   strcpy(name2, "d:/pix/output.tif");

   printf("\nCIPS> Enter input image name\n");
   get_image_name(name);
   printf("\nCIPS> Enter output image name\n");
   get_image_name(name2);
   get_parameters(&il, &ie, &ll, &le);

   read_tiff_header(name, &image_header);

   length = image_header.image_length/ROWS;
   width  = image_header.image_width/COLS;

   printf("\nMAINEDGE> lenght=%d  width=%d", length, width);

   /*************
   for(i=0; i<length; i++){
      for(j=0; j<width; j++){
      }
   }
   *************/

   perform_fourier_transform(name, name2, image1, 
                             image2, image3, image4,
                             il, ie, ll, le);


}  /* ends main */


clear_text_screen()
{
   _setvideomode(_TEXTC80);      /* MSC 6.0 statements */
   _setbkcolor(1);
   _settextcolor(7);
   _clearscreen(_GCLEARSCREEN);
}  /* ends clear_text_screen */

