
    /********************************************
    *
    *    file texture.c
    *
    *    Functions: This file contains
    *       main
    *
    *    Purpose:
    *       This file contains the main calling
    *       routine for texture subroutines.
    *
    *    External Calls:
    *      imageio.c - create_image_file
    *                  read_image_array
    *                  write_image_array
    *                  get_image_size
    *                  get_bitsperpixel
    *                  allocate_image_array
    *                  free_image_array
    *       txtrsubs.c - sigma
    *                    skewness
    *                    amean
    *                    adifference
    *                    hurst
    *                    compare
    *
    *    Modifications:
    *       12 August 1993 - created
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

   char name1[80], name2[80];
   int  size, t, type, v;
   long bits_per_pixel, length, width;

   if(argc < 7){
      show_texture_usage();
      exit(0);
   }

   strcpy(name1, argv[1]);
   strcpy(name2, argv[2]);
   t    = atoi(argv[4]);
   v    = atoi(argv[5]);
   size = atoi(argv[6]);

   if(does_not_exist(name1)){
    printf("\nERROR input file %s does not exist",
             name1);
    exit(0);
   }

   get_image_size(name1, &length, &width);
   get_bitsperpixel(name1, &bits_per_pixel);
   the_image = allocate_image_array(length, width);
   out_image = allocate_image_array(length, width);
   create_image_file(name1, name2);
   read_image_array(name1, the_image);


   if(strncmp(argv[3], "compare", 3) == 0){
      read_image_array(name1, out_image);
      compare(the_image, out_image,
              t, v, size,
              length,
              width,
              bits_per_pixel);
   }  /* ends if compare case */

   if(strncmp(argv[3], "hurst", 3) == 0)
      hurst(the_image, out_image,
            size,
            length,
            width,
            bits_per_pixel);

   if(strncmp(argv[3], "adifference", 3) == 0)
      adifference(the_image, 
                  out_image, 
                  size,
                  length,
                  width);

   if(strncmp(argv[3], "amean", 3) == 0)
      amean(the_image, out_image,
            size,
            length,
            width,
            bits_per_pixel);

   if(strncmp(argv[3], "skewness", 3) == 0){
      size = atoi(argv[4]);
      t    = atoi(argv[5]);
      v    = atoi(argv[6]);
      skewness(the_image, out_image,
               size, t, v,
               length,
               width,
               bits_per_pixel);
   }  /* ends skewness case */

   if(strncmp(argv[3], "sigma", 3) == 0){
      size = atoi(argv[4]);
      t    = atoi(argv[5]);
      v    = atoi(argv[6]);
      sigma(the_image, out_image,
            size, t, v,
            length,
            width,
            bits_per_pixel);
   }  /* ends sigma case */

   write_image_array(name2, out_image);
   free_image_array(out_image, length);
   free_image_array(the_image, length);

}  /* ends main  */


show_texture_usage()
{
 printf("\n\nNot enough parameters:");
 printf("\n");
 printf("\n   usage: texture in-file out-file type ");
 printf("threshold? threshold-value size");
 printf("\n   recall type: sigma      skewness");
 printf("\n                amean      adifference");
 printf("\n                hurst      compare");
 printf("\n   threshold?   1-threshold on");
 printf("\n                2-threshold off\n");
 printf("\n   usage for compare:");
 printf("\n   texture in-file out-file compare ");
 printf("line element size");
 printf("\n  "
  "\ntexture in-file out-file compare line element size"
  "\ntexture in-file out-file hurst x x size (3 5 or 7)"
  "\ntexture in-file out-file adifference x x size"
  "\ntexture in-file out-file amean x x size"
  "\ntexture in-file out-file skewness size "
            "threshold(0,1) high-threshold"
  "\ntexture in-file out-file sigma size "
            "threshold(0,1) high-threshold");
}
