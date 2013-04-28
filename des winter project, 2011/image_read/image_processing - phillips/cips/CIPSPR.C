

    /***********************************************
    *
    *   file d:\cips\cipspr.c
    *
    *   Functions: This file contains
    *      main
    *
    *   Purpose:
    *      This is the main routine of a
    *      stand alone program that prints
    *      TIFF images to the HP DeskJet printer
    *      graphically.
    *
    *      You can run this program using either
    *      the command line or via menus.
    *
    *      cipspr image-name [help -i -h -e -il -ie -t]
    *
    *      cipspr help - gives you a usage message
    *
    *      cipspr - gives you the menu interface
    *
    *      cipspr image-name - displays that image
    *
    *      Options:
    *         -i  - invert the image when displaying
    *         -h  - show the histogram
    *         -e  - use histogram equalization
    *         -il - specify initial line
    *         -ie - specify initial element
    *         -t  - means the words following -t
    *               are a title to display below
    *               the image.  Always put the -t
    *               as the final option.
    *
    *   External Calls:
    *      gin.c - get_image_name
    *      rtiff.c - read_tiff_image
    *      tiff.c - read_tiff_header
    *      display.c - display_menu_for_display_image
    *      djet.c - print_graphics_image
    *
    *   Modifications:
    *      5 June 1993 - created
    *
    *************************************************/

#include "cips.h"


short the_image[ROWS][COLS];
short the_image2[ROWS][COLS];

main(argc, argv)
   int argc;
   char *argv[];
{
   char color_transform[80],
        monitor_type[80],
        name[80],
        title[80];

   int  display_colors = 16,
        i              = 0,
        ie             = 1,
        il             = 1,
        image_colors   = 256,
        invert         = 0,
        j              = 0,
        l              = 0,
        le             = COLS+1,
        ll             = ROWS+1,
        show_hist      = 0;

   struct tiff_header_struct image_header;

   my_clear_text_screen();

   strcpy(name, "d:/pix/adam256.tif");
   strcpy(color_transform, "Straight mode");
   strcpy(monitor_type, "VGA");
   strcpy(title, "");

   if(argc == 2  && (strcmp(argv[1], "help") == 0)){
      printf("\nusage: cipspr image-name"
             " [help -i -h -e -il -ie -t]\n");
      exit(1);
   }  /* ends if cipspr help */

   if(argc > 1){
      strcpy(name, argv[1]);
      read_tiff_header(name, &image_header);

      i=2;
      while(i <= argc){
         if(strcmp(argv[i], "-i") == 0)
            invert = 1;
         if(strcmp(argv[i], "-h") == 0)
            show_hist = 1;
         if(strcmp(argv[i], "-e") == 0)
            strcpy(color_transform, 
                   "Histogram Equalization");
         if(strcmp(argv[i], "-il") == 0){
            i++;
            il = atoi(argv[i]);
            ll = il + COLS;
         }
         if(strcmp(argv[i], "-ie") == 0){
            i++;
            ie = atoi(argv[i]);
            le = ie + COLS;
         }
         if(strcmp(argv[i], "-t") == 0){
            i++;
            strcpy(title, " ");
            while(i < argc){
               strcat(title, argv[i]);
               strcat(title, " ");
               i++;
            }
         }
         i++;
      }  /* ends loop over i argc */

      print_graphics_image(the_image, the_image2,
               name, il, ie, ll, le, image_colors,
               invert, title, show_hist,
               color_transform);

      exit(2);
   }  /* ends if argc > 2 */


   get_image_name(name);
   read_tiff_header(name, &image_header);
   get_parameters(&il, &ie, &ll, &le);
   display_menu_for_display_image(&image_colors,
               &display_colors, &invert,
               color_transform, monitor_type,
               &show_hist);
   printf("\nEnter title>");
   gets(title);
   print_graphics_image(the_image, the_image2,
            name, il, ie, ll, le, image_colors,
            invert, title, show_hist,
            color_transform);

}  /* ends main */
