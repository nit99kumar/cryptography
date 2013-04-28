
       /***********************************************
       *
       *       file d:\cips\filt2.c
       *
       *       Functions: This file contains
       *          filter_image
       *          setup_fitlers
       *          get_filter_options
       *
       *       Purpose:
       *          These functions implement several
       *          types of basic edge detection.
       *
       *       External Calls:
       *          wtiff.c - does_not_exist
       *                    round_off_image_size
       *                    create_file_if_needed
       *                    write_array_into_tiff_image
       *          tiff.c - read_tiff_header
       *          rtiff.c - read_tiff_image
       *          numcvrt.c - get_integer
       *
       *
       *       Modifications:
       *          15 February 1992 - created
       *
       *************************************************/

#include "cips.h"

short lpf_filter_6[3][3] =
   { {0, 1, 0},
     {1, 2, 1},
     {0, 1, 0}};

short lpf_filter_9[3][3] =
   { {1, 1, 1},
     {1, 1, 1},
     {1, 1, 1}};

short lpf_filter_10[3][3] =
   { {1, 1, 1},
     {1, 2, 1},
     {1, 1, 1}};
short lpf_filter_16[3][3] =
   { {1, 2, 1},
     {2, 4, 2},
     {1, 2, 1}};

short hpf_filter_1[3][3] =
   { { 0, -1,  0},
     {-1,  5, -1},
     { 0, -1,  0}};

short hpf_filter_2[3][3] =
   { {-1, -1, -1},
     {-1,  9, -1},
     {-1, -1, -1}};

short hpf_filter_3[3][3] =
   { { 1, -2,  1},
     {-2,  5, -2},
     { 1, -2,  1}};





     /*******************************************
     *
     *   filter_image(...
     *
     *   This function filters an image by using
     *   a single 3x3 mask.
     *
     *******************************************/


bfilter_image(in_name, out_name, the_image, out_image,
             filter, type)
   char   in_name[], out_name[];
   int    type;
   short  filter[3][3],
          **the_image,
          **out_image;

{
   int    a, b, d, i, j, k,
          length, max, new_hi, new_low,
          sum, width;
   struct tiff_header_struct image_header;


   bcreate_file_if_needed(in_name, out_name, out_image);

   read_tiff_header(in_name, &image_header);
   new_hi  = 250;
   new_low = 16;
   if(image_header.bits_per_pixel == 4){
       new_hi  = 10;
       new_low = 3;
   }

   d = type;
   if(type == 2 || type == 3) d = 1;

   max = 255;
   if(image_header.bits_per_pixel == 4)
      max = 16;

   bread_tiff_image(in_name, the_image);

         /* Do convolution over image array */
   printf("\n");
   for(i=1; i<image_header.image_length-1; i++){
      if( (i%10) == 0) printf("%d ", i);
      for(j=1; j<image_header.image_width-1; j++){
         sum = 0;
         for(a=-1; a<2; a++){
            for(b=-1; b<2; b++){
               sum = sum +
                     the_image[i+a][j+b] *
                     filter[a+1][b+1];
            }
         }
         sum               = sum/d;
         if(sum < 0)   sum = 0;
         if(sum > max) sum = max;
         out_image[i][j]   = sum;

      }  /* ends loop over j */
   }  /* ends loop over i */

   fix_edges(out_image, 1);

   bwrite_tiff_image(out_name, out_image);

}  /* ends filter_image */




#ifdef NEVER
     /*******************************************
     *
     *   median_filter(..
     *
     *   This function performs a median filter
     *   on an image using a size (3x3, 5x5, etc.)
     *   specified in the call.
     *
     *******************************************/


median_filter(in_name, out_name, the_image, out_image,
              il, ie, ll, le, size)
   char   in_name[], out_name[];
   int    il, ie, ll, le, size;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];

