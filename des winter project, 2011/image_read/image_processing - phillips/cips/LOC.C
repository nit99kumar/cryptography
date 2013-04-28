
   /***********************************************
   *
   *  file loc.c
   *
   *  Functions: This file contains
   *     main
   *     is_a_loc
   *
   *  Purpose:
   *     This file contains a program that will
   *     count the lines of text and lines of code
   *     in a source code file.  The term lines 
   *     of code must meet the following
   *     requirements:
   *        1.  #include
   *        2.  #define
   *        3.  #undef
   *        4.  main
   *        5.  ;
   *        6.  switch
   *        7.  case
   *        8.  #ifdef
   *        9.  if
   *        10. else
   *        11. while
   *        12. for
   *        13. do
   *
   *
   *  External Calls:
   *     none
   *
   *  Modifications:
   *     14 March 1995 - created
   *
   *************************************************/


#include "cips.h"

#define LENGTH 80
#define RULES  13

char rule[RULES][LENGTH] = {
               "#include",
               "#define",
               "#undef",
               "main",
               ";",
               "switch",
               "case",
               "#ifdef",
               "if",
               "else",
               "while",
               "for",
               "do"};


main(argc, argv)
   char *argv[];
   int  argc;
{
   char string[LENGTH];

   FILE *input_file;

   int  lines_of_code = 0,
        lines_of_text = 0;

   if(argc != 2){
      printf(
      "\nusage: loc input-file");
      exit(1);
   }

   if((input_file = fopen(argv[1], "rt")) == NULL){
      printf(
     "\nloc: Error opening input file %s\n", 
      argv[1]);
      exit(2);
   }

      /***********************************************
      *
      *   This is the major loop of the program.
      *   It reads one line at a time from the 
      *   input file and searches it to see if it is
      *   a valid line of code.
      *
      ***********************************************/

   while(fgets(string, LENGTH, input_file)){
      lines_of_text++;
      if(is_loc(string))
         lines_of_code++;
   }   /* ends the major loop */

   fclose(input_file);

   printf("%s has:", argv[1]);
   printf("\n     %d lines of text", lines_of_text);
   printf("\n     %d lines of code", lines_of_code);
   return(111);
   
}  /* ends main */



      /***********************************************
      *
      *   is_loc(string)
      *   
      *   This function looks at the text string
      *   to see if it contains any of the lines of 
      *   code strings stored in the rule array.
      *
      ***********************************************/

int is_loc(string)
   char string[];
{
   char *place;
   int  i, result = 0;

   char response[LENGTH];

   for(i=0; i<RULES; i++){
     place = strstr(string, rule[i]);
     if(place != '\0'){
        result = 1;
        i      = RULES+1;
     }
   }

   return(result);

}  /* ends is_loc */
