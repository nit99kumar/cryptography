
   /*************************** 
   * 
   *   segments.c 
   *   COMPOSITE FILE COMPRISING: 
   *   segment.c 
   *   segment2.c 
   * 
   ***************************\ 



   /*********************************************
   *
   *       file d:\cips\segment.c
   *
   *       Functions: This file contains
   *           adaptive_threshold_segmentation
   *           append_stack_files
   *           copy_stack_files
   *           find_peaks
   *           find_valley_point
   *           grow
   *           insert_into_peaks
   *           insert_into_deltas
   *           label_and_check_neighbors
   *           manual_threshold_segmentation
   *           peak_threshold_segmentation
   *           peaks_high_low
   *           push_data_onto_stack_file
   *           pop_data_off_of_stack_file
   *           threshold_image_array
   *           valley_high_low
   *           valley_threshold_segmentation
   *           get_segmentation_options
   *
   *       Purpose:
   *           These functions are part of histogram
   *           based image segmentation.
   *
   *       External Calls:
   *          wtiff.c - round_off_image_size
   *                    create_file_if_needed
   *                    write_array_into_tiff_image
   *          tiff.c - read_tiff_header
   *          rtiff.c - read_tiff_image
   *          numcvrt.c - get_integer
   *                      get_short
   *
   *       Modifications:
   *           October 1992 - created
   *
   ************************************************/


#include "cips.h"


   /**************************************************
   *
   *   manual_threshold_segmentation(...
   *
   *   This function segments an image using thresholding
   *   given the hi and low values of the threshold
   *   by the calling routine.  It reads in an image
   *   and writes the result to the output image.
   *
   *   If the segment parameter is 0, you only
   *   threshold the array - you do not segment.
   *
   ***************************************************/

manual_threshold_segmentation(in_name, out_name,
                              the_image, out_image,
                              il, ie, ll, le,
                              hi, low, value, segment)
   char   in_name[], out_name[];
   int    il, ie, ll, le, segment;
   short  hi, low, the_image[ROWS][COLS],
          out_image[ROWS][COLS], value;
{
   int    length, width;
   struct tiff_header_struct image_header;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);
   threshold_image_array(the_image, out_image,
            hi, low, value);
   if(segment == 1)
      grow(out_image, value);
   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends manual_threshold_segmentation */





   /************************************************
   *
   *   peak_threshold_segmentation(...
   *
   *   This function segments an image using
   *   thresholding.  It uses the histogram peaks
   *   to find the hi and low values of the
   *   threshold.
   *
   *   If the segment parameter is 0, you only
   *   threshold the array - you do not segment.
   *
   *************************************************/

peak_threshold_segmentation(in_name, out_name,
                            the_image, out_image,
                            il, ie, ll, le,
                            value, segment)
   char   in_name[], out_name[];
   int    il, ie, ll, le, segment;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS], value;
{
   int      length, peak1, peak2, width;
   short    hi, low;
   struct   tiff_header_struct image_header;
   unsigned long histogram[GRAY_LEVELS+1];

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);
   zero_histogram(histogram);
   calculate_histogram(the_image, histogram);
   smooth_histogram(histogram);
   find_peaks(histogram, &peak1, &peak2);
   peaks_high_low(histogram, peak1, peak2,
                  &hi, &low);
   threshold_image_array(the_image, out_image,
                         hi, low, value);
   if(segment == 1)
      grow(out_image, value);
   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends peak_threshold_segmentation */





   /************************************************
   *
   *   valley_threshold_segmentation(...
   *
   *   This function segments an image using
   *   thresholding.  It uses the histogram valleys
   *   to find the hi and low values of the
   *   threshold.
   *
   *   If the segment parameter is 0, you only
   *   threshold the array - you do not segment.
   *
   *************************************************/

valley_threshold_segmentation(in_name, out_name,
                              the_image, out_image,
                              il, ie, ll, le,
                              value, segment)
   char   in_name[], out_name[];
   int    il, ie, ll, le, segment;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS], value;
{
   int      length, peak1, peak2, width;
   short    hi, low;
   struct   tiff_header_struct image_header;
   unsigned long histogram[GRAY_LEVELS+1];

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);
   zero_histogram(histogram);
   calculate_histogram(the_image, histogram);
   smooth_histogram(histogram);
   find_peaks(histogram, &peak1, &peak2);
   valley_high_low(histogram, peak1, peak2,
                   &hi, &low);
   threshold_image_array(the_image, out_image,
                         hi, low, value);
   if(segment == 1)
      grow(out_image, value);
   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends valley_threshold_segmentation */





   /************************************************
   *
   *   adaptive_threshold_segmentation(...
   *
   *   This function segments an image using
   *   thresholding.  It uses two passes
   *   to find the hi and low values of the
   *   threshold.  The first pass uses the peaks
   *   of the histogram to find the hi and low
   *   threshold values.  It thresholds the image
   *   using these hi lows and calculates the means
   *   of the object and background.  Then we use
   *   these means as new peaks to calculate new
   *   hi and low values.  Finally, we threshold
   *   the image again using these second hi low
   *   hi low values.
   *
   *   If the segment parameter is 0, you only
   *   threshold the array - you do not segment.
   *
   *************************************************/

adaptive_threshold_segmentation(in_name, out_name,
                                the_image, out_image,
                                il, ie, ll, le,
                                value, segment)
   char   in_name[], out_name[];
   int    il, ie, ll, le, segment;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS], value;
{
   int      length, peak1, peak2, width;
   short    background, hi, low, object;
   struct   tiff_header_struct image_header;
   unsigned long histogram[GRAY_LEVELS+1];

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);
   zero_histogram(histogram);
   calculate_histogram(the_image, histogram);
   smooth_histogram(histogram);
   find_peaks(histogram, &peak1, &peak2);
   peaks_high_low(histogram, peak1, peak2,
                  &hi, &low);
   threshold_and_find_means(the_image, out_image,
                            hi, low, value,
                            &object, &background);
   peaks_high_low(histogram, object, background,
                  &hi, &low);
   threshold_image_array(the_image, out_image,
                         hi, low, value);
   if(segment == 1)
      grow(out_image, value);
   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends adaptive_threshold_segmentation */





   /**************************************************
   *
   *   threshold_image_array(...
   *
   *   This function thresholds an input image array
   *   and produces a binary output image array.
   *   If the pixel in the input array is between
   *   the hi and low values, then it is set to value.
   *   Otherwise, it is set to 0.
   *
   ***************************************************/


