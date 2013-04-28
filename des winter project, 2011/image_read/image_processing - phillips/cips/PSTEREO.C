
   /***********************************************
   *
   *  file pstereo.c
   *
   *  Functions: This file contains
   *     main
   *     zero_line
   *     initialize_pattern
   *     lengthen_pattern
   *     no_change
   *     shorten_pattern
   *     get_random_values
   *     random_substitution
   *     read_image_line
   *     write_image_line
   *     test_print_line
   *
   *  Purpose:
   *     This file contains a program that will
   *     make pixel based random dot stereograms.
   *
   *  External Calls:
   *     tiff.c - read_tiff_header
   *     brwtiff.c - bread_tiff_image
   *                 bwrite_tiff_image
   *                 bcreate_allocate_tiff_file
   *                 allocate_image_array
   *
   *  Modifications:
   *     18 March 1995 - created  
   *     27 August 1998 - modified to work on 
   *            entire images at once.
   *
   *************************************************/

/***** ALGORITHM NOTES
   width is the width of the input and output lines
   pattern_width is the initial width of the pattern
   current_width is the current width of the pattern
      (this changes as elements are inserted into
      and deleted from the pattern).

   pattern[] must be wider than width and narrower than
             ? so I will try it at width=width
   depth_line[] will be width wide
   processed_pattern will be width wide
   index points to the current place in the pattern
      we are working on
   pp_index points to the current place in the processed
      pattern,  Cannot use the depth_line counter j
      for this because you will skip over places in
      the processed pattern.

*********/

#include "cips.h"

#define KONSTANT        2
#define PATTERN_START   0
#define PATTERN_END   255
#define ASCII_SIZE    256

#define TEST_NO_CHANGE
#undef  DEBUG_MODE
#define COMMAND_LINE_MODE


short **depth_image,
      **pattern,
      **processed_pattern;

void zero_line();
void initialize_pattern();
void lengthen_pattern();
void no_change();
void shorten_pattern();
void get_random_values();
void random_substitution();
void read_image_line();
void write_image_line();
void test_print_line();