{
   int    a, b, count, i, j, k,
          length, sd2, sd2p1, ss, width;
   short  *elements;
   struct tiff_header_struct image_header;

   sd2   = size/2;
   sd2p1 = sd2 + 1;

      /* allocate the elements array large enough
         to hold size*size shorts. */
   ss       = size*size;
   elements = (short *) malloc(ss * sizeof(short));

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_header(in_name, &image_header);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

         /* Loop over image array */
   printf("\n");
   for(i=sd2; i<ROWS-sd2; i++){
      if( (i%10) == 0) printf("%d ", i);
      for(j=sd2; j<COLS-sd2; j++){
         count = 0;
         for(a=-sd2; a<sd2p1; a++){
            for(b=-sd2; b<sd2p1; b++){
               elements[count] = the_image[i+a][j+b];
               count++;
            }
         }
         out_image[i][j] = median_of(elements, &ss);
      }  /* ends loop over j */
   }  /* ends loop over i */

   fix_edges(out_image, sd2);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

   free(elements);

}  /* ends median_filter */




     /*******************************************
     *
     *   statistical_differencing(..
     *
     *
     *******************************************/


statistical_differencing(in_name, out_name, the_image, out_image,
                         il, ie, ll, le, size)
   char   in_name[], out_name[];
   int    il, ie, ll, le, size;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];