threshold_image_array(in_image, out_image, hi, low, value)
   short hi, low, in_image[ROWS][COLS],
         out_image[ROWS][COLS], value;
{
   int   counter = 0, i, j;
   for(i=0; i<ROWS; i++){
      for(j=0; j<COLS; j++){
         if(in_image[i][j] >= low  &&
            in_image[i][j] <= hi){
            out_image[i][j] = value;
            counter++;
         }
         else
            out_image[i][j] = 0;
      }  /* ends loop over j */
   }  /* ends loop over i */
   printf("\n\tTIA> set %d points", counter);
}  /* ends threshold_image_array */





   /**************************************************
   *
   *   threshold_and_find_means(...
   *
   *   This function thresholds an input image array
   *   and produces a binary output image array.
   *   If the pixel in the input array is between
   *   the hi and low values, then it is set to value.
   *   Otherwise, it is set to 0.
   *
   ***************************************************/

threshold_and_find_means(in_image, out_image, hi,
                         low, value, object_mean,
                         background_mean)
   short *background_mean, hi, low,
         in_image[ROWS][COLS], *object_mean,
         out_image[ROWS][COLS], value;
{
   int      counter = 0,
            i,
            j;
   unsigned long object     = 0,
                 background = 0;

   for(i=0; i<ROWS; i++){
      for(j=0; j<COLS; j++){
         if(in_image[i][j] >= low  &&
            in_image[i][j] <= hi){
            out_image[i][j] = value;
            counter++;
            object = object + in_image[i][j];
         }
         else{
            out_image[i][j] = 0;
            background      = background + in_image[i][j];
         }
      }  /* ends loop over j */
   }  /* ends loop over i */
   object     = object/counter;
   background = background/((ROWS*COLS)-counter);
   *object_mean     = (short)(object);
   *background_mean = (short)(background);
   printf("\n\tTAFM> set %d points", counter);
   printf("\n\tTAFM> object=%d background=%d",
          *object_mean, *background_mean);
}  /* ends threshold_and_find_means */





    /**********************************************
    *
    *  grow(...
    *
    *  This function is an object detector.
    *  Its input is an binary image array
    *  containing 0's and value's.
    *  It searches through the image and connects
    *  the adjacent values.
    *
    ***********************************************/

grow(binary, value)
   short binary[ROWS][COLS],
         value;
{
   char name[80];

   int first_call,
       i,
       j,
       object_found,
       pointer,
       pop_i,
       pop_j,
       stack_empty,
       stack_file_in_use;

   short g_label, stack[STACK_SIZE][2];

            /*************************************
            *
            *   Now begin the process of growing
            *   regions.
            *
            **************************************/

   g_label       = 2;
   object_found  = 0;
   first_call    = 1;

   for(i=0; i<ROWS; i++){
      for(j=0; j<COLS; j++){

         stack_file_in_use =  0;
         stack_empty       =  1;
         pointer           = -1;

               /**********************************
               *
               *  Search for the first pixel of
               *  a region.
               *
               ***********************************/

         if(binary[i][j] == value){
            label_and_check_neighbor(binary, stack, 
                      g_label, &stack_empty, &pointer, 
                      i, j, value, &stack_file_in_use,
                      &first_call);
            object_found = 1;
         }  /* ends if binary[i]j] == value */

               /*****************************
               *
               *  If the stack is not empty,
               *  pop the coordinates of
               *  the pixel off the stack
               *  and check its 8 neighbors.
               *
               *******************************/

         while(stack_empty == 0){
            pop_i = stack[pointer][0]; /* POP       */
            pop_j = stack[pointer][1]; /* OPERATION */
            --pointer;
            if(pointer <= 0){
               if(stack_file_in_use){
                  pop_data_off_of_stack_file(
                                 stack,
                                 &pointer,
                                 &stack_file_in_use);
               }  /* ends if stack_file_in_use  */
               else{
                  pointer     = 0;
                  stack_empty = 1;
               }  /* ends else stack file is
                     not in use  */
            }  /*  ends if point <= 0  */

            label_and_check_neighbor(binary,
                        stack, g_label,
                        &stack_empty,
                        &pointer, pop_i,
                        pop_j, value,
                        &stack_file_in_use,
                        &first_call);
         }  /* ends while stack_empty == 0 */

         if(object_found == 1){
            object_found = 0;
            ++g_label;
         }  /* ends if object_found == 1 */

      }   /* ends loop over j */
   }  /* ends loop over i */

   printf("\nGROW> found %d objects", g_label);

} /* ends grow  */





   /********************************************
   *
   *  label_and_check_neighbors(...
   *
   *  This function labels a pixel with an object
   *  label and then checks the pixel's 8
   *  neighbors.  If any of the neigbors are
   *  set, then they are also labeled.
   *
   ***********************************************/

label_and_check_neighbor(binary_image, stack,
                         g_label, stack_empty,
                         pointer, r, e, value,
                         stack_file_in_use,
                         first_call)
int   e,
      *first_call,
      *pointer,
      r,
      *stack_empty,
      *stack_file_in_use;

short binary_image[ROWS][COLS],
      g_label,
      stack[STACK_SIZE][2],
      value;
{
   int already_labeled = 0,
       i, j;

   if (binary_image[r][e] == g_label)
      already_labeled = 1;

   binary_image[r][e] = g_label;

      /***************************************
      *
      *   Look at the 8 neighors of the
      *   point r,e.
      *
      *   Ensure the points you are checking
      *   are in the image, i.e. not less
      *   than zero and not greater than
      *   ROWS-1 or COLS-1.
      *
      ***************************************/

   for(i=(r-1); i<=(r+1); i++){
      for(j=(e-1); j<=(e+1); j++){

         if((i>=0)   &&
            (i<=ROWS-1)  &&
            (j>=0)   &&
            (j<=COLS-1)){

            if(binary_image[i][j] == value){
               *pointer           = *pointer + 1;
               stack[*pointer][0] = i; /* PUSH      */
               stack[*pointer][1] = j; /* OPERATION */
               *stack_empty       = 0;

               if(*pointer >= (STACK_SIZE -
                               STACK_FILE_LENGTH)){
                  push_data_onto_stack_file(stack,
                            pointer, first_call);
                  *stack_file_in_use = 1;
               }  /* ends if *pointer >=
                     STACK_SIZE - STACK_FILE_LENGTH*/

            }  /* end of if binary_image == value */
         }  /* end if i and j are on the image */
      }  /* ends loop over i rows           */
   }  /* ends loop over j columns        */
}  /* ends label_and_check_neighbors  */





   /****************************************
   *
   *   push_data_onto_stack_file(...
   *
   *   This function takes the stack array
   *   and pushes it onto the stack file.
   *
   *****************************************/

