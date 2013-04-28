
   /***********************************************
   *
   *  file spstereo.c
   *
   *  Functions: This file contains
   *     main
   *     zero_line
   *     initialize_pattern
   *     s_lengthen_pattern
   *     no_change
   *     shorten_pattern
   *     special_substitution
   *     read_image_line
   *     write_image_line
   *     test_print_line
   *     equate_headers
   *
   *  Purpose:
   *     This file contains a program that will
   *     make pixel based colorfield stereograms.
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
   *     29 April 1995 - created  
   *     29 August 1998 - modified to work on 
   *           entire images at once.
   *     24 September 1998 - modified to work with 
   *           all I O routines in imageio.c.
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
   depth_image[] will be width wide
   processed_pattern will be width wide
   index points to the current place in the pattern
      we are working on
   pp_index points to the current place in the processed
      pattern,  Cannot use the depth_image counter j
      for this because you will skip over places in
      the processed pattern.

*********/

#include "cips.h"

#define KONSTANT        2
#define PATTERN_START   0
#define PATTERN_END   255
#define ASCII_SIZE    256

#undef  DEBUG_MODE
#define COMMAND_LINE_MODE


short   **depth_image,
        **pattern,
        **processed_pattern,
        **special_pixels;

void zero_line();
void initialize_pattern();
void s_lengthen_pattern();
void no_change();
void shorten_pattern();
void special_substitution();
void read_image_line();
void write_image_line();
void equate_headers();
void test_print_line();


main(argc, argv)
   char *argv[];
   int  argc;
{
   char depth_file_name[MAX_NAME_LENGTH],
        s_file_name[MAX_NAME_LENGTH],
        pattern_file_name[MAX_NAME_LENGTH],
        pp_file_name[MAX_NAME_LENGTH],
        response[MAX_NAME_LENGTH];

   FILE *depth_file,
        *pattern_file,
        *processed_pattern_file,
        *stereo_file;

   int  current_width,
        i,
        j,
        index, 
        last_pixel,
        length_d,
        length_p,
        location,
        max_width,
        pattern_width,
        pp_index,
        s_length,
        this_pixel,
        width_d,
        width_p;

   long length, width;


   if(argc != 6){
      printf(
      "\nusage: spstereo pattern-file-name "
      "depth-file-name "
      "\n                stereo-file-name "
      "processed-pattern-file-name"
      "\n                length-of-pattern"
      "\n");
      exit(1);
   }

   strcpy(pattern_file_name, argv[1]);
   strcpy(depth_file_name,   argv[2]);
   strcpy(s_file_name,       argv[3]);
   strcpy(pp_file_name,      argv[4]);
   s_length           = atoi(argv[5]);

      /*********************************************
      *
      *   Ensure the input files exist.
      *
      **********************************************/

   if(does_not_exist(pattern_file_name)){
      printf("\nThe pattern file %s does not exist",
             pattern_file_name);
      exit(1);
   }

   if(does_not_exist(depth_file_name)){
      printf("\nThe depth file %s does not exist",
             depth_file_name);
      exit(1);
   }

      /*********************************************
      *
      *   The input files must be the same size.
      *
      **********************************************/
      
   if(are_not_same_size(depth_file_name, 
                        pattern_file_name)){
      printf(
       "\n Images %s and %s are not the same size", 
       depth_file_name, pattern_file_name);
      exit(1);
   }  /* ends if sizes not the same */

   get_image_size(depth_file_name, &length, &width);

   depth_image       = allocate_image_array(length, width);
   pattern           = allocate_image_array(length, width);
   processed_pattern = allocate_image_array(length, width);
   special_pixels    = allocate_image_array(length, width);

   create_image_file(depth_file_name, s_file_name);
   create_image_file(depth_file_name, pp_file_name);

   read_image_array(depth_file_name, depth_image);
   read_image_array(pattern_file_name, special_pixels);

   width         = width;
   pattern_width = s_length;
   length        = length;


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
                         s_length,
                         &index);

         /*******************************************
         *
         *   Loop through the pixels in the
         *   depth_image.  Call one of the three
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
            s_lengthen_pattern(
               (last_pixel-this_pixel), 
               pattern[i], &index, &current_width, 
               &width, &max_width, s_length);

         

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

   for(i=0; i<length; i++)
      special_substitution(processed_pattern[i], 
                           special_pixels[i],
                           width);

   write_image_array(s_file_name, processed_pattern);

   free_image_array(depth_image, length);
   free_image_array(pattern, length);
   free_image_array(processed_pattern, length);
   free_image_array(special_pixels, length);

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
   short  *temp_pattern;
   int    i, new_index, new_width;


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
         *index;
   long  pattern_width;
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
   short *array;
   long  length;
{
   int i;
   for(i=0; i<length; i++)
      array[i] = 0;
}  /* ends zero_line */





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




   /***********************************************
   *
   *   s_lengthen_pattern(...
   *
   *   This funtion lengthens the pattern by
   *   inserting an element(s) into it.  
   *   This is the special length pattern routine.
   *   It inserts only from the original pattern
   *   (the special-pixels).
   *
   ***********************************************/

void s_lengthen_pattern(size, pattern, index,
                        current_width, width, 
                        max_width, s_length)
   int   size, *index, *current_width, 
         *width, *max_width, s_length;
   short *pattern;
{
   short  *temp_pattern;
   int    count, element, i, new_index, new_width;

   temp_pattern = malloc((*width)*sizeof(short));

   for(i=0; i<(*width); i++)
      temp_pattern[i] = pattern[i];

      /******************************************
      *
      *   element is the value of the current
      *   element.  new_index points to its 
      *   position in the special_text and moves
      *   back to the left by size.
      *
      ******************************************/

   element   = pattern[*index] - PATTERN_START;
   new_index = (element - size) % s_length;

      /******************************************
      *
      *   Put a new pattern in the pattern array
      *   starting back at the new_index.
      *
      ******************************************/

   for(i=0; i<size; i++){
      pattern[i] = new_index + PATTERN_START;
      new_index++;
      if(new_index == s_length)
         new_index = 0;
   }  /* ends loop over i */

   new_width = size + *current_width;

   for(i=size; i<new_width; i++){
      pattern[i] = temp_pattern[*index];
      *index = *index + 1;
      if(*index >= *current_width)
         *index = 0;
   }  /* ends loop over i, count */

   *current_width = new_width;
   *index         = 0;

   free(temp_pattern);

}  /* ends s_lengthen_pattern */





   /***********************************************
   *
   *   special_substitution(...
   *
   *   This function takes the processed_pattern
   *   array and substitutes the special pixels
   *   into it.
   *
   ***********************************************/

void special_substitution(processed_pattern, 
                          special_pixels, width)
   short *processed_pattern, *special_pixels; 
   long  width;
{
   int  i, place;

   for(i=0; i<width; i++){
      place = processed_pattern[i] - PATTERN_START;
      processed_pattern[i] = 
         special_pixels[place];
   }  /* ends loop over i */
   
}  /* ends special_substitution */
                                 
   
