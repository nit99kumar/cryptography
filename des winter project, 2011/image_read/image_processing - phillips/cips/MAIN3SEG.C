



       /***********************************************
       *
       *       file d:\cips\main3seg.c
       *
       *       Functions: This file contains
       *          main
       *
       *       Purpose:
       *
       *       External Calls:
       *          gin.c - get_image_name
       *          numcvrt.c - get_integer
       *                      int_convert
       *          tiff.c - read_tiff_header
       *
       *       Modifications:
       *          5 December 1992 - created
       *
       *************************************************/

#include "cips.h"



short the_image[ROWS][COLS];
short out_image[ROWS][COLS];

main(argc, argv)
   int argc;
   char *argv[];
{

   char     name[80], name2[80], low_high[80], type[80];
   float    fd, fsum, sum_div, percent;
   int      count, i, ie, il, j, le, length, ll,
            looking = 1, lw,
            width;
   long     lc, lr, llength, lwidth, num=0, sum=0;
   unsigned long histogram[GRAY_LEVELS+1];
   short    cutoff, value, value2, value3, value4;
   struct   tiff_header_struct image_header;

   my_clear_text_screen();

       /***********************************************
       *
       *       Interpret the command line parameters.
       *
       ************************************************/

   if(argc < 4){
    printf(
    "\n\nNot enough parameters:"
     "\n"
     "\n usage: main2seg in-file out-file type"
     " [value] [value2] [value3]"
     "\n"
     "\n   recall type: edge-Region High-pixel Low-pixel"
     "\n                Erosion Dilation Swap-pixel sKeleton"
     "\n                pixel-Grow Automatic-threshold"
     "\n                OVERLAY-TEST"
     "\n");
    exit(0);
   }

   strcpy(name,  argv[1]);
   strcpy(name2, argv[2]);
   strcpy(type,  argv[3]);
   if(type[0] == 'a' || type[0] == 'A'  ||
      type[0] == 'r' || type[0] == 'R'){
      percent = atof(argv[4]);
      value   = atoi(argv[5]);
      value2  = atoi(argv[6]);
      value3  = atoi(argv[7]);
      value4  = atoi(argv[8]);
    }
   else{
      value  = atoi(argv[4]);
      value2 = atoi(argv[5]);
      value3 = atoi(argv[6]);
   }

   il = 1;
   ie = 1;
   ll = ROWS+1;
   le = COLS+1;

       /******************************************
       *
       *  Read the input image header and setup
       *  the looping counters.
       *
       *  If high or low pass filtering, setup
       *  the filter mask array.
       *
       *******************************************/

   read_tiff_header(name, &image_header);

   length = (90 + image_header.image_length)/ROWS;
   width  = (90 + image_header.image_width)/COLS;
   count  = 1;
   lw     = length*width;
   printf("\nlength=%d  width=%d", length, width);

       /******************************************
       *
       *  Loop over the input image and filter it
       *  using either the high or low pass filters
       *  using a mask OR using the median filter.
       *
       *******************************************/



      value   = atoi(argv[3]);
      value2  = atoi(argv[4]);
      for(i=0; i<length; i++){
         for(j=0; j<width; j++){
            printf("\nrunning %d of %d", count, lw);
            count++;
            di2lation(name, name2, the_image, out_image,
                      il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                      le+j*COLS, value, value2);
            er2osion(name2, name2, the_image, out_image,
                      il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                      le+j*COLS, value, value2);
         }
      }
     exit(1);





/*************


      value   = atoi(argv[3]);
      for(i=0; i<length; i++){
         for(j=0; j<width; j++){
            printf("\nrunning %d of %d", count, lw);
            count++;
            mat8(name, name2, the_image, out_image,
                 il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                 le+j*COLS, value);
         }
      }
     exit(1);


      for(i=0; i<length; i++){
         for(j=0; j<width; j++){
            printf("\nrunning %d of %d", count, lw);
            count++;
            alledm8(name, name2, the_image, out_image,
                    il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                    le+j*COLS);
         }
      }
     exit(1);


      for(i=0; i<length; i++){
         for(j=0; j<width; j++){
            printf("\nrunning %d of %d", count, lw);
            count++;
            alledm4(name, name2, the_image, out_image,
                    il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                    le+j*COLS);
         }
      }
     exit(1);


      value   = atoi(argv[3]);
      for(i=0; i<length; i++){
         for(j=0; j<width; j++){
            printf("\nrunning %d of %d", count, lw);
            count++;
            edm8(name, name2, the_image, out_image,
                 il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                 le+j*COLS, value);
         }
      }
     exit(1);

      value   = atoi(argv[3]);
      value2  = atoi(argv[4]);
      for(i=0; i<length; i++){
         for(j=0; j<width; j++){
            printf("\nrunning %d of %d", count, lw);
            count++;
            exterior_outline(name, name2, the_image, out_image,
                      il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                      le+j*COLS, value, value2);
         }
      }
     exit(1);

      value   = atoi(argv[3]);
      value2  = atoi(argv[4]);
      for(i=0; i<length; i++){
         for(j=0; j<width; j++){
            printf("\nrunning %d of %d", count, lw);
            count++;
            di2lation(name, name2, the_image, out_image,
                      il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                      le+j*COLS, value, value2);
         }
      }
     exit(1);

      value   = atoi(argv[3]);
      value2  = atoi(argv[4]);
      for(i=0; i<length; i++){
         for(j=0; j<width; j++){
            printf("\nrunning %d of %d", count, lw);
            count++;
            er2osion(name, name2, the_image, out_image,
                     il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                     le+j*COLS, value, value2);
         }
      }
     exit(1);
***************/


   if(type[0] == 'h'  || type[0] == 'H'){
      for(i=0; i<length; i++){
         for(j=0; j<width; j++){
            printf("\nrunning %d of %d", count, lw);
            count++;
            high_pixel(name, name2, the_image, out_image,
                       il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                       le+j*COLS, 3);
         }  /* ends loop over j */
      }  /* ends loop over i */
   }  /* ends high_pixel */

   if(type[0] == 'l'  || type[0] == 'L'){
      for(i=0; i<length; i++){
         for(j=0; j<width; j++){
            printf("\nrunning %d of %d", count, lw);
            count++;
            low_pixel(name, name2, the_image, out_image,
                      il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                      le+j*COLS, 3);
         }  /* ends loop over j */
      }  /* ends loop over i */
   }  /* ends low_pixel */


   if(type[0] == 'e'  || type[0] == 'E'){
      for(i=0; i<length; i++){
         for(j=0; j<width; j++){
            printf("\nrunning %d of %d", count, lw);
            count++;
            erosion(name, name2, the_image, out_image,
                    il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                    le+j*COLS, value, value2);
         }  /* ends loop over j */
      }  /* ends loop over i */
   }  /* ends erosion */

   if(type[0] == 'd'  || type[0] == 'D'){
      for(i=0; i<length; i++){
         for(j=0; j<width; j++){
            printf("\nrunning %d of %d", count, lw);
            count++;
            dilation(name, name2, the_image, out_image,
                     il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                     le+j*COLS, value, value2);
         }  /* ends loop over j */
      }  /* ends loop over i */
   }  /* ends dilation */

   /***********
   if(type[0] == 's'  || type[0] == 'S'){
      for(i=0; i<length; i++){
         for(j=0; j<width; j++){
            printf("\nrunning %d of %d", count, lw);
            count++;
            swap_pixels(name, name2, the_image, out_image,
                        il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                        le+j*COLS, value, value2);
         }
      } 
   }
   **************/

   if(type[0] == 'k'  || type[0] == 'K'){
      for(i=0; i<length; i++){
         for(j=0; j<width; j++){
            printf("\nrunning %d of %d", count, lw);
            count++;
            skeleton(name, name2, the_image, out_image,
                     il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                     le+j*COLS, value);
         }  /* ends loop over j */
      }  /* ends loop over i */
   }  /* ends skeleton */

   /***********
   if(type[0] == 'g'  || type[0] == 'G'){
      for(i=0; i<length; i++){
         for(j=0; j<width; j++){
            printf("\nrunning %d of %d", count, lw);
            count++;
            grow_regions(name, name2, the_image,
                         out_image, il+i*ROWS,
                         ie+j*COLS, ll+i*ROWS,
                         le+j*COLS, value,
                         value2, value3);
         }
      }
   }
   **************/

      /* Automatic threshold finding */
   if(type[0] == 'a'  || type[0] == 'A'){
      zero_histogram(histogram);
      for(i=0; i<length; i++){
         for(j=0; j<width; j++){
            printf("\nrunning %d of %d", count, lw);
            count++;
            read_tiff_image(name, the_image,
                  il+i*ROWS, ie+j*COLS,
                  ll+i*ROWS, le+j*COLS);
            calculate_histogram(the_image, histogram);
         }  /* ends loop over j */
      }  /* ends loop over i */

         /* make sure all the numbers are cast to
            long, otherwise this will not work */
      sum     = 0;
      i       = 0;
      llength = (long)(length);
      lwidth  = (long)(width);
      lr      = (long)(ROWS);
      lc      = (long)(COLS);
      num     = lr*lc*llength*lwidth;
      fd      = (float)(num);

      while(looking){
         fsum    = (float)(sum);
         sum_div = fsum/fd;
         if(sum_div >= percent)
            looking = 0;
         else
            sum = sum + histogram[i++];
      }  /* ends while looking */

      cutoff = i;
      printf("\nCutoff is %d sum=%ld", cutoff, sum);


      create_file_if_needed(name, name2, out_image);

      count = 1;
      for(i=0; i<length; i++){
         for(j=0; j<width; j++){
            printf("\nrunning %d of %d", count, lw);
            count++;
            read_tiff_image(name, the_image,
                  il+i*ROWS, ie+j*COLS,
                  ll+i*ROWS, le+j*COLS);
            threshold_image_array(the_image, out_image,
                                  255, cutoff, value);
            write_array_into_tiff_image(name2, out_image,
                              il+i*ROWS,
                              ie+j*COLS,
                              ll+i*ROWS,
                              le+j*COLS);
        }  /* ends loop over i */
     }  /* ends loop over j */

   }  /* ends automatic thresholding */


   /***********
   if(type[0] == 'o'  || type[0] == 'O'){
      for(i=0; i<length; i++){
         for(j=0; j<width; j++){
            printf("\nrunning %d of %d", count, lw);
            count++;
            read_tiff_image(name, the_image,
                  il+i*ROWS, ie+j*COLS,
                  ll+i*ROWS, le+j*COLS);
            read_tiff_image(name2, out_image,
                  il+i*ROWS, ie+j*COLS,
                  ll+i*ROWS, le+j*COLS);
            overlay_edge(out_image, the_image);
            write_array_into_tiff_image(name2, the_image,
                  il+i*ROWS, ie+j*COLS,
                  ll+i*ROWS, le+j*COLS);
         }
      }
   }
   **************/



}  /* ends main  */