push_data_onto_stack_file(stack, pointer, first_call)
   int   *first_call, *pointer;
   short stack[STACK_SIZE][2];
{
   char  backup_file_name[MAX_NAME_LENGTH];
   FILE  *backup_file_pointer, *stack_file_pointer;
   int   diff, i;
   short holder[STACK_FILE_LENGTH][2];

   printf("\nSFO> Start of push_data_onto_stack ");

   diff = STACK_SIZE - STACK_FILE_LENGTH;

       /*******************************************
       *
       *   Copy the elements to be stored to the
       *   stack file into holder
       *
       ********************************************/

   for(i=0; i<STACK_FILE_LENGTH; i++){
      holder[i][0] = stack[i][0];
      holder[i][1] = stack[i][1];
   }

       /*******************************************
       *
       *   Move the elements of the stack down
       *
       *******************************************/

   for(i=0; i<diff; i++){
      stack[i][0] = stack[i + STACK_FILE_LENGTH][0];
      stack[i][1] = stack[i + STACK_FILE_LENGTH][1];
   }

       /*******************************************
       *
       *   Fill the top of the stack with zeros
       *
       *******************************************/

   for(i=diff; i<STACK_SIZE; i++){
      stack[i][0] = 0;
      stack[i][1] = 0;
   }

   *pointer = *pointer - STACK_FILE_LENGTH;

   /************************************************
   *
   *   Store the holder array into the stack file.
   *   Open the stack file for writing in binary
   *   mode. If the file does not exist it will be
   *   created. If the file does exist it will be
   *   over written.
   *
   *   PUSH - IF first_time == 1 then write to stack
   *          ELSE write to stack.bak
   *          append stack onto stack.bak
   *          copy stack.bak to stack
   *          this has the effect of writing
   *          to the beginning of the stack.
   *
   ************************************************/

   if(*first_call == 1){

      *first_call = *first_call + 1;
      if((stack_file_pointer = fopen(STACK_FILE,"wb"))
                                     == NULL)
         printf("\nSFO> Could not open stack file");
      else{
         /*printf("\n\nSFO> Writing to stack file");*/
         fwrite(holder, sizeof(holder),
                1, stack_file_pointer);
         fclose(stack_file_pointer);
      }  /*  ends else could not open stack_file  */

   }  /*  ends if *first_call == 1  */
   else{  /* else stack file has been used already  */
      strcpy(backup_file_name, STACK_FILE);
      strcat(backup_file_name, ".bak");
      if((backup_file_pointer =
          fopen(backup_file_name, "wb")) == NULL)
         printf("\nSFO> Could not open backup file");
      else{
         /*printf("\n\nSFO> Writing to backup file");*/
         fwrite(holder, sizeof(holder),
                1, backup_file_pointer);
         fclose(backup_file_pointer);
      }  /*  ends else could not open backup_file  */

      append_stack_files(backup_file_name,
                         STACK_FILE, holder);
      copy_stack_files(backup_file_name,
                       STACK_FILE, holder);

   }  /*  ends else first_call != 1  */

   printf("--- End of push_data_onto_stack");

}  /* ends push_data_onto_stack_file  */





   /****************************************
   *
   *   pop_data_off_of_stack_file(...
   *
   *   This function pops the stack array
   *   off of the stack file.
   *
   *****************************************/

pop_data_off_of_stack_file(stack, pointer,
                           stack_file_in_use)
   int   *pointer, *stack_file_in_use;
   short stack[STACK_SIZE][2];
{
   char  backup_file_name[MAX_NAME_LENGTH];
   FILE  *backup_file_pointer, *stack_file_pointer;
   int   i;
   long  write_counter;
   short holder[STACK_FILE_LENGTH][2],
         holder2[STACK_FILE_LENGTH][2];

       /*******************************************
       *
       *   POP - Read 1 time from stack
       *         Copy the remainder of stack to
       *            stack.bak
       *         Copy stack.bak to stack
       *         This has the effect of popping off
       *         of the stack.
       *
       *   Read the holder array from the stack file.
       *   Open the stack file for reading in binary
       *   mode.
       *
       *   If it requires more than one write to
       *   copy the remainder of stack to
       *   stack.bak then there is still data in the
       *   stack file so set stack_file_in_use = 1.
       *   Else set it to 0.
       *
       **********************************************/

   printf("\nSFO> Start of pop_data_off_of_stack ");
   write_counter = 0;

   strcpy(backup_file_name, STACK_FILE);
   strcat(backup_file_name, ".bak");

   if( (stack_file_pointer =
          fopen(STACK_FILE, "rb")) == NULL)
      printf("\nSFO> Could not open stack file");
   else{
      /*printf("\n\nSFO> Reading from stack file");*/
      fread(holder, sizeof(holder),
            1, stack_file_pointer);

      backup_file_pointer =
            fopen(backup_file_name, "wb");
      while( fread(holder2, sizeof(holder2),
                   1, stack_file_pointer) ){
         fwrite(holder2, sizeof(holder2),
                1, backup_file_pointer);
         ++write_counter;
      }  /* ends while reading  */
      if(write_counter > 0)
         *stack_file_in_use = 1;
      else
         *stack_file_in_use = 0;

      fclose(backup_file_pointer);
      fclose(stack_file_pointer);
   }  /* ends else could not open stack file  */

   copy_stack_files(backup_file_name,
                    STACK_FILE, holder2);

   for(i=0; i<STACK_FILE_LENGTH; i++){
      stack[i][0] = holder[i][0];
      stack[i][1] = holder[i][1];
   }

   *pointer = *pointer + STACK_FILE_LENGTH - 1;

   printf("--- End of pop_data_off_of_stack");
}  /* ends pop_data_off_of_stack_file  */





   /*********************************************
   *
   *   append_stack_files(...
   *
   *   Append the second file onto the end
   *   of the first.
   *
   ***********************************************/

append_stack_files(first_file, second_file, holder)
   char first_file[], second_file[];
   short holder[STACK_FILE_LENGTH][2];
{
   FILE  *first, *second;
   int   i;

   if((first = fopen(first_file, "r+b")) == NULL)
      printf("\n\nSFO> Cannot open file %s",
             first_file);

   if((second = fopen(second_file, "rb")) == NULL)
      printf("\n\nSFO> Cannot open file %s",
             second_file);

          /*****************************************
          *
          *  Seek to the end of the first file and
          *  to the beginning of the second file.
          *
          *******************************************/

   fseek(first, 0L, 2);
   fseek(second, 0L, 0);

   while(fread(holder, sizeof(holder), 1, second) ){
      fwrite(holder, sizeof(holder), 1, first);
   }  /* ends while reading  */

   fclose(first);
   fclose(second);

}  /*  ends append_stack_files  */





   /********************************************
   *
   *   copy_stack_files(...
   *
   *   Copy the first file to the second.
   *
   **********************************************/

