
   /***********************************************
   *
   *  file scstereo.c
   *
   *  Functions: This file contains
   *     main
   *     fill_line
   *     initialize_pattern
   *     s_lengthen_pattern
   *     no_change
   *     shorten_pattern
   *     special_substitution
   *     test_print_line
   *
   *  Purpose:
   *     This file contains a program that will
   *     make character based repeating pattern 
   *     stereograms.
   *
   *  External Calls:
   *     none
   *
   *  Modifications:
   *     12 March 1995 - created
   *
   *************************************************/

#include "cips.h"

#define PATTERN_START  48 /* the zero character    */
#define PATTERN_END   122 /* the small z character */
#define KONSTANT        2
#define ASCII_SIZE    256
#define LENGTH         80

void fill_line();
void initialize_pattern();
void s_lengthen_pattern();
void no_change();
void shorten_pattern();
void special_substitution();
void test_print_line();


main(argc, argv)
   char *argv[];
   int  argc;
{
   char *depth_line,
        *pattern, 
        *processed_pattern,
        *pdest,
        response[LENGTH],
        special_text[LENGTH];

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
        pattern_width,
        pp_index,
        s_length,
        width;


   if(argc != 6){
     printf(
     "\nusage: scstereo width-of-line depth-file-name"
     "\n                stereo-file-name "
     "processed-pattern-file-name"
     "\n                special-text\n");
     exit(1);
   }

   width = atoi(argv[1]);
   strcpy(special_text, argv[5]);
   s_length = strlen(special_text);

   pattern           = malloc(KONSTANT*width);
   depth_line        = malloc(KONSTANT*width);
   processed_pattern = malloc(KONSTANT*width);

   if((depth_file = fopen(argv[2], "rt")) == NULL){
      printf(
     "\nscstereo: Error opening input file %s\n", 
      argv[3]);
      exit(3);
   }

   if((stereo_file = fopen(argv[3], "wt")) == NULL){
      printf(
    "\nscstereo: Error opening output file %s\n", 
      argv[4]);
      exit(4);
   }

   if((processed_pattern_file = 
      fopen(argv[4], "wt")) == NULL){
      printf(
    "\nscstereo: Error opening temp file %s\n", 
         argv[5]);
      exit(5);
   }

      /***********************************************
      *
      *   This is the major loop of the program.
      *   It reads one line at a time from the 
      *   depth file, processes that one line,
      *   and writes the resulting pattern to the
      *   processed pattern file.
      *
      ***********************************************/

   while(fgets(depth_line, 
               KONSTANT*width, 
               depth_file)){
   
      fill_line(pattern, KONSTANT*width);
      fill_line(processed_pattern, KONSTANT*width);

      initialize_pattern(pattern, 
                         &current_width, 
                         &max_width, 
                         s_length,
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
               width);


         if(this_character < last_character)
            s_lengthen_pattern(
               (last_character-this_character), 
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
         
         last_character = depth_line[j];

      }  /* ends loop over j */
      
      pdest    = strchr(processed_pattern, '\0');
      location = pdest - processed_pattern;
      processed_pattern[location] = '\n';
      
      fputs(processed_pattern,processed_pattern_file);
      special_substitution(processed_pattern, 
                           special_text, width);
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
                     current_width, width)
   char *pattern;
   int  size, *index, *current_width, width;
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
   *index     = 0;

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
   char pattern[];
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
   char *pattern, *processed_pattern;
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
   *   s_lengthen_pattern(...
   *
   *   This funtion lengthens the pattern by
   *   inserting an element(s) into it.  
   *   This is the special lengthen pattern routine.
   *   It inserts only from the original pattern
   *   (the special-text).
   *
   ***********************************************/

void s_lengthen_pattern(size, pattern, index,
                        current_width, width, 
                        max_width, s_length)
   char *pattern;
   int  size, *index, *current_width, 
        *width, *max_width, s_length;
{
   char *temp_pattern;
   int  count, element, i, new_index, new_width;

   temp_pattern = malloc(KONSTANT*(*width));

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

}  /* ends s_lengthen_pattern */





   /***********************************************
   *
   *   special_substitution(...
   *
   *   This function takes the processed_pattern
   *   array and substitutes the special text
   *   into it.
   *
   ***********************************************/

void special_substitution(processed_pattern, 
                          special_text, width)
   char *processed_pattern, special_text[];
   int  width;
{
   int  i, place;

   for(i=0; i<(KONSTANT*width); i++){
      if(processed_pattern[i] != '\n'   &&
         processed_pattern[i] != '\0'){
         place = processed_pattern[i] - PATTERN_START;
         processed_pattern[i] = 
            special_text[place];
      }  /* ends if */         
   }  /* ends loop over i */
   
}  /* ends special_substitution */





   /***********************************************
   *
   *   test_print_line(...
   *
   *   This is a debug routine that prints an
   *   array of characters.
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
