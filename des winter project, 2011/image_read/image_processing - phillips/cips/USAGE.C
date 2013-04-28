/*PAGE The Beginning */
/*
   usage.c
   Dwayne Phillips
   1 October 1994


USAGE
      usage file.c -e -p out-file-name
      usage file.cpp -e -p out-file-name
      usage file -e -p out-file-name
         given no extension to file look for .c 
         .cpp and .f
      -p means print it to the out-file-name
      -e means explain it to the programmer

      "man" like controls
      Q or q quits (of course ctrl-c does too)
      space advances a full screen
      Enter advances one line

      You can have more than one USAGE and EXPLAIN
      blocks in the source code.
ENDUSAGE


EXPLAIN
      This program pulls lines of information out of
      the source code text of a text file.  This is
      all basic text I/O using fgets, fputs, and
      printf (more EXPLAIN text to follow).
ENDEXPLAIN
*/
/**************************************************/
/*PAGE Includes and Defines */
#include <time.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define BLANK      ' '
#define LENGTH      80
#define LPP         66
#define LPS         15
#define MAX_STRING 255
#define PAGE_TYPE   10
#define TAB_SPACE    5
#define FOOT_SPACE  10
#define FOOTER       4
#define DEBUG

char s_use[LENGTH] = "USAGE";
char e_use[LENGTH] = "ENDUSAGE";
char s_exp[LENGTH] = "EXPLAIN";
char e_exp[LENGTH] = "ENDEXPLAIN";
/**************************************************/
/*PAGE Function Prototypes */
int  has_dot_in_name();
void display_usage();
FILE *find_and_open_input_file();
void pad_string();
void write_3_line_header();
void blank_line();
void paginate_file();
void make_new_page();
/**************************************************/
/*PAGE main */
/*
   A. Quit if the wrong number of parameters.
   B. Loop through the parameters
      C. If only two parameters, then the second
         one is the input file name.
      D. Otherwise, go through each one and look
         for the -e and -p options. 
   E. Open the input file if it exists.
   F. If the user chose to show the usage information
      G. Print it to a file, or
      H. Show it to the screen.
   I. If the user chose to show the explain information
      J. Print it to a file, or
      K. Show it to the screen.
   L. Close the files and end the program.

*/

main(int argc, char *argv[])
{
   char  out_file_name[LENGTH],
         temp_string[LENGTH];

   FILE  *input_file, *out_file;

   int   i,
         print_output      = 0,
         show_explain      = 0,
         show_usage        = 0;


/* A. */
   if(argc < 2  || argc > 5){
      printf("\nusage: usage input-file [-p -e]");
      exit(1);
   } 

/* B. */
/* C. */
   if(argc == 2){
      strcpy(temp_string, argv[1]);
   }

/* D. */
   else{
      i=1;

      while(i < argc){
         if(argv[i][0] == '-'){

            if(strcmp(argv[i], "-p") == 0){
               print_output = 1;
               i++;
               strcpy(out_file_name, argv[i]);
            }  /* ends if -p */
            if(strcmp(argv[i], "-e") == 0){
               show_explain = 1;
               show_usage   = 0;
            }  /* ends if -e */

         }  /* ends if argv[i][0] == '-' */

         else{  /* argv[i] is the input file name */
            strcpy(temp_string, argv[i]);
         }  /* ends else argv[i][0] != '-' */
         i++;
      }  /* ends while i < argc */

   }  /* ends else loop through the arguments */

   if(show_explain == 0) show_usage = 1;

/* E. */   
   input_file = find_and_open_input_file(temp_string);
   if(input_file == NULL){
      printf("\nusage: cannot open input file");
      exit(10);
   }


/* F. */
   if(show_usage){
/* G. */
      if(print_output){
         if((out_file = 
             fopen(out_file_name, "wt")) == NULL)
            printf("\nusage: Cannot open out file %s",
               out_file_name);
         paginate_file(input_file, out_file, 
                       temp_string, s_use, e_use);
      }  /* ends if print_output */
/* H. */
      else{  /* show usage to the screen */
         display_usage(input_file, s_use, e_use);
      }  /* ends show usage to the screen */
   }  /* ends if show usage */

/* I. */
   if(show_explain){
/* J. */
      if(print_output){
         if((out_file = 
             fopen(out_file_name, "wt")) == NULL)
            printf("\nusage: Cannot open out file %s",
               out_file_name);
         paginate_file(input_file, out_file, 
                       temp_string, s_exp, e_exp);
      }  /* ends if print_output */
/* K. */
      else{  /* show explain to the screen */
         display_usage(input_file, s_exp, e_exp);
      }  /* ends show explain to the screen */
   }  /* ends if show explain */

/* L. */
   fclose(input_file);
   fclose(out_file);
   return(1);
}  /* end of main */
/*************************************************/
/*PAGE has_dot_in_name function */
/*
   This function returns a 1 if the input string
   has a dot "." in it.  Else, it returns zero.
*/