copy_stack_files(first_file, second_file, holder)
   char first_file[], second_file[];
   short holder[STACK_FILE_LENGTH][2];
{
   FILE  *first, *second;
   int   i;

   if( (first = fopen(first_file, "rb")) == NULL)
      printf("\n\nSFO> Cannot open file %s",
             first_file);

   if( (second = fopen(second_file, "wb")) == NULL)
      printf("\n\nSFO> Cannot open file %s",
             second_file);

          /******************************************
          *
          *  Seek to the beginning of the first file.
          *
          *******************************************/

   fseek(first, 0L, 0);

   while( fread(holder, sizeof(holder), 1, first) ){
      fwrite(holder, sizeof(holder), 1, second);
   }  /* ends while reading  */

   fclose(first);
   fclose(second);

}  /*  ends copy_stack_files */




   /********************************************
   *
   *   find_peaks(...
   *
   *   This function looks through the histogram
   *   array and finds the two highest peaks.
   *   The peaks must be separated, cannot be
   *   next to each other, by a spacing defined
   *   in cips.h.
   *
   *   The peaks array holds the peak value
   *   in the first place and its location in
   *   the second place.
   *
   *********************************************/

find_peaks(histogram, peak1, peak2)
   unsigned long histogram[];
   int *peak1, *peak2;
{
   int distance[PEAKS], peaks[PEAKS][2];
   int i, j=0, max=0, max_place=0;

   for(i=0; i<PEAKS; i++){
      distance[i] =  0;
      peaks[i][0] = -1;
      peaks[i][1] = -1;
   }

   for(i=0; i<=GRAY_LEVELS; i++){
      max       = histogram[i];
      max_place = i;
      insert_into_peaks(peaks, max, max_place);
   }  /* ends loop over i */

   for(i=1; i<PEAKS; i++){
      distance[i] = peaks[0][1] - peaks[i][1];
      if(distance[i] < 0)
         distance[i] = distance[i]*(-1);
   }

   *peak1 = peaks[0][1];
   for(i=PEAKS-1; i>0; i--)
    if(distance[i] > PEAK_SPACE) *peak2 = peaks[i][1];

}  /* ends find_peaks */





   /********************************************
   *
   *   insert_into_peaks(...
   *
   *   This function takes a value and its
   *   place in the histogram and inserts them
   *   into a peaks array.  This helps us rank
   *   the the peaks in the histogram.
   *
   *   The objective is to build a list of
   *   histogram peaks and thier locations.
   *
   *   The peaks array holds the peak value
   *   in the first place and its location in
   *   the second place.
   *
   *********************************************/

insert_into_peaks(peaks, max, max_place)
   int max, max_place, peaks[PEAKS][2];
{
   int i, j;

      /* first case */
   if(max > peaks[0][0]){
      for(i=PEAKS-1; i>0; i--){
         peaks[i][0] = peaks[i-1][0];
         peaks[i][1] = peaks[i-1][1];
      }
      peaks[0][0] = max;
      peaks[0][1] = max_place;
   }  /* ends if */

      /* middle cases */
   for(j=0; j<PEAKS-3; j++){
      if(max < peaks[j][0]  && max > peaks[j+1][0]){
         for(i=PEAKS-1; i>j+1; i--){
            peaks[i][0] = peaks[i-1][0];
            peaks[i][1] = peaks[i-1][1];
         }
         peaks[j+1][0] = max;
         peaks[j+1][1] = max_place;
      }  /* ends if */
   }  /* ends loop over j */
      /* last case */
   if(max < peaks[PEAKS-2][0]  && 
      max > peaks[PEAKS-1][0]){
      peaks[PEAKS-1][0] = max;
      peaks[PEAKS-1][1] = max_place;
   }  /* ends if */

}  /* ends insert_into_peaks */





   /********************************************
   *
   *   peaks_high_low(...
   *
   *   This function uses the histogram array
   *   and the peaks to find the best high and
   *   low threshold values for the threshold
   *   function.  You want the hi and low values
   *   so that you will threshold the image around
   *   the smaller of the two "humps" in the
   *   histogram.  This is because the smaller
   *   hump represents the objects while the
   *   larger hump represents the background.
   *
   *********************************************/

peaks_high_low(histogram, peak1, peak2, hi, low)
   int  peak1, peak2;
   short *hi, *low;
   unsigned long histogram[];
{
   int i, mid_point;
   unsigned long sum1 = 0, sum2 = 0;

   if(peak1 > peak2)
      mid_point = ((peak1 - peak2)/2) + peak2;
   if(peak1 < peak2)
      mid_point = ((peak2 - peak1)/2) + peak1;

   for(i=0; i<mid_point; i++)
      sum1 = sum1 + histogram[i];

   for(i=mid_point; i<=GRAY_LEVELS; i++)
      sum2 = sum2 + histogram[i];
   if(sum1 >= sum2){
      *low = mid_point;
      *hi  = GRAY_LEVELS;
   }
   else{
      *low = 0;
      *hi  = mid_point;
   }

}  /* ends peaks_high_low */





      /********************************************
      *
      *   valley_high_low(...
      *
      *   This function uses the histogram array
      *   and the valleys to find the best high and
      *   low threshold values for the threshold
      *   function.  You want the hi and low values
      *   so that you will threshold the image around
      *   the smaller of the two "humps" in the
      *   histogram.  This is because the smaller
      *   hump represents the objects while the
      *   larger hump represents the background.
      *
      *********************************************/

valley_high_low(histogram, peak1, peak2, hi, low)
   int  peak1, peak2;
   short *hi, *low;
   unsigned long histogram[];
{
   int  i, valley_point;
   unsigned long sum1 = 0, sum2 = 0;

   find_valley_point(histogram, peak1, peak2,
                     &valley_point);
   /*printf("\nVHL> valley point is %d",
            valley_point);*/

   for(i=0; i<valley_point; i++)
      sum1 = sum1 + histogram[i];
   for(i=valley_point; i<=GRAY_LEVELS; i++)
      sum2 = sum2 + histogram[i];

   if(sum1 >= sum2){
      *low = valley_point;
      *hi  = GRAY_LEVELS;
   }
   else{
      *low = 0;
      *hi  = valley_point;
   }

}  /* ends valley_high_low */





   /********************************************
   *
   *   find_valley_point(...
   *
   *   This function finds the low point of
   *   the valley between two peaks in a
   *   histogram.  It starts at the lowest
   *   peak and works its way up to the
   *   highest peak.  Along the way, it looks
   *   at each point in the histogram and inserts
   *   them into a list of points.  When done,
   *   it has the location of the smallest histogram
   *   point - that is the valley point.
   *
   *   The deltas array holds the delta value
   *   in the first place and its location in
   *   the second place.
   *
   *********************************************/