main(argc, argv)
   char *argv[];
   int  argc;
{
   char depth_file_name[MAX_NAME_LENGTH],
        s_file_name[MAX_NAME_LENGTH],
        pp_file_name[MAX_NAME_LENGTH],
        response[MAX_NAME_LENGTH];

   FILE *depth_file,
        *processed_pattern_file,
        *stereo_file;

   int  current_width,
        i,
        j,
        index, 
        last_pixel,
        location,
        max_width,
        pattern_width,
        pp_index,
        this_pixel;

   long length, width;

   int line = 0;

#ifdef COMMAND_LINE_MODE
   if(argc != 5){
      printf(
      "\nusage: pstereo pattern-width "
      "\n               depth-file-name "
      "stereo-file-name "
      "\n               processed-pattern-file-name"
      "\n");
      exit(1);
   }


   pattern_width =        atoi(argv[1]);
   strcpy(depth_file_name,     argv[2]);
   strcpy(s_file_name,         argv[3]);
   strcpy(pp_file_name,        argv[4]);

   if(does_not_exist(depth_file_name)){
      printf("\nThe depth file %s does not exist",
             depth_file_name);
      exit(1);
   }

   get_image_size(depth_file_name, &length, &width);
   depth_image = allocate_image_array(length, width);
   pattern = allocate_image_array(length, width);
   processed_pattern = allocate_image_array(
                          length, width);
   create_image_file(depth_file_name, s_file_name); 
   create_image_file(depth_file_name, pp_file_name);
   read_image_array(depth_file_name, depth_image);

#endif  /* ifdef COMMAND_LINE_MODE */



#ifdef DEBUG_MODE


   strcpy(depth_file_name, "d:/src/dfile.tif");
   strcpy(s_file_name, "d:/src/sfile.tif");
   strcpy(pp_file_name, "d:/src/ppfile.tif");

   if(does_not_exist(depth_file_name)){
      printf("\nThe depth file %s does not exist",
             depth_file_name);
      exit(1);
   }

   read_tiff_header(depth_file_name, &image_header);
   round_off_image_size(&image_header, 
                        &length, &width);
   
   image_header.image_length = length*ROWS;
   image_header.image_width  = width*COLS;

   create_allocate_tiff_file(pp_file_name, 
                             &image_header,
                             the_image);
   create_allocate_tiff_file(s_file_name, 
                             &image_header,
                             the_image);

   width         = width*COLS;
   pattern_width = 15;

   pattern           = malloc(KONSTANT*width*sizeof(int));
   depth_line        = malloc(KONSTANT*width*sizeof(int));
   processed_pattern = malloc(KONSTANT*width*sizeof(int));


#endif  /* ifdef COMMAND_LINE_MODE */



      /***********************************************
      *
      *   This is the major loop of the program.
      *   It reads one line at a time from the 
      *   depth file, processes that one line,
      *   and writes the resulting pattern to the
      *   processed pattern file.
      *
      ***********************************************/

   for(i=0; i<length; i++){

      if(i%10 == 0) printf(" %d", i);

      zero_line(pattern[i], width);
      zero_line(processed_pattern[i], width);

      initialize_pattern(pattern[i], 
                         &current_width, 
                         &max_width, 
                         pattern_width,
                         &index);

         /*******************************************
         *
         *   Loop through the pixels in the
         *   depth_line.  Call one of the three
         *   processing routines based on the
         *   relationship between the last pixel
         *   and this pixel.
         *
         *******************************************/
    
      last_pixel = depth_image[i][0];

      pp_index = 0;
      
      for(j=0; j<width; j++){
         this_pixel = depth_image[i][j];


         if(this_pixel > last_pixel)
            shorten_pattern(
               (this_pixel-last_pixel),
               pattern[i], &index, &current_width,
               width);


         if(this_pixel < last_pixel)
            lengthen_pattern(
               (last_pixel-this_pixel), 
               pattern[i], &index, &current_width, 
               &width, &max_width);

         

            /****************************************
            *
            *   Perform the no_change in every
            *   pass.  Do it after you have done
            *   the shorten and lenghten pattern.
            *
            ****************************************/

         no_change(pattern[i], processed_pattern[i],
                   pp_index, current_width, &index); 
         pp_index++;
         
         
         if(index >= current_width)
            index = 0;
         
         last_pixel = depth_image[i][j];

      }  /* ends loop over j */

   }   /* ends loop over i */

   write_image_array(pp_file_name, processed_pattern);

printf("\nMAIN> width=%d",width);
   for(i=0; i<length; i++)
      random_substitution(processed_pattern[i], width);

   write_image_array(s_file_name, processed_pattern);



   free_image_array(depth_image, length);
   free_image_array(pattern, length);
   free_image_array(processed_pattern, length);

   return(111);
   
}  /* ends main                 */





   /***********************************************
   *
   *   shorten_pattern(...
   *
   *   This funtion shortens the pattern by
   *   deleting an element from it.  For example,
   *   if the input pattern is abcdefg, 
   *   the output pattern could be abcfg.
   *
   ***********************************************/

void shorten_pattern(size, pattern, index,
                     current_width, width)
   short  *pattern;
   int    size, *index, *current_width, width;
{
   short *temp_pattern;
   int   i, new_index, new_width;


   temp_pattern = malloc(width*sizeof(short));

   for(i=0; i<width; i++)
      temp_pattern[i] = pattern[i];
   zero_line(pattern, width);

   new_index = 0;
   new_width = *current_width - size;

      /*********************************************
      *
      *   Increase the index by the amount we want
      *   to shorten the pattern.   Then copy the
      *   temp_pattern back to the pattern skipping
      *   over parts of the pattern by the amount
      *   we want to shorten it.
      *
      *********************************************/

   *index = (*index + size) % (*current_width);

   for(new_index=0; new_index<new_width; new_index++){
      pattern[new_index] = temp_pattern[*index];
      *index = *index + 1;
      if(*index >= *current_width)
         *index = 0;
   }  /* ends loop over new_index */

   *current_width = new_width;
   *index         = 0;

   free(temp_pattern);

}  /* ends shorten_pattern */





   /***********************************************
   *
   *   initialize_pattern(...
   *
   *   This function creates an initial pattern
   *   that is as wide as the pattern_width
   *   parameter.
   *
   ***********************************************/

