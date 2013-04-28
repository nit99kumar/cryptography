
/********************************************************/
/********************************************************/
/********************************************************/

/******************* includes ***************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>



/******************* defines ****************************/
#define  MIN_PARAMS 2
#define  MAX_PARAMS 3
#define  LL       100
#define  BACKSLASH 92 
#define  PERCENT   37
#define  SLQOUTE   96 /* single left qoute */
#define  SRQOUTE   39 /* single right qoute */
#define  DBLQOUTE  34
#define  VERSION "DPDETEX Version 1.1 - 18 September 1996"



/**************** function declarations *****************/
int  contains_latex_commands(char []);
void copy_text_until(char [], char [],
                     int *, int *,
                     FILE *, FILE *,
                     char );
void copy_word(char [], int *, char []);
void fill_string(char [], int , char );
void print_help();
void print_usage();
void process_line(char [], char [], FILE *, FILE *);
void remove_latex_commands(FILE *, FILE *);



/********************************************************/
/*
   main

*/

int main (int argc, char *argv[])
{
   FILE *input_file, *output_file;

   printf("\n\n%s\n\n", VERSION);

   if(argc < MIN_PARAMS  ||  argc > MAX_PARAMS){
      print_usage();
      exit(0);
   }  /* ends if argc */

   if(argc == 2){
      if(strcmp(argv[1], "-?") == 0){
         print_help();
         exit(1);
      }  /* ends if -? */
      else{
         print_usage();
         exit(1);
      }  /* ends else not -? */
   }  /* ends if argc == 2 */

   if(strcmp(argv[1], argv[2]) == 0){
      printf(
       "\nERROR: Input and output file names must be different");
      print_usage();
      exit(0);
   } /* ends if strcmp */

   if((input_file = fopen(argv[1], "r+t")) == NULL){
      printf("\nERROR: Cannot open file %s", argv[1]);
      exit(1);
   }  /* ends if input_file */

   if((output_file = fopen(argv[2], "w+t")) == NULL){
      printf("\nERROR: Cannot create file %s", argv[2]);
      fclose(input_file);
      exit(1);
   }  /* ends if output_file */

   remove_latex_commands(input_file, output_file);

   fclose(input_file);
   fclose(output_file);

   printf("\n\n%s\n\n", VERSION);

}  /* ends main */





/********************************************************/
/*
   print_usage()
*/

void print_usage()
{
   printf("\nusage: dpdetex in-file out-file");
}  /* ends print_usage */




/********************************************************/
/*
   print_help()
*/

void print_help()
{
   printf("\n%s", VERSION);
   printf("\nThis program removes some LaTeX commands "
          "including:"
          "\n   PERCENT comment  begin end documentstyle"
          "\n   makefile tableofcontents chapter section"
          "\n   subsection item title author date emph"
          "\n   noindent BACKSLASH double BACKSLASH"
          "\n   & PERCENT # $ _ ldots"
          "\n");
}  /* ends print_help */





/********************************************************/
/*
   contains_latex_commands(...
*/

int  contains_latex_commands(char line[])
{
   int result = 0;
   if(  (strchr(line, BACKSLASH) != NULL)  ||
        (strchr(line, PERCENT)   != NULL)  ||
        (strchr(line, SLQOUTE)   != NULL))
      result = 1;
   return(result);
}  /* ends contains_latex_commands */





/********************************************************/
/*
   remove_latex_commands(...
*/

void remove_latex_commands(FILE *input_file, 
                           FILE *output_file) 
{
   char input_line[LL], output_line[LL];

   fill_string(input_line, LL, '\0');

   while(fgets(input_line, LL, input_file)){
      fill_string(output_line, LL, '\0');
      if(contains_latex_commands(input_line)){
         process_line(input_line, output_line,
                      input_file, output_file);
         fputs(output_line, output_file);
      }  /* ends if */
      else{
         fputs(input_line, output_file);
      }
      fill_string(input_line, LL, '\0');
   }  /* ends while fgets */

}  /* ends remove_latex_commands */



/********************************************************/
/*
   copy_word(...

   This function copies the alphabetic characters
   from the input line to the output word starting
   at the ip spot in the input word.

   At the start of the routine, ip is pointing to the
   first letter of the word.  At the end of the
   routine, ip is pointing past the word.  The output
   word is null terminated.
*/

