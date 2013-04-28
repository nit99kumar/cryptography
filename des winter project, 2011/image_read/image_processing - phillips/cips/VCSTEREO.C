&^%$#@!
14
Wed Apr 05 05:51:03 1995
This has the redundant pattern_index removed and the
code works.
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
   index points to the current place in the pattern
      we are working on
   pp_index points to the current place in the processed
      pattern,  Cannot use the depth_line counter j
      for this because you will skip over places in
      the processed pattern.

*********/

#include "cips.h"

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
        pattern_width,
        pp_index,
        width;

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
   *   lengthen_pattern(...
   *
   *   This funtion lengthens the pattern by
   *   inserting an element(s) from it.  For example,
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
   *   This is a debug function that prints
   *   an array of characters.
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
13
Wed Apr 05 05:34:53 1995
This one was a quick modification to equate index
and pattern_index.
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
12
Sat Mar 11 19:08:45 1995
More final cleanup.  I made the 
processed pattern file name a required
command line parameter vice an optional one.
Deleted the open_files() routine.
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
&^%$#@!
11
Fri Mar 10 06:04:02 1995
Cleaning out the code. 
!@#$%^&

   /***********************************************
   *
   *  file d:\cips\cips.c
   *
   *  Functions: This file contains
   *     main
   *     show_menu
   *     show_image
   *
   *  Purpose:
   *     This file contains the main calling
   *     routine in the C Image Processing System.
   *
   *  External Calls:
   *     none
   *
   *  Modifications:
   *     6 January 1995 - created
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

#undef TEST_INIT_PATTERN
#define TEST_NO_CHANGE
#undef DEBUG_MODE
#define COMMAND_LINE_MODE

void fill_line();
void initialize_pattern();
void lengthen_pattern();
void no_change();
void open_files();
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
   if(argc < 5  ||  argc > 6){
      printf(
      "\nusage: cstereo pattern-width width-of-line"
      "\n               depth-file-name "
      "stereo-file-name "
      "\n               [processed-pattern-file-name]"
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

   if(argc == 6){
      if((processed_pattern_file = 
         fopen(argv[5], "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", argv[5]);
         exit(5);
      }
   }  /* ends if argc == 6 */
   else{  /* else temp file name not given 
             so use the default name */
      if((processed_pattern_file = 
         fopen(PPFILENAME, "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", 
         PPFILENAME);
         exit(5);
      }
   }  /* ends else */
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
   *   open_files(...
   *
   *   This function opens the depth file 
   *   for reading and creates
   *   the processed pattern file and the
   *   stereo file.
   *
   ***********************************************/

void open_files(depth_file, stereo_file, 
                processed_pattern_file, argc, argv)
   char *argv[];
   FILE *depth_file,
        *stereo_file,
        *processed_pattern_file;
   int  argc;
{

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

   if(argc == 6){
      if((processed_pattern_file = 
         fopen(argv[5], "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", argv[5]);
         exit(5);
      }
   }  /* ends if argc == 6 */
   else{  /* else temp file name not given 
             so use the default name */
      if((processed_pattern_file = 
         fopen(PPFILENAME, "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", 
         PPFILENAME);
         exit(5);
      }
   }  /* ends else */

}  /* ends open_files */



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
10
Mon Mar 06 05:50:02 1995
This version has the random value substitution.
It works.
!@#$%^&

   /***********************************************
   *
   *  file d:\cips\cips.c
   *
   *  Functions: This file contains
   *     main
   *     show_menu
   *     show_image
   *
   *  Purpose:
   *     This file contains the main calling
   *     routine in the C Image Processing System.
   *
   *  External Calls:
   *     none
   *
   *  Modifications:
   *     6 January 1995 - created
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

#undef TEST_INIT_PATTERN
#define TEST_NO_CHANGE
#undef DEBUG_MODE
#define COMMAND_LINE_MODE

void fill_line();
void initialize_pattern();
void lengthen_pattern();
void no_change();
void open_files();
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
   if(argc < 5  ||  argc > 6){
      printf(
      "\nusage: cstereo pattern-width width-of-line"
      "\n               depth-file-name "
      "stereo-file-name "
      "\n               [processed-pattern-file-name]"
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

   if(argc == 6){
      if((processed_pattern_file = 
         fopen(argv[5], "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", argv[5]);
         exit(5);
      }
   }  /* ends if argc == 6 */
   else{  /* else temp file name not given 
             so use the default name */
      if((processed_pattern_file = 
         fopen(PPFILENAME, "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", 
         PPFILENAME);
         exit(5);
      }
   }  /* ends else */
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

   

#ifdef NEVER
printf("\ndepth_file %x stfile %x ppfile %x",
depth_file, stereo_file, processed_pattern_file);
fgets(depth_line, width, depth_file);
printf("\n**%s**",depth_line);
printf("Hit Enter to continue");
gets(response);
#endif

      
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
   
#ifdef NEVER   
line++;
printf("\n--%s--",depth_line);
printf("\nLine is %d long", strlen(depth_line));
printf("\nHit Enter to continue");
gets(response);         
#endif

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
    
      /*** ?????? 
      index = 0; ***/
      last_character = depth_line[0];

      pp_index = 0;
      
      for(j=0; j<width; j++){
         this_character = depth_line[j];
         if(this_character == '\n')
            this_character = last_character;

#ifdef NEVER
printf("\nline %d el %d this=%c last=%c",
line,j,this_character,last_character);
#endif


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

         /********************
         if(this_character == last_character){
            no_change(pattern, processed_pattern,
                      pp_index, &pattern_index, 
                      current_width, &index); 
            pp_index++;
         }
         **********************/    
         
         no_change(pattern, processed_pattern,
                   pp_index, &pattern_index, 
                   current_width, &index); 
         pp_index++;
         
         
         if(pattern_index >= current_width)
            pattern_index = 0;
         
         /***** ?????
         if(index >= current_width)
            index = 0;  
         ********/
            
         last_character = depth_line[j];

      }  /* ends loop over j */
      
      /************
      processed_pattern[width-1] = '\0';
      processed_pattern[width-2] = '\n';
      ************/
      
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
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *   open_files(...
   *
   *   This function opens the depth file 
   *   for reading and creates
   *   the processed pattern file and the
   *   stereo file.
   *
   ***********************************************/

void open_files(depth_file, stereo_file, 
                processed_pattern_file, argc, argv)
   char *argv[];
   FILE *depth_file,
        *stereo_file,
        *processed_pattern_file;
   int  argc;
{

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

   if(argc == 6){
      if((processed_pattern_file = 
         fopen(argv[5], "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", argv[5]);
         exit(5);
      }
   }  /* ends if argc == 6 */
   else{  /* else temp file name not given 
             so use the default name */
      if((processed_pattern_file = 
         fopen(PPFILENAME, "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", 
         PPFILENAME);
         exit(5);
      }
   }  /* ends else */

}  /* ends open_files */


   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

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

#ifdef NEVER
printf("\nStart of shorten_pattern");
test_print_line(pattern, *current_width);
#endif

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


#ifdef NEVER
printf("\nEnd of shorten_pattern");
test_print_line(pattern, *current_width);
printf("\nHit enter to continue");
gets(temp_pattern);     
#endif

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

#ifdef NEVER
printf("\nStart of lengthen_pattern");
test_print_line(pattern, *current_width);
#endif

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


#ifdef NEVER
printf("\nEnd of lengthen_pattern");
test_print_line(pattern, *current_width);
printf("\nHit enter to continue");
gets(temp_pattern);     
#endif


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




#ifdef TESTINITPATTERN
   /* command line is:
   cstereo 10 80 dfile.txt sfile.txt ppfile.txt */

   initialize_pattern(&pattern_index, pattern, 
                      &current_width, &max_width, 
                      pattern_width);
   printf("\nThe initialized pattern is:\n\t");
   for(i=0; i<width; i++)
      printf("%c", pattern[i]);
   printf("\n");
#endif

!@#$%^&
&^%$#@!
9
Sat Mar 04 16:49:51 1995
This version works on the tougher case of
dfile06.txt.   000000000000000000000
               000000111110000000000
               000000111222220000000
               000000000222220000000
!@#$%^&

   /***********************************************
   *
   *  file d:\cips\cips.c
   *
   *  Functions: This file contains
   *     main
   *     show_menu
   *     show_image
   *
   *  Purpose:
   *     This file contains the main calling
   *     routine in the C Image Processing System.
   *
   *  External Calls:
   *     none
   *
   *  Modifications:
   *     6 January 1995 - created
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
#define PATTERN_START 48 /* the zero character */
#define KONSTANT       2
#undef TEST_INIT_PATTERN
#define TEST_NO_CHANGE
#define DEBUG_MODE
#undef COMMAND_LINE_MODE

void fill_line();
void initialize_pattern();
void lengthen_pattern();
void no_change();
void open_files();
void shorten_pattern();

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
   if(argc < 5  ||  argc > 6){
      printf(
      "\nusage: cstereo pattern-width width-of-line"
      "\n               depth-file-name "
      "stereo-file-name "
      "\n               [processed-pattern-file-name]"
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

   if(argc == 6){
      if((processed_pattern_file = 
         fopen(argv[5], "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", argv[5]);
         exit(5);
      }
   }  /* ends if argc == 6 */
   else{  /* else temp file name not given 
             so use the default name */
      if((processed_pattern_file = 
         fopen(PPFILENAME, "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", 
         PPFILENAME);
         exit(5);
      }
   }  /* ends else */
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

   

#ifdef NEVER
printf("\ndepth_file %x stfile %x ppfile %x",
depth_file, stereo_file, processed_pattern_file);
fgets(depth_line, width, depth_file);
printf("\n**%s**",depth_line);
printf("Hit Enter to continue");
gets(response);
#endif

      
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
   
#ifdef COMMAND_LINE_MODE   
line++;
printf("\n--%s--",depth_line);
printf("\nLine is %d long", strlen(depth_line));
printf("\nHit Enter to continue");
gets(response);         
#endif

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
    
      /*** ?????? 
      index = 0; ***/
      last_character = depth_line[0];

      pp_index = 0;
      
      for(j=0; j<width; j++){
         this_character = depth_line[j];
         if(this_character == '\n')
            this_character = last_character;

#ifdef COMMAND_LINE_MODE
printf("\nline %d el %d this=%c last=%c",
line,j,this_character,last_character);
#endif


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

         /********************
         if(this_character == last_character){
            no_change(pattern, processed_pattern,
                      pp_index, &pattern_index, 
                      current_width, &index); 
            pp_index++;
         }
         **********************/    
         
         no_change(pattern, processed_pattern,
                   pp_index, &pattern_index, 
                   current_width, &index); 
         pp_index++;
         
         
         if(pattern_index >= current_width)
            pattern_index = 0;
         
         /***** ?????
         if(index >= current_width)
            index = 0;  
         ********/
            
         last_character = depth_line[j];

      }  /* ends loop over j */
      
      /************
      processed_pattern[width-1] = '\0';
      processed_pattern[width-2] = '\n';
      ************/
      pdest = strchr(processed_pattern, '\0');
      location = pdest - processed_pattern;
      processed_pattern[location] = '\n';
      fputs(processed_pattern, processed_pattern_file);
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
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *   open_files(...
   *
   *   This function opens the depth file 
   *   for reading and creates
   *   the processed pattern file and the
   *   stereo file.
   *
   ***********************************************/

void open_files(depth_file, stereo_file, 
                processed_pattern_file, argc, argv)
   char *argv[];
   FILE *depth_file,
        *stereo_file,
        *processed_pattern_file;
   int  argc;
{

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

   if(argc == 6){
      if((processed_pattern_file = 
         fopen(argv[5], "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", argv[5]);
         exit(5);
      }
   }  /* ends if argc == 6 */
   else{  /* else temp file name not given 
             so use the default name */
      if((processed_pattern_file = 
         fopen(PPFILENAME, "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", 
         PPFILENAME);
         exit(5);
      }
   }  /* ends else */

}  /* ends open_files */


   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

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

#ifdef COMMAND_LINE_MODE
printf("\nStart of shorten_pattern");
test_print_line(pattern, *current_width);
#endif

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


#ifdef COMMAND_LINE_MODE
printf("\nEnd of shorten_pattern");
test_print_line(pattern, *current_width);
printf("\nHit enter to continue");
gets(temp_pattern);     
#endif

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

#ifdef COMMAND_LINE_MODE
printf("\nStart of lengthen_pattern");
test_print_line(pattern, *current_width);
#endif

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


#ifdef COMMAND_LINE_MODE
printf("\nEnd of lengthen_pattern");
test_print_line(pattern, *current_width);
printf("\nHit enter to continue");
gets(temp_pattern);     
#endif


   free(temp_pattern);

}  /* ends lengthen_pattern */



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




#ifdef TESTINITPATTERN
   /* command line is:
   cstereo 10 80 dfile.txt sfile.txt ppfile.txt */

   initialize_pattern(&pattern_index, pattern, 
                      &current_width, &max_width, 
                      pattern_width);
   printf("\nThe initialized pattern is:\n\t");
   for(i=0; i<width; i++)
      printf("%c", pattern[i]);
   printf("\n");
#endif

!@#$%^&
&^%$#@!
8
Fri Mar 03 06:22:02 1995
I made some changes in lenghten pattern.  It
worked on a simple case in dfile03.txt.  Did
not work on a case where 0001111000
                         0001122220
!@#$%^&

   /***********************************************
   *
   *  file d:\cips\cips.c
   *
   *  Functions: This file contains
   *     main
   *     show_menu
   *     show_image
   *
   *  Purpose:
   *     This file contains the main calling
   *     routine in the C Image Processing System.
   *
   *  External Calls:
   *     none
   *
   *  Modifications:
   *     6 January 1995 - created
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
#define PATTERN_START 48 /* the zero character */
#define KONSTANT       2
#undef TEST_INIT_PATTERN
#define TEST_NO_CHANGE
#define DEBUG_MODE
#undef COMMAND_LINE_MODE

void fill_line();
void initialize_pattern();
void lengthen_pattern();
void no_change();
void open_files();
void shorten_pattern();

void test_print_line();


main(argc, argv)
   char *argv[];
   int  argc;
{
   char *depth_line,
        *pattern, 
        *processed_pattern,
        response[80];

   char last_character,
        this_character;

   FILE *depth_file,
        *processed_pattern_file,
        *stereo_file;

   int  current_width,
        j,
        index,
        max_width,
        pattern_index,
        pattern_width,
        pp_index,
        width;
int line = 0;

#ifdef COMMAND_LINE_MODE
   if(argc < 5  ||  argc > 6){
      printf(
      "\nusage: cstereo pattern-width width-of-line"
      "\n               depth-file-name "
      "stereo-file-name "
      "\n               [processed-pattern-file-name]"
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

   if(argc == 6){
      if((processed_pattern_file = 
         fopen(argv[5], "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", argv[5]);
         exit(5);
      }
   }  /* ends if argc == 6 */
   else{  /* else temp file name not given 
             so use the default name */
      if((processed_pattern_file = 
         fopen(PPFILENAME, "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", 
         PPFILENAME);
         exit(5);
      }
   }  /* ends else */
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

   

#ifdef NEVER
printf("\ndepth_file %x stfile %x ppfile %x",
depth_file, stereo_file, processed_pattern_file);
fgets(depth_line, width, depth_file);
printf("\n**%s**",depth_line);
printf("Hit Enter to continue");
gets(response);
#endif

      
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
   
#ifdef COMMAND_LINE_MODE   
line++;
printf("\n--%s--",depth_line);
printf("\nLine is %d long", strlen(depth_line));
printf("\nHit Enter to continue");
gets(response);         
#endif

      fill_line(pattern, width);
      fill_line(processed_pattern, width);

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
    
      /*** ?????? 
      index = 0; ***/
      last_character = depth_line[0];

      pp_index = 0;
      
      for(j=0; j<width; j++){
         this_character = depth_line[j];
         if(this_character == '\n')
            this_character = last_character;

#ifdef COMMAND_LINE_MODE
printf("\nline %d el %d this=%c last=%c",
line,j,this_character,last_character);
#endif


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


         if(this_character == last_character){
            no_change(pattern, processed_pattern,
                      pp_index, &pattern_index, 
                      current_width, &index); 
            pp_index++;
         }

         if(pattern_index >= current_width)
            pattern_index = 0;
         
         /***** ?????
         if(index >= current_width)
            index = 0;  
         ********/
            
         last_character = depth_line[j];

      }  /* ends loop over j */
     
      processed_pattern[width-1] = '\0';
      processed_pattern[width-2] = '\n';
      fputs(processed_pattern, processed_pattern_file);
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
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *   open_files(...
   *
   *   This function opens the depth file 
   *   for reading and creates
   *   the processed pattern file and the
   *   stereo file.
   *
   ***********************************************/

void open_files(depth_file, stereo_file, 
                processed_pattern_file, argc, argv)
   char *argv[];
   FILE *depth_file,
        *stereo_file,
        *processed_pattern_file;
   int  argc;
{

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

   if(argc == 6){
      if((processed_pattern_file = 
         fopen(argv[5], "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", argv[5]);
         exit(5);
      }
   }  /* ends if argc == 6 */
   else{  /* else temp file name not given 
             so use the default name */
      if((processed_pattern_file = 
         fopen(PPFILENAME, "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", 
         PPFILENAME);
         exit(5);
      }
   }  /* ends else */

}  /* ends open_files */


   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

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

#ifdef COMMAND_LINE_MODE
printf("\nStart of shorten_pattern");
test_print_line(pattern, *current_width);
#endif

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


#ifdef COMMAND_LINE_MODE
printf("\nEnd of shorten_pattern");
test_print_line(pattern, *current_width);
printf("\nHit enter to continue");
gets(temp_pattern);     
#endif

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

#ifdef COMMAND_LINE_MODE
printf("\nStart of lengthen_pattern");
test_print_line(pattern, *current_width);
#endif

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


#ifdef COMMAND_LINE_MODE
printf("\nEnd of lengthen_pattern");
test_print_line(pattern, *current_width);
printf("\nHit enter to continue");
gets(temp_pattern);     
#endif


   free(temp_pattern);

}  /* ends lengthen_pattern */



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




#ifdef TESTINITPATTERN
   /* command line is:
   cstereo 10 80 dfile.txt sfile.txt ppfile.txt */

   initialize_pattern(&pattern_index, pattern, 
                      &current_width, &max_width, 
                      pattern_width);
   printf("\nThe initialized pattern is:\n\t");
   for(i=0; i<width; i++)
      printf("%c", pattern[i]);
   printf("\n");
#endif

!@#$%^&
&^%$#@!
7
Thu Mar 02 16:08:03 1995
This version worked on dfile01.txt and dfile02.txt.
These shortened the pattern and did the no change
correctly.
!@#$%^&

   /***********************************************
   *
   *  file d:\cips\cips.c
   *
   *  Functions: This file contains
   *     main
   *     show_menu
   *     show_image
   *
   *  Purpose:
   *     This file contains the main calling
   *     routine in the C Image Processing System.
   *
   *  External Calls:
   *     none
   *
   *  Modifications:
   *     6 January 1995 - created
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
#define PATTERN_START 48 /* the zero character */
#define KONSTANT       2
#undef TEST_INIT_PATTERN
#define TEST_NO_CHANGE
#define DEBUG_MODE
#undef COMMAND_LINE_MODE

void fill_line();
void initialize_pattern();
void lengthen_pattern();
void no_change();
void open_files();
void shorten_pattern();

void test_print_line();


main(argc, argv)
   char *argv[];
   int  argc;
{
   char *depth_line,
        *pattern, 
        *processed_pattern,
        response[80];

   char last_character,
        this_character;

   FILE *depth_file,
        *processed_pattern_file,
        *stereo_file;

   int  current_width,
        j,
        index,
        max_width,
        pattern_index,
        pattern_width,
        pp_index,
        width;
int line = 0;

#ifdef COMMAND_LINE_MODE
   if(argc < 5  ||  argc > 6){
      printf(
      "\nusage: cstereo pattern-width width-of-line"
      "\n               depth-file-name "
      "stereo-file-name "
      "\n               [processed-pattern-file-name]"
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

   if(argc == 6){
      if((processed_pattern_file = 
         fopen(argv[5], "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", argv[5]);
         exit(5);
      }
   }  /* ends if argc == 6 */
   else{  /* else temp file name not given 
             so use the default name */
      if((processed_pattern_file = 
         fopen(PPFILENAME, "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", 
         PPFILENAME);
         exit(5);
      }
   }  /* ends else */
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

   

#ifdef NEVER
printf("\ndepth_file %x stfile %x ppfile %x",
depth_file, stereo_file, processed_pattern_file);
fgets(depth_line, width, depth_file);
printf("\n**%s**",depth_line);
printf("Hit Enter to continue");
gets(response);
#endif

      
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
   
#ifdef COMMAND_LINE_MODE   
line++;
printf("\n--%s--",depth_line);
printf("\nLine is %d long", strlen(depth_line));
printf("\nHit Enter to continue");
gets(response);         
#endif

      fill_line(pattern, width);
      fill_line(processed_pattern, width);

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
    
      /*** ?????? 
      index = 0; ***/
      last_character = depth_line[0];

      pp_index = 0;
      
      for(j=0; j<width; j++){
         this_character = depth_line[j];
         if(this_character == '\n')
            this_character = last_character;

#ifdef COMMAND_LINE_MODE
printf("\nline %d el %d this=%c last=%c",
line,j,this_character,last_character);
#endif


         if(this_character > last_character)
            shorten_pattern(
               (this_character-last_character),
               pattern, &index, &current_width,
               &pattern_index, width);


         if(this_character < last_character)
            lengthen_pattern(
               (this_character-last_character), 
               pattern, &index, &current_width, 
               &pattern_index, &width, &max_width);


         if(this_character == last_character){
            no_change(pattern, processed_pattern,
                      pp_index, &pattern_index, 
                      current_width, &index); 
            pp_index++;
         }

         if(pattern_index >= current_width)
            pattern_index = 0;
         
         /***** ?????
         if(index >= current_width)
            index = 0;  
         ********/
            
         last_character = depth_line[j];

      }  /* ends loop over j */
     
      processed_pattern[width-1] = '\0';
      processed_pattern[width-2] = '\n';
      fputs(processed_pattern, processed_pattern_file);
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
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *   open_files(...
   *
   *   This function opens the depth file 
   *   for reading and creates
   *   the processed pattern file and the
   *   stereo file.
   *
   ***********************************************/

void open_files(depth_file, stereo_file, 
                processed_pattern_file, argc, argv)
   char *argv[];
   FILE *depth_file,
        *stereo_file,
        *processed_pattern_file;
   int  argc;
{

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

   if(argc == 6){
      if((processed_pattern_file = 
         fopen(argv[5], "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", argv[5]);
         exit(5);
      }
   }  /* ends if argc == 6 */
   else{  /* else temp file name not given 
             so use the default name */
      if((processed_pattern_file = 
         fopen(PPFILENAME, "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", 
         PPFILENAME);
         exit(5);
      }
   }  /* ends else */

}  /* ends open_files */


   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

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

#ifdef COMMAND_LINE_MODE
printf("\nStart of shorten_pattern");
test_print_line(pattern, *current_width);
#endif

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


#ifdef COMMAND_LINE_MODE
printf("\nEnd of shorten_pattern");
test_print_line(pattern, *current_width);
printf("\nHit enter to continue");
gets(temp_pattern);     
#endif

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

#ifdef COMMAND_LINE_MODE
printf("\nStart of lengthen_pattern");
test_print_line(pattern, *current_width);
#endif

   temp_pattern = malloc(KONSTANT*(*width));

   for(i=0; i<(*width); i++)
      temp_pattern[i] = pattern[i];

   for(count=0, new_index=0; count<size; 
       count++, new_index++)
      pattern[new_index] = count + *max_width;

   new_width = *width + size;

   for( ; new_index < new_width; new_index++){
      pattern[new_index] = temp_pattern[*index];
      *index = *index + 1;
      if(*index >= *width)
         *index = 0;
   }  /* ends loop over new_index */

   *width         = new_width;
   *pattern_index = 0;
   *max_width     = *max_width + size;


#ifdef COMMAND_LINE_MODE
printf("\nEnd of lengthen_pattern");
test_print_line(pattern, *current_width);
printf("\nHit enter to continue");
gets(temp_pattern);     
#endif


   free(temp_pattern);

}  /* ends lengthen_pattern */



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




#ifdef TESTINITPATTERN
   /* command line is:
   cstereo 10 80 dfile.txt sfile.txt ppfile.txt */

   initialize_pattern(&pattern_index, pattern, 
                      &current_width, &max_width, 
                      pattern_width);
   printf("\nThe initialized pattern is:\n\t");
   for(i=0; i<width; i++)
      printf("%c", pattern[i]);
   printf("\n");
#endif

!@#$%^&
&^%$#@!
6
Sat Feb 11 16:59:39 1995
I entered the lenghthen pattern routine.
It does not work right.  I need to work through
all of this line by line.
!@#$%^&

   /***********************************************
   *
   *  file d:\cips\cips.c
   *
   *  Functions: This file contains
   *     main
   *     show_menu
   *     show_image
   *
   *  Purpose:
   *     This file contains the main calling
   *     routine in the C Image Processing System.
   *
   *  External Calls:
   *     none
   *
   *  Modifications:
   *     6 January 1995 - created
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

*********/

#include "cips.h"

#define PPFILENAME   "ppfile.txt"
#define PATTERN_START 48 /* the zero character */
#define KONSTANT       2
#undef TEST_INIT_PATTERN
#define TEST_NO_CHANGE

void initialize_pattern();
void lengthen_pattern();
void no_change();
void open_files();
void shorten_pattern();

void test_print_line();


main(argc, argv)
   char *argv[];
   int  argc;
{
   char *depth_line,
        *pattern, 
        *processed_pattern,
        response[80];

   char last_character,
        this_character;

   FILE *depth_file,
        *processed_pattern_file,
        *stereo_file;

   int  current_width,
        i,
        j,
        index,
        max_width,
        pattern_index,
        pattern_width,
        width;
int line = 0;

   if(argc < 5  ||  argc > 6){
      printf(
      "\nusage: cstereo pattern-width width-of-line"
      "\n               depth-file-name "
      "stereo-file-name "
      "\n               [processed-pattern-file-name]"
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

   if(argc == 6){
      if((processed_pattern_file = 
         fopen(argv[5], "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", argv[5]);
         exit(5);
      }
   }  /* ends if argc == 6 */
   else{  /* else temp file name not given 
             so use the default name */
      if((processed_pattern_file = 
         fopen(PPFILENAME, "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", 
         PPFILENAME);
         exit(5);
      }
   }  /* ends else */

#ifdef NEVER
printf("\ndepth_file %x stfile %x ppfile %x",
depth_file, stereo_file, processed_pattern_file);
fgets(depth_line, width, depth_file);
printf("\n**%s**",depth_line);
printf("Hit Enter to continue");
gets(response);
#endif

      
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
line++;
printf("\n--%s--",depth_line);
printf("\nLine is %d long", strlen(depth_line));
printf("\nHit Enter to continue");
gets(response);

      fill_line(pattern, width);
      fill_line(processed_pattern, width);

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
    
      /*** ?????? 
      index = 0; ***/
      last_character = depth_line[0];

      for(j=0; j<width; j++){
         this_character = depth_line[j];
         if(this_character == '\n')
            this_character = last_character;

printf("\nline %d el %d this=%c last=%c",
line,j,this_character,last_character);


         if(this_character > last_character)
            shorten_pattern(
               (this_character-last_character),
               pattern, &index, &current_width,
               &pattern_index, width);


         if(this_character < last_character)
            lengthen_pattern(
               (this_character-last_character), 
               pattern, &index, &current_width, 
               &pattern_index, &width, &max_width);


         if(this_character == last_character)
            no_change(pattern, processed_pattern, j,
                      &pattern_index, current_width,
                      &index);

         if(pattern_index >= current_width)
            pattern_index = 0;
         
         /***** ?????
         if(index >= current_width)
            index = 0;  
         ********/
            
         last_character = depth_line[j];

      }  /* ends loop over j */
     
      processed_pattern[width-1] = '\0';
      processed_pattern[width-2] = '\n';
      fputs(processed_pattern, processed_pattern_file);
   }   /* ends the major loop */

   fclose(depth_file);
   fclose(processed_pattern_file);
   fclose(stereo_file);

/**********
   free(pattern);
   free(depth_line);
   free(processed_pattern);
***********/

}  /* ends main                 */



   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *   open_files(...
   *
   *   This function opens the depth file 
   *   for reading and creates
   *   the processed pattern file and the
   *   stereo file.
   *
   ***********************************************/

void open_files(depth_file, stereo_file, 
                processed_pattern_file, argc, argv)
   char *argv[];
   FILE *depth_file,
        *stereo_file,
        *processed_pattern_file;
   int  argc;
{

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

   if(argc == 6){
      if((processed_pattern_file = 
         fopen(argv[5], "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", argv[5]);
         exit(5);
      }
   }  /* ends if argc == 6 */
   else{  /* else temp file name not given 
             so use the default name */
      if((processed_pattern_file = 
         fopen(PPFILENAME, "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", 
         PPFILENAME);
         exit(5);
      }
   }  /* ends else */

}  /* ends open_files */


   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

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

printf("\nStart of shorten_pattern");
test_print_line(pattern, *current_width);

   temp_pattern = malloc(KONSTANT*width);

   for(i=0; i<width; i++)
      temp_pattern[i] = pattern[i];

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
   *pattern_index = 0;

printf("\nEnd of shorten_pattern");
test_print_line(pattern, *current_width);
printf("\nHit enter to continue");
gets(temp_pattern);

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

void no_change(pattern, processed_pattern, j,
               pattern_index, current_width,
               index)
   char *pattern, *processed_pattern;
   int  j, *pattern_index, current_width, *index;
{
   processed_pattern[j] = pattern[*pattern_index];

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

fill_line(line, length)
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

printf("\nStart of lengthen_pattern");
test_print_line(pattern, *current_width);

   temp_pattern = malloc(KONSTANT*(*width));

   for(i=0; i<(*width); i++)
      temp_pattern[i] = pattern[i];

   for(count=0, new_index=0; count<size; 
       count++, new_index++)
      pattern[new_index] = count + *max_width;

   new_width = *width + size;

   for( ; new_index < new_width; new_index++){
      pattern[new_index] = temp_pattern[*index];
      *index = *index + 1;
      if(*index >= *width)
         *index = 0;
   }  /* ends loop over new_index */

   *width         = new_width;
   *pattern_index = 0;
   *max_width     = *max_width + size;


printf("\nEnd of lengthen_pattern");
test_print_line(pattern, *current_width);
printf("\nHit enter to continue");
gets(temp_pattern);

   free(temp_pattern);

}  /* ends lengthen_pattern */



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




#ifdef TESTINITPATTERN
   /* command line is:
   cstereo 10 80 dfile.txt sfile.txt ppfile.txt */

   initialize_pattern(&pattern_index, pattern, 
                      &current_width, &max_width, 
                      pattern_width);
   printf("\nThe initialized pattern is:\n\t");
   for(i=0; i<width; i++)
      printf("%c", pattern[i]);
   printf("\n");
#endif
!@#$%^&
&^%$#@!
5
Sat Feb 11 09:17:43 1995
This one reads lines correctly and I think
it processes them correctly .  I am not sure
of the shorten_pattern routine.  I am not
writing the way I want to in the fputs
processed_pattern.  Not sure why.
!@#$%^&

   /***********************************************
   *
   *  file d:\cips\cips.c
   *
   *  Functions: This file contains
   *     main
   *     show_menu
   *     show_image
   *
   *  Purpose:
   *     This file contains the main calling
   *     routine in the C Image Processing System.
   *
   *  External Calls:
   *     none
   *
   *  Modifications:
   *     6 January 1995 - created
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

*********/

#include "cips.h"

#define PPFILENAME   "ppfile.txt"
#define PATTERN_START 48 /* the zero character */
#undef TEST_INIT_PATTERN
#define TEST_NO_CHANGE

void initialize_pattern();
void lengthen_pattern();
void no_change();
void open_files();
void shorten_pattern();

void test_print_line();


main(argc, argv)
   char *argv[];
   int  argc;
{
   char *depth_line,
        *pattern, 
        *processed_pattern,
        response[80];

   char last_character,
        this_character;

   FILE *depth_file,
        *processed_pattern_file,
        *stereo_file;

   int  current_width,
        i,
        j,
        index,
        max_width,
        pattern_index,
        pattern_width,
        width;
int line = 0;

   if(argc < 5  ||  argc > 6){
      printf(
      "\nusage: cstereo pattern-width width-of-line"
      "\n               depth-file-name "
      "stereo-file-name "
      "\n               [processed-pattern-file-name]"
      "\n");
      exit(1);
   }

   pattern_width = atoi(argv[1]);
   width         = atoi(argv[2]);

   pattern           = malloc(width);
   depth_line        = malloc(width);
   processed_pattern = malloc(width);


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

   if(argc == 6){
      if((processed_pattern_file = 
         fopen(argv[5], "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", argv[5]);
         exit(5);
      }
   }  /* ends if argc == 6 */
   else{  /* else temp file name not given 
             so use the default name */
      if((processed_pattern_file = 
         fopen(PPFILENAME, "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", 
         PPFILENAME);
         exit(5);
      }
   }  /* ends else */

#ifdef NEVER
printf("\ndepth_file %x stfile %x ppfile %x",
depth_file, stereo_file, processed_pattern_file);
fgets(depth_line, width, depth_file);
printf("\n**%s**",depth_line);
printf("Hit Enter to continue");
gets(response);
#endif

      
      /***********************************************
      *
      *   This is the major loop of the program.
      *   It reads one line at a time from the 
      *   depth file, processes that one line,
      *   and writes the resulting pattern to the
      *   processed pattern file.
      *
      ***********************************************/

   while(fgets(depth_line, width, depth_file)){
line++;
printf("\n--%s--",depth_line);
printf("\nHit Enter to continue");
gets(response);

      fill_line(pattern, width);
      fill_line(processed_pattern, width);

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
    
      /*** ?????? 
      index = 0; ***/
      last_character = depth_line[0];

      for(j=0; j<width; j++){
         this_character = depth_line[j];

printf("\nline %d el %d this=%c last=%c",
line,j,this_character,last_character);

         if(this_character > last_character)
            shorten_pattern(
               (this_character-last_character),
               pattern, &index, &current_width,
               &pattern_index, width);



         if(this_character == last_character)
            no_change(pattern, processed_pattern, j,
                      &pattern_index, current_width,
                      &index);

         if(pattern_index >= current_width)
            pattern_index = 0;
         
         /***** ?????
         if(index >= current_width)
            index = 0;  
         ********/
            
         last_character = depth_line[j];

      }  /* ends loop over j */
     
      processed_pattern[width-1] = '\0';
      processed_pattern[width-2] = '\n';
      fputs(processed_pattern, processed_pattern_file);
   }   /* ends the major loop */

   fclose(depth_file);
   fclose(processed_pattern_file);
   fclose(stereo_file);

/**********
   free(pattern);
   free(depth_line);
   free(processed_pattern);
***********/

}  /* ends main                 */



   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *   open_files(...
   *
   *   This function opens the depth file 
   *   for reading and creates
   *   the processed pattern file and the
   *   stereo file.
   *
   ***********************************************/

void open_files(depth_file, stereo_file, 
                processed_pattern_file, argc, argv)
   char *argv[];
   FILE *depth_file,
        *stereo_file,
        *processed_pattern_file;
   int  argc;
{

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

   if(argc == 6){
      if((processed_pattern_file = 
         fopen(argv[5], "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", argv[5]);
         exit(5);
      }
   }  /* ends if argc == 6 */
   else{  /* else temp file name not given 
             so use the default name */
      if((processed_pattern_file = 
         fopen(PPFILENAME, "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", 
         PPFILENAME);
         exit(5);
      }
   }  /* ends else */

}  /* ends open_files */


   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

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

printf("\nStart of shorten_pattern");
test_print_line(pattern, *current_width);

   temp_pattern = malloc(width);

   for(i=0; i<width; i++)
      temp_pattern[i] = pattern[i];

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
   *pattern_index = 0;

printf("\nEnd of shorten_pattern");
test_print_line(pattern, *current_width);
printf("\nHit enter to continue");
gets(temp_pattern);

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

void no_change(pattern, processed_pattern, j,
               pattern_index, current_width,
               index)
   char *pattern, *processed_pattern;
   int  j, *pattern_index, current_width, *index;
{
   processed_pattern[j] = pattern[*pattern_index];

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

fill_line(line, length)
   char *line;
   int  length;
{
   int i;
   for(i=0; i<length; i++)
      line[i] = '\0';
}  /* ends fill_line */




void test_print_line(line, width)
   char *line;
   int  width;
{
   int i;
   for(i=0; i<width; i++)
      printf("%c", line[i]);
   printf("\n");
}  /* ends test_print_line */




#ifdef TESTINITPATTERN
   /* command line is:
   cstereo 10 80 dfile.txt sfile.txt ppfile.txt */

   initialize_pattern(&pattern_index, pattern, 
                      &current_width, &max_width, 
                      pattern_width);
   printf("\nThe initialized pattern is:\n\t");
   for(i=0; i<width; i++)
      printf("%c", pattern[i]);
   printf("\n");
#endif
!@#$%^&
&^%$#@!
4
Fri Feb 10 18:01:13 1995
This one works a little, still not right
!@#$%^&

   /***********************************************
   *
   *  file d:\cips\cips.c
   *
   *  Functions: This file contains
   *     main
   *     show_menu
   *     show_image
   *
   *  Purpose:
   *     This file contains the main calling
   *     routine in the C Image Processing System.
   *
   *  External Calls:
   *     none
   *
   *  Modifications:
   *     6 January 1995 - created
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

*********/

#include "cips.h"

#define PPFILENAME   "ppfile.txt"
#define PATTERN_START 48 /* the zero character */
#undef TEST_INIT_PATTERN
#define TEST_NO_CHANGE

void initialize_pattern();
void lengthen_pattern();
void no_change();
void open_files();
void shorten_pattern();

void test_print_line();


main(argc, argv)
   char *argv[];
   int  argc;
{
   char depth_line[200],
        pattern[200], 
        processed_pattern[200],
        response[80];

   char last_character,
        this_character;

   FILE *depth_file,
        *processed_pattern_file,
        *stereo_file;

   int  current_width,
        i,
        j,
        index,
        max_width,
        pattern_index,
        pattern_width,
        width;
int line = 0;

   if(argc < 5  ||  argc > 6){
      printf(
      "\nusage: cstereo pattern-width width-of-line"
      "\n               depth-file-name "
      "stereo-file-name "
      "\n               [processed-pattern-file-name]"
      "\n");
      exit(1);
   }

   pattern_width = atoi(argv[1]);
   width         = atoi(argv[2]);

/**************
   pattern           = malloc(width);
   depth_line        = malloc(width);
   processed_pattern = malloc(width);
**************/


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

   if(argc == 6){
      if((processed_pattern_file = 
         fopen(argv[5], "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", argv[5]);
         exit(5);
      }
   }  /* ends if argc == 6 */
   else{  /* else temp file name not given 
             so use the default name */
      if((processed_pattern_file = 
         fopen(PPFILENAME, "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", 
         PPFILENAME);
         exit(5);
      }
   }  /* ends else */

printf("\ndepth_file %x stfile %x ppfile %x",
depth_file, stereo_file, processed_pattern_file);
fgets(depth_line, width, depth_file);
printf("\n**%s**",depth_line);
printf("Hit Enter to continue");
gets(response);

#ifdef TESTINITPATTERN
   /* command line is:
   cstereo 10 80 dfile.txt sfile.txt ppfile.txt */

   initialize_pattern(&pattern_index, pattern, 
                      &current_width, &max_width, 
                      pattern_width);
   printf("\nThe initialized pattern is:\n\t");
   for(i=0; i<width; i++)
      printf("%c", pattern[i]);
   printf("\n");
#endif
      
      /***********************************************
      *
      *   This is the major loop of the program.
      *   It reads one line at a time from the 
      *   depth file, processes that one line,
      *   and writes the resulting pattern to the
      *   processed pattern file.
      *
      ***********************************************/

   while(fgets(depth_line, width, depth_file)){
line++;
printf("\n--%s--",depth_line);

      fill_line(pattern, width);
      fill_line(processed_pattern, width);

      initialize_pattern(&pattern_index, pattern, 
                         &current_width, 
                         &max_width, 
                         pattern_width);
test_print_line(pattern, width);
printf("\nHit Enter to continue");
gets(response);

         /*******************************************
         *
         *   Loop through the characters in the
         *   depth_line.  Call one of the three
         *   processing routines based on the
         *   relationship between the last character
         *   and this character.
         *
         *******************************************/
    
      /*** ?????? **/
      index = 0;
      last_character = depth_line[0];

      for(j=0; j<width; j++){
         this_character = depth_line[j];

printf("\nline %d el %d this=%c last=%c",
line,j,this_character,last_character);

         if(this_character > last_character)
            shorten_pattern(
               (this_character-last_character),
               pattern, &index, &current_width,
               &pattern_index, width);



         if(this_character == last_character)
            no_change(pattern, processed_pattern, j,
                      &pattern_index, current_width);

         if(pattern_index >= current_width)
            pattern_index = 0;

         if(index >= current_width)
            index = 0;  /** ????? */
            

      }  /* ends loop over j */
   }   /* ends the major loop */

   fclose(depth_file);
   fclose(processed_pattern_file);
   fclose(stereo_file);

/**********
   free(pattern);
   free(depth_line);
   free(processed_pattern);
***********/

}  /* ends main                 */



   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *   open_files(...
   *
   *   This function opens the depth file 
   *   for reading and creates
   *   the processed pattern file and the
   *   stereo file.
   *
   ***********************************************/

void open_files(depth_file, stereo_file, 
                processed_pattern_file, argc, argv)
   char *argv[];
   FILE *depth_file,
        *stereo_file,
        *processed_pattern_file;
   int  argc;
{

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

   if(argc == 6){
      if((processed_pattern_file = 
         fopen(argv[5], "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", argv[5]);
         exit(5);
      }
   }  /* ends if argc == 6 */
   else{  /* else temp file name not given 
             so use the default name */
      if((processed_pattern_file = 
         fopen(PPFILENAME, "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", 
         PPFILENAME);
         exit(5);
      }
   }  /* ends else */

}  /* ends open_files */


   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

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

void shorten_pattern(pattern, size, index,
                     current_width, pattern_index, width)
   char *pattern;
   int  size, *index, *current_width, 
        *pattern_index, width;
{
   char *temp_pattern;
   int  i, new_index, new_width;

   temp_pattern = (char *) 
                  malloc(width * sizeof(char));

   for(i=0; i<width; i++)
      temp_pattern[i] = pattern[i];

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
   *pattern_index = 0;

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
                        pattern_width)
   char pattern[];
   int *pattern_index,
       *current_width,
       *max_width,
       pattern_width;
{
   int i;

   for(i=0; i<pattern_width; i++)
      pattern[i] = i+PATTERN_START;

   *pattern_index = 0;
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

void no_change(pattern, processed_pattern, j,
               pattern_index, current_width)
   char *pattern, *processed_pattern;
   int  j, *pattern_index, current_width;
{
   processed_pattern[j] = pattern[*pattern_index];
   *pattern_index = *pattern_index + 1;
   if(*pattern_index >= current_width)
      *pattern_index = 0;
}   /* ends no_change */





   /***********************************************
   *
   *   fill_line(...
   *
   *   This function fills a character line
   *   with NULL characters.
   *
   ***********************************************/

fill_line(line, length)
   char *line;
   int  length;
{
   int i;
   for(i=0; i<length; i++)
      line[i] = '\0';
}  /* ends fill_line */




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
3
Fri Feb 10 17:40:40 1995
I think i found a "problem"
!@#$%^&

   /***********************************************
   *
   *  file d:\cips\cips.c
   *
   *  Functions: This file contains
   *     main
   *     show_menu
   *     show_image
   *
   *  Purpose:
   *     This file contains the main calling
   *     routine in the C Image Processing System.
   *
   *  External Calls:
   *     none
   *
   *  Modifications:
   *     6 January 1995 - created
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

*********/

#include "cips.h"

#define PPFILENAME   "ppfile.txt"
#define PATTERN_START 48 /* the zero character */
#undef TEST_INIT_PATTERN
#define TEST_NO_CHANGE

void open_files();

main(argc, argv)
   char *argv[];
   int  argc;
{
   char *depth_line,
        *pattern, 
        *processed_pattern,
        response[80];

   char last_character,
        this_character;

   FILE *depth_file,
        *processed_pattern_file,
        *stereo_file;

   int  width;



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





   width      = atoi(argv[1]);
printf("\nwidth=%d", width);
   depth_line = malloc(width);
   fgets(depth_line, width, depth_file);
printf("\n***%s***", depth_line);

   /************
   if((depth_file = fopen(argv[2], "rt")) == NULL){
      printf(
    "\ntoc: Error opening input file %s\n", argv[1]);
      exit(3);
   }
   **************/

   while(fgets(depth_line, width, depth_file)){
printf("\n--%s--",depth_line);

   }   /* ends the major loop */

   fclose(depth_file);
   free(depth_line);

}  /* ends main                 */


   /***********************************************
   *
   *   open_files(...
   *
   *   This function opens the depth file 
   *   for reading and creates
   *   the processed pattern file and the
   *   stereo file.
   *
   ***********************************************/

void open_files(depth_file, stereo_file, 
                processed_pattern_file, 
                d_name, s_name, pp_name)
   char d_name[], s_name[], pp_name[];
   FILE *depth_file,
        *stereo_file,
        *processed_pattern_file;
{

   if((depth_file = fopen(d_name, "rt")) == NULL){
      printf(
     "\ncstereo: Error opening input file %s\n", 
      d_name);
      exit(3);
   }

   if((stereo_file = fopen(s_name, "wt")) == NULL){
      printf(
    "\ncstereo: Error opening output file %s\n", 
      s_name);
      exit(4);
   }

   if((processed_pattern_file = 
      fopen(pp_name, "wt")) == NULL){
      printf(
    "\ncstereo: Error opening temp file %s\n", pp_name);
      exit(5);
   }

}  /* ends open_files */


   /***********************************************
   *
   *
   *
   ***********************************************/

!@#$%^&
&^%$#@!
2
Fri Feb 10 17:01:52 1995
this version does not work either, but
it has some deletions to keep.
!@#$%^&

   /***********************************************
   *
   *  file d:\cips\cips.c
   *
   *  Functions: This file contains
   *     main
   *     show_menu
   *     show_image
   *
   *  Purpose:
   *     This file contains the main calling
   *     routine in the C Image Processing System.
   *
   *  External Calls:
   *     none
   *
   *  Modifications:
   *     6 January 1995 - created
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

*********/

#include "cips.h"

#define PPFILENAME   "ppfile.txt"
#define PATTERN_START 48 /* the zero character */
#undef TEST_INIT_PATTERN
#define TEST_NO_CHANGE

void open_files();

void test_print_line();


main(argc, argv)
   char *argv[];
   int  argc;
{
   char *depth_line,
        *pattern, 
        *processed_pattern,
        response[80];

   char last_character,
        this_character;

   FILE *depth_file,
        *processed_pattern_file,
        *stereo_file;

   int  current_width,
        i,
        j,
        index,
        max_width,
        pattern_index,
        pattern_width,
        width;
int line = 0;

   if(argc < 5  ||  argc > 6){
      printf(
      "\nusage: cstereo pattern-width width-of-line"
      "\n               depth-file-name "
      "stereo-file-name "
      "\n               [processed-pattern-file-name]"
      "\n");
      exit(1);
   }

   pattern_width = atoi(argv[1]);
   width         = atoi(argv[2]);

   pattern    = (char *) malloc(width * sizeof(char));
   depth_line = (char *) malloc(width * sizeof(char));
   processed_pattern = (char *) 
                       malloc(width * sizeof(char));

   open_files(depth_file, stereo_file, 
              processed_pattern_file, 
              argc, argv);
printf("\ndepth_file %x stfile %x ppfile %x",
depth_file, stereo_file, processed_pattern_file);
fgets(depth_line, width, depth_file);
printf("\n**%s**",depth_line);
printf("Hit Enter to continue");
gets(response);

      
      /***********************************************
      *
      *   This is the major loop of the program.
      *   It reads one line at a time from the 
      *   depth file, processes that one line,
      *   and writes the resulting pattern to the
      *   processed pattern file.
      *
      ***********************************************/

   while(fgets(depth_line, width, depth_file)){
line++;
printf("\n--%s--",depth_line);

   }   /* ends the major loop */

   fclose(depth_file);
   fclose(processed_pattern_file);
   fclose(stereo_file);

   free(pattern);
   free(depth_line);
   free(processed_pattern);

}  /* ends main                 */



   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *   open_files(...
   *
   *   This function opens the depth file 
   *   for reading and creates
   *   the processed pattern file and the
   *   stereo file.
   *
   ***********************************************/

void open_files(depth_file, stereo_file, 
                processed_pattern_file, argc, argv)
   char *argv[];
   FILE *depth_file,
        *stereo_file,
        *processed_pattern_file;
   int  argc;
{

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

   if(argc == 6){
      if((processed_pattern_file = 
         fopen(argv[5], "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", argv[5]);
         exit(5);
      }
   }  /* ends if argc == 6 */
   else{  /* else temp file name not given 
             so use the default name */
      if((processed_pattern_file = 
         fopen(PPFILENAME, "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", 
         PPFILENAME);
         exit(5);
      }
   }  /* ends else */

}  /* ends open_files */


   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

!@#$%^&
&^%$#@!
1
Fri Feb 10 05:59:21 1995
something is wrong here, make a safe copy
and work on it.
!@#$%^&

   /***********************************************
   *
   *  file d:\cips\cips.c
   *
   *  Functions: This file contains
   *     main
   *     show_menu
   *     show_image
   *
   *  Purpose:
   *     This file contains the main calling
   *     routine in the C Image Processing System.
   *
   *  External Calls:
   *     none
   *
   *  Modifications:
   *     6 January 1995 - created
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

*********/

#include "cips.h"

#define PPFILENAME   "ppfile.txt"
#define PATTERN_START 48 /* the zero character */
#undef TEST_INIT_PATTERN
#define TEST_NO_CHANGE

void initialize_pattern();
void lengthen_pattern();
void no_change();
void open_files();
void shorten_pattern();

void test_print_line();


main(argc, argv)
   char *argv[];
   int  argc;
{
   char *depth_line,
        *pattern, 
        *processed_pattern,
        response[80];

   char last_character,
        this_character;

   FILE *depth_file,
        *processed_pattern_file,
        *stereo_file;

   int  current_width,
        i,
        j,
        index,
        max_width,
        pattern_index,
        pattern_width,
        width;
int line = 0;

   if(argc < 5  ||  argc > 6){
      printf(
      "\nusage: cstereo pattern-width width-of-line"
      "\n               depth-file-name "
      "stereo-file-name "
      "\n               [processed-pattern-file-name]"
      "\n");
      exit(1);
   }

   pattern_width = atoi(argv[1]);
   width         = atoi(argv[2]);

   pattern    = (char *) malloc(width * sizeof(char));
   depth_line = (char *) malloc(width * sizeof(char));
   processed_pattern = (char *) 
                       malloc(width * sizeof(char));

   open_files(depth_file, stereo_file, 
              processed_pattern_file, 
              argc, argv);
printf("\ndepth_file %x stfile %x ppfile %x",
depth_file, stereo_file, processed_pattern_file);
fgets(response, width, depth_file);
printf("\n**%s**",response);
printf("Hit Enter to continue");
gets(response);

#ifdef TESTINITPATTERN
   /* command line is:
   cstereo 10 80 dfile.txt sfile.txt ppfile.txt */

   initialize_pattern(&pattern_index, pattern, 
                      &current_width, &max_width, 
                      pattern_width);
   printf("\nThe initialized pattern is:\n\t");
   for(i=0; i<width; i++)
      printf("%c", pattern[i]);
   printf("\n");
#endif
      
      /***********************************************
      *
      *   This is the major loop of the program.
      *   It reads one line at a time from the 
      *   depth file, processes that one line,
      *   and writes the resulting pattern to the
      *   processed pattern file.
      *
      ***********************************************/

   while(fgets(depth_line, width, depth_file)){
line++;
printf("\n--%s--",depth_line);

      fill_line(pattern, width);
      fill_line(processed_pattern, width);

      initialize_pattern(&pattern_index, pattern, 
                         &current_width, 
                         &max_width, 
                         pattern_width);
test_print_line(pattern, width);
printf("\nHit Enter to continue");
gets(response);

         /*******************************************
         *
         *   Loop through the characters in the
         *   depth_line.  Call one of the three
         *   processing routines based on the
         *   relationship between the last character
         *   and this character.
         *
         *******************************************/
    
      /*** ?????? **/
      index = 0;
      last_character = depth_line[0];

      for(j=0; j<width; j++){
         this_character = depth_line[j];

printf("\nline %d el %d this=%c last=%c",
line,j,this_character,last_character);

         if(this_character > last_character)
            shorten_pattern(
               (this_character-last_character),
               pattern, &index, &current_width,
               &pattern_index, width);



         if(this_character == last_character)
            no_change(pattern, processed_pattern, j,
                      &pattern_index, current_width);

         if(pattern_index >= current_width)
            pattern_index = 0;

         if(index >= current_width)
            index = 0;  /** ????? */
            

      }  /* ends loop over j */
   }   /* ends the major loop */

   fclose(depth_file);
   fclose(processed_pattern_file);
   fclose(stereo_file);

   free(pattern);
   free(depth_line);
   free(processed_pattern);

}  /* ends main                 */



   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *
   *
   ***********************************************/
   /***********************************************
   *
   *   open_files(...
   *
   *   This function opens the depth file 
   *   for reading and creates
   *   the processed pattern file and the
   *   stereo file.
   *
   ***********************************************/

void open_files(depth_file, stereo_file, 
                processed_pattern_file, argc, argv)
   char *argv[];
   FILE *depth_file,
        *stereo_file,
        *processed_pattern_file;
   int  argc;
{

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

   if(argc == 6){
      if((processed_pattern_file = 
         fopen(argv[5], "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", argv[5]);
         exit(5);
      }
   }  /* ends if argc == 6 */
   else{  /* else temp file name not given 
             so use the default name */
      if((processed_pattern_file = 
         fopen(PPFILENAME, "wt")) == NULL){
         printf(
       "\ncstereo: Error opening temp file %s\n", 
         PPFILENAME);
         exit(5);
      }
   }  /* ends else */

}  /* ends open_files */


   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

   /***********************************************
   *
   *
   *
   ***********************************************/

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

void shorten_pattern(pattern, size, index,
                     current_width, pattern_index, width)
   char *pattern;
   int  size, *index, *current_width, 
        *pattern_index, width;
{
   char *temp_pattern;
   int  i, new_index, new_width;

   temp_pattern = (char *) 
                  malloc(width * sizeof(char));

   for(i=0; i<width; i++)
      temp_pattern[i] = pattern[i];

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
   *pattern_index = 0;

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
                        pattern_width)
   char pattern[];
   int *pattern_index,
       *current_width,
       *max_width,
       pattern_width;
{
   int i;

   for(i=0; i<pattern_width; i++)
      pattern[i] = i+PATTERN_START;

   *pattern_index = 0;
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

void no_change(pattern, processed_pattern, j,
               pattern_index, current_width)
   char *pattern, *processed_pattern;
   int  j, *pattern_index, current_width;
{
   processed_pattern[j] = pattern[*pattern_index];
   *pattern_index = *pattern_index + 1;
   if(*pattern_index >= current_width)
      *pattern_index = 0;
}   /* ends no_change */





   /***********************************************
   *
   *   fill_line(...
   *
   *   This function fills a character line
   *   with NULL characters.
   *
   ***********************************************/

fill_line(line, length)
   char *line;
   int  length;
{
   int i;
   for(i=0; i<length; i++)
      line[i] = '\0';
}  /* ends fill_line */




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