find_valley_point(histogram, peak1, 
                  peak2, valley_point)
   int  peak1, peak2, *valley_point;
   unsigned long histogram[];
{
   int  deltas[PEAKS][2], delta_hist, i;

   for(i=0; i<PEAKS; i++){
      deltas[i][0] = 10000;
      deltas[i][1] =    -1;
   }

   if(peak1 < peak2){
      for(i=peak1+1; i<peak2; i++){
         delta_hist = (int)(histogram[i]);
         insert_into_deltas(deltas, delta_hist, i);
      }  /* ends loop over i */
   }  /* ends if peak1 < peak2 */

   if(peak2 < peak1){
      for(i=peak2+1; i<peak1; i++){
         delta_hist = (int)(histogram[i]);
         insert_into_deltas(deltas, delta_hist, i);
      }  /* ends loop over i */
   }  /* ends if peak2 < peak1 */

   *valley_point = deltas[0][1];

}  /* ends find_valley_point */






   /********************************************
   *
   *   insert_into_deltas(...
   *
   *   This function inserts histogram deltas
   *   into a deltas array.  The smallest delta
   *   will be at the top of the array.
   *
   *   The objective is to build a list of
   *   histogram area deltas and thier locations.
   *
   *   The deltas array holds the delta value
   *   in the first place and its location in
   *   the second place.
   *
   *********************************************/

insert_into_deltas(deltas, value, place)
   int value, place, deltas[PEAKS][2];
{
   int i, j;

      /* first case */
   if(value < deltas[0][0]){
      for(i=PEAKS-1; i>0; i--){
         deltas[i][0] = deltas[i-1][0];
         deltas[i][1] = deltas[i-1][1];
      }
      deltas[0][0] = value;
      deltas[0][1] = place;
   }  /* ends if */

      /* middle cases */
   for(j=0; j<PEAKS-3; j++){
      if(value > deltas[j][0]  &&
         value < deltas[j+1][0]){
         for(i=PEAKS-1; i>j+1; i--){
            deltas[i][0] = deltas[i-1][0];
            deltas[i][1] = deltas[i-1][1];
         }
         deltas[j+1][0] = value;
         deltas[j+1][1] = place;
      }  /* ends if */
   }  /* ends loop over j */

      /* last case */
   if(value > deltas[PEAKS-2][0]  &&
      value < deltas[PEAKS-1][0]){
      deltas[PEAKS-1][0] = value;
      deltas[PEAKS-1][1] = place;
   }  /* ends if */

}  /* ends insert_into_deltas */





   /********************************************
   *
   *   get_segmentation_options(...
   *
   *   This function interacts with the user
   *   to obtain the options for image
   *   segmentation.
   *
   *********************************************/


get_segmentation_options(method, hi, low, value)
   char   method[];
   short  *hi, *low, *value;
{
   int   i, not_finished = 1, response;

   while(not_finished){
      printf(
         "\n\nThe image segmentation options are:\n");
      printf("\n\t1. Method is %s", method);
      printf("\n\t   (options are manual peaks");
      printf(        " valleys adapative)");
      printf("\n\t2. Value is %d", *value);
      printf("\n\t3. Hi    is %d", *hi);
      printf("\n\t4. Low   is %d", *low);
      printf("\n\t   Hi and Low needed only for");
      printf(        " manual method");
      printf("\n\nEnter choice (0 = no change):_\b");

      get_integer(&response);

      if(response == 0)
         not_finished = 0;

      if(response == 1){
         printf("\nEnter method (options are:");
         printf(" manual peaks valleys adaptive)\n\t");
         gets(method);
      }

      if(response == 2){
         printf("\nEnter value: ___\b\b\b");
         get_short(value);
      }

      if(response == 3){
         printf("\nEnter hi: ___\b\b\b");
         get_short(hi);
      }
      if(response == 4){
         printf("\nEnter low: ___\b\b\b");
         get_short(low);
      }

   }  /* ends while not_finished */
}  /* ends get_segmentation_options */






   /********************************************
   *
   *   get_threshold_options(...
   *
   *   This function interacts with the user
   *   to obtain the options for image
   *   threshold.
   *
   *********************************************/


get_threshold_options(method, hi, low, value)
   char   method[];
   short  *hi, *low, *value;
{
   int   i, not_finished = 1, response;

   while(not_finished){
      printf("\n\nThe image threshold options are:\n");
      printf("\n\t1. Method is %s", method);
      printf("\n\t   (options are manual peaks");
      printf(        " valleys adapative)");
      printf("\n\t2. Value is %d", *value);
      printf("\n\t3. Hi    is %d", *hi);
      printf("\n\t4. Low   is %d", *low);
      printf("\n\t   Hi and Low needed only for");
      printf(        " manual method");
      printf("\n\nEnter choice (0 = no change):_\b");

      get_integer(&response);

      if(response == 0)
         not_finished = 0;

      if(response == 1){
         printf("\nEnter method (options are:");
         printf(" manual peaks valleys adaptive)\n\t");
         gets(method);
      }

      if(response == 2){
         printf("\nEnter value: ___\b\b\b");
         get_short(value);
      }

      if(response == 3){
         printf("\nEnter hi: ___\b\b\b");
         get_short(hi);
      }
      if(response == 4){
         printf("\nEnter low: ___\b\b\b");
         get_short(low);
      }

   }  /* ends while not_finished */
}  /* ends get_threshold_options */



       /***********************************************
       *
       *       file d:\cips\segment2.c
       *
       *       Functions: This file contains
       *          find_cutoff_point
       *          edge_region
       *          gray_shade_region
       *          edge_gray_shade_region
       *          pixel_grow
       *          pixel_label_and_check_neighbors
       *          is_close
       *          erode_image_array
       *          get_edge_region_options
       *
       *       Purpose:
       *          These function implement the three
       *          segmentation techniques in Image
       *          Processing part 10.
       *
       *       External Calls:
       *          wtiff.c - round_off_image_size
       *                    create_file_if_needed
       *                    write_array_into_tiff_image
       *          tiff.c - read_tiff_header
       *          rtiff.c - read_tiff_image
       *          numcvrt.c - get_integer
       *          edges.c - quick_edge
       *                    homogeneity
       *                    difference_edge
       *                    contrast_edge
       *                    gaussian_edge
       *                    range
       *                    variance
       *                    detect_edges
       *          hist.c - calculate_histogram
       *                   zero_histogram
       *          thresh.c - threshold_image_array
       *
       *       Modifications:
       *          5 December 1992 - created
       *
       *************************************************/




     /*******************************************
     *
     *   find_cutoff_point(..
     *
     *   This function looks at a histogram
     *   and sets a cuttoff point at a given
     *   percentage of pixels.
     *   For example, if percent=0.6, you
     *   start at 0 in the histogram and count
     *   up until you've hit 60% of the pixels.
     *   Then you stop and return that pixel
     *   value.
     *
     ********************************************/