int has_dot_in_name(char string[])
{
   int result = 1;
   if(strchr(string, '.') == NULL)
      result = 0;
   return(result);
}  /* ends has_dot_in_name */
/*************************************************/
/*PAGE display_usage function
/*
   display_usage(...

   This function displays the usage or explain 
   information to the screen.

   A. Read all the lines in the input file.
   B. Look for the start marker.
      C. Once you find the start marker, read the
         information.
      D. Display the line as long as it is not 
         the end marker.
      E. If you display a full screen of text, stop
         and wait for a response
      F. If the response is:
         space - show another full screen of text
         return - show one line of text
         Q or q - quit
      G. When you hit the end marker, quit 
         displaying the text you read.
*/

void display_usage(FILE *input_file, 
                   char start[], 
                   char end[])
{
   char *f_result, line[LENGTH];
   int  lines_displayed,
        reading_usage,
        response;

   lines_displayed = 0;

/* A. */
   while(fgets(line, LENGTH+1, input_file)){
/* B. */
      if(strncmp(line, start, 3) == 0){
         reading_usage = 1;
/* C. */
         while(reading_usage){
            f_result = fgets(line, 
                             LENGTH+1, 
                             input_file);
            if(f_result == NULL) 
               reading_usage = 0;

/* D. */
            if((reading_usage == 1)   &&
               (strncmp(line, end, 6) != 0)){
               printf("%s", line);
               lines_displayed++;
/* E. */
               if(lines_displayed >= LPS){
                  response = getchar();
/* F. */
                  if(response == ' ')
                     lines_displayed = 0;
                  if(response == '/n')
                     lines_displayed = LPS-1;
                  if(response == 'Q' ||
                     response == 'q'){
                      exit(20);
                  }  /* ends if response == quit */
               }  /* ends if lines_displayed >= LPS */
            }  /* ends if */
       
/* G. */      
            if(strncmp(line, end, 6) == 0)
               reading_usage = 0;
         }  /* ends while reading_usage */
         
      }  /* ends if found USAGE */

   }  /* ends while fgets */

}  /* ends display_usage */
/*************************************************/
/*PAGE find_and_open_input_file function  */
/*
   find_and_open_input_file(...

   This function attempts to find and open the
   source code file we are examining.  The user
   can enter the source code file name directly
   i.e. file.c file.cpp, etc. or without the 
   extension, i.e. file.  The case where the user
   does not enter the extension is nice because
   the user does not need to know if the source
   code is C, C++, FORTRAN, etc.

   This routine looks to see if the file name has
   a dot (.) in it.  If it does it tries to open
   that file.  If it does not have a dot in it,
   the routine tries several common extenions
   (.c .cpp .f).

   A. If the file name has a dot (.) in it, 
      try to open it.
   B. If no dot in the name, try some common
      file extensions.
      C. Try .c
      D. Try .cpp
      E. Try .f

         You can copy the section of code shown
         and insert the file extension you would
         try.

*/
FILE  *find_and_open_input_file(char input_name[])
{
   char input_file_name[LENGTH],
        temp_string[LENGTH];
   FILE *input_file;
   int  havent_found_file = 1;


   input_file = NULL;

/* A. */
   strcpy(input_file_name, input_name);
   if(has_dot_in_name(input_file_name)){
      printf("\nTry to open %s\n", input_file_name);
      if((input_file = 
             fopen(input_file_name, "rt")) == NULL){
         printf("\nusage: Cannot open input file %s\n",
                input_file_name);
      }  /* ends if input_file */
   }  /* ends if has_dot_in_name */

/* B. */
   else{

/* C. */
         /* Try .c */
      strcpy(temp_string, input_file_name);
      strcat(temp_string, ".c");
      printf("\nTry to open %s\n", temp_string);
      if((input_file = 
         fopen(temp_string, "rt")) == NULL){
         printf("\nusage: Could not open file %s",
            temp_string);
      }  /* ends if input_file .c */
      else
         havent_found_file = 0;

/* D. */
            /* Try .cpp */
      if(havent_found_file){
         strcpy(temp_string, input_file_name);
         strcat(temp_string, ".cpp");
         printf("\nTry to open %s\n", temp_string);
         if((input_file = 
            fopen(temp_string, "rt")) == NULL){
            printf("\nusage: Could not open file %s",
               temp_string);
         }  /* ends if input_file .cpp */
         else
            havent_found_file = 0;
      }  /* ends if havent_found_file */

/* E. */
            /* Try .f or anything other extension */
      if(havent_found_file){
         strcpy(temp_string, input_file_name);
         strcat(temp_string, ".f");
         printf("\nTry to open %s\n", temp_string);
         if((input_file = 
             fopen(temp_string, "rt")) == NULL){
             printf("\nusage: Could not open file %s",
                    temp_string);
         }  /* ends if input_file .f */
         else
            havent_found_file = 0;
      }  /* ends if havent_found_file */

   }  /* ends else does not have dot in name */

   return(input_file);

}  /* ends find_and_open_input_file */
/**************************************************/
/*PAGE paginate_file function

  paginate_file(...

  This function breaks the input file into pages 
  and inserts spaces at the left edge of the page.
  The output is sent to an output text file.

   A. Read the date and time and put them into 
      the time string.
   B. Print the header for the first page.
   C. Read all the lines in the input file.
   D. Look for the start marker.
      E. Once you find the start marker, read the
         information.
      F. Send the line to the output file as long 
         as it is not the end marker.
      G. If you send a full screen of text to the
         output file, make a new page in the output
         file.
      H. When you hit the end marker, quit 
         sending the text to the output file.
      I. At the end of the input file, fill the last
         page of the output file with blank lines.

*/
void paginate_file(FILE *input_file, 
                   FILE *output_file,
                   char *name_of_input_file,
                   char start[],
                   char end[])
{
   char   *f_result, 
          string[LENGTH], 
          time_string[LENGTH];
   int    i,
          first_page       = 1,
          line_counter     = 0,
          page_counter     = 1,
          reading_usage,
          toc_line_counter = 0;
   struct tm *time_of_day;
   time_t seconds;

/* A. */
   time(&seconds);
   time_of_day = localtime(&seconds);
   sprintf(time_string, "%d-%d-%d\0",
           time_of_day->tm_mon+1,
           time_of_day->tm_mday,
           time_of_day->tm_year);

/* B. */
   write_3_line_header(output_file, name_of_input_file,
                       time_string, &page_counter);
   line_counter = line_counter + 3;
   blank_line(output_file);
   ++line_counter;

/* C. */
   while(fgets(string, LENGTH+1, input_file)){
/* D. */
      if(strncmp(string, start, 3) == 0){
         reading_usage = 1;
/* E. */
         while(reading_usage){
            f_result = fgets(string, 
                             LENGTH+1, 
                             input_file);
            if(f_result == NULL) 
               reading_usage = 0;
/* F. */
            if((reading_usage == 1)   &&
               (strncmp(string, end, 6) != 0)){
               pad_string(string, TAB_SPACE);
               fputs(string, output_file);
               ++line_counter;
/* G. */
               if(line_counter >= LPP-FOOT_SPACE)
                  make_new_page(input_file, 
                                output_file,
                                name_of_input_file, 
                                time_string,
                                &page_counter, 
                                &line_counter);
                     }  /* ends if */
/* H. */             
            if(strncmp(string, end, 6) == 0)
               reading_usage = 0;
         }  /* ends while reading_usage */
         
      }  /* ends if found USAGE */

   }  /* end of while fgets */

/* I. */
   for(i=line_counter; i<=LPP; i++)
      blank_line(output_file);

} /* end of paginate_file function */
/**************************************************/
/*PAGE pad_string function

  pad_string(...

  This function inserts spaces into the beginning 
  (left side) of a string.

  A. Declare variables.
  B. Fill a temporary string with num_blanks spaces.
  C. Concatenate input string onto temporary string.
  D. Copy temporary string back to input string.

*/