{
   double diff, dsigma, dsigma_square, value;
   int    a, b, count, i, j, k,
          length, sd2, sd2p1, ss, width;
   short  max, mean, sigma, sigma_square;
   struct tiff_header_struct image_header;

   ss    = size*size;
   sd2   = size/2;
   sd2p1 = sd2 + 1;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_header(in_name, &image_header);
   max = 255;
   if(image_header.bits_per_pixel == 4)
      max = 16;

   read_tiff_image(in_name, the_image, il, ie, ll, le);

         /* Loop over image array */
   printf("\n");
   for(i=sd2; i<ROWS-sd2; i++){
      if( (i%10) == 0) printf("%d ", i);
      for(j=sd2; j<COLS-sd2; j++){

            /* mean is the average of the image over 
               the neighborhood */
         mean = 0;
         for(a=-sd2; a<sd2p1; a++){
            for(b=-sd2; b<sd2p1; b++){
               mean = mean + the_image[i+a][j+b];
            }
         }
         mean = mean/ss;

         dsigma_square = 0;
         for(a=-sd2; a<sd2p1; a++){
            for(b=-sd2; b<sd2p1; b++){
               diff          = the_image[i+a][j+b] - mean;
               dsigma_square = dsigma_square + diff*diff;
            }
         }

         dsigma          = sqrt(dsigma_square);
         value           = (double)the_image[i][j];
         value           = value*dsigma;
         if(value > (double)max) 
                   value = (double)max;
         out_image[i][j] = (short)value;

         /***********************************
         sigma = the_image[i][j] - mean;
         if(sigma == 0) sigma = 1;
         if(sigma < 0)  sigma = sigma * (-1);
         out_image[i][j] = the_image[i][j]*sigma;
         if(out_image[i][j] > max) out_image[i][j] = max;
         ***********************************/

      }  /* ends loop over j */
   }  /* ends loop over i */

   fix_edges(out_image, sd2);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends statistical_differencing */
#endif


   /***********************************************
    *
    *    setup_filters(...
    *
    *    This function copies the filter mask values defined
    *    at the top of this file into the mask
    *    array.
    *
    ***********************************************/



setup_filters(filter_type, low_high, filter)
   char   low_high[];
   int    filter_type;
   short  filter[3][3];
{
   int i, j;

   if(low_high[0] == 'l'  || low_high[0] =='L'){
      if(filter_type == 6){
         for(i=0; i<3; i++){
           for(j=0; j<3; j++){
             filter[i][j] = lpf_filter_6[i][j];
           }
         }
      }  /* ends if filter_type == 6 */

      if(filter_type == 9){
         for(i=0; i<3; i++){
           for(j=0; j<3; j++){
             filter[i][j] = lpf_filter_9[i][j];
           }
         }
      }  /* ends if filter_type == 9 */

      if(filter_type == 10){
         for(i=0; i<3; i++){
           for(j=0; j<3; j++){
             filter[i][j] = lpf_filter_10[i][j];
           }
         }
      }  /* ends if filter_type == 10 */


      if(filter_type == 16){
         for(i=0; i<3; i++){
           for(j=0; j<3; j++){
             filter[i][j] = lpf_filter_16[i][j];
           }
         }
      }  /* ends if filter_type == 16 */
   }  /* ends low pass filter */

   if(low_high[0] == 'h'  || low_high[0] =='H'){
      if(filter_type == 1){
         for(i=0; i<3; i++){
           for(j=0; j<3; j++){
             filter[i][j] = hpf_filter_1[i][j];
           }
         }
      }  /* ends if filter_type == 1 */

      if(filter_type == 2){
         for(i=0; i<3; i++){
           for(j=0; j<3; j++){
             filter[i][j] = hpf_filter_2[i][j];
           }
         }
      }  /* ends if filter_type == 2 */

      if(filter_type == 3){
         for(i=0; i<3; i++){
           for(j=0; j<3; j++){
             filter[i][j] = hpf_filter_3[i][j];
           }
         }
      }  /* ends if filter_type == 3 */
   }  /* ends high pass filter */

}  /* ends setup_filters */

#ifdef NEVER

   /***********************************************
    *
    *    get_filter_options(...
    *
    *    This function queries the user for the
    *    parameters need to perform filtering.
    *
    ***********************************************/


get_filter_options(filter_type, low_high)
    char low_high[];
    int  *filter_type;
{
    int not_finished, response;
    not_finished = 1;
    while(not_finished){

      printf("\nThe filter options are:\n");
      printf("\n\t1.  Type of filter is %d", *filter_type);
      printf("\n\t      (recall 6, 9, 10, 16");
      printf("\n\t2.  Low or High Pass filter is %s",
             low_high);
      printf("\n\t      l=low pass h=high pass "
             "m=median filter ");
      printf("\n\nEnter choice (0 = no change) _\b");


      get_integer(&response);

      if(response == 0){
        not_finished = 0;
      }

      if(response == 1){
        printf("\n\nEnter type of filter");
        printf("\n  _\b");
        get_integer(filter_type);
      }

      if(response == 2){
        printf("\n\nEnter l=low pass h=high pass "
                 "m=median filter");
        printf("\n  _\b");
        gets(low_high);
      }

    }  /* ends while not_finished */

}  /* ends get_filter_options */

#endif

   /***********************************************
    *
    *    median_of(...         
    *
    *    This function finds and returns the   
    *    median value of the elements array.  
    *    
    *    As a side result, it also sorts the 
    *    elements array. 
    *
    ***********************************************/

median_of(elements, count)
   int   *count;
   short elements[];
{
   short median;

   sort_elements(elements, count);
   median = elements[*count/2];
   return(median);
}  /* ends median_of */


   /***********************************************
    *
    *    sort_elements(...     
    *
    *    This function performs a simple bubble
    *    sort on the elements from the median 
    *    filter.
    *
    ***********************************************/

sort_elements(elements, count)
   int   *count;
   short elements[];
{
   int i, j;
   j = *count;
   while(j-- > 1){
      for(i=0; i<j; i++){
         if(elements[i] > elements[i+1])
            swap(&elements[i], &elements[i+1]);
      }
   }
}  /* ends sort_elements */


   /***********************************************
    *
    *    swap(...     
    *
    *    This function swaps two shorts.
    *
    ***********************************************/

swap(a, b)
   short *a, *b;
{
   short temp;
   temp  = *a;
   *a    = *b;
   *b    = temp;
}  /* ends swap */




   /***********************************************
    *
    *    fix_edges(...
    *
    *    This function fixes the edges of an image
    *    array after convolution was performed.
    *    It copies the points near the edge of the
    *    array out to the edge of the array.
    *
    ***********************************************/



fix_edges(im, w)
      int   w;
      short im[ROWS][COLS];
{
   int i, j;


      /* four corners */
   for(i=w; i>0; i--){
    im[i-1][i-1] = im[i][i];
    im[i-1][COLS-(i-1)] = im[i][COLS-1-(i-1)];
    im[ROWS-(i-1)][i-1] = im[ROWS-1-(i-1)][i];
    im[ROWS-(i-1)][COLS-(i-1)] = im[ROWS-1-(i-1)][COLS-1-(i-1)];
   }  /* ends four corners loop */

   for(i=0; i<ROWS; i++){
      for(j=w; j>0; j--){
       im[i][j-1] = im[i][j];
       im[i][COLS-j] = im[i][COLS-j-1];
      }
   }

   for(j=0; j<COLS; j++){
      for(i=w; i>0; i--){
       im[i-1][j] = im[i][j];
       im[ROWS-i][j] = im[ROWS-i-1][j];
      }
   }

}  /* ends fix_edges */