find_cutoff_point(histogram, percent, cutoff)
   unsigned long histogram[];
   float    percent;
   short    *cutoff;
{
   float  fd, fsum, sum_div;
   int    i, looking;
   long   lc, lr, num=0, sum=0;

   sum     = 0;
   i       = 0;
   lr      = (long)(ROWS);
   lc      = (long)(COLS);
   num     = lr*lc;
   fd      = (float)(num);

   while(looking){
      fsum    = (float)(sum);
      sum_div = fsum/fd;
      if(sum_div >= percent)
         looking = 0;
      else
         sum = sum + histogram[i++];
   }  /* ends while looking */

   if(i >= 256) i = 255;
   *cutoff = i;
   printf("\nCutoff is %d sum=%ld", *cutoff, sum);
}  /* ends find_cutoff_point */





     /*******************************************
     *
     *   edge_region(..
     *
     *   This function segments an image by
     *   growing regions inside of edges.
     *   The steps are:
     *      . detect edges
     *      . threshold edge output to a
     *        percent value
     *      . remove edges from consideration
     *      . grow regions
     *
     *******************************************/


edge_region(in_name, out_name, the_image, out_image,
            il, ie, ll, le, edge_type, min_area,
            max_area, diff, percent, set_value,
            erode)
   char     in_name[], out_name[];
   float    percent;
   int      edge_type, il, ie, ll, le;
   short    diff, erode, 
            max_area, min_area, 
            set_value,
            the_image[ROWS][COLS],
            out_image[ROWS][COLS];
{

   int    a, b, count, i, j, k,
          length, width;
   short  cutoff;
   struct tiff_header_struct image_header;
   unsigned long histogram[GRAY_LEVELS+1];


   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

      /***************************
      *
      *   Detect the edges.  Do
      *   not threshold.
      *
      ****************************/

   if(edge_type == 1  ||
      edge_type == 2  ||
      edge_type == 3)
      detect_edges(in_name, out_name, the_image,
                   out_image, il, ie, ll, le,
                   edge_type, 0, 0);

   if(edge_type == 4){
      quick_edge(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 0, 0);
   }  /* ends if 4 */

   if(edge_type == 5){
      homogeneity(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 0, 0);
   }  /* ends if 5 */

   if(edge_type == 6){
      difference_edge(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 0, 0);
   }  /* ends if 6 */

   if(edge_type == 7){
      contrast_edge(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 0, 0);
   }  /* ends if 7 */

   if(edge_type == 8){
      gaussian_edge(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 3, 0, 0);
   }  /* ends if 8 */

   if(edge_type == 10){
      range(in_name, out_name, the_image,
            out_image, il, ie, ll, le,
            3, 0, 0);
   }  /* ends if 10 */

   if(edge_type == 11){
      variance(in_name, out_name, the_image,
               out_image, il, ie, ll, le,
               0, 0);
   }  /* ends if 11 */

/**write_array_into_tiff_image("f:e1.tif", out_image,
il, ie, ll, le);**/

      /* copy out_image to the_image */
   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         the_image[i][j] = out_image[i][j];

      /******************************
      *
      *   Threshold the edge detector
      *   output at a given percent.
      *   This eliminates the weak
      *   edges.
      *
      *******************************/
   zero_histogram(histogram);
   calculate_histogram(the_image, histogram);
   find_cutoff_point(histogram, percent, &cutoff);
   threshold_image_array(the_image, out_image,
                         255, cutoff, set_value);
/**write_array_into_tiff_image("f:e2.tif", out_image,
il, ie, ll, le);**/

   if(erode != 0){
         /* copy out_image to the_image */
      for(i=0; i<ROWS; i++)
         for(j=0; j<COLS; j++)
            the_image[i][j] = out_image[i][j];
      erode_image_array(the_image, out_image,
                        set_value, erode);
   }  /* ends if erode */

/**write_array_into_tiff_image("f:e3.tif", out_image,
il, ie, ll, le);**/

      /*******************************
      *
      *   Set all the edge values to
      *   FORGET_IT so the region
      *   growing will not use those
      *   points.
      *
      *******************************/

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         if(out_image[i][j] == set_value)
            out_image[i][j] = FORGET_IT;

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         the_image[i][j] = out_image[i][j];

   pixel_grow(the_image, out_image, diff,
              min_area, max_area);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends edge_region */



     /*******************************************
     *
     *   gray_shade_region(...
     *
     *   This function segments an image by
     *   growing regions based only on gray
     *   shade.
     *
     *******************************************/


gray_shade_region(in_name, out_name, the_image,
                  out_image, il, ie, ll, le,
                  diff, min_area, max_area)
   char   in_name[], out_name[];
   int    il, ie, ll, le;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          diff, min_area, max_area;
{
   int    a, b, big_count, count, i, j, k, l,
          not_finished, length, width;
   short  temp[3][3];
   struct tiff_header_struct image_header;

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);
   pixel_grow(the_image, out_image, diff,
              min_area, max_area);
   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends gray_shade_region */





     /*******************************************
     *
     *   edge_gray_shade_region(..
     *
     *   This function segments an image by
     *   growing gray shade regions inside of
     *   edges.  It combines the techniques
     *   of the edge_region and gray_shade_region
     *   functions.
     *
     *   The steps are:
     *      . detect edges
     *      . threshold edge output to a
     *        percent value
     *      . lay the edges on top of the original
     *        image to eliminate them from
     *        consideration
     *      . grow regions
     *
     *******************************************/

edge_gray_shade_region(in_name, out_name, the_image,
            out_image, il, ie, ll, le, edge_type,
            min_area, max_area, diff, percent,
            set_value, erode)
   char     in_name[], out_name[];
   float    percent;
   int      edge_type, il, ie, ll, le;
   short    diff, erode, 
            max_area, min_area, 
            set_value,
            the_image[ROWS][COLS],
            out_image[ROWS][COLS];
{
   int    a, b, count, i, j, k,
          length, width;
   short  cutoff;
   struct tiff_header_struct image_header;
   unsigned long histogram[GRAY_LEVELS+1];

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

      /***************************
      *
      *   Detect the edges.  Do
      *   not threshold.
      *
      ****************************/

   if(edge_type == 1  ||
      edge_type == 2  ||
      edge_type == 3)
      detect_edges(in_name, out_name, the_image,
                   out_image, il, ie, ll, le,
                   edge_type, 0, 0);

   if(edge_type == 4){
      quick_edge(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 0, 0);
   }  /* ends if 4 */
   if(edge_type == 5){
      homogeneity(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 0, 0);
   }  /* ends if 5 */

   if(edge_type == 6){
      difference_edge(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 0, 0);
   }  /* ends if 6 */

   if(edge_type == 7){
      contrast_edge(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 0, 0);
   }  /* ends if 7 */

   if(edge_type == 8){
      gaussian_edge(in_name, out_name, the_image,
                 out_image, il, ie, ll, le,
                 3, 0, 0);
   }  /* ends if 8 */

   if(edge_type == 10){
      range(in_name, out_name, the_image,
            out_image, il, ie, ll, le,
            3, 0, 0);
   }  /* ends if 10 */

   if(edge_type == 11){
      variance(in_name, out_name, the_image,
               out_image, il, ie, ll, le,
               0, 0);
   }  /* ends if 11 */

/**write_array_into_tiff_image("f:e1.tif", out_image,
il, ie, ll, le);**/

      /* copy out_image to the_image */
   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         the_image[i][j] = out_image[i][j];

      /******************************
      *
      *   Threshold the edge detector
      *   output at a given percent.
      *   This eliminates the weak
      *   edges.
      *
      *******************************/

   zero_histogram(histogram);
   calculate_histogram(the_image, histogram);
   find_cutoff_point(histogram, percent, &cutoff);
   threshold_image_array(the_image, out_image,
                         255, cutoff, set_value);

/**write_array_into_tiff_image("f:e2.tif", out_image,
il, ie, ll, le);**/

   if(erode != 0){
         /* copy out_image to the_image */
      for(i=0; i<ROWS; i++)
         for(j=0; j<COLS; j++)
            the_image[i][j] = out_image[i][j];
      erode_image_array(the_image, out_image,
                        set_value, erode);
   }  /* ends if erode */

/**write_array_into_tiff_image("f:e3.tif", out_image,
il, ie, ll, le);**/

      /*******************************
      *
      *   Read the original gray shade
      *   image back into the_image.
      *
      *******************************/

   read_tiff_image(in_name, the_image, il, ie, ll, le);

      /*******************************
      *
      *   Overlay the edge values
      *   on top of the original
      *   image by setting them to
      *   FORGET_IT so the region
      *   growing will not use those
      *   points.
      *
      *******************************/

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         if(out_image[i][j] == set_value)
            the_image[i][j] = FORGET_IT;

/**write_array_into_tiff_image("f:e4.tif", the_image,
il, ie, ll, le);**/

   pixel_grow(the_image, out_image, diff,
              min_area, max_area);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);

}  /* ends edge_gray_shade_region */




    /**********************************************
    *
    *   pixel_grow(...
    *
    *   The function grows regions.  It is similar
    *   to the grow function in segment.c, but it
    *   has several new capabilities.  It can
    *   eliminate regions if they are too large or
    *   too small.
    *
    *   It ignores pixels = FORGET_IT.  This allows
    *   it to ignore edges or regions already
    *   eliminated from consideration.
    *
    *   It adds pixels to a growing region only if
    *   the pixel is close enough to the average gray
    *   level of that region.
    *
    ***********************************************/

