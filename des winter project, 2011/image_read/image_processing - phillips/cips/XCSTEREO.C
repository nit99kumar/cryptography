
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


#include "cips.h"

#define PATTERN_START  48 /* the zero character    */
#define PATTERN_END   122 /* the small z character */
#define KONSTANT        2
#define ASCII_SIZE    256


void shorten_pattern();
void lengthen_pattern();
void no_change();
void initialize_pattern();
void get_random_values();
void random_substitution();
void fill_line();


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
        pattern_width,
        pp_index,
        width;

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

      initialize_pattern(pattern, 
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
               width);


         if(this_character < last_character)
            lengthen_pattern(
               (last_character-this_character), 
               pattern, &index, &current_width, 
               &width, &max_width);

         

            /****************************************
            *
            *   Perform the no_change in every
            *   pass.  
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

   *current_width = new_width;
   *index         = 0;

   free(temp_pattern);

}  /* ends shorten_pattern */





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
   char *pattern;
   int  size, *index, *current_width, 
        *width, *max_width;
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
   *index         = 0;
   *max_width     = *max_width + size;

   free(temp_pattern);

}  /* ends lengthen_pattern */





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
   *   fill_line(...
   *
   *   This function fills a character array
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

#ifdef NOTTHISTIME
/* these lines used all the printable
   characters from 0 through small z   */
   for(i=0; i<ASCII_SIZE; i++){
       number   = rand();
       number   = number % (PATTERN_END - PATTERN_START);
       number   = number + PATTERN_START;
       array[i] = number;
   }  /* ends loop over i */     
#endif

#ifdef NOTTHISTIME
/* These lines only use the
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
   
/* These lines only use the
   characters A-Z 
   A-Z are 65-90 */

   for(i=0; i<ASCII_SIZE; i++){
       number   = rand();
       number   = number % 26;
       number   = number + 65;
       array[i] = number;
   }  /* ends loop over i */     
   
}  /* ends get_random_values */
