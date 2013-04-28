

    /********************************************
    *
    *    file warp.c
    *
    *    Functions: This file contains
    *       main
    *
    *    Purpose:
    *       This file contains the main calling
    *       routine for warping subroutines.
    *
    *    External Calls:
    *      imageio.c - create_image_file
    *                  read_image_array
    *                  write_image_array
    *                  get_image_size
    *                  allocate_image_array
    *                  free_image_array
    *       warpsubs.c - warp
    *                    object_warp
    *
    *    Modifications:
    *       26 October 1993 - created
    *       27 August 1998 - modified to work on 
    *            entire images at once.
    *       19 September 1998 - modified to work with 
    *           all I O routines in imageio.c.
    *
    ********************************************/

#include "cips.h"


short **the_image;
short **out_image;

main(argc, argv)
   int argc;
   char *argv[];
{

   char   name1[80], name2[80], type[80];
   float  theta, x_stretch, y_stretch,
          x_cross, y_cross;
   int    bilinear,
          x1, x2, x3, x4, 
          y1, y2, y3, y4;
   int    x_control, y_control;
   long   length, width;
   short  m, n, x_displace, y_displace;

      /*************************************
      *
      *   This program will use a different
      *   command line for each type of
      *   call.
      *
      *   Print a usage statement that
      *   gives an example of each type
      *   of call.
      *
      *************************************/

   if(argc < 7){
    printf("\n\nNot enough parameters:");
    printf("\n");
    printf("\n   Two Operations: ");
    printf("\n      warp      object-warp");
    printf("\n\n   Examples:");
    printf("\n");
    printf("\n   warp in out warp x-control ");
    printf("y-control bilinear (1 or 0)");
    printf("\n");
    printf("\n   warp in out object-warp x1 y1");
    printf(" x2 y2 x3 y3 x4 y4 ");
    printf("bilinear (1 or 0)");
    printf("\n");
    exit(0);
   }

      /*************************************
      *
      *   Interpret the command line
      *   depending on the type of call.
      *
      *************************************/

   if(strncmp(argv[3], "warp", 3) == 0){
      strcpy(name1, argv[1]);
      strcpy(name2, argv[2]);
      strcpy(type,  argv[3]);
      x_control = atoi(argv[4]);
      y_control = atoi(argv[5]);
      bilinear  = atoi(argv[6]);
   }

   if(strncmp(argv[3], "object-warp", 3) == 0){
      strcpy(name1, argv[1]);
      strcpy(name2, argv[2]);
      strcpy(type,  argv[3]);
      x1        = atoi(argv[4]);
      y1        = atoi(argv[5]);
      x2        = atoi(argv[6]);
      y2        = atoi(argv[7]);
      x3        = atoi(argv[8]);
      y3        = atoi(argv[9]);
      x4        = atoi(argv[10]);
      y4        = atoi(argv[11]);
      bilinear  = atoi(argv[12]);
   }

   if(does_not_exist(name1)){
    printf("\nERROR input file %s does not exist",
             name1);
    exit(0);
   }

   get_image_size(name1, &length, &width);
   the_image = allocate_image_array(length, width);
   out_image = allocate_image_array(length, width);
   create_image_file(name1, name2);
   read_image_array(name1, the_image);

      /*************************************
      *
      *   Call the routines
      *
      *************************************/

   if(strncmp(type, "warp", 3) == 0){
      warp(the_image, out_image,
           x_control, y_control,
           bilinear,
           length,
           width);
   }  /* ends if */



   if(strncmp(argv[3], "object-warp", 3) == 0){
      object_warp(the_image, out_image,
                  x1, y1, x2, y2, 
                  x3, y3, x4, y4,
                  bilinear,
                  length,
                  width);
    }   /* ends if */

   write_image_array(name2, out_image);
   free_image_array(out_image, length);
   free_image_array(the_image, length);

}  /* ends main  */