pixel_grow(input, output, diff, min_area, max_area)
   short input[ROWS][COLS],
         output[ROWS][COLS],
         max_area,
         min_area,
         diff;
{
   char name[80];

   int count,
       first_call,
       i,
       ii,
       j,
       jj,
       object_found,
       pointer,
       pop_i,
       pop_j,
       stack_empty,
       stack_file_in_use;

   short g_label, target, sum, stack[STACK_SIZE][2];

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         output[i][j] = 0;

   g_label       = 2;
   object_found  = 0;
   first_call    = 1;

            /*************************************
            *
            *   Now begin the process of growing
            *   regions.
            *
            **************************************/

   for(i=0; i<ROWS; i++){
if( (i%4) == 0) printf("\n");
printf("-i=%3d label=%3d", i, g_label);
      for(j=0; j<COLS; j++){

         target            = input[i][j];
         sum               = target;
         count             = 0;
         stack_file_in_use = 0;
         stack_empty       = 1;
         pointer           = -1;
               /**********************************
               *
               *  Search for the first pixel of
               *  a region.  It must not equal
               *  FORGET_IT, and it must be close
               *  enough to the target (ave value).
               *
               ***********************************/

         if(input[i][j] != FORGET_IT            &&
            is_close(input[i][j], target, diff) &&
            output[i][j] == 0){
            pixel_label_and_check_neighbor(input,
                           output, &target, &sum,
                           &count, stack, g_label,
                           &stack_empty, &pointer,
                           i, j, diff,
                           &stack_file_in_use,
                           &first_call);
            object_found = 1;
         }  /* ends if is_close */

               /*****************************
               *
               *  If the stack is not empty,
               *  pop the coordinates of
               *  the pixel off the stack
               *  and check its 8 neighbors.
               *
               *******************************/

         while(stack_empty == 0){
            pop_i = stack[pointer][0]; /* POP       */
            pop_j = stack[pointer][1]; /* OPERATION */
            --pointer;
            if(pointer <= 0){
               if(stack_file_in_use){
                  pop_data_off_of_stack_file(
                                 stack,
                                 &pointer,
                                 &stack_file_in_use);
               }  /* ends if stack_file_in_use  */
               else{
                  pointer     = 0;
                  stack_empty = 1;
               }  /* ends else stack file is
                     not in use  */
            }  /*  ends if point <= 0  */
            pixel_label_and_check_neighbor(input,
                           output, &target, &sum,
                           &count, stack, g_label,
                           &stack_empty, &pointer,
                           pop_i, pop_j,
                           diff, &stack_file_in_use,
                           &first_call);
         }  /* ends while stack_empty == 0 */

         if(object_found == 1){
            object_found = 0;

                  /**********************************
                  *
                  *  The object must be in the
                  *  size constraints given by
                  *  min_area and max_area
                  *
                  *********************************/

            if(count >= min_area  &&
               count <= max_area)
               ++g_label;
                  /**********************************
                  *
                  *   Remove the object from the
                  *   output.  Set all pixels in the
                  *   object you are removing to
                  *   FORGET_IT.
                  *
                  **********************************/

            else{
               for(ii=0; ii<ROWS; ii++){
                  for(jj=0; jj<COLS; jj++){
                     if(output[ii][jj] == g_label){
                        output[ii][jj] = 0;
                        input[ii][jj]  = FORGET_IT;
                     }  /* ends if output == g_label */
                  }  /* ends loop over jj */
               }  /* ends loop over ii */
            }  /* ends else remove object */
         }  /* ends if object_found == 1 */

      }   /* ends loop over j */
   }  /* ends loop over i */

   printf("\nGROW> found %d objects", g_label);

} /* ends pixel_grow  */





   /********************************************
   *
   *  pixel_label_and_check_neighbors(...
   *
   *  This function labels a pixel with an object
   *  label and then checks the pixel's 8
   *  neighbors.  If any of the neigbors are
   *  set, then they are also labeled.
   *
   *  It also updates the target or ave pixel
   *  value of the pixels in the region being
   *  grown.
   *
   ***********************************************/