void initialize_pattern(pattern, current_width, 
                        max_width, pattern_width, 
                        index)
   short *pattern;
   int   *current_width,
         *max_width,
         *index,
         pattern_width;
{
   int i;

   for(i=0; i<pattern_width; i++)
      pattern[i] = i+PATTERN_START;

   *index         = 0;
   *current_width = pattern_width;
   *max_width     = pattern_width;
}  /* ends initialize_pattern */





   /***********************************************
   *
   *   no_change(...
   *
   *   This function processes the pattern
   *   and does not make any changes to it.
   *
   ***********************************************/

void no_change(pattern, processed_pattern, pp_index,
               current_width, index)
   short *pattern, *processed_pattern;
   int   pp_index, current_width, *index;
{
   processed_pattern[pp_index] = 
      pattern[*index];

   *index = *index + 1;
   if(*index >= current_width)
      *index = 0;

}   /* ends no_change */





   /***********************************************
   *
   *   zero_line(...
   *
   *   This function fills an int array with
   *   zeros.
   *
   ***********************************************/

void zero_line(array, length)
   short  *array;
   int    length;
{
   int i;
   for(i=0; i<length; i++)
      array[i] = 0;
}  /* ends zero_line */





   /***********************************************
   *
   *   lengthen_pattern(...
   *
   *   This funtion lengthens the pattern by
   *   inserting an element(s) into it.  For example,
   *   if the input pattern is abcdefg, 
   *   the output pattern could be abcdefgh.
   *
   ***********************************************/

void lengthen_pattern(size, pattern, index,
                     current_width, width, max_width)
   short  *pattern;
   int    size, *index, *current_width, 
          *width, *max_width;
{
   int  *temp_pattern;
   int  count, i, new_index, new_width;


   temp_pattern = malloc(KONSTANT*(*width)*sizeof(int));

   for(i=0; i<(*width); i++)
      temp_pattern[i] = pattern[i];
      
   zero_line(pattern, KONSTANT*(*width));

   for(count=0, new_index=0; count<size; 
       count++, new_index++){
      pattern[new_index] = 
         count + *max_width + PATTERN_START;
   }  /* ends loop over count */

   new_width = *current_width + size;

   for( ; new_index < new_width; new_index++){
      pattern[new_index] = temp_pattern[*index];
      *index = *index + 1;
      if(*index >= *current_width)
         *index = 0;
   }  /* ends loop over new_index */

   *current_width = new_width;
   *index         = 0;
   *max_width     = *max_width + size;

   free(temp_pattern);

}  /* ends lengthen_pattern */





   /***********************************************
   *
   *   random_substitution(...
   *
   *   This function takes the processed_pattern
   *   array and substitutes random values for each
   *   value in the array.
   *
   *   Fill the substitution_values array with
   *   random characters that are all printable
   *   (PATTERN_START to PATTERN_END).
   *
   ***********************************************/

void random_substitution(processed_pattern, width)
   short  *processed_pattern;
   int    width;
{
   int  substitution_values[GRAY_LEVELS+1];
   int  i, place;

   get_random_values(substitution_values);
   
   for(i=0; i<(width); i++){
      place = processed_pattern[i];
      processed_pattern[i] = 
         substitution_values[place];
   }  /* ends loop over i */
   
}  /* ends random_substitution */

                                 



   /***********************************************
   *
   *   get_random_values(...
   *
   *   This function fills array with random values.
   *   The limit on the random values are from     
   *   PATTERN_START to PATTERN_END.
   *
   ***********************************************/

