&^%$#@!
10
Tue Apr 04 06:28:45 1995
This is the clean version that works.
!@#$%^&

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
   *     rtiff.c - read_tiff_image
   *     wtiff.c - does_not_exist
   *               round_off_image_size
   *               create_allocate_tiff_file
   *               write_array_into_tiff_image
   *
   *  Modifications:
   *     18 March 1995 - created  
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


short the_image[ROWS][COLS];

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
        *depth_line,
        i,
        j,
        index, 
        last_pixel,
        length,
        location,
        max_width,
        *pattern,
        pattern_width,
        pp_index,
        *processed_pattern,
        this_pixel,
        width;

   struct tiff_header_struct image_header;

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

   strcpy(depth_file_name, argv[2]);
   strcpy(s_file_name, argv[3]);
   strcpy(pp_file_name, argv[4]);

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
   pattern_width = atoi(argv[1]);

   pattern           = malloc(KONSTANT*width*sizeof(int));
   depth_line        = malloc(KONSTANT*width*sizeof(int));
   processed_pattern = malloc(KONSTANT*width*sizeof(int));


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

   printf("\npstereo>> %d rows", length*ROWS);
  
   for(i=0; i<length*ROWS; i++){

      if(i%10 == 0) printf(" %d", i);

      read_image_line(the_image, depth_file_name,
                      i+1, depth_line, width);

      zero_line(pattern, KONSTANT*width);
      zero_line(processed_pattern, KONSTANT*width);

      initialize_pattern(pattern, 
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
    
      last_pixel = depth_line[0];

      pp_index = 0;
      
      for(j=0; j<width; j++){
         this_pixel = depth_line[j];


         if(this_pixel > last_pixel)
            shorten_pattern(
               (this_pixel-last_pixel),
               pattern, &index, &current_width,
               width);


         if(this_pixel < last_pixel)
            lengthen_pattern(
               (last_pixel-this_pixel), 
               pattern, &index, &current_width, 
               &width, &max_width);

         

            /****************************************
            *
            *   Perform the no_change in every
            *   pass.  Do it after you have done
            *   the shorten and lenghten pattern.
            *
            ****************************************/

         no_change(pattern, processed_pattern,
                   pp_index, current_width, &index); 
         pp_index++;
         
         
         if(index >= current_width)
            index = 0;
         
         last_pixel = depth_line[j];

      }  /* ends loop over j */
      

      write_image_line(the_image, pp_file_name, i+1,
                       processed_pattern, width);
      random_substitution(processed_pattern, width);
      write_image_line(the_image, s_file_name, i+1,
                       processed_pattern, width);

   }   /* ends the major loop */


   free(pattern);
   free(depth_line);
   free(processed_pattern);


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
   int  *pattern;
   int  size, *index, *current_width, width;
{
   int  *temp_pattern;
   int  i, new_index, new_width;


   temp_pattern = malloc(KONSTANT*width*sizeof(int));

   for(i=0; i<width; i++)
      temp_pattern[i] = pattern[i];
   zero_line(pattern, KONSTANT*width);

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
   int  pattern[];
   int *current_width,
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
   int  *pattern, *processed_pattern;
   int  pp_index, current_width, *index;
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
   int  *array;
   int  length;
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
   int  *pattern;
   int  size, *index, *current_width, 
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
   int  *processed_pattern;
   int  width;
{
   int  substitution_values[GRAY_LEVELS+1];
   int  i, place;

   get_random_values(substitution_values);
   
   for(i=0; i<(KONSTANT*width); i++){
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
   int  *line;
   int  width;
{
   int i;
   for(i=0; i<width; i++)
      printf("-%3d", line[i]);
   printf("\n");
}  /* ends test_print_line */

                                 
   


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
!@#$%^&
&^%$#@!
9
Tue Apr 04 05:45:37 1995
This has the debug statement out.  Now
I will remove the redundant pattern_index
and index.  I will delete pattern_index.
!@#$%^&

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
   *     make character based stereograms.
   *
   *  External Calls:
   *     tiff.c - read_tiff_header
   *     rtiff.c - read_tiff_image
   *     wtiff.c - does_not_exist
   *               round_off_image_size
   *               create_allocate_tiff_file
   *               write_array_into_tiff_image
   *
   *  Modifications:
   *     18 March 1995 - created  
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
   pattern_index usually points to the end of 
      the pattern
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


short the_image[ROWS][COLS];

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
        *depth_line,
        i,
        j,
        index, 
        last_pixel,
        length,
        location,
        max_width,
        *pattern,
        pattern_index,
        pattern_width,
        pp_index,
        *processed_pattern,
        this_pixel,
        width;

   struct tiff_header_struct image_header;

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

   strcpy(depth_file_name, argv[2]);
   strcpy(s_file_name, argv[3]);
   strcpy(pp_file_name, argv[4]);

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
   pattern_width = atoi(argv[1]);

   pattern           = malloc(KONSTANT*width*sizeof(int));
   depth_line        = malloc(KONSTANT*width*sizeof(int));
   processed_pattern = malloc(KONSTANT*width*sizeof(int));


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



#ifdef DEBUG_MODE






#endif /* ifdef DEBUG_MODE */

   

      /***********************************************
      *
      *   This is the major loop of the program.
      *   It reads one line at a time from the 
      *   depth file, processes that one line,
      *   and writes the resulting pattern to the
      *   processed pattern file.
      *
      ***********************************************/

printf("\npstereo>> %d rows", length*ROWS);
   for(i=0; i<length*ROWS; i++){

if(i%10 == 0) printf(" %d", i);

      read_image_line(the_image, depth_file_name,
                      i+1, depth_line, width);

      zero_line(pattern, KONSTANT*width);
      zero_line(processed_pattern, KONSTANT*width);

      initialize_pattern(&pattern_index, pattern, 
                         &current_width, 
                         &max_width, 
                         pattern_width,
                         &index);

         /*******************************************
         *
         *   Loop through the characters in the
         *   depth_line.  Call one of the three
         *   processing routines based on the
         *   relationship between the last pixel
         *   and this pixel.
         *
         *******************************************/
    
      last_pixel = depth_line[0];

      pp_index = 0;
      
      for(j=0; j<width; j++){
         this_pixel = depth_line[j];

         /******** ?????
         if(this_pixel == '\n')
            this_pixel = last_pixel;
         ***************/


         if(this_pixel > last_pixel)
            shorten_pattern(
               (this_pixel-last_pixel),
               pattern, &index, &current_width,
               &pattern_index, width);


         if(this_pixel < last_pixel)
            lengthen_pattern(
               (last_pixel-this_pixel), 
               pattern, &index, &current_width, 
               &pattern_index, &width, &max_width);

         

            /****************************************
            *
            *   Perform the no_change in every
            *   pass.  Do it after you have done
            *   the shorten and lenghten pattern.
            *
            ****************************************/

         no_change(pattern, processed_pattern,
                   pp_index, &pattern_index, 
                   current_width, &index); 
         pp_index++;
         
         
         if(pattern_index >= current_width)
            pattern_index = 0;
         
         last_pixel = depth_line[j];

      }  /* ends loop over j */
      

      write_image_line(the_image, pp_file_name, i+1,
                       processed_pattern, width);
      random_substitution(processed_pattern, width);
      write_image_line(the_image, s_file_name, i+1,
                       processed_pattern, width);

   }   /* ends the major loop */


   free(pattern);
   free(depth_line);
   free(processed_pattern);


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
                     current_width, pattern_index, 
                     width)
   int  *pattern;
   int  size, *index, *current_width, 
        *pattern_index, width;
{
   int  *temp_pattern;
   int  i, new_index, new_width;


   temp_pattern = malloc(KONSTANT*width*sizeof(int));

   for(i=0; i<width; i++)
      temp_pattern[i] = pattern[i];
   zero_line(pattern, KONSTANT*width);

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

   *current_width     = new_width;
   *pattern_index     = 0;
*index = *pattern_index;

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

void initialize_pattern(pattern_index, pattern, 
                        current_width, max_width, 
                        pattern_width, index)
   int  pattern[];
   int *pattern_index,
       *current_width,
       *max_width,
       *index,
       pattern_width;
{
   int i;

   for(i=0; i<pattern_width; i++)
      pattern[i] = i+PATTERN_START;

   *pattern_index = 0;
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
               pattern_index, current_width,
               index)
   int  *pattern, *processed_pattern;
   int  pp_index, *pattern_index, 
        current_width, *index;
{
   processed_pattern[pp_index] = 
      pattern[*pattern_index];

   *pattern_index = *pattern_index + 1;
   if(*pattern_index >= current_width)
      *pattern_index = 0;

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
   int  *array;
   int  length;
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
   *   inserting an element(s) from it.  For example,
   *   if the input pattern is abcdefg, 
   *   the output pattern could be abcdefgh.
   *
   ***********************************************/

void lengthen_pattern(size, pattern, index,
                     current_width, pattern_index, 
                     width, max_width)
   int  *pattern;
   int  size, *index, *current_width, 
        *pattern_index, *width, *max_width;
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
   *pattern_index = 0;
*index = *pattern_index;
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
   int  *processed_pattern;
   int  width;
{
   int  substitution_values[GRAY_LEVELS+1];
   int  i, place;

   get_random_values(substitution_values);
   
   for(i=0; i<(KONSTANT*width); i++){
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
       array[i] = number;
   }  /* ends loop over i */     
   
}  /* ends get_random_values */
                                 
                                 
   /***********************************************
   *
   *  test_print_line(...
   *
   ***********************************************/

void test_print_line(line, width)
   int  *line;
   int  width;
{
   int i;
   for(i=0; i<width; i++)
      printf("-%3d", line[i]);
   printf("\n");
}  /* ends test_print_line */



                                 
                                 
   /***********************************************
   *
   *  read_image_line(...
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
   *  write_image_line(...
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
!@#$%^&
&^%$#@!
8
Sun Mar 26 11:48:35 1995
This has lots of debug printf statements
BUT IT WORKS RIGHT!
It has the correction to set index equal
to pattern_index.  Corrects an error in
the text book. 
!@#$%^&

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
   *     make character based stereograms.
   *
   *  External Calls:
   *     tiff.c - read_tiff_header
   *     ritff.c - read_tiff_image
   *     wtiff.c - does_not_exist
   *               round_off_image_size
   *               create_allocate_tiff_file
   *               write_array_into_tiff_image
   *
   *  Modifications:
   *     18 March 1995 - created  
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
   pattern_index usually points to the end of 
      the pattern
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


short the_image[ROWS][COLS];

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
        *depth_line,
        i,
        j,
        index, 
        last_pixel,
        length,
        location,
        max_width,
        *pattern,
        pattern_index,
        pattern_width,
        pp_index,
        *processed_pattern,
        this_pixel,
        width;

   struct tiff_header_struct image_header;

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

   strcpy(depth_file_name, argv[2]);
   strcpy(s_file_name, argv[3]);
   strcpy(pp_file_name, argv[4]);

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
   pattern_width = atoi(argv[1]);

   pattern           = malloc(KONSTANT*width*sizeof(int));
   depth_line        = malloc(KONSTANT*width*sizeof(int));
   processed_pattern = malloc(KONSTANT*width*sizeof(int));


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



#ifdef DEBUG_MODE






#endif /* ifdef DEBUG_MODE */

   

      /***********************************************
      *
      *   This is the major loop of the program.
      *   It reads one line at a time from the 
      *   depth file, processes that one line,
      *   and writes the resulting pattern to the
      *   processed pattern file.
      *
      ***********************************************/

printf("\npstereo>> %d rows", length*ROWS);
   for(i=0; i<length*ROWS; i++){

if(i%10 == 0) printf(" %d", i);

      read_image_line(the_image, depth_file_name,
                      i+1, depth_line, width);

      zero_line(pattern, KONSTANT*width);
      zero_line(processed_pattern, KONSTANT*width);

      initialize_pattern(&pattern_index, pattern, 
                         &current_width, 
                         &max_width, 
                         pattern_width,
                         &index);

         /*******************************************
         *
         *   Loop through the characters in the
         *   depth_line.  Call one of the three
         *   processing routines based on the
         *   relationship between the last pixel
         *   and this pixel.
         *
         *******************************************/
    
      last_pixel = depth_line[0];

      pp_index = 0;
      
      for(j=0; j<width; j++){
         this_pixel = depth_line[j];

         /******** ?????
         if(this_pixel == '\n')
            this_pixel = last_pixel;
         ***************/


         if(this_pixel > last_pixel)
            shorten_pattern(
               (this_pixel-last_pixel),
               pattern, &index, &current_width,
               &pattern_index, width);


         if(this_pixel < last_pixel)
            lengthen_pattern(
               (last_pixel-this_pixel), 
               pattern, &index, &current_width, 
               &pattern_index, &width, &max_width);

         

            /****************************************
            *
            *   Perform the no_change in every
            *   pass.  Do it after you have done
            *   the shorten and lenghten pattern.
            *
            ****************************************/

         no_change(pattern, processed_pattern,
                   pp_index, &pattern_index, 
                   current_width, &index); 
         pp_index++;
         
         
         if(pattern_index >= current_width)
            pattern_index = 0;
         
         last_pixel = depth_line[j];

      }  /* ends loop over j */
      

      write_image_line(the_image, pp_file_name, i+1,
                       processed_pattern, width);
      random_substitution(processed_pattern, width);
      write_image_line(the_image, s_file_name, i+1,
                       processed_pattern, width);

   }   /* ends the major loop */


   free(pattern);
   free(depth_line);
   free(processed_pattern);


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
                     current_width, pattern_index, 
                     width)
   int  *pattern;
   int  size, *index, *current_width, 
        *pattern_index, width;
{
   int  *temp_pattern;
   int  i, new_index, new_width;


char testr[80];
printf("\nSHORTEN> enter Test (t) or Not (n)");
gets(testr);
if(testr[0] == 't'  ||  testr[0] == 'T'){
printf("\n|pndx=%d|ndx=%d|wid=%d|", *pattern_index, *index, *current_width);
for(i=0; i<*current_width; i++)
printf("-%3d", pattern[i]);
}  /* ends if testr */

   temp_pattern = malloc(KONSTANT*width*sizeof(int));

   for(i=0; i<width; i++)
      temp_pattern[i] = pattern[i];
   zero_line(pattern, KONSTANT*width);

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

   *current_width     = new_width;
   *pattern_index     = 0;
*index = *pattern_index;

   free(temp_pattern);

if(testr[0] == 't'  ||  testr[0] == 'T'){
printf("\n|pndx=%d|ndx=%d|wid=%d|", *pattern_index, *index, *current_width);
for(i=0; i<*current_width; i++)
printf("-%3d", pattern[i]);
}  /* ends if testr */

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

void initialize_pattern(pattern_index, pattern, 
                        current_width, max_width, 
                        pattern_width, index)
   int  pattern[];
   int *pattern_index,
       *current_width,
       *max_width,
       *index,
       pattern_width;
{
   int i;

   for(i=0; i<pattern_width; i++)
      pattern[i] = i+PATTERN_START;

   *pattern_index = 0;
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
               pattern_index, current_width,
               index)
   int  *pattern, *processed_pattern;
   int  pp_index, *pattern_index, 
        current_width, *index;
{
   processed_pattern[pp_index] = 
      pattern[*pattern_index];

   *pattern_index = *pattern_index + 1;
   if(*pattern_index >= current_width)
      *pattern_index = 0;

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
   int  *array;
   int  length;
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
   *   inserting an element(s) from it.  For example,
   *   if the input pattern is abcdefg, 
   *   the output pattern could be abcdefgh.
   *
   ***********************************************/

void lengthen_pattern(size, pattern, index,
                     current_width, pattern_index, 
                     width, max_width)
   int  *pattern;
   int  size, *index, *current_width, 
        *pattern_index, *width, *max_width;
{
   int  *temp_pattern;
   int  count, i, new_index, new_width;

char testr[80];
printf("\nLENGTHEN> enter Test (t) or Not (n)");
gets(testr);
if(testr[0] == 't'  ||  testr[0] == 'T'){
printf("\n|pndx=%d|ndx=%d|wid=%d|", *pattern_index, *index, *current_width);
for(i=0; i<*current_width; i++)
printf("-%3d", pattern[i]);
}  /* ends if testr */

   temp_pattern = malloc(KONSTANT*(*width)*sizeof(int));

   for(i=0; i<(*width); i++)
      temp_pattern[i] = pattern[i];
      
   zero_line(pattern, KONSTANT*(*width));

   for(count=0, new_index=0; count<size; 
       count++, new_index++){
      pattern[new_index] = 
         count + *max_width + PATTERN_START;
if(testr[0] == 't'  ||  testr[0] == 'T')
printf("=%3d", pattern[new_index]);
   }  /* ends loop over count */

   new_width = *current_width + size;

   for( ; new_index < new_width; new_index++){
      pattern[new_index] = temp_pattern[*index];
if(testr[0] == 't'  ||  testr[0] == 'T')
printf("=%3d", pattern[new_index]);
      *index = *index + 1;
      if(*index >= *current_width)
         *index = 0;
   }  /* ends loop over new_index */

   *current_width = new_width;
   *pattern_index = 0;
*index = *pattern_index;
   *max_width     = *max_width + size;

   free(temp_pattern);

if(testr[0] == 't'  ||  testr[0] == 'T'){
printf("\n|pndx=%d|ndx=%d|wid=%d|", *pattern_index, *index, *current_width);
for(i=0; i<*current_width; i++) printf("-%3d", pattern[i]);
printf("\nPress Enter to continue");
gets(testr);
}  /* ends if testr */

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
   int  *processed_pattern;
   int  width;
{
   int  substitution_values[GRAY_LEVELS+1];
   int  i, place;

   get_random_values(substitution_values);
   
   for(i=0; i<(KONSTANT*width); i++){
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
       array[i] = number;
   }  /* ends loop over i */     
   
}  /* ends get_random_values */
                                 
                                 
   /***********************************************
   *
   *  test_print_line(...
   *
   ***********************************************/

void test_print_line(line, width)
   int  *line;
   int  width;
{
   int i;
   for(i=0; i<width; i++)
      printf("-%3d", line[i]);
   printf("\n");
}  /* ends test_print_line */



                                 
                                 
   /***********************************************
   *
   *  read_image_line(...
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
   *  write_image_line(...
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
!@#$%^&
&^%$#@!
7
Mon Mar 20 06:24:58 1995
The read_image_line and write_image_line work.
!@#$%^&

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
   *     make character based stereograms.
   *
   *  External Calls:
   *     tiff.c - read_tiff_header
   *     ritff.c - read_tiff_image
   *     wtiff.c - does_not_exist
   *               round_off_image_size
   *               create_allocate_tiff_file
   *               write_array_into_tiff_image
   *
   *  Modifications:
   *     18 March 1995 - created  
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
   pattern_index usually points to the end of 
      the pattern
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


short the_image[ROWS][COLS];

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
        *depth_line,
        i,
        j,
        index, 
        last_pixel,
        length,
        location,
        max_width,
        *pattern,
        pattern_index,
        pattern_width,
        pp_index,
        *processed_pattern,
        this_pixel,
        width;

   struct tiff_header_struct image_header;

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

   strcpy(depth_file_name, argv[2]);
   strcpy(s_file_name, argv[3]);
   strcpy(pp_file_name, argv[4]);

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
   pattern_width = atoi(argv[1]);

   pattern           = malloc(KONSTANT*width*sizeof(int));
   depth_line        = malloc(KONSTANT*width*sizeof(int));
   processed_pattern = malloc(KONSTANT*width*sizeof(int));


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
   pattern_width = 10;

   pattern           = malloc(KONSTANT*width*sizeof(int));
   depth_line        = malloc(KONSTANT*width*sizeof(int));
   processed_pattern = malloc(KONSTANT*width*sizeof(int));


#endif  /* ifdef COMMAND_LINE_MODE */



#ifdef DEBUG_MODE






#endif /* ifdef DEBUG_MODE */

   

      /***********************************************
      *
      *   This is the major loop of the program.
      *   It reads one line at a time from the 
      *   depth file, processes that one line,
      *   and writes the resulting pattern to the
      *   processed pattern file.
      *
      ***********************************************/

   for(i=0; i<length*ROWS; i++){

      read_image_line(the_image, depth_file_name,
                      i+1, depth_line, width);

      zero_line(pattern, KONSTANT*width);
      zero_line(processed_pattern, KONSTANT*width);

      initialize_pattern(&pattern_index, pattern, 
                         &current_width, 
                         &max_width, 
                         pattern_width,
                         &index);

         /*******************************************
         *
         *   Loop through the characters in the
         *   depth_line.  Call one of the three
         *   processing routines based on the
         *   relationship between the last pixel
         *   and this pixel.
         *
         *******************************************/
    
      last_pixel = depth_line[0];

      pp_index = 0;
      
      for(j=0; j<width; j++){
         this_pixel = depth_line[j];

         /******** ?????
         if(this_pixel == '\n')
            this_pixel = last_pixel;
         ***************/


         if(this_pixel > last_pixel)
            shorten_pattern(
               (this_pixel-last_pixel),
               pattern, &index, &current_width,
               &pattern_index, width);


         if(this_pixel < last_pixel)
            lengthen_pattern(
               (last_pixel-this_pixel), 
               pattern, &index, &current_width, 
               &pattern_index, &width, &max_width);

         

            /****************************************
            *
            *   Perform the no_change in every
            *   pass.  Do it after you have done
            *   the shorten and lenghten pattern.
            *
            ****************************************/

         no_change(pattern, processed_pattern,
                   pp_index, &pattern_index, 
                   current_width, &index); 
         pp_index++;
         
         
         if(pattern_index >= current_width)
            pattern_index = 0;
         
         last_pixel = depth_line[j];

      }  /* ends loop over j */
      

      write_image_line(the_image, pp_file_name, i+1,
                       processed_pattern, width);
      random_substitution(processed_pattern, width);
      write_image_line(the_image, s_file_name, i+1,
                       processed_pattern, width);

   }   /* ends the major loop */


   free(pattern);
   free(depth_line);
   free(processed_pattern);


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
                     current_width, pattern_index, 
                     width)
   int  *pattern;
   int  size, *index, *current_width, 
        *pattern_index, width;
{
   int  *temp_pattern;
   int  i, new_index, new_width;

   temp_pattern = malloc(KONSTANT*width*sizeof(int));

   for(i=0; i<width; i++)
      temp_pattern[i] = pattern[i];
   zero_line(pattern, KONSTANT*width);

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

   *current_width     = new_width;
   *pattern_index     = 0;

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

void initialize_pattern(pattern_index, pattern, 
                        current_width, max_width, 
                        pattern_width, index)
   int  pattern[];
   int *pattern_index,
       *current_width,
       *max_width,
       *index,
       pattern_width;
{
   int i;

   for(i=0; i<pattern_width; i++)
      pattern[i] = i+PATTERN_START;

   *pattern_index = 0;
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
               pattern_index, current_width,
               index)
   int  *pattern, *processed_pattern;
   int  pp_index, *pattern_index, 
        current_width, *index;
{
   processed_pattern[pp_index] = 
      pattern[*pattern_index];

   *pattern_index = *pattern_index + 1;
   if(*pattern_index >= current_width)
      *pattern_index = 0;

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
   int  *array;
   int  length;
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
   *   inserting an element(s) from it.  For example,
   *   if the input pattern is abcdefg, 
   *   the output pattern could be abcdefgh.
   *
   ***********************************************/

void lengthen_pattern(size, pattern, index,
                     current_width, pattern_index, 
                     width, max_width)
   int  *pattern;
   int  size, *index, *current_width, 
        *pattern_index, *width, *max_width;
{
   int  *temp_pattern;
   int  count, i, new_index, new_width;

   temp_pattern = malloc(KONSTANT*(*width)*sizeof(int));

   for(i=0; i<(*width); i++)
      temp_pattern[i] = pattern[i];
      
   zero_line(pattern, KONSTANT*(*width));

   for(count=0, new_index=0; count<size; 
       count++, new_index++)
      pattern[new_index] = 
         count + *max_width + PATTERN_START;

   new_width = *current_width + size;

   for( ; new_index < new_width; new_index++){
      pattern[new_index] = temp_pattern[*index];
      *index = *index + 1;
      if(*index >= *current_width)
         *index = 0;
   }  /* ends loop over new_index */

   *current_width = new_width;
   *pattern_index = 0;
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
   int  *processed_pattern;
   int  width;
{
   int  substitution_values[GRAY_LEVELS+1];
   int  i, place;

   get_random_values(substitution_values);
   
   for(i=0; i<(KONSTANT*width); i++){
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
   
/* Let's try something different, only use the
   characters A-Z 
   A-Z are 65-90 */

   for(i=0; i<GRAY_LEVELS+1; i++){
       number   = rand();
       number   = number % GRAY_LEVELS+1;
       array[i] = number;
   }  /* ends loop over i */     
   
}  /* ends get_random_values */
                                 
                                 
   /***********************************************
   *
   *  test_print_line(...
   *
   ***********************************************/

void test_print_line(line, width)
   int  *line;
   int  width;
{
   int i;
   for(i=0; i<width; i++)
      printf("-%3d", line[i]);
   printf("\n");
}  /* ends test_print_line */



                                 
                                 
   /***********************************************
   *
   *  read_image_line(...
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
   *  write_image_line(...
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
!@#$%^&
&^%$#@!
6
Sun Mar 19 10:56:58 1995
Coded write_image_line. No testing yet.
!@#$%^&

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
   *     make character based stereograms.
   *
   *  External Calls:
   *     tiff.c - read_tiff_header
   *     ritff.c - read_tiff_image
   *     wtiff.c - does_not_exist
   *               round_off_image_size
   *               create_allocate_tiff_file
   *               write_array_into_tiff_image
   *
   *  Modifications:
   *     18 March 1995 - created  
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
   pattern_index usually points to the end of 
      the pattern
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
#define DEBUG_MODE
#undef COMMAND_LINE_MODE


short the_image[ROWS][COLS];

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
        *depth_line,
        i,
        j,
        index, 
        last_pixel,
        length,
        location,
        max_width,
        *pattern,
        pattern_index,
        pattern_width,
        pp_index,
        *processed_pattern,
        this_pixel,
        width;

   struct tiff_header_struct image_header;

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

   strcpy(depth_file_name, argv[2]);
   strcpy(s_file_name, argv[3]);
   strcpy(pp_file_name, argv[4]);

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
   pattern_width = atoi(argv[1]);

   pattern           = malloc(KONSTANT*width*sizeof(int));
   depth_line        = malloc(KONSTANT*width*sizeof(int));
   processed_pattern = malloc(KONSTANT*width*sizeof(int));


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
   pattern_width = 10;

   pattern           = malloc(KONSTANT*width*sizeof(int));
   depth_line        = malloc(KONSTANT*width*sizeof(int));
   processed_pattern = malloc(KONSTANT*width*sizeof(int));


#endif  /* ifdef COMMAND_LINE_MODE */



#ifdef DEBUG_MODE






#endif /* ifdef DEBUG_MODE */

   

      /***********************************************
      *
      *   This is the major loop of the program.
      *   It reads one line at a time from the 
      *   depth file, processes that one line,
      *   and writes the resulting pattern to the
      *   processed pattern file.
      *
      ***********************************************/

   for(i=0; i<length*ROWS; i++){

      read_image_line(the_image, depth_file_name,
                      i+1, depth_line, width);
   
      zero_line(pattern, KONSTANT*width);
      zero_line(processed_pattern, KONSTANT*width);

      initialize_pattern(&pattern_index, pattern, 
                         &current_width, 
                         &max_width, 
                         pattern_width,
                         &index);

         /*******************************************
         *
         *   Loop through the characters in the
         *   depth_line.  Call one of the three
         *   processing routines based on the
         *   relationship between the last pixel
         *   and this pixel.
         *
         *******************************************/
    
      last_pixel = depth_line[0];

      pp_index = 0;
      
      for(j=0; j<width; j++){
         this_pixel = depth_line[j];

         /******** ?????
         if(this_pixel == '\n')
            this_pixel = last_pixel;
         ***************/


         if(this_pixel > last_pixel)
            shorten_pattern(
               (this_pixel-last_pixel),
               pattern, &index, &current_width,
               &pattern_index, width);


         if(this_pixel < last_pixel)
            lengthen_pattern(
               (last_pixel-this_pixel), 
               pattern, &index, &current_width, 
               &pattern_index, &width, &max_width);

         

            /****************************************
            *
            *   Perform the no_change in every
            *   pass.  Do it after you have done
            *   the shorten and lenghten pattern.
            *
            ****************************************/

         no_change(pattern, processed_pattern,
                   pp_index, &pattern_index, 
                   current_width, &index); 
         pp_index++;
         
         
         if(pattern_index >= current_width)
            pattern_index = 0;
         
         last_pixel = depth_line[j];

      }  /* ends loop over j */
      

      write_image_line(the_image, pp_file_name, i+1,
                       processed_pattern, width);
      random_substitution(processed_pattern, width);
      write_image_line(the_image, s_file_name, i+1,
                       processed_pattern, width);
      
   }   /* ends the major loop */


   free(pattern);
   free(depth_line);
   free(processed_pattern);


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
                     current_width, pattern_index, 
                     width)
   int  *pattern;
   int  size, *index, *current_width, 
        *pattern_index, width;
{
   int  *temp_pattern;
   int  i, new_index, new_width;

   temp_pattern = malloc(KONSTANT*width*sizeof(int));

   for(i=0; i<width; i++)
      temp_pattern[i] = pattern[i];
   zero_line(pattern, KONSTANT*width);

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

   *current_width     = new_width;
   *pattern_index     = 0;

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

void initialize_pattern(pattern_index, pattern, 
                        current_width, max_width, 
                        pattern_width, index)
   int  pattern[];
   int *pattern_index,
       *current_width,
       *max_width,
       *index,
       pattern_width;
{
   int i;

   for(i=0; i<pattern_width; i++)
      pattern[i] = i+PATTERN_START;

   *pattern_index = 0;
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
               pattern_index, current_width,
               index)
   int  *pattern, *processed_pattern;
   int  pp_index, *pattern_index, 
        current_width, *index;
{
   processed_pattern[pp_index] = 
      pattern[*pattern_index];

   *pattern_index = *pattern_index + 1;
   if(*pattern_index >= current_width)
      *pattern_index = 0;

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
   int  *array;
   int  length;
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
   *   inserting an element(s) from it.  For example,
   *   if the input pattern is abcdefg, 
   *   the output pattern could be abcdefgh.
   *
   ***********************************************/

void lengthen_pattern(size, pattern, index,
                     current_width, pattern_index, 
                     width, max_width)
   int  *pattern;
   int  size, *index, *current_width, 
        *pattern_index, *width, *max_width;
{
   int  *temp_pattern;
   int  count, i, new_index, new_width;

   temp_pattern = malloc(KONSTANT*(*width)*sizeof(int));

   for(i=0; i<(*width); i++)
      temp_pattern[i] = pattern[i];
      
   zero_line(pattern, KONSTANT*(*width));

   for(count=0, new_index=0; count<size; 
       count++, new_index++)
      pattern[new_index] = 
         count + *max_width + PATTERN_START;

   new_width = *current_width + size;

   for( ; new_index < new_width; new_index++){
      pattern[new_index] = temp_pattern[*index];
      *index = *index + 1;
      if(*index >= *current_width)
         *index = 0;
   }  /* ends loop over new_index */

   *current_width = new_width;
   *pattern_index = 0;
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
   int  *processed_pattern;
   int  width;
{
   int  substitution_values[GRAY_LEVELS+1];
   int  i, place;

   get_random_values(substitution_values);
   
   for(i=0; i<(KONSTANT*width); i++){
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
   
/* Let's try something different, only use the
   characters A-Z 
   A-Z are 65-90 */

   for(i=0; i<GRAY_LEVELS+1; i++){
       number   = rand();
       number   = number % GRAY_LEVELS+1;
       array[i] = number;
   }  /* ends loop over i */     
   
}  /* ends get_random_values */
                                 
                                 
   /***********************************************
   *
   *  test_print_line(...
   *
   ***********************************************/

void test_print_line(line, width)
   int  *line;
   int  width;
{
   int i;
   for(i=0; i<width; i++)
      printf("-%3d", line[i]);
   printf("\n");
}  /* ends test_print_line */



                                 
                                 
   /***********************************************
   *
   *  read_image_line(...
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
   *  write_image_line(...
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
!@#$%^&
&^%$#@!
5
Sun Mar 19 10:33:56 1995
Finished testing read_image_line. 
OK I think.
!@#$%^&

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
   *     make character based stereograms.
   *
   *  External Calls:
   *     tiff.c - read_tiff_header
   *     ritff.c - read_tiff_image
   *     wtiff.c - does_not_exist
   *               round_off_image_size
   *               create_allocate_tiff_file
   *
   *  Modifications:
   *     18 March 1995 - created  
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
   pattern_index usually points to the end of 
      the pattern
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
#define DEBUG_MODE
#undef COMMAND_LINE_MODE


short the_image[ROWS][COLS];

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
        *depth_line,
        i,
        j,
        index, 
        last_pixel,
        length,
        location,
        max_width,
        *pattern,
        pattern_index,
        pattern_width,
        pp_index,
        *processed_pattern,
        this_pixel,
        width;

   struct tiff_header_struct image_header;

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

   strcpy(depth_file_name, argv[2]);
   strcpy(s_file_name, argv[3]);
   strcpy(pp_file_name, argv[4]);

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
   pattern_width = atoi(argv[1]);

   pattern           = malloc(KONSTANT*width*sizeof(int));
   depth_line        = malloc(KONSTANT*width*sizeof(int));
   processed_pattern = malloc(KONSTANT*width*sizeof(int));


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
   pattern_width = 10;

   pattern           = malloc(KONSTANT*width*sizeof(int));
   depth_line        = malloc(KONSTANT*width*sizeof(int));
   processed_pattern = malloc(KONSTANT*width*sizeof(int));


#endif  /* ifdef COMMAND_LINE_MODE */



#ifdef DEBUG_MODE






#endif /* ifdef DEBUG_MODE */

   

      /***********************************************
      *
      *   This is the major loop of the program.
      *   It reads one line at a time from the 
      *   depth file, processes that one line,
      *   and writes the resulting pattern to the
      *   processed pattern file.
      *
      ***********************************************/

   for(i=0; i<length*ROWS; i++){

      read_image_line(the_image, depth_file_name,
                      i+1, depth_line, width);
   
      zero_line(pattern, KONSTANT*width);
      zero_line(processed_pattern, KONSTANT*width);

      initialize_pattern(&pattern_index, pattern, 
                         &current_width, 
                         &max_width, 
                         pattern_width,
                         &index);

         /*******************************************
         *
         *   Loop through the characters in the
         *   depth_line.  Call one of the three
         *   processing routines based on the
         *   relationship between the last pixel
         *   and this pixel.
         *
         *******************************************/
    
      last_pixel = depth_line[0];

      pp_index = 0;
      
      for(j=0; j<width; j++){
         this_pixel = depth_line[j];

         /******** ?????
         if(this_pixel == '\n')
            this_pixel = last_pixel;
         ***************/


         if(this_pixel > last_pixel)
            shorten_pattern(
               (this_pixel-last_pixel),
               pattern, &index, &current_width,
               &pattern_index, width);


         if(this_pixel < last_pixel)
            lengthen_pattern(
               (last_pixel-this_pixel), 
               pattern, &index, &current_width, 
               &pattern_index, &width, &max_width);

         

            /****************************************
            *
            *   Perform the no_change in every
            *   pass.  Do it after you have done
            *   the shorten and lenghten pattern.
            *
            ****************************************/

         no_change(pattern, processed_pattern,
                   pp_index, &pattern_index, 
                   current_width, &index); 
         pp_index++;
         
         
         if(pattern_index >= current_width)
            pattern_index = 0;
         
         last_pixel = depth_line[j];

      }  /* ends loop over j */
      

      write_image_line(the_image, pp_file_name, i+1,
                       processed_pattern, width);
      random_substitution(processed_pattern, width);
      write_image_line(the_image, s_file_name, i+1,
                       processed_pattern, width);
      
   }   /* ends the major loop */


   free(pattern);
   free(depth_line);
   free(processed_pattern);


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
                     current_width, pattern_index, 
                     width)
   int  *pattern;
   int  size, *index, *current_width, 
        *pattern_index, width;
{
   int  *temp_pattern;
   int  i, new_index, new_width;

   temp_pattern = malloc(KONSTANT*width*sizeof(int));

   for(i=0; i<width; i++)
      temp_pattern[i] = pattern[i];
   zero_line(pattern, KONSTANT*width);

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

   *current_width     = new_width;
   *pattern_index     = 0;

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

void initialize_pattern(pattern_index, pattern, 
                        current_width, max_width, 
                        pattern_width, index)
   int  pattern[];
   int *pattern_index,
       *current_width,
       *max_width,
       *index,
       pattern_width;
{
   int i;

   for(i=0; i<pattern_width; i++)
      pattern[i] = i+PATTERN_START;

   *pattern_index = 0;
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
               pattern_index, current_width,
               index)
   int  *pattern, *processed_pattern;
   int  pp_index, *pattern_index, 
        current_width, *index;
{
   processed_pattern[pp_index] = 
      pattern[*pattern_index];

   *pattern_index = *pattern_index + 1;
   if(*pattern_index >= current_width)
      *pattern_index = 0;

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
   int  *array;
   int  length;
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
   *   inserting an element(s) from it.  For example,
   *   if the input pattern is abcdefg, 
   *   the output pattern could be abcdefgh.
   *
   ***********************************************/

void lengthen_pattern(size, pattern, index,
                     current_width, pattern_index, 
                     width, max_width)
   int  *pattern;
   int  size, *index, *current_width, 
        *pattern_index, *width, *max_width;
{
   int  *temp_pattern;
   int  count, i, new_index, new_width;

   temp_pattern = malloc(KONSTANT*(*width)*sizeof(int));

   for(i=0; i<(*width); i++)
      temp_pattern[i] = pattern[i];
      
   zero_line(pattern, KONSTANT*(*width));

   for(count=0, new_index=0; count<size; 
       count++, new_index++)
      pattern[new_index] = 
         count + *max_width + PATTERN_START;

   new_width = *current_width + size;

   for( ; new_index < new_width; new_index++){
      pattern[new_index] = temp_pattern[*index];
      *index = *index + 1;
      if(*index >= *current_width)
         *index = 0;
   }  /* ends loop over new_index */

   *current_width = new_width;
   *pattern_index = 0;
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
   int  *processed_pattern;
   int  width;
{
   int  substitution_values[GRAY_LEVELS+1];
   int  i, place;

   get_random_values(substitution_values);
   
   for(i=0; i<(KONSTANT*width); i++){
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
   
/* Let's try something different, only use the
   characters A-Z 
   A-Z are 65-90 */

   for(i=0; i<GRAY_LEVELS+1; i++){
       number   = rand();
       number   = number % GRAY_LEVELS+1;
       array[i] = number;
   }  /* ends loop over i */     
   
}  /* ends get_random_values */
                                 
                                 
   /***********************************************
   *
   *  test_print_line(...
   *
   ***********************************************/

void test_print_line(line, width)
   int  *line;
   int  width;
{
   int i;
   for(i=0; i<width; i++)
      printf("-%3d", line[i]);
   printf("\n");
}  /* ends test_print_line */



                                 
                                 
   /***********************************************
   *
   *  read_image_line(...
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
   *  write_image_line(...
   *
   ***********************************************/

void write_image_line(the_image, file_name, 
                      line_number, array, width)
   char  file_name[];
   short the_image[ROWS][COLS];
   int   *array, line_number, width;
{


}  /* ends write_image_line */
!@#$%^&
&^%$#@!
4
Sun Mar 19 09:47:18 1995
Finished coding read_image_line, starting to 
test. 
!@#$%^&

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
   *     make character based stereograms.
   *
   *  External Calls:
   *     tiff.c - read_tiff_header
   *     ritff.c - read_tiff_image
   *     wtiff.c - does_not_exist
   *               round_off_image_size
   *               create_allocate_tiff_file
   *
   *  Modifications:
   *     18 March 1995 - created  
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
   pattern_index usually points to the end of 
      the pattern
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
#undef DEBUG_MODE
#define COMMAND_LINE_MODE


short the_image[ROWS][COLS];

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
        *depth_line,
        i,
        j,
        index, 
        last_pixel,
        length,
        location,
        max_width,
        *pattern,
        pattern_index,
        pattern_width,
        pp_index,
        *processed_pattern,
        this_pixel,
        width;

   struct tiff_header_struct image_header;

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

   strcpy(depth_file_name, argv[2]);
   strcpy(s_file_name, argv[3]);
   strcpy(pp_file_name, argv[4]);

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
   pattern_width = atoi(argv[1]);

   pattern           = malloc(KONSTANT*width*sizeof(int));
   depth_line        = malloc(KONSTANT*width*sizeof(int));
   processed_pattern = malloc(KONSTANT*width*sizeof(int));


#endif  /* ifdef COMMAND_LINE_MODE */



#ifdef DEBUG_MODE


   strcpy(depth_file_name, "d:\src\dfile.tif");
   strcpy(s_file_name, "d:\src\sfile.tif");
   strcpy(pp_file_name, "d:\src\ppfile.tif");

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
   pattern_width = 10;

   pattern           = malloc(KONSTANT*width*sizeof(int));
   depth_line        = malloc(KONSTANT*width*sizeof(int));
   processed_pattern = malloc(KONSTANT*width*sizeof(int));


#endif  /* ifdef COMMAND_LINE_MODE */



#ifdef DEBUG_MODE






#endif /* ifdef DEBUG_MODE */

   

      /***********************************************
      *
      *   This is the major loop of the program.
      *   It reads one line at a time from the 
      *   depth file, processes that one line,
      *   and writes the resulting pattern to the
      *   processed pattern file.
      *
      ***********************************************/

   for(i=0; i<length*ROWS; i++){

      read_image_line(the_image, depth_file_name,
                      i+1, depth_line, width);
test_print_line(depth_line, width);
printf("\nHit return to continue");
gets(response);
   
      zero_line(pattern, KONSTANT*width*sizeof(int));
      zero_line(processed_pattern, KONSTANT*width*sizeof(int));

      initialize_pattern(&pattern_index, pattern, 
                         &current_width, 
                         &max_width, 
                         pattern_width,
                         &index);

         /*******************************************
         *
         *   Loop through the characters in the
         *   depth_line.  Call one of the three
         *   processing routines based on the
         *   relationship between the last pixel
         *   and this pixel.
         *
         *******************************************/
    
      last_pixel = depth_line[0];

      pp_index = 0;
      
      for(j=0; j<width; j++){
         this_pixel = depth_line[j];

         /******** ?????
         if(this_pixel == '\n')
            this_pixel = last_pixel;
         ***************/


         if(this_pixel > last_pixel)
            shorten_pattern(
               (this_pixel-last_pixel),
               pattern, &index, &current_width,
               &pattern_index, width);


         if(this_pixel < last_pixel)
            lengthen_pattern(
               (last_pixel-this_pixel), 
               pattern, &index, &current_width, 
               &pattern_index, &width, &max_width);

         

            /****************************************
            *
            *   Perform the no_change in every
            *   pass.  Do it after you have done
            *   the shorten and lenghten pattern.
            *
            ****************************************/

         no_change(pattern, processed_pattern,
                   pp_index, &pattern_index, 
                   current_width, &index); 
         pp_index++;
         
         
         if(pattern_index >= current_width)
            pattern_index = 0;
         
         last_pixel = depth_line[j];

      }  /* ends loop over j */
      

      write_image_line(the_image, pp_file_name, i+1,
                       processed_pattern, width);
      random_substitution(processed_pattern, width);
      write_image_line(the_image, s_file_name, i+1,
                       processed_pattern, width);
      
   }   /* ends the major loop */


   free(pattern);
   free(depth_line);
   free(processed_pattern);


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
                     current_width, pattern_index, 
                     width)
   int  *pattern;
   int  size, *index, *current_width, 
        *pattern_index, width;
{
   int  *temp_pattern;
   int  i, new_index, new_width;

   temp_pattern = malloc(KONSTANT*width*sizeof(int));

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

   *current_width     = new_width;
   *pattern_index     = 0;

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

void initialize_pattern(pattern_index, pattern, 
                        current_width, max_width, 
                        pattern_width, index)
   int  pattern[];
   int *pattern_index,
       *current_width,
       *max_width,
       *index,
       pattern_width;
{
   int i;

   for(i=0; i<pattern_width; i++)
      pattern[i] = i+PATTERN_START;

   *pattern_index = 0;
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
               pattern_index, current_width,
               index)
   int  *pattern, *processed_pattern;
   int  pp_index, *pattern_index, 
        current_width, *index;
{
   processed_pattern[pp_index] = 
      pattern[*pattern_index];

   *pattern_index = *pattern_index + 1;
   if(*pattern_index >= current_width)
      *pattern_index = 0;

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
   int  array[];
   int  length;
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
   *   inserting an element(s) from it.  For example,
   *   if the input pattern is abcdefg, 
   *   the output pattern could be abcdefgh.
   *
   ***********************************************/

void lengthen_pattern(size, pattern, index,
                     current_width, pattern_index, 
                     width, max_width)
   int  *pattern;
   int  size, *index, *current_width, 
        *pattern_index, *width, *max_width;
{
   int  *temp_pattern;
   int  count, i, new_index, new_width;

   temp_pattern = malloc(KONSTANT*(*width)*sizeof(int));

   for(i=0; i<(*width); i++)
      temp_pattern[i] = pattern[i];
      
   zero_line(pattern, *width);

   for(count=0, new_index=0; count<size; 
       count++, new_index++)
      pattern[new_index] = 
         count + *max_width + PATTERN_START;

   new_width = *current_width + size;

   for( ; new_index < new_width; new_index++){
      pattern[new_index] = temp_pattern[*index];
      *index = *index + 1;
      if(*index >= *current_width)
         *index = 0;
   }  /* ends loop over new_index */

   *current_width = new_width;
   *pattern_index = 0;
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
   int  *processed_pattern;
   int  width;
{
   int  substitution_values[GRAY_LEVELS+1];
   int  i, place;

   get_random_values(substitution_values);
   
   for(i=0; i<(KONSTANT*width); i++){
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
   
/* Let's try something different, only use the
   characters A-Z 
   A-Z are 65-90 */

   for(i=0; i<GRAY_LEVELS+1; i++){
       number   = rand();
       number   = number % GRAY_LEVELS+1;
       array[i] = number;
   }  /* ends loop over i */     
   
}  /* ends get_random_values */
                                 
                                 
   /***********************************************
   *
   *  test_print_line(...
   *
   ***********************************************/

void test_print_line(line, width)
   int  *line;
   int  width;
{
   int i;
   for(i=0; i<width; i++)
      printf("-%3d", line[i]);
   printf("\n");
}  /* ends test_print_line */



                                 
                                 
   /***********************************************
   *
   *  read_image_line(...
   *
   ***********************************************/

void read_image_line(the_image, file_name, 
                     line_number, array, width)
   char  file_name[];
   short the_image[ROWS][COLS];
   int   *array, line_number, width;
{
   int i, il, ie, ll, le, 
       pixels_to_read, still_reading;

   il = line_number;
   ll = il + 1;
   ie = 1; 
   le = ie + COLS;

   if(width < COLS)
      le  = ie + (width - 1);

   pixels_to_read = width;

   still_reading = 1;
   while(still_reading){

      read_tiff_image(file_name, the_image, 
                      il, ie, ll, le);
      for(i=0; i<(le-ie); i++)
         array[i] = the_image[0][i];

      pixels_to_read = pixels_to_read - (le-ie);
      if(pixels_to_read <=0)
         still_reading = 0;
      else{
         ie = ie + COLS;
         le = ie + COLS;
            if(le < width)
               le = width;
      }  /* ends else */
         
   }  /* ends while still_reading */

}  /* ends read_image_line */



                                 
                                 
   /***********************************************
   *
   *  write_image_line(...
   *
   ***********************************************/

void write_image_line(the_image, file_name, 
                      line_number, array, width)
   char  file_name[];
   short the_image[ROWS][COLS];
   int   *array, line_number, width;
{


}  /* ends write_image_line */
!@#$%^&
&^%$#@!
3
Sat Mar 18 16:46:10 1995
Almost all coding done, need to write read_image_line
and write_image_line.
!@#$%^&

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
   *     make character based stereograms.
   *
   *  External Calls:
   *     tiff.c - read_tiff_header
   *     ritff.c - read_tiff_image
   *     wtiff.c - does_not_exist
   *               round_off_image_size
   *               create_allocate_tiff_file
   *
   *  Modifications:
   *     18 March 1995 - created  
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
   pattern_index usually points to the end of 
      the pattern
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
#undef DEBUG_MODE
#define COMMAND_LINE_MODE


short the_image[ROWS][COLS];

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
        *depth_line,
        i,
        j,
        index, 
        last_pixel,
        length,
        location,
        max_width,
        *pattern,
        pattern_index,
        pattern_width,
        pp_index,
        *processed_pattern,
        this_pixel,
        width;

   struct tiff_header_struct image_header;

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

   strcpy(depth_file_name, argv[2]);
   strcpy(s_file_name, argv[3]);
   strcpy(pp_file_name, argv[4]);

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

   create_allocate_tiff_file(depth_file_name, 
                             &image_header,
                             the_image);
   create_allocate_tiff_file(s_file_name, 
                             &image_header,
                             the_image);

   width         = width*COLS;
   pattern_width = atoi(argv[1]);

   pattern           = malloc(KONSTANT*width*sizeof(int));
   depth_line        = malloc(KONSTANT*width*sizeof(int));
   processed_pattern = malloc(KONSTANT*width*sizeof(int));


#endif  /* ifdef COMMAND_LINE_MODE */

/*************** got this far on coding 18 mar 95 ************/
/*************** got this far on coding 18 mar 95 ************/
/*************** got this far on coding 18 mar 95 ************/
/*************** got this far on coding 18 mar 95 ************/


#ifdef DEBUG_MODE
   pattern_width = 10;
   width         = 60;

   pattern           = malloc(KONSTANT*width);
   depth_line        = malloc(KONSTANT*width);
   processed_pattern = malloc(KONSTANT*width);

   if((depth_file = fopen("dfile.txt", "rt")) == NULL){
      printf(
     "\ncstereo: Error opening input file %s\n", 
      "dfile.txt");
      exit(3);
   }

   if((stereo_file = fopen("sfile.txt", "wt")) == NULL){
      printf(
    "\ncstereo: Error opening output file %s\n", 
      "sfile.txt");
      exit(4);
   }


   if((processed_pattern_file = 
      fopen("ppfile.txt", "wt")) == NULL){
      printf(
    "\ncstereo: Error opening temp file %s\n", "ppfile.txt");
      exit(5);
   }




#endif /* ifdef DEBUG_MODE */

   

      /***********************************************
      *
      *   This is the major loop of the program.
      *   It reads one line at a time from the 
      *   depth file, processes that one line,
      *   and writes the resulting pattern to the
      *   processed pattern file.
      *
      ***********************************************/

   for(i=0; i<length*ROWS; i++){

      read_image_line(the_image, depth_file_name,
                      i+1, depth_line, width);
   
      zero_line(pattern, KONSTANT*width*sizeof(int));
      zero_line(processed_pattern, KONSTANT*width*sizeof(int));

      initialize_pattern(&pattern_index, pattern, 
                         &current_width, 
                         &max_width, 
                         pattern_width,
                         &index);

         /*******************************************
         *
         *   Loop through the characters in the
         *   depth_line.  Call one of the three
         *   processing routines based on the
         *   relationship between the last pixel
         *   and this pixel.
         *
         *******************************************/
    
      last_pixel = depth_line[0];

      pp_index = 0;
      
      for(j=0; j<width; j++){
         this_pixel = depth_line[j];

         /******** ?????
         if(this_pixel == '\n')
            this_pixel = last_pixel;
         ***************/


         if(this_pixel > last_pixel)
            shorten_pattern(
               (this_pixel-last_pixel),
               pattern, &index, &current_width,
               &pattern_index, width);


         if(this_pixel < last_pixel)
            lengthen_pattern(
               (last_pixel-this_pixel), 
               pattern, &index, &current_width, 
               &pattern_index, &width, &max_width);

         

            /****************************************
            *
            *   Perform the no_change in every
            *   pass.  Do it after you have done
            *   the shorten and lenghten pattern.
            *
            ****************************************/

         no_change(pattern, processed_pattern,
                   pp_index, &pattern_index, 
                   current_width, &index); 
         pp_index++;
         
         
         if(pattern_index >= current_width)
            pattern_index = 0;
         
         last_pixel = depth_line[j];

      }  /* ends loop over j */
      

      write_image_line(the_image, pp_file_name, 
                       processed_pattern, width);
      random_substitution(processed_pattern, width);
      write_image_line(the_image, s_file_name, 
                       processed_pattern, width);
      
   }   /* ends the major loop */


   free(pattern);
   free(depth_line);
   free(processed_pattern);


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
                     current_width, pattern_index, 
                     width)
   int  *pattern;
   int  size, *index, *current_width, 
        *pattern_index, width;
{
   int  *temp_pattern;
   int  i, new_index, new_width;

   temp_pattern = malloc(KONSTANT*width*sizeof(int));

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

   *current_width     = new_width;
   *pattern_index     = 0;

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

void initialize_pattern(pattern_index, pattern, 
                        current_width, max_width, 
                        pattern_width, index)
   int  pattern[];
   int *pattern_index,
       *current_width,
       *max_width,
       *index,
       pattern_width;
{
   int i;

   for(i=0; i<pattern_width; i++)
      pattern[i] = i+PATTERN_START;

   *pattern_index = 0;
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
               pattern_index, current_width,
               index)
   int  *pattern, *processed_pattern;
   int  pp_index, *pattern_index, 
        current_width, *index;
{
   processed_pattern[pp_index] = 
      pattern[*pattern_index];

   *pattern_index = *pattern_index + 1;
   if(*pattern_index >= current_width)
      *pattern_index = 0;

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
   int  array[];
   int  length;
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
   *   inserting an element(s) from it.  For example,
   *   if the input pattern is abcdefg, 
   *   the output pattern could be abcdefgh.
   *
   ***********************************************/

void lengthen_pattern(size, pattern, index,
                     current_width, pattern_index, 
                     width, max_width)
   int  *pattern;
   int  size, *index, *current_width, 
        *pattern_index, *width, *max_width;
{
   int  *temp_pattern;
   int  count, i, new_index, new_width;

   temp_pattern = malloc(KONSTANT*(*width)*sizeof(int));

   for(i=0; i<(*width); i++)
      temp_pattern[i] = pattern[i];
      
   zero_line(pattern, *width);

   for(count=0, new_index=0; count<size; 
       count++, new_index++)
      pattern[new_index] = 
         count + *max_width + PATTERN_START;

   new_width = *current_width + size;

   for( ; new_index < new_width; new_index++){
      pattern[new_index] = temp_pattern[*index];
      *index = *index + 1;
      if(*index >= *current_width)
         *index = 0;
   }  /* ends loop over new_index */

   *current_width = new_width;
   *pattern_index = 0;
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
   int  *processed_pattern;
   int  width;
{
   int  substitution_values[GRAY_LEVELS+1];
   int  i, place;

   get_random_values(substitution_values);
   
   for(i=0; i<(KONSTANT*width); i++){
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
   
/* Let's try something different, only use the
   characters A-Z 
   A-Z are 65-90 */

   for(i=0; i<GRAY_LEVELS+1; i++){
       number   = rand();
       number   = number % GRAY_LEVELS+1;
       array[i] = number;
   }  /* ends loop over i */     
   
}  /* ends get_random_values */
                                 
                                 
   /***********************************************
   *
   *  test_print_line(...
   *
   ***********************************************/

void test_print_line(line, width)
   int  *line;
   int  width;
{
   int i;
   for(i=0; i<width; i++)
      printf("-%3d", line[i]);
   printf("\n");
}  /* ends test_print_line */



                                 
                                 
   /***********************************************
   *
   *  read_image_line(...
   *
   ***********************************************/

void read_image_line(the_image, file_name, array,
                     width)
   char  file_name[];
   short the_image[ROWS][COLS];
   int   *array, width;
{


}  /* ends read_image_line */



                                 
                                 
   /***********************************************
   *
   *  write_image_line(...
   *
   ***********************************************/

void write_image_line(the_image, file_name, array,
                     width)
   char  file_name[];
   short the_image[ROWS][COLS];
   int   *array, width;
{


}  /* ends write_image_line */
!@#$%^&
&^%$#@!
2
Sat Mar 18 06:55:44 1995
First coding session done.
!@#$%^&

   /***********************************************
   *
   *  file pstereo.c
   *
   *  Functions: This file contains
   *     main
   *     fill_line
   *     initialize_pattern
   *     lengthen_pattern
   *     no_change
   *     shorten_pattern
   *     get_random_values
   *     random_substitution
   *     test_print_line
   *
   *  Purpose:
   *     This file contains a program that will
   *     make character based stereograms.
   *
   *  External Calls:
   *     tiff.c - read_tiff_header
   *     ritff.c - read_tiff_image
   *     wtiff.c - does_not_exist
   *               round_off_image_size
   *               create_allocate_tiff_file
   *
   *  Modifications:
   *     18 March 1995 - created  
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
   pattern_index usually points to the end of 
      the pattern
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
#undef DEBUG_MODE
#define COMMAND_LINE_MODE

void fill_line();
void initialize_pattern();
void lengthen_pattern();
void no_change();
void shorten_pattern();
void get_random_values();
void random_substitution();
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
        *depth_line,
        j,
        index, 
        last_pixel,
        length,
        location,
        max_width,
        *pattern,
        pattern_index,
        pattern_width,
        pp_index,
        *processed_pattern,
        this_pixel,
        width;

   struct tiff_header_struct image_header;

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

   strcpy(depth_file_name, argv[2]);
   strcpy(s_file_name, argv[3]);
   strcpy(pp_file_name, argc[4]);

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

   create_allocate_tiff_file(depth_file_name, 
                             &image_header,
                             the_image);
   create_allocate_tiff_file(s_file_name, 
                             &image_header,
                             the_image);

   width         = width*COLS;
   pattern_width = atoi(argv[1]);

   pattern           = malloc(KONSTANT*width*sizeof(int));
   depth_line        = malloc(KONSTANT*width*sizeof(int));
   processed_pattern = malloc(KONSTANT*width*sizeof(int));


#endif  /* ifdef COMMAND_LINE_MODE */

/*************** got this far on coding 18 mar 95 ************/
/*************** got this far on coding 18 mar 95 ************/
/*************** got this far on coding 18 mar 95 ************/
/*************** got this far on coding 18 mar 95 ************/


#ifdef DEBUG_MODE
   pattern_width = 10;
   width         = 60;

   pattern           = malloc(KONSTANT*width);
   depth_line        = malloc(KONSTANT*width);
   processed_pattern = malloc(KONSTANT*width);

   if((depth_file = fopen("dfile.txt", "rt")) == NULL){
      printf(
     "\ncstereo: Error opening input file %s\n", 
      "dfile.txt");
      exit(3);
   }

   if((stereo_file = fopen("sfile.txt", "wt")) == NULL){
      printf(
    "\ncstereo: Error opening output file %s\n", 
      "sfile.txt");
      exit(4);
   }


   if((processed_pattern_file = 
      fopen("ppfile.txt", "wt")) == NULL){
      printf(
    "\ncstereo: Error opening temp file %s\n", "ppfile.txt");
      exit(5);
   }




#endif /* ifdef DEBUG_MODE */

   

      /***********************************************
      *
      *   This is the major loop of the program.
      *   It reads one line at a time from the 
      *   depth file, processes that one line,
      *   and writes the resulting pattern to the
      *   processed pattern file.
      *
      ***********************************************/

   while(fgets(depth_line, KONSTANT*width, depth_file)){
   
      fill_line(pattern, KONSTANT*width);
      fill_line(processed_pattern, KONSTANT*width);

      initialize_pattern(&pattern_index, pattern, 
                         &current_width, 
                         &max_width, 
                         pattern_width,
                         &index);

         /*******************************************
         *
         *   Loop through the characters in the
         *   depth_line.  Call one of the three
         *   processing routines based on the
         *   relationship between the last character
         *   and this character.
         *
         *******************************************/
    
      last_character = depth_line[0];

      pp_index = 0;
      
      for(j=0; j<width; j++){
         this_character = depth_line[j];
         if(this_character == '\n')
            this_character = last_character;


         if(this_character > last_character)
            shorten_pattern(
               (this_character-last_character),
               pattern, &index, &current_width,
               &pattern_index, width);


         if(this_character < last_character)
            lengthen_pattern(
               (last_character-this_character), 
               pattern, &index, &current_width, 
               &pattern_index, &width, &max_width);

         

            /****************************************
            *
            *   Perform the no_change in every
            *   pass.  Do it after you have done
            *   the shorten and lenghten pattern.
            *
            ****************************************/

         no_change(pattern, processed_pattern,
                   pp_index, &pattern_index, 
                   current_width, &index); 
         pp_index++;
         
         
         if(pattern_index >= current_width)
            pattern_index = 0;
         
         last_character = depth_line[j];

      }  /* ends loop over j */
      
      pdest    = strchr(processed_pattern, '\0');
      location = pdest - processed_pattern;
      processed_pattern[location] = '\n';
      
      fputs(processed_pattern, processed_pattern_file);
      random_substitution(processed_pattern, width);
      fputs(processed_pattern, stereo_file);
      
      
   }   /* ends the major loop */

   fclose(depth_file);
   fclose(processed_pattern_file);
   fclose(stereo_file);


   free(pattern);
   free(depth_line);
   free(processed_pattern);


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
                     current_width, pattern_index, 
                     width)
   char *pattern;
   int  size, *index, *current_width, 
        *pattern_index, width;
{
   char *temp_pattern;
   int  i, new_index, new_width;

   temp_pattern = malloc(KONSTANT*width);

   for(i=0; i<width; i++)
      temp_pattern[i] = pattern[i];
   fill_line(pattern, width);

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

   *current_width     = new_width;
   *pattern_index     = 0;

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

void initialize_pattern(pattern_index, pattern, 
                        current_width, max_width, 
                        pattern_width, index)
   char pattern[];
   int *pattern_index,
       *current_width,
       *max_width,
       *index,
       pattern_width;
{
   int i;

   for(i=0; i<pattern_width; i++)
      pattern[i] = i+PATTERN_START;

   *pattern_index = 0;
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
               pattern_index, current_width,
               index)
   char *pattern, *processed_pattern;
   int  pp_index, *pattern_index, 
        current_width, *index;
{
   processed_pattern[pp_index] = 
      pattern[*pattern_index];

   *pattern_index = *pattern_index + 1;
   if(*pattern_index >= current_width)
      *pattern_index = 0;

   *index = *index + 1;
   if(*index >= current_width)
      *index = 0;

}   /* ends no_change */





   /***********************************************
   *
   *   fill_line(...
   *
   *   This function fills a character line
   *   with NULL characters.
   *
   ***********************************************/

void fill_line(line, length)
   char *line;
   int  length;
{
   int i;
   for(i=0; i<length; i++)
      line[i] = '\0';
}  /* ends fill_line */



   /***********************************************
   *
   *   lengthen_pattern(...
   *
   *   This funtion lengthens the pattern by
   *   inserting an element(s) from it.  For example,
   *   if the input pattern is abcdefg, 
   *   the output pattern could be abcdefgh.
   *
   ***********************************************/

void lengthen_pattern(size, pattern, index,
                     current_width, pattern_index, 
                     width, max_width)
   char *pattern;
   int  size, *index, *current_width, 
        *pattern_index, *width, *max_width;
{
   char *temp_pattern;
   int  count, i, new_index, new_width;

   temp_pattern = malloc(KONSTANT*(*width));

   for(i=0; i<(*width); i++)
      temp_pattern[i] = pattern[i];
      
   fill_line(pattern, *width);

   for(count=0, new_index=0; count<size; 
       count++, new_index++)
      pattern[new_index] = 
         count + *max_width + PATTERN_START;

   new_width = *current_width + size;

   for( ; new_index < new_width; new_index++){
      pattern[new_index] = temp_pattern[*index];
      *index = *index + 1;
      if(*index >= *current_width)
         *index = 0;
   }  /* ends loop over new_index */

   *current_width = new_width;
   *pattern_index = 0;
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
   char *processed_pattern;
   int  width;
{
   char substitution_values[ASCII_SIZE];
   int  i, place;

   get_random_values(substitution_values);
   
   for(i=0; i<(KONSTANT*width); i++){
      if(processed_pattern[i] != '\n'   &&
         processed_pattern[i] != '\0'){
         place = processed_pattern[i];
         processed_pattern[i] = 
            substitution_values[place];
      }  /* ends if */         
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
   char array[];
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
   
/* Let's try something different, only use the
   characters A-Z 
   A-Z are 65-90 */

   for(i=0; i<ASCII_SIZE; i++){
       number   = rand();
       number   = number % 26;
       number   = number + 65;
       array[i] = number;
   }  /* ends loop over i */     
   
}  /* ends get_random_values */
                                 
                                 
   /***********************************************
   *
   *  test_print_line(...
   *
   ***********************************************/

void test_print_line(line, width)
   char *line;
   int  width;
{
   int i;
   for(i=0; i<width; i++)
      printf("%c", line[i]);
   printf("\n");
}  /* ends test_print_line */
!@#$%^&
&^%$#@!
1
Sat Mar 18 06:22:51 1995
This is the starting point.
!@#$%^&

   /***********************************************
   *
   *  file cstereo.c
   *
   *  Functions: This file contains
   *     main
   *     fill_line
   *     initialize_pattern
   *     lengthen_pattern
   *     no_change
   *     shorten_pattern
   *     get_random_values
   *     random_substitution
   *     test_print_line
   *
   *  Purpose:
   *     This file contains a program that will
   *     make character based stereograms.
   *
   *  External Calls:
   *     none
   *
   *  Modifications:
   *     7 February 1995 - created
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
   pattern_index usually points to the end of 
      the pattern
   index points to the current place in the pattern
      we are working on
   pp_index points to the current place in the processed
      pattern,  Cannot use the depth_line counter j
      for this because you will skip over places in
      the processed pattern.

*********/

#include "cips.h"

#define PPFILENAME   "ppfile.txt"
#define PATTERN_START  48 /* the zero character    */
#define PATTERN_END   122 /* the small z character */
#define KONSTANT        2
#define ASCII_SIZE    256

#define TEST_NO_CHANGE
#undef DEBUG_MODE
#define COMMAND_LINE_MODE

void fill_line();
void initialize_pattern();
void lengthen_pattern();
void no_change();
void shorten_pattern();
void get_random_values();
void random_substitution();
void test_print_line();


main(argc, argv)
   char *argv[];
   int  argc;
{
   char *depth_line,
        *pattern, 
        *processed_pattern,
        *pdest,
        response[80];

   char last_character,
        this_character;

   FILE *depth_file,
        *processed_pattern_file,
        *stereo_file;

   int  current_width,
        j,
        index, 
        location,
        max_width,
        pattern_index,
        pattern_width,
        pp_index,
        width;
int line = 0;

#ifdef COMMAND_LINE_MODE
   if(argc != 6){
      printf(
      "\nusage: cstereo pattern-width width-of-line"
      "\n               depth-file-name "
      "stereo-file-name "
      "\n               processed-pattern-file-name"
      "\n");
      exit(1);
   }

   pattern_width = atoi(argv[1]);
   width         = atoi(argv[2]);

   pattern           = malloc(KONSTANT*width);
   depth_line        = malloc(KONSTANT*width);
   processed_pattern = malloc(KONSTANT*width);

   if((depth_file = fopen(argv[3], "rt")) == NULL){
      printf(
     "\ncstereo: Error opening input file %s\n", 
      argv[3]);
      exit(3);
   }

   if((stereo_file = fopen(argv[4], "wt")) == NULL){
      printf(
    "\ncstereo: Error opening output file %s\n", 
      argv[4]);
      exit(4);
   }

   if((processed_pattern_file = 
      fopen(argv[5], "wt")) == NULL){
      printf(
    "\ncstereo: Error opening temp file %s\n", argv[5]);
      exit(5);
   }
#endif  /* ifdef COMMAND_LINE_MODE */

#ifdef DEBUG_MODE
   pattern_width = 10;
   width         = 60;

   pattern           = malloc(KONSTANT*width);
   depth_line        = malloc(KONSTANT*width);
   processed_pattern = malloc(KONSTANT*width);

   if((depth_file = fopen("dfile.txt", "rt")) == NULL){
      printf(
     "\ncstereo: Error opening input file %s\n", 
      "dfile.txt");
      exit(3);
   }

   if((stereo_file = fopen("sfile.txt", "wt")) == NULL){
      printf(
    "\ncstereo: Error opening output file %s\n", 
      "sfile.txt");
      exit(4);
   }


   if((processed_pattern_file = 
      fopen("ppfile.txt", "wt")) == NULL){
      printf(
    "\ncstereo: Error opening temp file %s\n", "ppfile.txt");
      exit(5);
   }




#endif /* ifdef DEBUG_MODE */

   

      /***********************************************
      *
      *   This is the major loop of the program.
      *   It reads one line at a time from the 
      *   depth file, processes that one line,
      *   and writes the resulting pattern to the
      *   processed pattern file.
      *
      ***********************************************/

   while(fgets(depth_line, KONSTANT*width, depth_file)){
   
      fill_line(pattern, KONSTANT*width);
      fill_line(processed_pattern, KONSTANT*width);

      initialize_pattern(&pattern_index, pattern, 
                         &current_width, 
                         &max_width, 
                         pattern_width,
                         &index);

         /*******************************************
         *
         *   Loop through the characters in the
         *   depth_line.  Call one of the three
         *   processing routines based on the
         *   relationship between the last character
         *   and this character.
         *
         *******************************************/
    
      last_character = depth_line[0];

      pp_index = 0;
      
      for(j=0; j<width; j++){
         this_character = depth_line[j];
         if(this_character == '\n')
            this_character = last_character;


         if(this_character > last_character)
            shorten_pattern(
               (this_character-last_character),
               pattern, &index, &current_width,
               &pattern_index, width);


         if(this_character < last_character)
            lengthen_pattern(
               (last_character-this_character), 
               pattern, &index, &current_width, 
               &pattern_index, &width, &max_width);

         

            /****************************************
            *
            *   Perform the no_change in every
            *   pass.  Do it after you have done
            *   the shorten and lenghten pattern.
            *
            ****************************************/

         no_change(pattern, processed_pattern,
                   pp_index, &pattern_index, 
                   current_width, &index); 
         pp_index++;
         
         
         if(pattern_index >= current_width)
            pattern_index = 0;
         
         last_character = depth_line[j];

      }  /* ends loop over j */
      
      pdest    = strchr(processed_pattern, '\0');
      location = pdest - processed_pattern;
      processed_pattern[location] = '\n';
      
      fputs(processed_pattern, processed_pattern_file);
      random_substitution(processed_pattern, width);
      fputs(processed_pattern, stereo_file);
      
      
   }   /* ends the major loop */

   fclose(depth_file);
   fclose(processed_pattern_file);
   fclose(stereo_file);


   free(pattern);
   free(depth_line);
   free(processed_pattern);


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
                     current_width, pattern_index, 
                     width)
   char *pattern;
   int  size, *index, *current_width, 
        *pattern_index, width;
{
   char *temp_pattern;
   int  i, new_index, new_width;

   temp_pattern = malloc(KONSTANT*width);

   for(i=0; i<width; i++)
      temp_pattern[i] = pattern[i];
   fill_line(pattern, width);

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

   *current_width     = new_width;
   *pattern_index     = 0;

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

void initialize_pattern(pattern_index, pattern, 
                        current_width, max_width, 
                        pattern_width, index)
   char pattern[];
   int *pattern_index,
       *current_width,
       *max_width,
       *index,
       pattern_width;
{
   int i;

   for(i=0; i<pattern_width; i++)
      pattern[i] = i+PATTERN_START;

   *pattern_index = 0;
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
               pattern_index, current_width,
               index)
   char *pattern, *processed_pattern;
   int  pp_index, *pattern_index, 
        current_width, *index;
{
   processed_pattern[pp_index] = 
      pattern[*pattern_index];

   *pattern_index = *pattern_index + 1;
   if(*pattern_index >= current_width)
      *pattern_index = 0;

   *index = *index + 1;
   if(*index >= current_width)
      *index = 0;

}   /* ends no_change */





   /***********************************************
   *
   *   fill_line(...
   *
   *   This function fills a character line
   *   with NULL characters.
   *
   ***********************************************/

void fill_line(line, length)
   char *line;
   int  length;
{
   int i;
   for(i=0; i<length; i++)
      line[i] = '\0';
}  /* ends fill_line */



   /***********************************************
   *
   *   lengthen_pattern(...
   *
   *   This funtion lengthens the pattern by
   *   inserting an element(s) from it.  For example,
   *   if the input pattern is abcdefg, 
   *   the output pattern could be abcdefgh.
   *
   ***********************************************/

void lengthen_pattern(size, pattern, index,
                     current_width, pattern_index, 
                     width, max_width)
   char *pattern;
   int  size, *index, *current_width, 
        *pattern_index, *width, *max_width;
{
   char *temp_pattern;
   int  count, i, new_index, new_width;

   temp_pattern = malloc(KONSTANT*(*width));

   for(i=0; i<(*width); i++)
      temp_pattern[i] = pattern[i];
      
   fill_line(pattern, *width);

   for(count=0, new_index=0; count<size; 
       count++, new_index++)
      pattern[new_index] = 
         count + *max_width + PATTERN_START;

   new_width = *current_width + size;

   for( ; new_index < new_width; new_index++){
      pattern[new_index] = temp_pattern[*index];
      *index = *index + 1;
      if(*index >= *current_width)
         *index = 0;
   }  /* ends loop over new_index */

   *current_width = new_width;
   *pattern_index = 0;
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
   char *processed_pattern;
   int  width;
{
   char substitution_values[ASCII_SIZE];
   int  i, place;

   get_random_values(substitution_values);
   
   for(i=0; i<(KONSTANT*width); i++){
      if(processed_pattern[i] != '\n'   &&
         processed_pattern[i] != '\0'){
         place = processed_pattern[i];
         processed_pattern[i] = 
            substitution_values[place];
      }  /* ends if */         
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
   char array[];
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
   
/* Let's try something different, only use the
   characters A-Z 
   A-Z are 65-90 */

   for(i=0; i<ASCII_SIZE; i++){
       number   = rand();
       number   = number % 26;
       number   = number + 65;
       array[i] = number;
   }  /* ends loop over i */     
   
}  /* ends get_random_values */
                                 
                                 
   /***********************************************
   *
   *  test_print_line(...
   *
   ***********************************************/

void test_print_line(line, width)
   char *line;
   int  width;
{
   int i;
   for(i=0; i<width; i++)
      printf("%c", line[i]);
   printf("\n");
}  /* ends test_print_line */
!@#$%^&