void copy_word(char input_line[], 
               int *ip, 
               char output_word[])
{
   int i,
       copying = 1,
       owp     = 0;

   fill_string(output_word, LL, '\0');
   while(copying){
      if(isalpha(input_line[*ip])){
         output_word[owp] = input_line[*ip];
         owp++;
         *ip = *ip + 1;
      }  /* ends if isalpha */
      else
         copying = 0;
   }  /* ends while copying */

}  /* ends copy_word */


/********************************************************/
/*
   copy_text_until(...

   This routine copies text from the input_line to
   the output_line.  The copying begins in input_line
   at the pointer ip.  It ends when the input line
   hits the end delimeter end_character.

   At the end of the routine, ip points past the
   end delimeter and op (output_line pointer) points
   past the copied text in the output_line.

   The text to copy can extend over line breaks.  That
   is why the file pointers are passed into this routine.
   If necessary, the routine will read a new line from
   the input file and will write lines to the output
   file.
*/

void copy_text_until(char input_line[], char output_line[],
                     int *ip, int *op,
                     FILE *input_file, FILE *output_file,
                     char end_character)
{
   int copying = 1;

      /**********************************************/
      /*
         There are three major cases while copying
         characters.

         case 1: the delimeted text can stretch over
         line breaks, so write the output_line to the
         output_file, read in a new input_lne from
         the input_file, and continue.

         case 2: the next character is not a line break
         and is not the end_character, so copying it
         from the input_line to the output_line and
         go on.

         case 3: you hit the end character.
         This has two cases of its own.

         case 3.1 the end_character is actually
         two characters, two single right qoutes.

         case 3.2 the end_character is one character.

         In both cases, copy the last character and 
         stop the copying.
      */

   while(copying){

         /* case 1 */
      if(input_line[*ip] == '\n'){
         output_line[*op] = '\n';
         fputs(output_line, output_file);
         fill_string(input_line, LL, '\0');
         fgets(input_line, LL, input_file);
         *ip = 0;
         *op = 0;
         fill_string(output_line, LL, '\0');
      }  /* ends case 1 */

         /* case 2 */
      if(input_line[*ip] != end_character   &&
         input_line[*ip] != '\n'){
         output_line[*op] = input_line[*ip];
         *op = *op + 1;
         *ip = *ip + 1;
      }  /* ends case 2 */

         /* case 3 */
      if(input_line[*ip] == end_character){
            /* case 3.1 */
         if(end_character == SRQOUTE){
            if(input_line[*ip+1] == SRQOUTE){ /* 2 in a row */
               output_line[*op] = DBLQOUTE;
               *ip = *ip + 2;
               *op = *op + 1;
               copying = 0;
            }  /* ends if second SRQOUTE */
            else{  /* else just 1 single right qoute */
               output_line[*op] = input_line[*ip];
               *ip = *ip + 1;
               *op = *op + 1;
            }  /* ends else just 1 single right qoute */
         }  /* ends if SRQOUTE */
            /* case 3.2 */
         else{  /* else you hit the end_character and it
                   is not a single right qoute */
            *ip     = *ip + 1;
            /*** *op     = *op + 1;****/
            copying = 0;
         }  /* ends else 3.2 */
      }  /* ends case 3 */

   }  /* ends while copying */

}  /* ends copy_text_until */





/********************************************************/
/*
   process_line(...

*/