void pad_string(char string[], 
               int   num_blanks)
{
/* A. */
   int   count;
   char  temp[LENGTH];

/* B. */
   for(count = 0; count < num_blanks; count++)
      temp[count] = ' ';
   temp[count] = '\0';
/* C. */
   strcat(temp, string);
/* D. */
   strcpy(string, temp);

}  /* end of pad_string function */
/**************************************************/
/*PAGE write_3_line_header function

  write_3_line_header(...

  This function writes a three line header to the 
  top of a new page.  The header contains the file 
  name, date, and page number.

  A. Declare variables.
  B. Fill strings with blanks.
  C. Fill strings with information and send to
        the output file.

*/
void write_3_line_header(FILE *file, 
                         char *file_name,
                         char *time_string, 
                         int  *page_number)
{
/* A. */
   char b1[LENGTH], 
        b2[LENGTH], 
        b3[LENGTH], 
        string[LENGTH];
   int  i;

/* B. */
   for(i=0; i<LENGTH; i++){
      b1[i]     = ' ';
      b2[i]     = ' ';
      b3[i]     = ' ';
      string[i] = ' ';
   } /* end of loop over i */

   b1[LENGTH-15] = '\0';
   b2[LENGTH-15] = '\0';
   b3[LENGTH-15] = '\0';

/* C. */
   sprintf(string, "%s\n", file_name);
   strcat(b1, string);
   fputs(b1, file);

   sprintf(string, "%s\n", time_string);
   strcat(b2, string);
   fputs(b2, file);

   sprintf(string, "Page %d\n", *page_number);
   strcat(b3, string);
   fputs(b3, file);

}  /* end of write_3_line_header function */
/**************************************************/
/*PAGE blank_line function
 
  blank_line(...
  
  This function sends a blank line to a file.
*/

