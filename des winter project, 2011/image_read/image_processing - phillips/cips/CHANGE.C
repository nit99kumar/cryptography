
    /***********************************************
    *
    *  file d:\cips\change.c
    *
    *  Functions: This file contains
    *     main
    *
    *  Purpose:
    *     This file contains the main calling
    *     routine which will change the values
    *     of pixels based on upper and lower
    *     thresholds.
    *
    *  External Calls:
    *      imageio.c - create_image_file
    *                  read_image_array
    *                  write_image_array
    *                  get_image_size
    *                  allocate_image_array
    *                  free_image_array
    *
    *  Modifications:
    *     20 May 1995 - created
    *      3 October 1998 - created to work with 
    *           all I O routines in imageio.c.
    *
    *************************************************/

#include "cips.h"


short **the_image;
short **out_image;

main(argc, argv)
   int argc;
   char *argv[];
{

   char  in_name[MAX_NAME_LENGTH], out_name[MAX_NAME_LENGTH];
   int    i, ie, il, j;
   long   length, width;
   short lower_threshold, upper_threshold, value;


   if(argc != 6){
    printf("\n");
    printf("\nusage: change in-file out-file ");
    printf("upper-threshold lower-threshold value");
    exit(0);
   }

   strcpy(in_name, argv[1]);
   strcpy(out_name, argv[2]);
   upper_threshold = atoi(argv[3]);
   lower_threshold = atoi(argv[4]);
   value           = atoi(argv[5]);

   if(does_not_exist(in_name)){
      printf("\nERROR input file %s does not exist",
             in_name);
      exit(0);
   }  /* ends if does_not_exist */

   create_image_file(in_name, out_name);
   get_image_size(in_name, &length, &width);
   the_image = allocate_image_array(length, width);
   out_image = allocate_image_array(length, width);
   read_image_array(in_name, the_image);


   for(i=0; i<length; i++){
      for(j=0; j<width; j++){
         if(the_image[i][j] >= lower_threshold &&
            the_image[i][j] <= upper_threshold)
            out_image[i][j] = value;
         else
            out_image[i][j] = the_image[i][j];
      }  /* ends loop over j */
   }  /* ends loop over i */

   write_image_array(out_name, out_image);
   free_image_array(the_image, length);
   free_image_array(out_image, length);

}  /* ends main  */