void process_line(char input_line[], char output_line[],
                  FILE *input_file, FILE *output_file)
{
   char latex_word[LL], 
        temp[LL], 
        this,
        that,
        response[LL];

   int  case1 = 0,
        case4 = 0,
        ip    = 0, 
        op    = 0,
        tp    = 0;

   printf("||%s", input_line);

   while(input_line[ip] != '\0'){
      case1 = 0;
      case4 = 0;
      this  = input_line[ip];
      that  = input_line[ip+1];

      if(this == '{')
         case1 = 1;
      if(this == PERCENT)
         case1 = 2;
      if(this == SLQOUTE  &&
         that == SLQOUTE)
         case1 = 3;
      if(this == BACKSLASH)
         case1 = 4;

      switch(case1){

         case 1:
            ip = ip + 4;
            while(input_line[ip] == ' ')
               ip++;
            copy_text_until(input_line, output_line, 
                            &ip, &op,
                            input_file, output_file, 
                            '}');
            break;

         case 2:
            output_line[op] = '\n';
            input_line[ip]  = '\0';
            break;

         case 3:
            output_line[op] = DBLQOUTE;
            op = op + 1;
            ip = ip + 2;
            copy_text_until(input_line, output_line,
                            &ip, &op, input_file,
                            output_file, SRQOUTE);
            break;

         case 4:  /* BACKSLASH */
            case4 = 0;
            if(input_line[ip+1] == '&'  ||
               input_line[ip+1] == '%'  ||
               input_line[ip+1] == '#'  ||
               input_line[ip+1] == '$'  ||
               input_line[ip+1] == '_')
               case4 = 1;

            strcpy(temp, input_line);
            tp = ip + 1;
            copy_word(temp, &tp, latex_word);
            if( (strcmp(latex_word, "begin") == 0)        ||
                (strcmp(latex_word, "end") == 0)          ||
                (strcmp(latex_word, "documentstyle") == 0)||
                (strcmp(latex_word, "makefile") == 0)     ||
                (strcmp(latex_word, "noindent") == 0)     ||
                (strcmp(latex_word, "tableofcontents") == 0))
               case4 = 2;

            if( (strcmp(latex_word, "chapter") == 0)    ||
                (strcmp(latex_word, "section") == 0)    ||
                (strcmp(latex_word, "subsection") == 0) ||
                (strcmp(latex_word, "item") == 0)       ||
                (strcmp(latex_word, "title") == 0)      ||
                (strcmp(latex_word, "author") == 0)     ||
                (strcmp(latex_word, "emph") == 0)     ||
                (strcmp(latex_word, "date") == 0))
               case4 = 3;

            if(is_ldots(input_line, ip))
               case4 = 4;

            if(that == BACKSLASH)
               case4 = 5;

            switch(case4){
               case 1:
                  output_line[op] = input_line[ip+1];
                  op++;
                  if(input_line[ip+1] == '_')
                     ip = ip + 4;
                  else
                     ip = ip + 2;
                  break;

               case 2:
                  output_line[op] = '\n';
                  input_line[ip]  = '\0';
                  break;

               case 3:
                  ip++;
                  copy_word(input_line, &ip, latex_word);
                  ip++;
                  if(input_line[ip] == '{')
                     ip++;
                  copy_text_until(input_line, output_line,
                                  &ip, &op, input_file,
                                  output_file, '}');
                  break;

               case 4:
                  output_line[op++] = '.';
                  output_line[op++] = '.';
                  output_line[op++] = '.';
                  ip = ip + 6;
                  break;

               case 5:
                  ip = ip + 2;
                  break;

            }  /* ends if switch case4 */
            break; /* ends case 4 */

         default:
            output_line[op++] = input_line[ip++];
            break;

      }  /* ends switch case1 */

   }  /* ends while input_line[ip] != NULL */

}  /* ends process_line */




/********************************************************/
/*
   fill_string(...
*/

void fill_string(char string[], int size, char fill)
{
   int i;
   for(i=0; i<size; i++)
      string[i] = fill;
}  /* ends fill_string */




/********************************************************/
/*
   is_ldots(...
*/

int is_ldots(char input[], int ip)
{
   char temp[LL];
   int  i,
        result = 0;
   fill_string(temp, LL, '\0');
   for(i=0; i<5; i++)
      temp[i] = input[ip+i+1];
   if(strcmp(temp, "ldots") == 0)
      result = 1;
   return(result);
}  /* ends is_ldots */


#ifdef NEVER





/********************************************************/
/*
   process_line(...

   This routine does the main part of the program.
   It processes each line in the input file, strips
   the Latex commands, and writes the result to the
   output file.

   This looks for five basic types of Latex commands.

   case 1: The command begins with a '{'.  Inside the {
   is a \xx command like em bf rm it sl sf sc.  Skip
   over the { and these three characters and copy 
   the text until you hit the }.  You may need
   to read several lines of input.

   case 2: Latex comment.  You hit a %, the rest
   of the line is a comment so delete it.

   case 3: You hit a single left qoute.  If the next
   character is another single left qoute, replace the
   two by a double qoute character and copy text until
   you hit two single right qoutes, then replace them
   by another double qoute character.  If the character
   just past the first single left qoute is not another
   single left qoute, just copy and go on.

   case 4: Simple case where you hit a \ then
   a & % # $ or _.  Delete the \ and keep the
   next character.

   case 5:  Two parts

   case 5.1: The kill all case.  The Latex command is a 
   \begin end documenstyle maketitle tableofcontents or
   %comments.  Delete this line.  Kill it all.

   case 5.2: Keep the insides of the {}.  The command is
   a \chapter{} or section subsection item title author or
   date.  Keep the text inside the {} and remove the rest.
*/