void get_random_values(array)  
   int  array[];
{
   int i, number;

#ifdef NEVER
these lines worked ok, they used all the printable
characters from 0 through small z   
   for(i=0; i<ASCII_SIZE; i++){
       number   = rand();
       number   = number % (PATTERN_END - PATTERN_START);
       number   = number + PATTERN_START;
       array[i] = number;
   }  /* ends loop over i */     
#endif

#ifdef NEVER
/* Let's try something different, only use the
   characters 0-9 and A-Z 
   0-9 are 48-57 decimal A-Z are 65-90 */

   for(i=0; i<ASCII_SIZE; i++){
       number   = rand();
       number   = number % 36;
       number   = number + PATTERN_START;
       if(number > 57   &&   number < 65)
          number = number + 7;
       array[i] = number;
   }  /* ends loop over i */     
#endif
  
#ifdef NEVER
/* Let's try something different, only use the
   characters A-Z 
   A-Z are 65-90 */

   for(i=0; i<GRAY_LEVELS+1; i++){
       number   = rand();
       number   = number % GRAY_LEVELS+1;
       array[i] = number;
   }  /* ends loop over i */     
#endif


/* Let's try something different, only use the
   1's and 0's */

   for(i=0; i<GRAY_LEVELS+1; i++){
       number   = rand();
       number   = number % 2;
       if(number == 1) number = PATTERN_END;
       array[i] = number;
   }  /* ends loop over i */     
   
}  /* ends get_random_values */
                                 




   /***********************************************
   *
   *  test_print_line(...
   *
   *   This is a debug function that prints
   *   an array of integers.
   *
   ***********************************************/

void test_print_line(line, width)
   short  *line;
   int    width;
{
   int i;
   for(i=0; i<width; i++)
      printf("-%3d", line[i]);
   printf("\n");
}  /* ends test_print_line */

                                 
   

#ifdef NEVER

   /***********************************************
   *
   *   read_image_line(...
   *
   *   This function reads a line of pixels from
   *   a TIFF image file.
   *
   ***********************************************/

void read_image_line(the_image, file_name, 
                     line_number, array, width)
   char  file_name[];
   short the_image[ROWS][COLS];
   int   *array, line_number, width;
{
   int i, il, ie, ll, le, 
       pixels_to_read, 
       reading_counter, 
       still_reading;

   il = line_number;
   ll = il + 1;
   ie = 1; 
   le = ie + COLS;

   if(width < COLS)
      le  = ie + (width - 1);

   pixels_to_read  = width;
   reading_counter = 0;

   still_reading = 1;
   while(still_reading){

      read_tiff_image(file_name, the_image, 
                      il, ie, ll, le);
      for(i=0; i<(le-ie); i++)
         array[i + (reading_counter*COLS)] = 
            the_image[0][i];
      reading_counter++;

      pixels_to_read = pixels_to_read - (le-ie);
      if(pixels_to_read <= 1)
         still_reading = 0;
      else{
         ie = ie + COLS;
         le = ie + COLS;
         if(le > width)
            le = width;
      }  /* ends else */
         
   }  /* ends while still_reading */

}  /* ends read_image_line */



                                 
                                 
   /***********************************************
   *
   *   write_image_line(...
   *
   *   This function writes a line of pixels to
   *   a TIFF image file.
   *
   ***********************************************/

void write_image_line(the_image, file_name, 
                      line_number, array, width)
   char  file_name[];
   short the_image[ROWS][COLS];
   int   *array, line_number, width;
{
   int  i, il, ie, ll, le,
        pixels_to_write,
        writing_counter,
        still_writing;

   il = line_number; 
   ll = il + 1;
   ie = 1;
   le = ie + COLS;

   if(width < COLS)
      le = ie + (width - 1);

   pixels_to_write = width;
   writing_counter = 0;

   still_writing = 1;
   while(still_writing){
      
      for(i=0; i<(le-ie); i++)
         the_image[0][i] =
            array[i + (writing_counter*COLS)];
      writing_counter++;

      write_array_into_tiff_image(file_name, the_image,
                                  il, ie, ll, le);

      pixels_to_write = pixels_to_write - (le-ie);
      if(pixels_to_write <= 1)
         still_writing = 0;
      else{
         ie = ie + COLS;
         le = ie + COLS;
         if(le > width)
            le = width;
      }  /* ends else */

   }  /* ends while still_writing */

}  /* ends write_image_line */

#endif