pixel_label_and_check_neighbor(input_image,
                         output_image, target,
                         sum, count, stack,
                         g_label, stack_empty,
                         pointer, r, e, diff,
                         stack_file_in_use,
                         first_call)
int   *count,
      e,
      *first_call,
      *pointer,
      r,
      *stack_empty,
      *stack_file_in_use;

short input_image[ROWS][COLS],
      output_image[ROWS][COLS],
      g_label,
      *sum,
      *target,
      stack[STACK_SIZE][2],
      diff;
{
   int already_labeled = 0,
       i, j;

   if (output_image[r][e] != 0)
      already_labeled = 1;

   output_image[r][e] = g_label;
   *count  = *count + 1;
   if(*count > 1){
      *sum    = *sum + input_image[r][e];
      *target = *sum / *count;
   }

      /***************************************
      *
      *   Look at the 8 neighors of the
      *   point r,e.
      *
      *   Ensure the points are close enough
      *   to the target and do not equal
      *   FORGET_IT.
      *
      *   Ensure the points you are checking
      *   are in the image, i.e. not less
      *   than zero and not greater than
      *   ROWS-1 or COLS-1.
      *
      ***************************************/

   for(i=(r-1); i<=(r+1); i++){
      for(j=(e-1); j<=(e+1); j++){

         if((i>=0)   &&
            (i<=ROWS-1)  &&
            (j>=0)   &&
            (j<=COLS-1)){

            if( input_image[i][j] != FORGET_IT   &&
                is_close(input_image[i][j],
                            *target, diff)       &&
                output_image[i][j] == 0){
               *pointer           = *pointer + 1;
               stack[*pointer][0] = i; /* PUSH      */
               stack[*pointer][1] = j; /* OPERATION */
               *stack_empty       = 0;

               if(*pointer >= (STACK_SIZE -
                               STACK_FILE_LENGTH)){
                  push_data_onto_stack_file(stack,
                            pointer, first_call);
                  *stack_file_in_use = 1;
               }  /* ends if *pointer >=
                     STACK_SIZE - STACK_FILE_LENGTH*/

            }  /* ends if is_close */
         }  /* end if i and j are on the image */
      }  /* ends loop over i rows           */
   }  /* ends loop over j columns        */
}  /* ends pixel_label_and_check_neighbors  */



   /********************************************
   *
   *  is_close(...
   *
   *  This function tests to see if two pixel
   *  values are close enough together.  It
   *  uses the delta parameter to make this
   *  judgement.
   *
   ***********************************************/

is_close(a, b, delta)
   short a, b, delta;
{
   int   result = 0;
   short diff;

   diff = a-b;
   if(diff < 0) diff = diff*(-1);
   if(diff < delta)
      result = 1;
   return(result);
}  /* ends is_close */




     /*******************************************
     *
     *   erode_image_array(..
     *
     *   This function erodes pixels.  If a pixel
     *   equals value and has more than threshold
     *   neighbors equal to 0, then set that
     *   pixel in the output to 0.
     *
     *******************************************/


erode_image_array(the_image, out_image,
                  value, threshold)
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS],
          threshold,
          value;
{
   int    a, b, count, i, j, k,
          length, width;

      /***************************
      *
      *   Loop over image array
      *
      ****************************/

   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
         out_image[i][j] = the_image[i][j];

   printf("\n");

   for(i=1; i<ROWS-1; i++){
      if( (i%10) == 0) printf("%3d", i);
      for(j=1; j<COLS-1; j++){
         if(the_image[i][j] == value){
            count = 0;
            for(a=-1; a<=1; a++){
                for(b=-1; b<=1; b++){
                      if(the_image[i+a][j+b] == 0)
                         count++;
                }  /*  ends loop over b */
            }  /* ends loop over a */
            if(count > threshold) out_image[i][j] = 0;
         }  /* ends if the_image == value */
      }  /* ends loop over j */
   }  /* ends loop over i */

}  /* ends erode_image_array */





   /********************************************
   *
   *  get_edge_region_options(...
   *
   *  This function interacts with the user to   
   *  get the options needed to call the 
   *  edge and region based segmentation 
   *  routines.
   *
   ********************************************/

get_edge_region_options(method, edge_type, 
         min_area, max_area, set_value, 
         diff, percent, erode)
   char  method[];
   float *percent;
   int   *edge_type;
   short *diff, *erode, 
         *min_area, *max_area, 
         *set_value;
{
   int not_finished = 1, response;

   while(not_finished){
      printf("\n\nEdge Region Segmentation Options:");
      printf("\n\t1.  Method is %s", method);
      printf("\n\t    Recall: Edge, Gray shade, "
                      "Combination");
      printf("\n\t2.  Edge type is %d", *edge_type);
      printf("\n\t    Recall: ");
      printf("\n\t     1=Prewitt     2=Kirsch");
      printf("\n\t     3=Sobel       4=quick");
      printf("\n\t     5=homogeneity 6=difference");
      printf("\n\t     7=contrast    8=gaussian");
      printf("\n\t     10=range      11=variance");
      printf("\n\t3.  Min area is %d", *min_area);
      printf("\n\t4.  Max area is %d", *max_area);
      printf("\n\t5.  Set value is %d", *set_value);
      printf("\n\t6.  Difference value is %d", *diff);
      printf("\n\t7.  Threshold percentage is %f",
                      *percent);
      printf("\n\t8.  Erode is %d", *erode);
      printf("\n\nEnter choice (0 = no change) _\b");

      get_integer(&response);

      if(response == 0){
        not_finished = 0;
      }

      if(response == 1){
         printf("\n\t    Recall: Edge, Gray shade, "
                         "Combination");
         printf("\n\t> ");
         gets(method);
      }

      if(response == 2){
         printf("\n\t    Recall:"); 
         printf("\n\t     1=Prewitt     2=Kirsch");
         printf("\n\t     3=Sobel       4=quick");
         printf("\n\t     5=homogeneity 6=difference");
         printf("\n\t     7=contrast    8=gaussian");
         printf("\n\t     10=range      11=variance");
         printf("\n\t__\b");
         get_integer(edge_type);
      }

      if(response == 3){
         printf("\nEnter min area:__\b\b");
         get_integer(min_area);
      }

      if(response == 4){
         printf("\nEnter max area:__\b\b");
         get_integer(max_area);
      }

      if(response == 5){
         printf("\nEnter set value:__\b\b");
         get_integer(set_value);
      }

      if(response == 6){
         printf("\nEnter difference:__\b\b");
         get_integer(diff);
      }

      if(response == 7){
         printf("\nEnter threshold percentage:__\b\b");
         get_float(percent);
      }

      if(response == 8){
         printf("\nEnter erode:__\b\b");
         get_integer(erode);
      }

   }  /* ends while not_finished */
}  /* ends get_edge_region_options */