void process_line(char input_line[], char output_line[],
                  FILE *input_file, FILE *output_file)
{
   char latex_word[LL], response[LL];
   int  case4 = 0,
        ip    = 0, 
        op    = 0;

   while(input_line[ip] != '\0'){

         /* The nothing case */
      if(input_line[ip] != '{'       &&
         input_line[ip] != PERCENT   &&
         input_line[ip] != SLQOUTE   &&
         input_line[ip] != BACKSLASH){
         output_line[op] = input_line[ip];
         op++;
         ip++;
      }  /* ends nothing case */
      
         /* case  1 */
      if(input_line[ip] == '{'){
         ip = ip + 4;
         while(input_line[ip] == ' ')
            ip++;
         copy_text_until(input_line, output_line, &ip, &op,
                         input_file, output_file, '}');
      }  /* ends case 1 */

         /* case 2 */
      if(input_line[ip] == PERCENT){
         output_line[op] = '\n';
         input_line[ip]  = '\0';
      }  /* ends case 2 */

         /* case 3 */
      if(input_line[ip] == SLQOUTE){
         if(input_line[ip+1] == SLQOUTE){
            output_line[op] = DBLQOUTE;
            op = op + 1;
            ip = ip + 2;
            copy_text_until(input_line, output_line, 
                            &ip, &op,
                            input_file, output_file, 
                            SRQOUTE);
         }  /* ends second if SLQOUTE */
         else { /* else just 1 SLQOUTE */
            output_line[op++] = input_line[ip++];
         }  /* ends else just 1 SLQOUTE */
      }  /* ends case 3 */

      if(input_line[ip] == BACKSLASH){

            /* case 4 */
         if(input_line[ip+1] == '&'  ||
            input_line[ip+1] == '%'  ||
            input_line[ip+1] == '#'  ||
            input_line[ip+1] == '$'  ||
            input_line[ip+1] == '_'){
            case4 = 1;
            output_line[op] = input_line[ip+1];
            op++;
            if(input_line[ip+1] == '_')
               ip = ip + 4;  /* the _ case has a */
            else             /* \_{} ==> _       */
               ip = ip + 2;
         }  /* ends if case 4 */

            /* if you did case4, jump around cases 
               5.1 and 5.2 */

         if(case4 == 0){

            ip++;

            copy_word(input_line, &ip, latex_word);

               /* case 5.1 */
            if( (strcmp(latex_word, "begin") == 0)         ||
                (strcmp(latex_word, "end") == 0)           ||
                (strcmp(latex_word, "documentstyle") == 0) ||
                (strcmp(latex_word, "makefile") == 0)      ||
                (strcmp(latex_word, "tableofcontents") == 0)){
               output_line[op] = '\n';
               input_line[ip] = '\0';
            }  /* ends case 5.1 */

               /* case 5.2 */
            if( (strcmp(latex_word, "chapter") == 0)    ||
                (strcmp(latex_word, "section") == 0)    ||
                (strcmp(latex_word, "subsection") == 0) ||
                (strcmp(latex_word, "item") == 0)       ||
                (strcmp(latex_word, "title") == 0)      ||
                (strcmp(latex_word, "author") == 0)     ||
                (strcmp(latex_word, "date") == 0)){
               ip++; /* ip points to { so increment it */
               if(input_line[ip] == '{') ip++;
               copy_text_until(input_line, output_line, 
                               &ip, &op,
                               input_file, output_file, 
                               '}');
            }  /* ends case 5.2 */

         }  /* ends if case4 == 0 */

      }  /* ends if BACKSLASH */

   }  /* ends while input line is not null */

}  /* ends process_line */


#endif
