
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
   *     make pixel based repeating pattern 
   *     stereograms.
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
   *     29 April 1995 - created  
   *
   *************************************************/


#include "cips.h"

#define KONSTANT        2
#define PATTERN_START   0
#define PATTERN_END   255
#define ASCII_SIZE    256

short the_image[ROWS][COLS];


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
        *depth_line,
        i,
        j,
        index, 
        last_pixel,
        length,
        length_d,
        length_p,
        line = 0,
        location,
        max_width,
        *pattern,
        pattern_width,
        pp_index,
        *processed_pattern,
        s_length,
        *special_pixels,
        this_pixel,
        width,
        width_d,
        width_p;

   struct tiff_header_struct depth_header,
                             image_header,
                             pattern_header;


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
   strcpy(depth_file_name, argv[2]);
   strcpy(s_file_name, argv[3]);
   strcpy(pp_file_name, argv[4]);
   s_length = atoi(argv[5]);

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
      *   The processing will be based on the 
      *   smallest length and width of the two
      *   input files.
      *
      **********************************************/

   read_tiff_header(depth_file_name, &depth_header);
   round_off_image_size(&depth_header, 
                        &length_d, &width_d);
   
   depth_header.image_length = length_d*ROWS;
   depth_header.image_width  = width_d*COLS;

   read_tiff_header(pattern_file_name,&pattern_header);
   round_off_image_size(&pattern_header, 
                        &length_p, &width_p);
   
   pattern_header.image_length = length_p*ROWS;
   pattern_header.image_width  = width_p*COLS;

   equate_headers(&depth_header, &image_header);

   if(image_header.image_length >
      pattern_header.image_length)
      image_header.image_length = 
         pattern_header.image_length;

   if(image_header.image_width >
      pattern_header.image_width)
      image_header.image_width = 
         pattern_header.image_width;

   create_allocate_tiff_file(pp_file_name, 
                             &image_header,
                             the_image);
   create_allocate_tiff_file(s_file_name, 
                             &image_header,
                             the_image);

   width         = image_header.image_width;
   pattern_width = s_length;
   length        = image_header.image_length/ROWS;

   pattern        = malloc(KONSTANT*width*sizeof(int));
   depth_line     = malloc(KONSTANT*width*sizeof(int));
   special_pixels = malloc(KONSTANT*width*sizeof(int));
   processed_pattern = 
                    malloc(KONSTANT*width*sizeof(int));


      /***********************************************
      *
      *   This is the major loop of the program.
      *   It reads one line at a time from the 
      *   depth file, processes that one line,
      *   and writes the resulting pattern to the
      *   processed pattern file.
      *
      ***********************************************/

   printf("\nspstereo>> %d rows", length*ROWS);
  
   for(i=0; i<length*ROWS; i++){

      if(i%10 == 0) printf(" %d", i);

      read_image_line(the_image, depth_file_name,
                      i+1, depth_line, width);
      
      read_image_line(the_image, pattern_file_name,
                      i+1, special_pixels, width);

      zero_line(pattern, KONSTANT*width);
      zero_line(processed_pattern, KONSTANT*width);

      initialize_pattern(pattern, 
                         &current_width, 
                         &max_width, 
                         s_length,
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
            s_lengthen_pattern(
               (last_pixel-this_pixel), 
               pattern, &index, &current_width, 
               &width, &max_width, s_length);

         

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
      special_substitution(processed_pattern, 
                           special_pixels, 
                           width);
      write_image_line(the_image, s_file_name, i+1,
                       processed_pattern, width);

   }   /* ends the major loop */


   free(pattern);
   free(depth_line);
   free(processed_pattern);
   free(special_pixels);


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

      write_array_into_tiff_image(file_name, 
                                  the_image,
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
                                




   /***********************************************
   *
   *   equate_headers(...
   *
   *   This function sets the b image header
   *   struct equal to all elements of the a
   *   image header.
   *
   ***********************************************/

void equate_headers(a, b)
   struct tiff_header_struct *a, *b;
{

   b->lsb            = a->lsb;
   b->bits_per_pixel = a->bits_per_pixel;
   b->image_length   = a->image_length;
   b->image_width    = a->image_width;
   b->strip_offset   = a->strip_offset;

}  /* ends equate_headers */





   /***********************************************
   *
   *   s_lengthen_pattern(...
   *
   *   This funtion lengthens the pattern by
   *   inserting an element(s) into it.  
   *   This is the special lengthen pattern routine.
   *   It inserts only from the original pattern
   *   (the special-pixels).
   *
   ***********************************************/

void s_lengthen_pattern(size, pattern, index,
                        current_width, width, 
                        max_width, s_length)
   int  size, *index, *current_width, 
        *pattern, *width, *max_width, s_length;
{
   int  *temp_pattern;
   int  count, element, i, new_index, new_width;

   temp_pattern = 
      malloc(KONSTANT*(*width)*sizeof(int));

   for(i=0; i<(*width); i++)
      temp_pattern[i] = pattern[i];

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
   int *processed_pattern, *special_pixels, width;
{
   int  i, place;

   for(i=0; i<(KONSTANT*width); i++){
      place = processed_pattern[i] - PATTERN_START;
      processed_pattern[i] = 
         special_pixels[place];
   }  /* ends loop over i */
   
}  /* ends special_substitution */