void blank_line(FILE *file)
{
   fputs("          \n", file);
} /* end of blank_line function */
/**************************************************/
/*PAGE make_new_page function

  make_new_page(...

  This function fills the remainder of a page with 
  blank lines and prints a header at the top of the 
  next page.

  A. Declare variables.
  B. Loop until the page runs out of lines.
  C. If you reach the end of a page, then
        D. Print FOOTER blank lines, jump to the 
           next page, and print a page header.

*/
void make_new_page(FILE *input_file, 
                   FILE *output_file,
                   char *name_of_input_file, 
                   char *time_string,
                   int  *page_counter, 
                   int  *line_counter)
{
/* A. */
   int  i, not_finished = 1;

/* B. */
   while(not_finished){

/* C. */
      if(*line_counter >= LPP - FOOT_SPACE){

/* D. */
         not_finished = 0;
         for(i=0; i<FOOTER; i++) 
            blank_line(output_file);
         *line_counter = 0;
         *page_counter = *page_counter + 1;
         blank_line(output_file);
         *line_counter = *line_counter + 1;
         write_3_line_header(output_file, 
                             name_of_input_file,
                             time_string, 
                             page_counter);
         *line_counter = *line_counter + 3;
         blank_line(output_file);
         *line_counter = *line_counter + 1;
      }  /* end of if line counter >= LPP - 10 */
      fputs("     \n", output_file);
      *line_counter = *line_counter + 1;
   }  /* end of while not_finished */
}  /* end of make_new_page function */
/**************************************************/
